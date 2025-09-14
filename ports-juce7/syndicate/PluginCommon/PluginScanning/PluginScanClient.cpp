#include "PluginScanClient.h"
#include "CustomScanner.hpp"

PluginScanClient::PluginScanClient(juce::AudioProcessor::WrapperType wrapperType) : juce::Thread("Scan Client"),
                                       _wrapperType(wrapperType),
                                       _hasAttemptedRestore(false),
                                       _shouldRestart(false),
                                       _shouldExit(false),
                                       _isClearOnlyScan(false) {
    _scannedPluginsFile = Utils::DataDirectory.getChildFile(Utils::SCANNED_PLUGINS_FILE_NAME);
    _scannedPluginsFile.create();
}

juce::Array<juce::PluginDescription> PluginScanClient::getPluginTypes() const {
    return _pluginList->getTypes();
}

void PluginScanClient::restore() {
    _hasAttemptedRestore = true;

    _pluginList = std::make_unique<juce::KnownPluginList>();
    _pluginList->setCustomScanner(std::make_unique<CustomPluginScanner>(_wrapperType));

    const juce::File scannedPluginsFile(Utils::DataDirectory.getChildFile(Utils::SCANNED_PLUGINS_FILE_NAME));

    if (scannedPluginsFile.existsAsFile()) {
        std::unique_ptr<juce::XmlElement> pluginsXml = juce::parseXML(scannedPluginsFile);

        if (pluginsXml.get() != nullptr) {
            _pluginList->recreateFromXml(*pluginsXml);
        }

        // Notify the listeners
        {
            std::scoped_lock lock(_listenersMutex);
            for (juce::MessageListener* listener : _listeners) {
                _notifyListener(listener);
            }
        }

        juce::Logger::writeToLog("Restored " + juce::String(_pluginList->getNumTypes()) + " plugins from disk");

    } else {
        juce::Logger::writeToLog("Nothing to restore plugins from");
    }
}

void PluginScanClient::startScan() {
    if (_state == ScanState::STARTING) {
        juce::Logger::writeToLog("Can't start scan - already starting");
        return;
    }

    if (_state == ScanState::RUNNING) {
        juce::Logger::writeToLog("Can't start scan - already running");
        return;
    }

    if (_state == ScanState::STOPPING) {
        juce::Logger::writeToLog("Can't stop scan - is currently stopping");
        return;
    }

    // Check the scanner binary exists
    if (Utils::getPluginScanServerBinary(_wrapperType).existsAsFile()) {
        _errorMessage = "";
    } else {
        juce::Logger::writeToLog("PluginScanServer binary is missing");
        _errorMessage = "Unable to find plugin scanner - please reinstall";

        // Notify the listeners
        _notifyAllListeners();

        return;
    }

    _state = ScanState::STARTING;

    startThread();
}

void PluginScanClient::stopScan() {
    if (_state == ScanState::STOPPED) {
        juce::Logger::writeToLog("Can't stop scan - already stopped");
        return;
    }

    if (_state == ScanState::STARTING) {
        juce::Logger::writeToLog("Can't stop scan - is currently starting");
        return;
    }

    if (_state == ScanState::STOPPING) {
        juce::Logger::writeToLog("Can't stop scan - already stopping");
        return;
    }

    // Only do this if we're currently scanning
    _state = ScanState::STOPPING;
    _shouldExit = true;

    juce::Logger::writeToLog("Stopping plugin scan server");
}

void PluginScanClient::clearMissingPlugins() {
    if (_state == ScanState::STARTING) {
        juce::Logger::writeToLog("Can't clear missing plugins - already starting");
        return;
    }

    if (_state == ScanState::RUNNING) {
        juce::Logger::writeToLog("Can't clear missing plugins - already running");
        return;
    }

    if (_state == ScanState::STOPPING) {
        juce::Logger::writeToLog("Can't clear missing plugins - is currently stopping");
        return;
    }

    _state = ScanState::STARTING;

    _isClearOnlyScan = true;

    startThread();
}

void PluginScanClient::rescanAllPlugins() {
    juce::Logger::writeToLog("Attempting rescan of all plugins");

    if (_state == ScanState::STOPPED) {
        juce::Logger::writeToLog("Deleting all plugin data files");
        Utils::DataDirectory.getChildFile(Utils::SCANNED_PLUGINS_FILE_NAME).deleteFile();
        Utils::DataDirectory.getChildFile(Utils::CRASHED_PLUGINS_FILE_NAME).deleteFile();

        _hasAttemptedRestore = false;
        startScan();
    } else {
        juce::Logger::writeToLog("Couldn't delete plugin data files, scan is still running");
    }
}

void PluginScanClient::rescanCrashedPlugins() {
    juce::Logger::writeToLog("Attempting rescan of crashed plugins");

    if (_state == ScanState::STOPPED) {
        juce::Logger::writeToLog("Deleting crashed plugins file");
        Utils::DataDirectory.getChildFile(Utils::CRASHED_PLUGINS_FILE_NAME).deleteFile();

        _hasAttemptedRestore = false;
        startScan();
    } else {
        juce::Logger::writeToLog("Couldn't delete crashed plugins file, scan is still running");
    }
}

