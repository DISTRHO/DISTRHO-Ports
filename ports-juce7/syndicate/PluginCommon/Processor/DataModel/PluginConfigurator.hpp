#pragma once

#include <JuceHeader.h>

inline bool canDoStereoSplitTypes(const juce::AudioProcessor::BusesLayout& layout) {
    return layout.getMainInputChannels() == layout.getMainOutputChannels() &&
           layout.getMainOutputChannels() == 2;
}

inline int getTotalNumInputChannels(const juce::AudioProcessor::BusesLayout& layout) {
    int retVal {0};

    for (int index {0}; index < layout.inputBuses.size(); index++) {
        retVal += layout.getNumChannels(true, index);
    }

    return retVal;
}

inline bool layoutHasSidechain(const juce::AudioProcessor::BusesLayout& layout) {
    return layout.getBuses(true).size() > 1 && layout.getBuses(true)[1].size() > 0;
}

struct HostConfiguration {
    juce::AudioProcessor::BusesLayout layout;
    double sampleRate;
    int blockSize;
};

class PluginConfigurator {
public:
    PluginConfigurator();

    /**
     * To simpify plugin routing we assume that all plugins support both mono and stereo layouts,
     * and just set them to use the same layout as Syndicate. Syndicate also supports sidechain so
     * we'll try to configure sidechain layouts for the plugins, and drop back to a non-sidechain
     * layout if it doesn't work. In processBlock we'll still pass the full buffer with sidechain to
     * the plugin, which should just be ignored if it doesn't need it.
     *
     * We assume the buses layout will never change while the plugin is running, so only need to be
     * configured when a plugin is added by the user or has been restored from XML.
     *
     * We could do things like try to load plugins in mono for split types that only ever need mono
     * (ie. left/right and mid/side) but that gets really complicated and most users won't see any
     * benefit from it.
     */
    bool configure(std::shared_ptr<juce::AudioPluginInstance> plugin,
                   HostConfiguration configuration) const;

private:
    juce::AudioProcessor::BusesLayout monoInMonoOut;
    juce::AudioProcessor::BusesLayout monoInMonoOutSC;
    juce::AudioProcessor::BusesLayout stereoInStereoOut;
    juce::AudioProcessor::BusesLayout stereoInStereoOutSC;
};
