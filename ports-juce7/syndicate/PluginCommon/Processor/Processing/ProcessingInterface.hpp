#pragma once

#include "DataModelInterface.hpp"

namespace ModelInterface {
    void prepareToPlay(StateManager& manager, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout);
    void releaseResources(StateManager& manager);
    void reset(StateManager& manager);
    void processBlock(StateManager& manager, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, juce::AudioPlayHead* newPlayHead, juce::AudioPlayHead::CurrentPositionInfo tempoInfo);

    // Do not call from anything outside the model - they assume the locks are already held
    double getLfoModulationValue(StateManager& manager, int lfoNumber);
    double getEnvelopeModulationValue(StateManager& manager, int envelopeNumber);
    double getRandomModulationValue(StateManager& manager, int randomNumber);
}