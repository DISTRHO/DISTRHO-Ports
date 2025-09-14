#pragma once

#include <JuceHeader.h>

#include "AllUtils.h"
#include "MainLogger.h"
#include "NullLogger.hpp"
#include "ServerProcess.h"

// TODO use macros for name and version

class ServerApplication : public juce::JUCEApplicationBase {
public:
    ServerApplication() {
        const Utils::Config config = Utils::LoadConfig();
        if (config.enableLogFile) {
            _fileLogger = std::make_unique<MainLogger>("Syndicate Plugin Scan Server", ProjectInfo::versionString, Utils::PluginScanServerLogDirectory);
            juce::Logger::setCurrentLogger(_fileLogger.get());
        } else {
            juce::Logger::setCurrentLogger(&_nullLogger);
        }
    }

    ~ServerApplication() {
        // Logger must be removed before being deleted
        // (this must be the last thing we do before exiting)
        juce::Logger::setCurrentLogger(nullptr);
    }

    const juce::String getApplicationName() override { return "Syndicate Plugin Scan Server"; }

    const juce::String getApplicationVersion() override { return "0.0.1"; }

    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String& commandLineParameters) override {
        juce::Logger::writeToLog("Initialising");
        _process.reset(new ServerProcess());

        if (_process->initialiseFromCommandLine(commandLineParameters, Utils::PLUGIN_SCAN_SERVER_UID)) {
            juce::Logger::writeToLog("Process started");
        }
    }

    void shutdown() override {
        juce::Logger::writeToLog("Shutting down");
    }

    void anotherInstanceStarted(const juce::String& commandLine) override {
        // TODO
    }

    void systemRequestedQuit() override {
        juce::Logger::writeToLog("System Requested Quit");
        quit();
    }

    void suspended() override {
        // TODO
    }

    void resumed() override {
        // TODO
    }

    void unhandledException(const std::exception*,
                            const juce::String& sourceFilename,
                            int lineNumber) override {
        juce::Logger::writeToLog("Unhandled exception");
    }

private:
    std::unique_ptr<MainLogger> _fileLogger;
    NullLogger _nullLogger;
    std::unique_ptr<ServerProcess> _process;
};
