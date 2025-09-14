#include "PluginConfigurator.hpp"

PluginConfigurator::PluginConfigurator() {
    monoInMonoOut.inputBuses.add(juce::AudioChannelSet::mono());
    monoInMonoOut.outputBuses.add(juce::AudioChannelSet::mono());

    monoInMonoOutSC.inputBuses.add(juce::AudioChannelSet::mono());
    monoInMonoOutSC.inputBuses.add(juce::AudioChannelSet::mono());
    monoInMonoOutSC.outputBuses.add(juce::AudioChannelSet::mono());

    stereoInStereoOut.inputBuses.add(juce::AudioChannelSet::stereo());
    stereoInStereoOut.outputBuses.add(juce::AudioChannelSet::stereo());

    stereoInStereoOutSC.inputBuses.add(juce::AudioChannelSet::stereo());
    stereoInStereoOutSC.inputBuses.add(juce::AudioChannelSet::stereo());
    stereoInStereoOutSC.outputBuses.add(juce::AudioChannelSet::stereo());
}

bool PluginConfigurator::configure(std::shared_ptr<juce::AudioPluginInstance> plugin,
                                   HostConfiguration configuration) const {
    // Note: for this layout stuff to work correctly it *must* be done before prepareToPlay() is
    // called on the plugin we just loaded. If we try it afterwards JUCE can't actually query the
    // layouts that the plugin supports so might do something weird.
    std::vector<const juce::AudioProcessor::BusesLayout*> rankedLayouts;

    const bool isSyndicateStereo {
        configuration.layout.getMainInputChannels() == 2 && configuration.layout.getMainOutputChannels() == 2
    };

    const bool isSyndicateSidechain {layoutHasSidechain(configuration.layout)};

    if (isSyndicateStereo) {
        if (isSyndicateSidechain) {
            rankedLayouts.push_back(&stereoInStereoOutSC);
            rankedLayouts.push_back(&stereoInStereoOut);
        } else {
            rankedLayouts.push_back(&stereoInStereoOut);
            rankedLayouts.push_back(&stereoInStereoOutSC);
        }
    } else {
        if (isSyndicateSidechain) {
            rankedLayouts.push_back(&monoInMonoOutSC);
            rankedLayouts.push_back(&monoInMonoOut);
        } else {
            rankedLayouts.push_back(&monoInMonoOut);
            rankedLayouts.push_back(&monoInMonoOutSC);
        }
    }

    // Try each layout in order
    bool setLayoutOk {false};
    for (const juce::AudioProcessor::BusesLayout* layout : rankedLayouts) {
        if (plugin->setBusesLayout(*layout)) {
            setLayoutOk = true;
            break;
        }
    }

    if (setLayoutOk) {
        plugin->enableAllBuses();
        plugin->setRateAndBufferSizeDetails(configuration.sampleRate, configuration.blockSize);
        plugin->prepareToPlay(configuration.sampleRate, configuration.blockSize);
    }

    return setLayoutOk;
}
