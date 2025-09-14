#pragma once

#include <JuceHeader.h>
#include "DataModelInterface.hpp"

namespace ModulationProcessors {
    void prepareToPlay(ModelInterface::ModulationSourcesState& state, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout);
    void releaseResources(ModelInterface::ModulationSourcesState& state);
    void reset(ModelInterface::ModulationSourcesState& state);
    void processBlock(ModelInterface::ModulationSourcesState& state, juce::AudioBuffer<float>& buffer, juce::AudioPlayHead::CurrentPositionInfo tempoInfo);

    double getLfoModulationValue(ModelInterface::ModulationSourcesState& state, int lfoNumber);
    double getEnvelopeModulationValue(ModelInterface::ModulationSourcesState& state, int envelopeNumber);
    double getRandomModulationValue(ModelInterface::ModulationSourcesState& state, int randomNumber);
}
