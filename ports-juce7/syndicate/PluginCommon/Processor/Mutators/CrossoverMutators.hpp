#pragma once

#include "CrossoverState.hpp"

namespace CrossoverMutators {
    void setIsSoloed(std::shared_ptr<CrossoverState> state, size_t bandNumber, bool isSoloed);
    bool setCrossoverFrequency(std::shared_ptr<CrossoverState> state, size_t crossoverNumber, double val);
    void setPluginChain(std::shared_ptr<CrossoverState> state, size_t bandNumber, std::shared_ptr<PluginChain> chain);

    bool getIsSoloed(std::shared_ptr<CrossoverState> state, size_t bandNumber);
    double getCrossoverFrequency(std::shared_ptr<CrossoverState> state, size_t crossoverNumber);
    size_t getNumBands(std::shared_ptr<CrossoverState> state);

    void addBand(std::shared_ptr<CrossoverState> state);
    bool removeBand(std::shared_ptr<CrossoverState> state, size_t bandNumber);
}
