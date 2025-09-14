#pragma once

#include <JuceHeader.h>
#include "PluginChain.hpp"

namespace ChainProcessors {
    void prepareToPlay(PluginChain& chain, HostConfiguration config);
    void releaseResources(PluginChain& chain);
    void reset(PluginChain& chain);
    void processBlock(PluginChain& chain,
                      juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages,
                      juce::AudioPlayHead* newPlayHead);
}
