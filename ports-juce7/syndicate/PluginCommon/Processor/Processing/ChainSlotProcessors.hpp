#pragma once

#include <JuceHeader.h>
#include "ChainSlots.hpp"

namespace ChainProcessors {
    void prepareToPlay(ChainSlotGainStage& gainStage, HostConfiguration config);
    void releaseResources(ChainSlotGainStage& gainStage);
    void reset(ChainSlotGainStage& gainStage);
    void processBlock(ChainSlotGainStage& gainStage, juce::AudioBuffer<float>& buffer);

    void prepareToPlay(ChainSlotPlugin& slot, HostConfiguration config);
    void releaseResources(ChainSlotPlugin& slot);
    void reset(ChainSlotPlugin& slot);
    void processBlock(ChainSlotPlugin& slot,
                      juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages,
                      juce::AudioPlayHead* newPlayHead);
}
