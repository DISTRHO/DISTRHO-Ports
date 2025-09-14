#pragma once

#include <JuceHeader.h>

#include "ChainMutators.hpp"
#include "PluginChain.hpp"
#include "FFTProvider.hpp"
#include "CrossoverState.hpp"
#include "CrossoverMutators.hpp"
#include "CrossoverProcessors.hpp"

/**
 * Stores a plugin chain and any associated data.
 */
struct PluginChainWrapper {
    PluginChainWrapper(std::shared_ptr<PluginChain> newChain, bool newIsSoloed)
            : chain(newChain), isSoloed(newIsSoloed) {}

    std::shared_ptr<PluginChain> chain;
    bool isSoloed;
};

/**
 * Base class which provides the audio splitting functionality.
 *
 * Each derived class contains one or more plugin chains (one for each split).
 *
 * A splitter may contain more chains than it can actually use if they have been carried over from
 * a previous splitter that could handle more. In this case its processBlock will just ignore the
 * extra chains.
 */
class PluginSplitter {
public:
    std::vector<PluginChainWrapper> chains;
    size_t numChainsSoloed;
    HostConfiguration config;
    std::function<float(int, MODULATION_TYPE)> getModulationValueCallback;
    std::function<void(int)> notifyProcessorOnLatencyChange;
    bool shouldNotifyProcessorOnLatencyChange;

    PluginSplitter(int defaultNumChains,
                   HostConfiguration newConfig,
                   std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
                   std::function<void(int)> latencyChangeCallback)
                   : numChainsSoloed(0),
                     config(newConfig),
                     getModulationValueCallback(newGetModulationValueCallback),
                     notifyProcessorOnLatencyChange(latencyChangeCallback),
                     shouldNotifyProcessorOnLatencyChange(true) {

        // Set up the default number of chains
        for (int idx {0}; idx < defaultNumChains; idx++) {
            chains.emplace_back(std::make_shared<PluginChain>(getModulationValueCallback), false);
            chains[chains.size() - 1].chain->latencyListener.setSplitter(this);
        }
        onLatencyChange();
    }

    PluginSplitter(std::shared_ptr<PluginSplitter> otherSplitter, int defaultNumChains)
                   : chains(otherSplitter->chains),
                     numChainsSoloed(otherSplitter->numChainsSoloed),
                     config(otherSplitter->config),
                     getModulationValueCallback(otherSplitter->getModulationValueCallback),
                     notifyProcessorOnLatencyChange(otherSplitter->notifyProcessorOnLatencyChange),
                     shouldNotifyProcessorOnLatencyChange(true) {

        // Move the latency listeners for the existing chains to point to this splitter
        for (auto& chain : chains) {
            chain.chain->latencyListener.setSplitter(this);
        }

        // Add chains if we still need to reach the default
        while (defaultNumChains > chains.size()) {
            chains.emplace_back(std::make_shared<PluginChain>(getModulationValueCallback), false);
            chains[chains.size() - 1].chain->latencyListener.setSplitter(this);
        }
        onLatencyChange();
    }

    virtual ~PluginSplitter() = default;

    void onLatencyChange() {
        // The latency of the splitter is the latency of the slowest chain, so iterate through each
        // chain and report the highest latency
        int highestLatency {0};

        for (const PluginChainWrapper& chain : chains) {
            const int thisLatency {chain.chain->latencyListener.calculatedTotalPluginLatency};
            if (highestLatency < thisLatency) {
                highestLatency = thisLatency;
            }
        }

        // Tell each chain the latency of the slowest chain, so they can all add compensation to match
        // it
        for (PluginChainWrapper& chain : chains) {
            ChainMutators::setRequiredLatency(chain.chain, highestLatency, config);
        }

        if (shouldNotifyProcessorOnLatencyChange) {
            notifyProcessorOnLatencyChange(highestLatency);
        }
    }

