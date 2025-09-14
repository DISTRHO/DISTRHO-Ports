#pragma once

#include <JuceHeader.h>

class ScanConfiguration {
public:
#ifdef __APPLE__
    juce::AudioUnitPluginFormat auFormat;
#endif
    juce::VSTPluginFormat vstFormat;
    juce::VST3PluginFormat vst3Format;

    bool VSTDefaultPaths;
    juce::FileSearchPath customVSTPaths;

    bool VST3DefaultPaths;
    juce::FileSearchPath customVST3Paths;

    ScanConfiguration();

    juce::FileSearchPath getAUPaths();

    juce::FileSearchPath getVSTPaths();

    juce::FileSearchPath getVST3Paths();

    void restoreFromXml();
    void writeToXml() const;
};
