#include "MainLogger.h"

#include "AllUtils.h"

namespace {
    juce::String getTimestamp() {
        const juce::Time currentTime(juce::Time::getCurrentTime());

        juce::String milliseconds(currentTime.getMilliseconds());
        while (milliseconds.length() < 3) {
            milliseconds = "0" + milliseconds;
        }

        return currentTime.formatted("%Y-%m-%d_%H-%M-%S.") + milliseconds;
    }
}

MainLogger::MainLogger(const char* appName, const char* appVersion, const juce::File& logDirectory) {
    // Open log file
    _logFile = logDirectory.getNonexistentChildFile(getTimestamp(), ".txt", true);
    _logFile.create();

    _logEnvironment(appName, appVersion);
}

void MainLogger::logMessage(const juce::String& message) {

    const juce::String outputMessage = getTimestamp() + " :    " + message + "\n";

    juce::FileOutputStream output(_logFile);
    if (output.openedOk()) {
        output.writeText(outputMessage, false, false, "\n");
    }
}

void MainLogger::_logEnvironment(const char* appName, const char* appVersion) {

    juce::FileOutputStream output(_logFile);

    if (output.openedOk()) {
        const juce::String archString(
#if defined(__x86_64__) || defined(_M_AMD64)
        "x86_64"
#elif defined(__aarch64__) || defined(_M_ARM64)
        "arm64"
#else
    #error "Unknown arch"
#endif
        );

        const juce::String outputMessage(
            "******************************************************\n" +
            juce::String(appName) + ": " + juce::String(appVersion) + "\n"
            "OS:   " + juce::SystemStats::getOperatingSystemName() + "\n"
            "ARCH: " + archString + "\n"
            "CPUs: " + juce::String(juce::SystemStats::getNumPhysicalCpus()) + " (" + juce::String(juce::SystemStats::getNumCpus()) + ")\n"
            "RAM:  " + juce::String(juce::SystemStats::getMemorySizeInMegabytes()) + "MB\n"
            "******************************************************\n\n");

        output.writeText(outputMessage, false, false, "\n");
    }
}