    virtual PluginSplitter* clone() const = 0;

protected:
    PluginSplitter(std::vector<PluginChainWrapper> newChains,
                   HostConfiguration newConfig,
                   std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
                   std::function<void(int)> newNotifyProcessorOnLatencyChange) :
                       numChainsSoloed(0),
                       config(newConfig),
                       getModulationValueCallback(newGetModulationValueCallback),
                       notifyProcessorOnLatencyChange(newNotifyProcessorOnLatencyChange),
                       shouldNotifyProcessorOnLatencyChange(true) {
        for (auto& chain : newChains) {
            std::shared_ptr<PluginChain> newChain(chain.chain->clone());
            newChain->latencyListener.setSplitter(this);

            chains.emplace_back(newChain, chain.isSoloed);
            if (chain.isSoloed) {
                numChainsSoloed++;
            }
        }
    }
};

/**
 * Contains a single plugin graph for plugins arranged in series.
 */
class PluginSplitterSeries : public PluginSplitter {
public:
    static constexpr int DEFAULT_NUM_CHAINS {1};

    PluginSplitterSeries(HostConfiguration newConfig,
                         std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                         std::function<void(int)> latencyChangeCallback)
                         : PluginSplitter(DEFAULT_NUM_CHAINS, newConfig, getModulationValueCallback, latencyChangeCallback) {
        juce::Logger::writeToLog("Constructed PluginSplitterSeries");
    }

    PluginSplitterSeries(std::shared_ptr<PluginSplitter> otherSplitter)
                         : PluginSplitter(otherSplitter, DEFAULT_NUM_CHAINS) {
        juce::Logger::writeToLog("Converted to PluginSplitterSeries");

        // We only have one active chain in the series splitter, so it can't be muted or soloed
        ChainMutators::setChainMute(chains[0].chain, false);
    }

    PluginSplitterSeries* clone() const override {
        return new PluginSplitterSeries(chains, config, getModulationValueCallback, notifyProcessorOnLatencyChange);
    }

private:
    PluginSplitterSeries(std::vector<PluginChainWrapper> newChains,
                         HostConfiguration newConfig,
                         std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
                         std::function<void(int)> newNotifyProcessorOnLatencyChange) :
                                PluginSplitter(newChains, newConfig, newGetModulationValueCallback, newNotifyProcessorOnLatencyChange) {
    }
};

/**
 * Contains a single plugin graph for plugins arranged in parallel.
 */
class PluginSplitterParallel : public PluginSplitter {
public:
    static constexpr int DEFAULT_NUM_CHAINS {1};

    std::unique_ptr<juce::AudioBuffer<float>> inputBuffer;
    std::unique_ptr<juce::AudioBuffer<float>> outputBuffer;

    PluginSplitterParallel(HostConfiguration newConfig,
                           std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                           std::function<void(int)> latencyChangeCallback)
                           : PluginSplitter(DEFAULT_NUM_CHAINS, newConfig, getModulationValueCallback, latencyChangeCallback) {
        juce::Logger::writeToLog("Constructed PluginSplitterParallel");
    }

    PluginSplitterParallel(std::shared_ptr<PluginSplitter> otherSplitter)
                           : PluginSplitter(otherSplitter, DEFAULT_NUM_CHAINS) {
        juce::Logger::writeToLog("Converted to PluginSplitterParallel");
    }

    PluginSplitterParallel* clone() const override {
        return new PluginSplitterParallel(
            chains,
            config,
            getModulationValueCallback,
            notifyProcessorOnLatencyChange,
            *inputBuffer,
            *outputBuffer);
    }

private:
    PluginSplitterParallel(std::vector<PluginChainWrapper> newChains,
                           HostConfiguration newConfig,
                           std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
                           std::function<void(int)> newNotifyProcessorOnLatencyChange,
                           const juce::AudioBuffer<float>& newInputBuffer,
                           const juce::AudioBuffer<float>& newOutputBuffer) :
                                PluginSplitter(newChains, newConfig, newGetModulationValueCallback, newNotifyProcessorOnLatencyChange) {
        // We need to copy the buffers as well
        inputBuffer.reset(new juce::AudioBuffer<float>(newInputBuffer));
        outputBuffer.reset(new juce::AudioBuffer<float>(newOutputBuffer));
    }
};

