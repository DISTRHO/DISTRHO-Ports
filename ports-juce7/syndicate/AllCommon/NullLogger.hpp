#pragma once

#include <JuceHeader.h>

class NullLogger : public juce::Logger {
public:
    NullLogger() = default;
    virtual ~NullLogger() = default;

private:
    virtual void logMessage(const juce::String& message) override { }
};