void PluginScanClient::addListener(juce::MessageListener* listener) {
    if (listener != nullptr) {
        std::scoped_lock lock(_listenersMutex);
        if (std::find(_listeners.begin(), _listeners.end(), listener) == _listeners.end()) {
            _listeners.push_back(listener);
        }

        // Notify the listener of the current state
        _notifyListener(listener);
    }
}

void PluginScanClient::removeListener(juce::MessageListener* listener) {
    if (listener != nullptr) {
        std::scoped_lock lock(_listenersMutex);
        _listeners.erase(std::remove(_listeners.begin(), _listeners.end(), listener), _listeners.end());
    }
}

void PluginScanClient::run() {
    _shouldExit = false;
    _state = ScanState::RUNNING;

    // We need to force an update after changing state
    _notifyAllListeners();

    if (!_hasAttemptedRestore) {
        restore();
    }

    // Just in case the config has been changed via another plugin instance, restore it here
    config.restoreFromXml();

    {
        const juce::MessageManagerLock mml(juce::Thread::getCurrentThread());
        if (mml.lockWasGained()) {
            _pluginList->addChangeListener(this);
        }
    }

    if (_isClearOnlyScan) {
        auto checkPluginsForFormat = [](juce::KnownPluginList* pluginList, juce::AudioPluginFormat& format) {
            for (juce::PluginDescription plugin : pluginList->getTypesForFormat(format)) {
                if (!format.doesPluginStillExist(plugin)) {
                    juce::Logger::writeToLog("[" + plugin.name + "] is missing, removing from list");
                    pluginList->removeType(plugin);
                }
            }
        };

        #ifdef __APPLE__
            checkPluginsForFormat(_pluginList.get(), config.auFormat);
        #endif

        checkPluginsForFormat(_pluginList.get(), config.vstFormat);
        checkPluginsForFormat(_pluginList.get(), config.vst3Format);
    } else {
        #ifdef __APPLE__
            _scanForFormat(config.auFormat, config.getAUPaths());
        #endif

        _scanForFormat(config.vstFormat, config.getVSTPaths());
        _scanForFormat(config.vst3Format, config.getVST3Paths());
    }

    {
        const juce::MessageManagerLock mml(juce::Thread::getCurrentThread());
        if (mml.lockWasGained()) {
            _pluginList->removeChangeListener(this);
        }
    }

    _isClearOnlyScan = false;
    _state = ScanState::STOPPED;

    // We need to force an update after changing state
    _notifyAllListeners();

    juce::Logger::writeToLog("All plugin scan jobs finished");
}


void PluginScanClient::_scanForFormat(juce::AudioPluginFormat& format, juce::FileSearchPath searchPaths) {
    if (_shouldExit) {
        return;
    }

    juce::Logger::writeToLog("[" + format.getName() + "] Scanning with paths: " + searchPaths.toString());

    juce::File deadMansPedalFile = Utils::DataDirectory.getChildFile(Utils::CRASHED_PLUGINS_FILE_NAME);
    deadMansPedalFile.create();

    juce::PluginDirectoryScanner scanner(*_pluginList.get(),
                                         format,
                                         searchPaths,
                                         true,
                                         deadMansPedalFile,
                                         true);
    bool isFinished {false};

    while (!_shouldExit && !isFinished) {
        // Prevent the plugin scanning itself or a plugin that previously stalled a scan
        if (scanner.getNextPluginFileThatWillBeScanned() == "Syndicate") {
            if (!scanner.skipNextFile()) {
                return;
            }
        }

        // Scan the plugin
        juce::Logger::writeToLog("[" + format.getName() + "] plugin #" + juce::String(_pluginList->getNumTypes()) + ": " + scanner.getNextPluginFileThatWillBeScanned());
        juce::String currentPluginName;
        isFinished = !scanner.scanNextFile(true, currentPluginName);
    }
}

void PluginScanClient::changeListenerCallback(juce::ChangeBroadcaster* changed) {
    if (changed == _pluginList.get()) {
        std::unique_ptr<juce::XmlElement> pluginsXml = std::unique_ptr<juce::XmlElement>(_pluginList->createXml());

        if (pluginsXml.get() != nullptr) {
            // Delete and recreate the file so that it's empty
            _scannedPluginsFile.deleteFile();
            _scannedPluginsFile.create();

            // Write the Xml to the file
            juce::FileOutputStream output(_scannedPluginsFile);

            if (output.openedOk()) {
                pluginsXml->writeTo(output);
            }
        }

        _notifyAllListeners();
    }
}

void PluginScanClient::_notifyAllListeners() {
    std::scoped_lock lock(_listenersMutex);
    for (juce::MessageListener* listener : _listeners) {
        _notifyListener(listener);
    }
}

void PluginScanClient::_notifyListener(juce::MessageListener* listener) {
    listener->postMessage(new PluginScanStatusMessage(
        _pluginList->getNumTypes(), _state == ScanState::RUNNING, _errorMessage));
}
