#pragma once

#include <JuceHeader.h>
#include "PluginSplitter.hpp"

namespace SplitterProcessors {
    void prepareToPlay(PluginSplitter& splitter, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout);
    void releaseResources(PluginSplitter& splitter);
    void reset(PluginSplitter& splitter);
    void processBlock(PluginSplitter& splitter,
                      juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages,
                      juce::AudioPlayHead* newPlayHead);
}