/**
 * Contains a single plugin graph for plugins arranged in a multiband split.
 */
class PluginSplitterMultiband : public PluginSplitter {
public:
    static constexpr int DEFAULT_NUM_CHAINS {2};

    std::shared_ptr<CrossoverState> crossover;
    FFTProvider fftProvider;

    PluginSplitterMultiband(HostConfiguration newConfig,
                            std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                            std::function<void(int)> latencyChangeCallback)
                            : PluginSplitter(DEFAULT_NUM_CHAINS, newConfig, getModulationValueCallback, latencyChangeCallback),
                              crossover(createDefaultCrossoverState(config)) {
        juce::Logger::writeToLog("Constructed PluginSplitterMultiband");

        CrossoverProcessors::prepareToPlay(*crossover.get(), config.sampleRate, config.blockSize, config.layout);
    }

    PluginSplitterMultiband(std::shared_ptr<PluginSplitter> otherSplitter, std::optional<std::vector<float>> crossoverFrequencies)
                            : PluginSplitter(otherSplitter, DEFAULT_NUM_CHAINS),
                              crossover(createDefaultCrossoverState(config)) {
        juce::Logger::writeToLog("Converted to PluginSplitterMultiband");

        // Set the crossover to have the correct number of bands (this will also default the frequencies)
        while (chains.size() > CrossoverMutators::getNumBands(crossover)) {
            CrossoverMutators::addBand(crossover);
        }

        // Restore the crossover frequencies if there are previous ones
        if (crossoverFrequencies.has_value()) {
            const size_t numCrossovers {std::min(crossoverFrequencies.value().size(), CrossoverMutators::getNumBands(crossover))};
            for (int index {0}; index < numCrossovers; index++) {
                CrossoverMutators::setCrossoverFrequency(crossover, index, crossoverFrequencies.value()[index]);
            }
        }

        // Set the processors
        for (size_t bandIndex {0}; bandIndex < CrossoverMutators::getNumBands(crossover); bandIndex++) {
            std::shared_ptr<PluginChain> newChain {chains[bandIndex].chain};
            CrossoverMutators::setPluginChain(crossover, bandIndex, newChain);
            CrossoverMutators::setIsSoloed(crossover, bandIndex, chains[bandIndex].isSoloed);
        }

        CrossoverProcessors::prepareToPlay(*crossover.get(), config.sampleRate, config.blockSize, config.layout);
    }

    PluginSplitterMultiband* clone() const override {
        auto clonedSplitter = new PluginSplitterMultiband(
            chains,
            config,
            getModulationValueCallback,
            notifyProcessorOnLatencyChange,
            std::shared_ptr<CrossoverState>(crossover->clone()));

        // The crossover we cloned needs to be updated to use the new chains that were just cloned
        for (int chainIndex {0}; chainIndex < clonedSplitter->chains.size(); chainIndex++) {
            CrossoverMutators::setPluginChain(clonedSplitter->crossover, chainIndex, clonedSplitter->chains[chainIndex].chain);
        }

        // Set up the fft provider
        clonedSplitter->fftProvider.setSampleRate(config.sampleRate);
        clonedSplitter->fftProvider.setIsStereo(canDoStereoSplitTypes(config.layout));

        return clonedSplitter;
    }

private:
    PluginSplitterMultiband(std::vector<PluginChainWrapper> newChains,
                            HostConfiguration newConfig,
                            std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
                            std::function<void(int)> newNotifyProcessorOnLatencyChange,
                            std::shared_ptr<CrossoverState> newCrossover) :
                                PluginSplitter(newChains, newConfig, newGetModulationValueCallback, newNotifyProcessorOnLatencyChange),
                                crossover(newCrossover) {
    }
};

