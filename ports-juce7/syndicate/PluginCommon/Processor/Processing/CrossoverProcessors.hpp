#pragma once

#include "CrossoverState.hpp"

namespace CrossoverProcessors {
    void prepareToPlay(CrossoverState& state, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout);
    void reset(CrossoverState& state);
    void processBlock(CrossoverState& state, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, juce::AudioPlayHead* newPlayHead);
}
