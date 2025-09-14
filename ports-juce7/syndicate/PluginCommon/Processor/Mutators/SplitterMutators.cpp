#include "SplitterMutators.hpp"
#include "ChainMutators.hpp"
#include "ChainProcessors.hpp"
#include "MONSTRFilters/MONSTRParameters.h"

namespace {
    void copyNextSlot(std::shared_ptr<PluginSplitter> splitter, std::function<void()> onSuccess, juce::AudioPluginFormatManager& formatManager, int fromChainNumber, int toChainNumber, int slotIndex) {
        const int numSlotsToCopy = splitter->chains[fromChainNumber].chain->chain.size();
        if (slotIndex >= numSlotsToCopy) {
            // We're done
            onSuccess();
            return;
        }

        auto next = [splitter, onSuccess, &formatManager, fromChainNumber, toChainNumber, slotIndex]() {
            copyNextSlot(splitter, onSuccess, formatManager, fromChainNumber, toChainNumber, slotIndex + 1);
        };

        auto insertThisPlugin = [splitter, toChainNumber, slotIndex, next](std::shared_ptr<juce::AudioPluginInstance> sharedPlugin, juce::MemoryBlock sourceState, bool isBypassed, PluginModulationConfig sourceConfig) {
            // Hand the plugin over to the splitter
            if (SplitterMutators::insertPlugin(splitter, sharedPlugin, toChainNumber, slotIndex)) {
                // Apply plugin state
                sharedPlugin->setStateInformation(sourceState.getData(), sourceState.getSize());

                // Apply bypass
                SplitterMutators::setSlotBypass(splitter, toChainNumber, slotIndex, isBypassed);

                // Apply modulation
                SplitterMutators::setPluginModulationConfig(splitter, sourceConfig, toChainNumber, slotIndex);

            } else {
                juce::Logger::writeToLog("SyndicateAudioProcessor::copySlot: Failed to insert plugin");
            }

            next();
        };

        SplitterMutators::copySlot(splitter, insertThisPlugin, next, formatManager, fromChainNumber, slotIndex, toChainNumber, slotIndex);
    }
}

namespace SplitterMutators {
    bool insertPlugin(std::shared_ptr<PluginSplitter> splitter, std::shared_ptr<juce::AudioPluginInstance> plugin, int chainNumber, int positionInChain) {
        if (splitter->chains.size() > chainNumber) {
            ChainMutators::insertPlugin(splitter->chains[chainNumber].chain, plugin, positionInChain, splitter->config);
            return true;
        }

        return false;
    }

    bool replacePlugin(std::shared_ptr<PluginSplitter> splitter, std::shared_ptr<juce::AudioPluginInstance> plugin, int chainNumber, int positionInChain) {
        if (splitter->chains.size() > chainNumber) {
            ChainMutators::replacePlugin(splitter->chains[chainNumber].chain, plugin, positionInChain, splitter->config);
            return true;
        }

        return false;
    }

    bool removeSlot(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain) {
        if (splitter->chains.size() > chainNumber) {
            return ChainMutators::removeSlot(splitter->chains[chainNumber].chain, positionInChain);
        }

        return false;
    }

    bool insertGainStage(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain) {
        if (splitter->chains.size() > chainNumber) {
            ChainMutators::insertGainStage(splitter->chains[chainNumber].chain, positionInChain, splitter->config);
            return true;
        }

        return false;
    }

    std::shared_ptr<juce::AudioPluginInstance> getPlugin(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain) {
        if (splitter->chains.size() > chainNumber) {
            return ChainMutators::getPlugin(splitter->chains[chainNumber].chain, positionInChain);
        }

        return nullptr;
    }

    bool setPluginModulationConfig(std::shared_ptr<PluginSplitter> splitter, PluginModulationConfig config, int chainNumber, int positionInChain) {
        if (chainNumber < splitter->chains.size()) {
            return ChainMutators::setPluginModulationConfig(splitter->chains[chainNumber].chain, config, positionInChain);
        }

        return false;
    }

