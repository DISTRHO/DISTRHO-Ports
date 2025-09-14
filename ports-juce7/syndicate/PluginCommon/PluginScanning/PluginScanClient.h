#pragma once

#include <JuceHeader.h>
#include "AllUtils.h"
#include "PluginScanStatusMessage.h"
#include "ScanConfiguration.hpp"

enum class ScanState {
    STOPPED,
    STARTING,
    RUNNING,
    STOPPING
};

class PluginScanClient : public juce::ChangeListener,
                         public juce::Thread {
public:
    ScanConfiguration config;

    PluginScanClient(juce::AudioProcessor::WrapperType wrapperType);

    juce::Array<juce::PluginDescription> getPluginTypes() const;

    /**
     * Called to update the internal list of plugins to match what is on disk. Public so that a
     * restore can be done without needing to start a scan.
     */
    void restore();

    /**
     * Called when the user starts a scan.
     */
    void startScan();

    /**
     * Called when the user has initiated the stop.
     */
    void stopScan();

    /**
     * Called to clear plugins that may be uninstalled or missing.
     */
    void clearMissingPlugins();

    /**
     * Called when the user has requested a full rescan.
     */
    void rescanAllPlugins();

    /**
     * Called when the user has requested a rescan of crashed plugins
     */
    void rescanCrashedPlugins();

    void addListener(juce::MessageListener* listener);

    void removeListener(juce::MessageListener* listener);

    /**
     * Performs actions as needed - don't call this manually
     */
    void run() override;

    void changeListenerCallback(juce::ChangeBroadcaster* changed) override;

private:
    const juce::AudioProcessor::WrapperType _wrapperType;
    std::unique_ptr<juce::KnownPluginList> _pluginList;
    juce::File _scannedPluginsFile;
    std::vector<juce::MessageListener*> _listeners;
    std::mutex _listenersMutex;

    // True if an attempt has been made to restore from previous scan (whether successful or not)
    bool _hasAttemptedRestore;

    // True if the scan process should be restarted in the event that it exits
    bool _shouldRestart;

    std::atomic<bool> _shouldExit;

    ScanState _state;

    juce::String _errorMessage;

    bool _isClearOnlyScan;

    void _notifyAllListeners();

    void _notifyListener(juce::MessageListener* listener);

    void _onConnectionLost();

    void _readScannerFilesForUpdates();

    void _scanForFormat(juce::AudioPluginFormat& format, juce::FileSearchPath searchPaths);
};
