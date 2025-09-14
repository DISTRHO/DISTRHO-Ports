#include "CrossoverMutators.hpp"
#include "CrossoverProcessors.hpp"

namespace {
    constexpr int MAX_FREQ {20000};
}

namespace CrossoverMutators {
    void setIsSoloed(std::shared_ptr<CrossoverState> state, size_t bandNumber, bool isSoloed) {
        if (state->bands.size() > bandNumber) {
            if (state->bands[bandNumber].isSoloed != isSoloed) {
                state->bands[bandNumber].isSoloed = isSoloed;

                if (isSoloed) {
                    state->numBandsSoloed++;
                } else {
                    state->numBandsSoloed--;
                }
            }
        }
    }

    bool setCrossoverFrequency(std::shared_ptr<CrossoverState> state, size_t crossoverNumber, double val) {
        if (val > MAX_FREQ) {
            return false;
        }

        if (state->lowpassFilters.size() <= crossoverNumber) {
            return false;
        }

        state->lowpassFilters[crossoverNumber]->setCutoffFrequency(val);
        state->highpassFilters[crossoverNumber]->setCutoffFrequency(val);

        // We might also have an allpass filter to set
        if (state->allpassFilters.size() > crossoverNumber - 1) {
            state->allpassFilters[crossoverNumber - 1]->setCutoffFrequency(val);
        }

        // Make sure the crossover frequencies are still in the correct order
        for (size_t otherCrossoverIndex {0}; otherCrossoverIndex < state->lowpassFilters.size(); otherCrossoverIndex++) {
            const double otherCrossoverFrequency {getCrossoverFrequency(state, otherCrossoverIndex)};

            const bool needsCrossoverUpdate {
                // We've moved the crossover frequency of index below another one that should be
                // below it - move the other one to the new value
                (val < otherCrossoverFrequency && otherCrossoverIndex < crossoverNumber) ||

                // We've moved the crossover frequency of index above another one that should be
                // above it - move the other one to the new value
                (otherCrossoverFrequency > val && crossoverNumber > otherCrossoverIndex)
            };

            if (needsCrossoverUpdate) {
                // We've moved the crossover frequency of index below another one that should be
                // below it - move the other one to the new value
                setCrossoverFrequency(state, otherCrossoverIndex, val);
            }
        }

        return true;
    }

    void setPluginChain(std::shared_ptr<CrossoverState> state, size_t bandNumber, std::shared_ptr<PluginChain> chain) {
        if (state->bands.size() > bandNumber) {
            state->bands[bandNumber].chain = chain;
        }
    }

    bool getIsSoloed(std::shared_ptr<CrossoverState> state, size_t bandNumber) {
        if (state->bands.size() > bandNumber) {
            return state->bands[bandNumber].isSoloed;
        }

        return false;
    }

    double getCrossoverFrequency(std::shared_ptr<CrossoverState> state, size_t crossoverNumber) {
        if (state->lowpassFilters.size() > crossoverNumber) {
            return state->lowpassFilters[crossoverNumber]->getCutoffFrequency();
        }

        return 0;
    }

    size_t getNumBands(std::shared_ptr<CrossoverState> state) {
        return state->bands.size();
    }

    void addBand(std::shared_ptr<CrossoverState> state) {
        const double oldHighestCrossover {CrossoverMutators::getCrossoverFrequency(state, state->lowpassFilters.size() - 1)};

        // Add all the new state
        state->lowpassFilters.emplace_back(new CloneableLRFilter<float>());
        state->lowpassFilters[state->lowpassFilters.size() - 1]->setType(juce::dsp::LinkwitzRileyFilterType::lowpass);

        state->highpassFilters.emplace_back(new CloneableLRFilter<float>());
        state->highpassFilters[state->highpassFilters.size() - 1]->setType(juce::dsp::LinkwitzRileyFilterType::highpass);

        state->allpassFilters.emplace_back(new CloneableLRFilter<float>());
        state->allpassFilters[state->allpassFilters.size() - 1]->setType(juce::dsp::LinkwitzRileyFilterType::allpass);

        state->buffers.emplace_back();

        state->bands.emplace_back();

        // Set the crossover frequency of the new band and make room for it if needed
        if (oldHighestCrossover < MAX_FREQ) {
            // The old highest crossover frequency is below the maximum, insert the new one halfway
            // between it and the maximum
            const double topBandWidth {MAX_FREQ - oldHighestCrossover};
            const double newCrossoverFreq {oldHighestCrossover + (topBandWidth / 2)};
            setCrossoverFrequency(state, state->lowpassFilters.size() - 1, newCrossoverFreq);
        } else {
            // The old highest crossover is at the maximum, move it halfway down to the one below
            // and place the new one at the maximum
            const double thirdHighestCrossover {CrossoverMutators::getCrossoverFrequency(state, state->lowpassFilters.size() - 3)};
            const double bandWidth {MAX_FREQ - thirdHighestCrossover};
            const double adjustedCrossoverFreq {thirdHighestCrossover + (bandWidth / 2)};

            setCrossoverFrequency(state, state->lowpassFilters.size() - 2, adjustedCrossoverFreq);
            setCrossoverFrequency(state, state->lowpassFilters.size() - 1, MAX_FREQ);
        }

        CrossoverProcessors::prepareToPlay(*state.get(), state->config.sampleRate, state->config.blockSize, state->config.layout);
        CrossoverProcessors::reset(*state.get());
    }

    bool removeBand(std::shared_ptr<CrossoverState> state, size_t bandNumber) {
        const size_t numBands {state->bands.size()};
        if (numBands < 3) {
            return false;
        }

        if (bandNumber >= numBands) {
            return false;
        }

        // Remove the associated band
        if (state->bands[bandNumber].isSoloed) {
            state->numBandsSoloed--;
        }

        state->bands.erase(state->bands.begin() + bandNumber);

        // For the filters, it's easier to remove an arbitrary one and then reset them all
        // First get all the crossover frequencies that we want to keep
        std::vector<float> newCrossoverFrequencies;
        for (size_t crossoverNumber {0}; crossoverNumber < numBands - 1; crossoverNumber++) {
            if (bandNumber == 0 && crossoverNumber == 0) {
                // We're removing the first band, so we need to remove the first crossover
            } else if (crossoverNumber == bandNumber - 1) {
                // We're removing the band that this crossover is below, so we need to remove this
                // crossover
            } else {
                // Keep this crossover
                newCrossoverFrequencies.push_back(getCrossoverFrequency(state, crossoverNumber));
            }
        }

        // Now remove an arbitrary filter
        state->buffers.erase(state->buffers.end() - 1);
        state->lowpassFilters.erase(state->lowpassFilters.end() - 1);
        state->highpassFilters.erase(state->highpassFilters.end() - 1);

        // We might also have an allpass filter to delete
        if (state->allpassFilters.size() > 0) {
            state->allpassFilters.erase(state->allpassFilters.end() - 1);
        }

        // Now we can reset the filters
        for (size_t crossoverNumber {0}; crossoverNumber < newCrossoverFrequencies.size(); crossoverNumber++) {
            setCrossoverFrequency(state, crossoverNumber, newCrossoverFrequencies[crossoverNumber]);
        }

        CrossoverProcessors::reset(*state.get());

        return true;
    }
}