    PluginModulationConfig getPluginModulationConfig(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain) {
        if (splitter->chains.size() > chainNumber) {
            return ChainMutators::getPluginModulationConfig(splitter->chains[chainNumber].chain, positionInChain);
        }

        return PluginModulationConfig();
    }

    bool setSlotBypass(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain, bool isBypassed) {
        if (splitter->chains.size() > chainNumber) {
            return ChainMutators::setSlotBypass(splitter->chains[chainNumber].chain, positionInChain, isBypassed);
        }

        return false;
    }

    bool getSlotBypass(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain) {
        if (splitter->chains.size() > chainNumber) {
            return ChainMutators::getSlotBypass(splitter->chains[chainNumber].chain, positionInChain);
        }

        return false;
    }

    bool setChainSolo(std::shared_ptr<PluginSplitter> splitter, int chainNumber, bool val) {
        // The multiband crossover can handle soloed bands, so let it do that first
        if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
            CrossoverMutators::setIsSoloed(multibandSplitter->crossover, chainNumber, val);
        }

        if (chainNumber < splitter->chains.size()) {
            // If the new value is different to the existing one, update it and the counter
            if (val != splitter->chains[chainNumber].isSoloed) {

                splitter->chains[chainNumber].isSoloed = val;

                if (val) {
                    splitter->numChainsSoloed++;
                } else {
                    splitter->numChainsSoloed--;
                }

                return true;
            }
        }

