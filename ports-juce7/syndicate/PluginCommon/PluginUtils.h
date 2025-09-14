/*
  ==============================================================================

    Utils.h
    Created: 16 Mar 2021 9:09:05pm
    Author:  Jack Devlin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Utils {
    const juce::String pluginSelectorComponentID("PluginSelectorComponent");

    inline juce::String busesLayoutToString(juce::AudioProcessor::BusesLayout layout) {
        juce::String retVal;

        retVal += "Inputs: ";
        for (const juce::AudioChannelSet& bus : layout.inputBuses) {
            for (const juce::AudioChannelSet::ChannelType channelType : bus.getChannelTypes()) {
                retVal += juce::AudioChannelSet::getAbbreviatedChannelTypeName(channelType) + " ";
            }
            retVal += "| ";
        }
        retVal += "\n";

        retVal += "Outputs: ";
        for (const juce::AudioChannelSet& bus : layout.outputBuses) {
            for (const juce::AudioChannelSet::ChannelType channelType : bus.getChannelTypes()) {
                retVal += juce::AudioChannelSet::getAbbreviatedChannelTypeName(channelType) + " ";
            }
            retVal += "| ";
        }

        return retVal;
    }

    inline void processBalance(float panValue, juce::AudioBuffer<float>& buffer) {
        // Check we have enough channels
        if (buffer.getNumChannels() >= 2) {
            if (panValue > 0) {
                // Balance is to the right - so linearly attenuate the left
                const float leftGain {1 - panValue};
                juce::FloatVectorOperations::multiply(buffer.getWritePointer(0),
                                                      leftGain,
                                                      buffer.getNumSamples());
            } else if (panValue < 0) {
                // Balance is to the left - so linearly attenuate the right
                const float rightGain {1 + panValue};
                juce::FloatVectorOperations::multiply(buffer.getWritePointer(1),
                                                      rightGain,
                                                      buffer.getNumSamples());
            }
        }
    }
}