/**
 * Contains a single plugin graph for plugins arranged in a left right split.
 */
class PluginSplitterLeftRight : public PluginSplitter {
public:
    static constexpr int DEFAULT_NUM_CHAINS {2};

    std::unique_ptr<juce::AudioBuffer<float>> leftBuffer;
    std::unique_ptr<juce::AudioBuffer<float>> rightBuffer;

    PluginSplitterLeftRight(HostConfiguration newConfig,
                            std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                            std::function<void(int)> latencyChangeCallback)
                            : PluginSplitter(DEFAULT_NUM_CHAINS, newConfig, getModulationValueCallback, latencyChangeCallback) {
        juce::Logger::writeToLog("Constructed PluginSplitterLeftRight");
    }

    PluginSplitterLeftRight(std::shared_ptr<PluginSplitter> otherSplitter)
                            : PluginSplitter(otherSplitter, DEFAULT_NUM_CHAINS) {
        juce::Logger::writeToLog("Converted to PluginSplitterLeftRight");
    }

    PluginSplitterLeftRight* clone() const override {
        return new PluginSplitterLeftRight(
            chains,
            config,
            getModulationValueCallback,
            notifyProcessorOnLatencyChange,
            *leftBuffer,
            *rightBuffer);
    }

private:
    PluginSplitterLeftRight(std::vector<PluginChainWrapper> newChains,
                            HostConfiguration newConfig,
                            std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
                            std::function<void(int)> newNotifyProcessorOnLatencyChange,
                            const juce::AudioBuffer<float>& newLeftBuffer,
                            const juce::AudioBuffer<float>& newRightBuffer) :
                                PluginSplitter(newChains, newConfig, newGetModulationValueCallback, newNotifyProcessorOnLatencyChange) {
        // We need to copy the buffers as well
        leftBuffer.reset(new juce::AudioBuffer<float>(newLeftBuffer));
        rightBuffer.reset(new juce::AudioBuffer<float>(newRightBuffer));
    }
};

/**
 * Contains a single plugin graph for plugins arranged in a mid side split.
 */
class PluginSplitterMidSide : public PluginSplitter {
public:
    static constexpr int DEFAULT_NUM_CHAINS {2};

    std::unique_ptr<juce::AudioBuffer<float>> midBuffer;
    std::unique_ptr<juce::AudioBuffer<float>> sideBuffer;

    PluginSplitterMidSide(HostConfiguration newConfig,
                          std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                          std::function<void(int)> latencyChangeCallback)
                          : PluginSplitter(DEFAULT_NUM_CHAINS, newConfig, getModulationValueCallback, latencyChangeCallback) {
        juce::Logger::writeToLog("Constructed PluginSplitterMidSide");
    }

    PluginSplitterMidSide(std::shared_ptr<PluginSplitter> otherSplitter)
                          : PluginSplitter(otherSplitter, DEFAULT_NUM_CHAINS) {
        juce::Logger::writeToLog("Converted to PluginSplitterMidSide");
    }

    PluginSplitterMidSide* clone() const override {
        return new PluginSplitterMidSide(
            chains,
            config,
            getModulationValueCallback,
            notifyProcessorOnLatencyChange,
            *midBuffer,
            *sideBuffer);
    }

private:
    PluginSplitterMidSide(std::vector<PluginChainWrapper> newChains,
                          HostConfiguration newConfig,
                          std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
                          std::function<void(int)> newNotifyProcessorOnLatencyChange,
                          const juce::AudioBuffer<float>& newMidBuffer,
                          const juce::AudioBuffer<float>& newSideBuffer) :
                              PluginSplitter(newChains, newConfig, newGetModulationValueCallback, newNotifyProcessorOnLatencyChange) {
        // We need to copy the buffers as well
        midBuffer.reset(new juce::AudioBuffer<float>(newMidBuffer));
        sideBuffer.reset(new juce::AudioBuffer<float>(newSideBuffer));
    }
};