        return false;
    }

    bool getChainSolo(std::shared_ptr<PluginSplitter> splitter, int chainNumber) {
        if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
            return CrossoverMutators::getIsSoloed(multibandSplitter->crossover, chainNumber);
        }

        if (chainNumber < splitter->chains.size()) {
            return splitter->chains[chainNumber].isSoloed;
        }

        return false;
    }

    bool moveSlot(std::shared_ptr<PluginSplitter> splitter, int fromChainNumber, int fromSlotNumber, int toChainNumber, int toSlotNumber) {
        // Copy everything we need
        std::shared_ptr<juce::AudioPluginInstance> plugin =
            SplitterMutators::getPlugin(splitter, fromChainNumber, fromSlotNumber);

         if (fromChainNumber == toChainNumber && fromSlotNumber < toSlotNumber) {
            // Decrement the target position if the original we removed was before it in the same chain
            toSlotNumber--;
        }

        if (plugin != nullptr) {
            // This is a plugin
            const bool isBypassed {SplitterMutators::getSlotBypass(splitter, fromChainNumber, fromSlotNumber)};
            PluginModulationConfig config =
                SplitterMutators::getPluginModulationConfig(splitter, fromChainNumber, fromSlotNumber);

            // Remove it from the chain
            if (SplitterMutators::removeSlot(splitter, fromChainNumber, fromSlotNumber)) {
                // Add it in the new position
                SplitterMutators::insertPlugin(splitter, plugin, toChainNumber, toSlotNumber);
                SplitterMutators::setSlotBypass(splitter, toChainNumber, toSlotNumber, isBypassed);
                SplitterMutators::setPluginModulationConfig(splitter, config, toChainNumber, toSlotNumber);
                return true;
            }
        } else {
            // This is a gain stage
            const float gain {SplitterMutators::getGainLinear(splitter, fromChainNumber, fromSlotNumber)};
            const float pan {SplitterMutators::getPan(splitter, fromChainNumber, fromSlotNumber)};

            // Remove it from the chain
            if (SplitterMutators::removeSlot(splitter, fromChainNumber, fromSlotNumber)) {
                // Add it in the new position
                SplitterMutators::insertGainStage(splitter, toChainNumber, toSlotNumber);
                SplitterMutators::setGainLinear(splitter, toChainNumber, toSlotNumber, gain);
                SplitterMutators::setPan(splitter, toChainNumber, toSlotNumber, pan);
                return true;
            }
        }

        return false;
    }

    void copySlot(std::shared_ptr<PluginSplitter> splitter,
                  std::function<void(std::shared_ptr<juce::AudioPluginInstance> sharedPlugin, juce::MemoryBlock sourceState, bool isBypassed, PluginModulationConfig sourceConfig)> insertPlugin,
                  std::function<void()> onSuccess,
                  juce::AudioPluginFormatManager& formatManager,
                  int fromChainNumber,
                  int fromSlotNumber,
                  int toChainNumber,
                  int toSlotNumber) {
        std::shared_ptr<juce::AudioPluginInstance> sourcePlugin =
                SplitterMutators::getPlugin(splitter, fromChainNumber, fromSlotNumber);

        if (sourcePlugin != nullptr) {
            // This is a plugin

            // Get the state and config before making changes that might change the plugin's position
            juce::MemoryBlock sourceState;
            sourcePlugin->getStateInformation(sourceState);

            const bool isBypassed {SplitterMutators::getSlotBypass(splitter, fromChainNumber, fromSlotNumber)};
            PluginModulationConfig sourceConfig =
                SplitterMutators::getPluginModulationConfig(splitter, fromChainNumber, fromSlotNumber);

            // Create the callback
            // Be careful about what is used in this callback - anything in local scope needs to be captured by value
            auto onPluginCreated = [splitter, insertPlugin, sourceState, isBypassed, sourceConfig, toChainNumber, toSlotNumber](std::unique_ptr<juce::AudioPluginInstance> plugin, const juce::String& error) {
                if (plugin != nullptr) {
                    // Create the shared pointer here as we need it for the window
                    std::shared_ptr<juce::AudioPluginInstance> sharedPlugin = std::move(plugin);

                    PluginConfigurator pluginConfigurator;

                    if (pluginConfigurator.configure(sharedPlugin, splitter->config)) {
                        insertPlugin(sharedPlugin, sourceState, isBypassed, sourceConfig);
                    } else {
                        juce::Logger::writeToLog("SyndicateAudioProcessor::copySlot: Failed to configure plugin");
                    }
                } else {
                    juce::Logger::writeToLog("SyndicateAudioProcessor::copySlot: Failed to load plugin: " + error);
                }
            };

            // Try to load the plugin
            formatManager.createPluginInstanceAsync(
                sourcePlugin->getPluginDescription(),
                splitter->config.sampleRate,
                splitter->config.blockSize,
                onPluginCreated);
        } else {
            // This is a gain stage
            const float gain {SplitterMutators::getGainLinear(splitter, fromChainNumber, fromSlotNumber)};
            const float pan {SplitterMutators::getPan(splitter, fromChainNumber, fromSlotNumber)};

            // Add it in the new position
            if (SplitterMutators::insertGainStage(splitter, toChainNumber, toSlotNumber)) {
                SplitterMutators::setGainLinear(splitter, toChainNumber, toSlotNumber, gain);
                SplitterMutators::setPan(splitter, toChainNumber, toSlotNumber, pan);

                onSuccess();
            }
        }
    }

    bool moveChain(std::shared_ptr<PluginSplitter> splitter, int fromChainNumber, int toChainNumber) {
        if (fromChainNumber >= splitter->chains.size()) {
            return false;
        }

        // Create a copy of the chain and remove the original
        std::shared_ptr<PluginChain> chainToMove = splitter->chains[fromChainNumber].chain;
        const bool isSoloed = splitter->chains[fromChainNumber].isSoloed;
        splitter->chains.erase(splitter->chains.begin() + fromChainNumber);

        if (toChainNumber > splitter->chains.size()) {
            // Insert at the end
            toChainNumber = splitter->chains.size();
        } else if (fromChainNumber < toChainNumber) {
            // Decrement the target position if the original we removed was before it
            toChainNumber--;
        }

        // Insert the copy at the new position
        splitter->chains.emplace(splitter->chains.begin() + toChainNumber, chainToMove, isSoloed);

        if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
            // Update the crossover
            for (int chainIndex {0}; chainIndex < splitter->chains.size(); chainIndex++) {
                CrossoverMutators::setPluginChain(multibandSplitter->crossover, chainIndex, splitter->chains[chainIndex].chain);
                CrossoverMutators::setIsSoloed(multibandSplitter->crossover, chainIndex, splitter->chains[chainIndex].isSoloed);
            }
        }

        return true;
    }

    void copyChain(std::shared_ptr<PluginSplitter> splitter, std::function<void()> onSuccess, juce::AudioPluginFormatManager& formatManager, int fromChainNumber, int toChainNumber) {
        if (fromChainNumber >= splitter->chains.size()) {
            return;
        }

        // Create a copy of the chain and insert it at the new position
        std::shared_ptr<PluginChain> chainToCopy = splitter->chains[fromChainNumber].chain;
        const bool isSoloed = splitter->chains[fromChainNumber].isSoloed;

        if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
            // Add a new band
            addBand(multibandSplitter);
        } else {
            // Add a new chain
            addChain(splitter);
        }

        moveChain(splitter, splitter->chains.size() - 1, toChainNumber);
        if (toChainNumber <= fromChainNumber) {
            // When we use fromChainNumber later, we need to account for the original chain having
            // been moved by inserting a chain below it
            fromChainNumber++;
        }

        splitter->chains[toChainNumber].chain->isChainBypassed = chainToCopy->isChainBypassed;
        splitter->chains[toChainNumber].chain->isChainMuted = chainToCopy->isChainMuted;
        splitter->chains[toChainNumber].chain->customName = chainToCopy->customName;
        splitter->chains[toChainNumber].isSoloed = isSoloed;

        copyNextSlot(splitter, onSuccess, formatManager, fromChainNumber, toChainNumber, 0);
    }

    bool setGainLinear(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain, float gain) {
        if (chainNumber < splitter->chains.size()) {
            return ChainMutators::setGainLinear(splitter->chains[chainNumber].chain, positionInChain, gain);
        }

        return false;
    }

    float getGainLinear(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain) {
        if (chainNumber < splitter->chains.size()) {
            return ChainMutators::getGainLinear(splitter->chains[chainNumber].chain, positionInChain);
        }

        return 0.0f;
    }

    float getGainStageOutputAmplitude(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain, int channelNumber) {
        if (chainNumber < splitter->chains.size()) {
            return ChainMutators::getGainStageOutputAmplitude(splitter->chains[chainNumber].chain, positionInChain, channelNumber);
        }

        return 0.0f;
    }

    bool setPan(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain, float pan) {
        if (chainNumber < splitter->chains.size()) {
            return ChainMutators::setPan(splitter->chains[chainNumber].chain, positionInChain, pan);
        }

        return false;
    }

    float getPan(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain) {
        if (chainNumber < splitter->chains.size()) {
            return ChainMutators::getPan(splitter->chains[chainNumber].chain, positionInChain);
        }

        return 0.0f;
    }

    std::shared_ptr<PluginEditorBounds> getPluginEditorBounds(std::shared_ptr<PluginSplitter> splitter, int chainNumber, int positionInChain) {
        if (chainNumber < splitter->chains.size()) {
            return ChainMutators::getPluginEditorBounds(splitter->chains[chainNumber].chain, positionInChain);
        }

        return std::make_shared<PluginEditorBounds>();
    }

    SPLIT_TYPE getSplitType(const std::shared_ptr<PluginSplitter> splitter) {
        if (std::dynamic_pointer_cast<PluginSplitterSeries>(splitter)) {
            return SPLIT_TYPE::SERIES;
        }

        if (std::dynamic_pointer_cast<PluginSplitterParallel>(splitter)) {
            return SPLIT_TYPE::PARALLEL;
        }

        if (std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
            return SPLIT_TYPE::MULTIBAND;
        }

        if (std::dynamic_pointer_cast<PluginSplitterLeftRight>(splitter)) {
            return SPLIT_TYPE::LEFTRIGHT;
        }

        if (std::dynamic_pointer_cast<PluginSplitterMidSide>(splitter)) {
            return SPLIT_TYPE::MIDSIDE;
        }

        return SPLIT_TYPE::SERIES;
    }

    void addChain(std::shared_ptr<PluginSplitter> splitter) {
        splitter->chains.emplace_back(std::make_shared<PluginChain>(splitter->getModulationValueCallback), false);

        ChainProcessors::prepareToPlay(*(splitter->chains[splitter->chains.size() - 1].chain.get()), splitter->config);
        splitter->chains[splitter->chains.size() - 1].chain->latencyListener.setSplitter(splitter.get());

        splitter->onLatencyChange();
    }

    bool removeChain(std::shared_ptr<PluginSplitterParallel> splitter, int chainNumber) {
        if (splitter->chains.size() > 1 && chainNumber < splitter->chains.size()) {
            splitter->chains[chainNumber].chain->latencyListener.removeSplitter();
            splitter->chains.erase(splitter->chains.begin() + chainNumber);

            splitter->onLatencyChange();
            return true;
        }

        return false;
    }

    void addBand(std::shared_ptr<PluginSplitterMultiband> splitter) {
        // Create the chain first, then add the band and set the processor
        splitter->chains.emplace_back(std::make_unique<PluginChain>(splitter->getModulationValueCallback), false);
        CrossoverMutators::addBand(splitter->crossover);

        std::shared_ptr<PluginChain> newChain {splitter->chains[splitter->chains.size() - 1].chain};
        ChainProcessors::prepareToPlay(*newChain, splitter->config);
        CrossoverMutators::setPluginChain(splitter->crossover, CrossoverMutators::getNumBands(splitter->crossover) - 1, newChain);

        newChain->latencyListener.setSplitter(splitter.get());
        splitter->onLatencyChange();
    }

    size_t getNumBands(std::shared_ptr<PluginSplitterMultiband> splitter) {
        return CrossoverMutators::getNumBands(splitter->crossover);
    }

    bool removeBand(std::shared_ptr<PluginSplitterMultiband> splitter, int bandNumber) {
        if (CrossoverMutators::getNumBands(splitter->crossover) > WECore::MONSTR::Parameters::NUM_BANDS.minValue &&
            CrossoverMutators::getNumBands(splitter->crossover) > bandNumber) {
            // Remove the band first, then the chain
            if (CrossoverMutators::removeBand(splitter->crossover, bandNumber)) {
                splitter->chains[bandNumber].chain->latencyListener.removeSplitter();
                splitter->chains.erase(splitter->chains.begin() + bandNumber);
                splitter->onLatencyChange();
                return true;
            }

            return false;
        }

        return false;
    }

    bool setCrossoverFrequency(std::shared_ptr<PluginSplitterMultiband> splitter, size_t index, double val) {
        return CrossoverMutators::setCrossoverFrequency(splitter->crossover, index, val);
    }

    double getCrossoverFrequency(std::shared_ptr<PluginSplitterMultiband> splitter, size_t index) {
        return CrossoverMutators::getCrossoverFrequency(splitter->crossover, index);
    }

    bool setChainCustomName(std::shared_ptr<PluginSplitter> splitter, int chainNumber, const juce::String& name) {
        if (chainNumber >= splitter->chains.size()) {
            return false;
        }

        splitter->chains[chainNumber].chain->customName = name;
        return true;
    }

    juce::String getChainCustomName(std::shared_ptr<PluginSplitter> splitter, int chainNumber) {
        if (chainNumber >= splitter->chains.size()) {
            return juce::String();
        }

        return splitter->chains[chainNumber].chain->customName;
    }
}
