#pragma once

#include <JuceHeader.h>

class MainLogger : public juce::Logger {
public:
    MainLogger(const char* appName, const char* appVersion, const juce::File& logDirectory);
    virtual ~MainLogger() = default;

private:
    juce::File _logFile;

    virtual void logMessage(const juce::String& message) override;

    void _logEnvironment(const char* appName, const char* appVersion);
};
