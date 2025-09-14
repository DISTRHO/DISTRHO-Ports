#pragma once

#include <JuceHeader.h>
#include "PluginChain.hpp"
#include "CloneableLRFilter.hpp"

struct BandState {
    bool isSoloed;
    std::shared_ptr<PluginChain> chain;

    BandState() : isSoloed(false) {
    }
};

class CrossoverState {
public:
    // Num low/highpass filters = num bands - 1 (= num crossovers)
    std::vector<std::shared_ptr<CloneableLRFilter<float>>> lowpassFilters;
    std::vector<std::shared_ptr<CloneableLRFilter<float>>> highpassFilters;

    // Num allpass filters = num bands - 2
    std::vector<std::shared_ptr<CloneableLRFilter<float>>> allpassFilters;

    // Num buffers = num bands - 1 (= num crossovers)
    std::vector<juce::AudioBuffer<float>> buffers;

    std::vector<BandState> bands;

    HostConfiguration config;

    // We only need to implement solo at this level - chains handle bypass and mute themselves
    int numBandsSoloed;

    CrossoverState() : numBandsSoloed(0) {}

    CrossoverState* clone() const {
        auto newState = new CrossoverState();

        const int numFilterChannels {canDoStereoSplitTypes(config.layout) ? 2 : 1};

        for (auto& filter : lowpassFilters) {
            newState->lowpassFilters.emplace_back(filter->clone());
        }

        for (auto& filter : highpassFilters) {
            newState->highpassFilters.emplace_back(filter->clone());
        }

        for (auto& filter : allpassFilters) {
            newState->allpassFilters.emplace_back(filter->clone());
        }

        for (auto& buffer : buffers) {
            newState->buffers.emplace_back(buffer);
        }

        for (auto& band : bands) {
            newState->bands.emplace_back();
            newState->bands.back().isSoloed = band.isSoloed;
            newState->bands.back().chain = band.chain;
        }

        newState->config = config;

        newState->numBandsSoloed = numBandsSoloed;

        return newState;
    }
};

inline std::shared_ptr<CrossoverState> createDefaultCrossoverState(HostConfiguration newConfig) {
    auto state = std::make_shared<CrossoverState>();

    // Initialise configuration for two bands
    constexpr int DEFAULT_FREQ {1000};

    state->lowpassFilters.emplace_back(new CloneableLRFilter<float>());
    state->lowpassFilters[0]->setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    state->lowpassFilters[0]->setCutoffFrequency(DEFAULT_FREQ);

    state->highpassFilters.emplace_back(new CloneableLRFilter<float>());
    state->highpassFilters[0]->setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    state->highpassFilters[0]->setCutoffFrequency(DEFAULT_FREQ);

    state->buffers.emplace_back();

    state->bands.emplace_back();
    state->bands.emplace_back();

    state->config = newConfig;

    return state;
}
