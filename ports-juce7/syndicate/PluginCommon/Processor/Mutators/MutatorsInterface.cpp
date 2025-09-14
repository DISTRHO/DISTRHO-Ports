#include "MutatorsInterface.hpp"

#include <assert.h>

#include "SplitterMutators.hpp"
#include "ModulationMutators.hpp"
#include "XmlConsts.hpp"
#include "XmlReader.hpp"
#include "XmlWriter.hpp"

namespace {
    std::vector<std::shared_ptr<PluginParameterModulationSource>> deleteSourceFromTargetSources(std::vector<std::shared_ptr<PluginParameterModulationSource>> sources, ModulationSourceDefinition definition) {
        bool needsToDelete {false};
        int indexToDelete {0};

        // Iterate through each configured source
        for (int sourceIndex {0}; sourceIndex < sources.size(); sourceIndex++) {
            std::shared_ptr<PluginParameterModulationSource> thisSource = sources[sourceIndex];

            if (thisSource->definition == definition) {
                // We need to come back and delete this one
                needsToDelete = true;
                indexToDelete = sourceIndex;
            } else if (thisSource->definition.type == definition.type &&
                       thisSource->definition.id > definition.id) {
                // We need to renumber this one
                thisSource->definition.id--;
            }
        }

        if (needsToDelete) {
            sources.erase(sources.begin() + indexToDelete);
        }

        return sources;
    }

    constexpr int MAX_HISTORY_SIZE = 20;

    void pushState(ModelInterface::StateManager& manager,
                   std::shared_ptr<ModelInterface::StateWrapper> state) {
        WECore::AudioSpinLock sharedLock(manager.sharedMutex);

        // Disable the latency change callback from the previous state, make sure the new one is enabled
        manager.undoHistory.back()->splitterState->splitter->shouldNotifyProcessorOnLatencyChange = false;
        state->splitterState->splitter->shouldNotifyProcessorOnLatencyChange = true;

        manager.undoHistory.push_back(state);

        while (manager.undoHistory.size() > MAX_HISTORY_SIZE) {
            manager.undoHistory.pop_front();
        }

        // If the state is updated, the redo history is no longer valid
        manager.redoHistory.clear();
    }

    /* Splitter */

    std::shared_ptr<ModelInterface::SplitterState> cloneSplitterState(const ModelInterface::StateManager& manager) {
        if (manager.undoHistory.size() == 0) {
            return nullptr;
        }

        return std::shared_ptr<ModelInterface::SplitterState>(manager.undoHistory.back()->splitterState->clone());
    }

    // Use this instead of manager.cloneSplitterState() for parameter changes
    // It will only clone the splitter state if we're not already in the middle of a change
    std::shared_ptr<ModelInterface::SplitterState> cloneSplitterStateIfNeeded(const ModelInterface::StateManager& manager, const juce::String& operation) {
        const std::optional<juce::String> previousOperation = ModelInterface::getUndoOperation(manager);

        if (manager.undoHistory.size() == 0) {
            return nullptr;
        }

        if (previousOperation.has_value() && previousOperation.value() == operation) {
            // We're already in the middle of this change, return the current state
            return manager.undoHistory.back()->splitterState;
        }

        // We're not in the middle of this change, clone the current state
        return cloneSplitterState(manager);
    }

    void pushSplitter(ModelInterface::StateManager& manager,
                      std::shared_ptr<ModelInterface::SplitterState> splitterState,
                      juce::String operation) {
        if (manager.undoHistory.size() == 0) {
            return;
        }

        if (manager.undoHistory.back()->splitterState == splitterState) {
            // This is a parameter change that we're already in the middle of, don't push a new state
            return;
        }

        auto newState = std::make_shared<ModelInterface::StateWrapper>(
            splitterState, manager.undoHistory.back()->modulationSourcesState, operation);
        pushState(manager, newState);
    }

    /* Sources */

    std::shared_ptr<ModelInterface::ModulationSourcesState> cloneSourcesState(const ModelInterface::StateManager& manager) {
        if (manager.undoHistory.size() == 0) {
            return nullptr;
        }

        return std::shared_ptr<ModelInterface::ModulationSourcesState>(manager.undoHistory.back()->modulationSourcesState->clone());
    }

    // Use this instead of manager.cloneSourcesState() for parameter changes
    // It will only clone the sources state if we're not already in the middle of a change
    std::shared_ptr<ModelInterface::ModulationSourcesState> cloneSourcesStateIfNeeded(const ModelInterface::StateManager& manager, const juce::String& operation) {
        const std::optional<juce::String> previousOperation = ModelInterface::getUndoOperation(manager);

        if (manager.undoHistory.size() == 0) {
            return nullptr;
        }

        if (previousOperation.has_value() && previousOperation.value() == operation) {
            // We're already in the middle of this change, return the current state
            return manager.undoHistory.back()->modulationSourcesState;
        }

        // We're not in the middle of this change, clone the current state
        return cloneSourcesState(manager);
    }

    void pushSources(ModelInterface::StateManager& manager,
                     std::shared_ptr<ModelInterface::ModulationSourcesState> sourcesState,
                     juce::String operation) {
        if (manager.undoHistory.size() == 0) {
            return;
        }

        if (manager.undoHistory.back()->modulationSourcesState == sourcesState) {
            // This is a parameter change that we're already in the middle of, don't push a new state
            return;
        }

        auto newState = std::make_shared<ModelInterface::StateWrapper>(
            manager.undoHistory.back()->splitterState, sourcesState, operation);
        pushState(manager, newState);
    }
}

namespace ModelInterface {
    bool setSplitType(StateManager& manager, SPLIT_TYPE splitType, HostConfiguration config) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        const SPLIT_TYPE previousSplitType = SplitterMutators::getSplitType(splitter->splitter);

        if (splitType != previousSplitType) {
            if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter->splitter)) {
                splitter->cachedcrossoverFrequencies = std::vector<float>();

                for (int index {0}; index < CrossoverMutators::getNumBands(multibandSplitter->crossover); index++) {
                    splitter->cachedcrossoverFrequencies.value().push_back(CrossoverMutators::getCrossoverFrequency(multibandSplitter->crossover, index));
                }
            }

            switch (splitType) {
                case SPLIT_TYPE::SERIES:
                    splitter->splitter.reset(new PluginSplitterSeries(splitter->splitter));
                    break;
                case SPLIT_TYPE::PARALLEL:
                    splitter->splitter.reset(new PluginSplitterParallel(splitter->splitter));
                    break;
                case SPLIT_TYPE::MULTIBAND:
                    splitter->splitter.reset(new PluginSplitterMultiband(splitter->splitter, splitter->cachedcrossoverFrequencies));
                    break;
                case SPLIT_TYPE::LEFTRIGHT:
                    if (canDoStereoSplitTypes(config.layout)) {
                        splitter->splitter.reset(new PluginSplitterLeftRight(splitter->splitter));
                    } else {
                        juce::Logger::writeToLog("SyndicateAudioProcessor::setSplitType: Attempted to use left/right split while not in 2in2out configuration");
                        assert(false);
                    }
                    break;
                case SPLIT_TYPE::MIDSIDE:
                    if (canDoStereoSplitTypes(config.layout)) {
                        splitter->splitter.reset(new PluginSplitterMidSide(splitter->splitter));
                    } else {
                        juce::Logger::writeToLog("SyndicateAudioProcessor::setSplitType: Attempted to use mid/side split while not in 2in2out configuration");
                        assert(false);
                    }
                    break;
            }

            // Make sure prepareToPlay has been called on the splitter as we don't actually know if the host
            // will call it via the PluginProcessor
            if (splitter->splitter != nullptr) {
                SplitterProcessors::prepareToPlay(*splitter->splitter.get(), config.sampleRate, config.blockSize, config.layout);
            }

            pushSplitter(manager, splitter, "change split type");
            return true;
        }

        return false;
    }

    SPLIT_TYPE getSplitType(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getSplitType(splitter.splitter);
        }

        return SPLIT_TYPE::SERIES;
    }

    bool replacePlugin(StateManager& manager, std::shared_ptr<juce::AudioPluginInstance> plugin, int chainNumber, int positionInChain) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }


        if (SplitterMutators::replacePlugin(splitter->splitter, std::move(plugin), chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, "replace plugin " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1));
            return true;
        }

        return false;
    }

    bool removeSlot(StateManager& manager, int chainNumber, int positionInChain) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        if (SplitterMutators::removeSlot(splitter->splitter, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, "remove slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1));
            return true;
        }

        return false;
    }

    bool insertGainStage(StateManager& manager, int chainNumber, int positionInChain) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        if (SplitterMutators::insertGainStage(splitter->splitter, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, "insert gain stage in chain " + juce::String(chainNumber + 1));
            return true;
        }

        return false;
    }

    std::shared_ptr<juce::AudioPluginInstance> getPlugin(StateManager& manager, int chainNumber, int positionInChain) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getPlugin(splitter.splitter, chainNumber, positionInChain);
        }

        return nullptr;
    }

    bool setGainLinear(StateManager& manager, int chainNumber, int positionInChain, float gain) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set gain in chain " + juce::String(chainNumber + 1) + " slot " + juce::String(positionInChain + 1);
        std::shared_ptr<SplitterState> splitter = cloneSplitterStateIfNeeded(manager, operation);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        if (SplitterMutators::setGainLinear(splitter->splitter, chainNumber, positionInChain, gain)) {
            pushSplitter(manager, splitter, operation);
            return true;
        }

        return false;
    }

    bool setPan(StateManager& manager, int chainNumber, int positionInChain, float pan) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set pan in chain " + juce::String(chainNumber + 1) + " slot " + juce::String(positionInChain + 1);
        std::shared_ptr<SplitterState> splitter = cloneSplitterStateIfNeeded(manager, operation);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        if (SplitterMutators::setPan(splitter->splitter, chainNumber, positionInChain, pan)) {
            pushSplitter(manager, splitter, operation);
            return true;
        }

        return false;
    }

    std::tuple<float, float> getGainLinearAndPan(StateManager& manager, int chainNumber, int positionInChain) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            const float gain {SplitterMutators::getGainLinear(splitter.splitter, chainNumber, positionInChain)};
            const float pan {SplitterMutators::getPan(splitter.splitter, chainNumber, positionInChain)};

            return std::make_tuple(gain, pan);
        }

        return std::make_tuple<float, float>(0, 0);
    }

    float getGainStageOutputAmplitude(StateManager& manager, int chainNumber, int positionInChain, int channelNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getGainStageOutputAmplitude(splitter.splitter, chainNumber, positionInChain, channelNumber);
        }

        return 0.0f;
    }

    PluginModulationConfig getPluginModulationConfig(StateManager& manager, int chainNumber, int positionInChain) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getPluginModulationConfig(splitter.splitter, chainNumber, positionInChain);
        }

        return PluginModulationConfig();
    }

    void setPluginModulationIsActive(StateManager& manager, int chainNumber, int positionInChain, bool val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        PluginModulationConfig config = SplitterMutators::getPluginModulationConfig(splitter->splitter, chainNumber, positionInChain);
        config.isActive = val;

        if (SplitterMutators::setPluginModulationConfig(splitter->splitter, config, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, "toggle modulation for slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1));
        }
    }

    void setModulationTarget(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, juce::String targetName) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        PluginModulationConfig config = SplitterMutators::getPluginModulationConfig(splitter->splitter, chainNumber, positionInChain);

        // Increase the number of configs if needed
        while (config.parameterConfigs.size() <= targetNumber) {
            config.parameterConfigs.push_back(std::make_shared<PluginParameterModulationConfig>());
        }

        config.parameterConfigs[targetNumber]->targetParameterName = targetName;

        if (SplitterMutators::setPluginModulationConfig(splitter->splitter, config, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, "set modulation target for slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1));
        }
    }

    void removeModulationTarget(StateManager& manager, int chainNumber, int positionInChain, int targetNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        PluginModulationConfig config = SplitterMutators::getPluginModulationConfig(splitter->splitter, chainNumber, positionInChain);

        if (config.parameterConfigs.size() <= targetNumber) {
            return;
        }

        config.parameterConfigs.erase(config.parameterConfigs.begin() + targetNumber);

        if (SplitterMutators::setPluginModulationConfig(splitter->splitter, config, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, "remove modulation target from slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1));
        }
    }

    void addModulationSourceToTarget(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        PluginModulationConfig config = SplitterMutators::getPluginModulationConfig(splitter->splitter, chainNumber, positionInChain);

        if (config.parameterConfigs.size() <= targetNumber) {
            return;
        }

        config.parameterConfigs[targetNumber]->sources.emplace_back(std::make_shared<PluginParameterModulationSource>(source, 0));

        if (SplitterMutators::setPluginModulationConfig(splitter->splitter, config, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, "add modulation source to slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1));
        }
    }

    void removeModulationSourceFromTarget(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        PluginModulationConfig config = SplitterMutators::getPluginModulationConfig(splitter->splitter, chainNumber, positionInChain);

        if (config.parameterConfigs.size() <= targetNumber) {
            return;
        }

        std::vector<std::shared_ptr<PluginParameterModulationSource>> updatedSources = config.parameterConfigs[targetNumber]->sources;
        for (int index {0}; index < updatedSources.size(); index++) {
            if (updatedSources[index]->definition == source) {
                updatedSources.erase(updatedSources.begin() + index);
                break;
            }
        }

        config.parameterConfigs[targetNumber]->sources = updatedSources;

        if (SplitterMutators::setPluginModulationConfig(splitter->splitter, config, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, "remove modulation source from slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1));
        }
    }

    void setModulationTargetValue(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, float val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set value for target " + juce::String(targetNumber + 1) + " in slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1);
        std::shared_ptr<SplitterState> splitter = cloneSplitterStateIfNeeded(manager, operation);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        PluginModulationConfig config = SplitterMutators::getPluginModulationConfig(splitter->splitter, chainNumber, positionInChain);

        if (config.parameterConfigs.size() <= targetNumber) {
            return;
        }

        config.parameterConfigs[targetNumber]->restValue = val;

        if (SplitterMutators::setPluginModulationConfig(splitter->splitter, config, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, operation);
        }
    }

    void setModulationSourceValue(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, int sourceNumber, float val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set amount for source " + juce::String(sourceNumber + 1) + " on target " + juce::String(targetNumber + 1) + " in slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1);
        std::shared_ptr<SplitterState> splitter = cloneSplitterStateIfNeeded(manager, operation);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        PluginModulationConfig config = SplitterMutators::getPluginModulationConfig(splitter->splitter, chainNumber, positionInChain);

        if (config.parameterConfigs.size() <= targetNumber) {
            return;
        }

        if (config.parameterConfigs[targetNumber]->sources.size() <= sourceNumber) {
            return;
        }

        config.parameterConfigs[targetNumber]->sources[sourceNumber]->modulationAmount = val;

        if (SplitterMutators::setPluginModulationConfig(splitter->splitter, config, chainNumber, positionInChain)) {
            pushSplitter(manager, splitter, operation);
        }
    }

    void setSlotBypass(StateManager& manager, int chainNumber, int positionInChain, bool isBypassed) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        if (SplitterMutators::setSlotBypass(splitter->splitter, chainNumber, positionInChain, isBypassed)) {
            pushSplitter(manager, splitter, "set bypass slot " + juce::String(positionInChain + 1) + " in chain " + juce::String(chainNumber + 1));
        }
    }

    bool getSlotBypass(StateManager& manager, int chainNumber, int positionInChain) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getSlotBypass(splitter.splitter, chainNumber, positionInChain);
        }

        return false;
    }

    void setChainBypass(StateManager& manager, int chainNumber, bool val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        if (chainNumber >= splitter->splitter->chains.size()) {
            return;
        }

        ChainMutators::setChainBypass(splitter->splitter->chains[chainNumber].chain, val);
        pushSplitter(manager, splitter, "set chain " + juce::String(chainNumber + 1) + " bypass");
    }

    void setChainMute(StateManager& manager, int chainNumber, bool val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        if (chainNumber >= splitter->splitter->chains.size()) {
            return;
        }

        ChainMutators::setChainMute(splitter->splitter->chains[chainNumber].chain, val);
        pushSplitter(manager, splitter, "set chain " + juce::String(chainNumber + 1) + " mute");
    }

    void setChainSolo(StateManager& manager, int chainNumber, bool val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        if (SplitterMutators::setChainSolo(splitter->splitter, chainNumber, val)) {
            pushSplitter(manager, splitter, "set chain " + juce::String(chainNumber + 1) + " solo");
        }
    }

    bool getChainBypass(StateManager& manager, int chainNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return splitter.splitter->chains[chainNumber].chain->isChainBypassed;
        }

        return false;
    }

    bool getChainMute(StateManager& manager, int chainNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return splitter.splitter->chains[chainNumber].chain->isChainMuted;
        }

        return false;
    }

    bool getChainSolo(StateManager& manager, int chainNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getChainSolo(splitter.splitter, chainNumber);
        }

        return false;
    }

    void moveSlot(StateManager& manager, int fromChainNumber, int fromSlotNumber, int toChainNumber, int toSlotNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        if (SplitterMutators::moveSlot(splitter->splitter, fromChainNumber, fromSlotNumber, toChainNumber, toSlotNumber)) {
            pushSplitter(manager, splitter, "move slot " + juce::String(fromSlotNumber + 1) + " in chain " + juce::String(fromChainNumber + 1) + " to slot " + juce::String(toSlotNumber + 1) + " in chain " + juce::String(toChainNumber + 1));
        }
    }

    void copySlot(StateManager& manager,
                  std::function<void()> onSuccess,
                  juce::AudioPluginFormatManager& formatManager,
                  int fromChainNumber,
                  int fromSlotNumber,
                  int toChainNumber,
                  int toSlotNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        const juce::String operation = "copy slot " + juce::String(fromSlotNumber + 1) + " in chain " + juce::String(fromChainNumber + 1) + " to slot " + juce::String(toSlotNumber + 1) + " in chain " + juce::String(toChainNumber + 1);
        auto wrappedOnSuccess = [onSuccess, &manager, splitter, operation](void) {
            pushSplitter(manager, splitter, operation);
            onSuccess();
        };

        auto insertPlugin = [splitter, wrappedOnSuccess, toChainNumber, toSlotNumber](std::shared_ptr<juce::AudioPluginInstance> sharedPlugin, juce::MemoryBlock sourceState, bool isBypassed, PluginModulationConfig sourceConfig) {
            // Hand the plugin over to the splitter
            if (SplitterMutators::insertPlugin(splitter->splitter, sharedPlugin, toChainNumber, toSlotNumber)) {
                // Apply plugin state
                sharedPlugin->setStateInformation(sourceState.getData(), sourceState.getSize());

                // Apply bypass
                SplitterMutators::setSlotBypass(splitter->splitter, toChainNumber, toSlotNumber, isBypassed);

                // Apply modulation
                SplitterMutators::setPluginModulationConfig(splitter->splitter, sourceConfig, toChainNumber, toSlotNumber);

                wrappedOnSuccess();
            } else {
                juce::Logger::writeToLog("SyndicateAudioProcessor::copySlot: Failed to insert plugin");
            }
        };

        SplitterMutators::copySlot(splitter->splitter, insertPlugin, wrappedOnSuccess, formatManager, fromChainNumber, fromSlotNumber, toChainNumber, toSlotNumber);
    }

    void moveChain(StateManager& manager, int fromChainNumber, int toChainNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        if (SplitterMutators::moveChain(splitter->splitter, fromChainNumber, toChainNumber)) {
            pushSplitter(manager, splitter, "move chain " + juce::String(fromChainNumber + 1) + " to " + juce::String(toChainNumber + 1));
        }
    }

    void copyChain(StateManager& manager,
                   std::function<void()> onSuccess,
                   juce::AudioPluginFormatManager& formatManager,
                   int fromChainNumber,
                   int toChainNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        const juce::String operation = "copy chain " + juce::String(fromChainNumber + 1) + " to " + juce::String(toChainNumber + 1);

        auto wrappedOnSuccess = [onSuccess, &manager, splitter, operation]() {
            pushSplitter(manager, splitter, operation);
            onSuccess();
        };

        SplitterMutators::copyChain(splitter->splitter, wrappedOnSuccess, formatManager, fromChainNumber, toChainNumber);
    }

    size_t getNumChains(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getNumChains(splitter.splitter);
        }

        return 0;
    }

    bool addParallelChain(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        auto parallelSplitter = std::dynamic_pointer_cast<PluginSplitterParallel>(splitter->splitter);

        if (parallelSplitter != nullptr) {
            SplitterMutators::addChain(parallelSplitter);
            pushSplitter(manager, splitter, "add chain");
            return true;
        }

        return false;
    }

    bool removeParallelChain(StateManager& manager, int chainNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        auto parallelSplitter = std::dynamic_pointer_cast<PluginSplitterParallel>(splitter->splitter);

        if (parallelSplitter != nullptr) {
            if (SplitterMutators::removeChain(parallelSplitter, chainNumber)) {
                pushSplitter(manager, splitter, "remove chain " + juce::String(chainNumber + 1));
                return true;
            }
        }

        return false;
    }

    void addCrossoverBand(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return;
        }

        auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter->splitter);

        if (multibandSplitter != nullptr) {
            SplitterMutators::addBand(multibandSplitter);
            pushSplitter(manager, splitter, "add band");
        }
    }

    bool removeCrossoverBand(StateManager& manager, int bandNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter->splitter);

        if (multibandSplitter != nullptr) {
            if (SplitterMutators::removeBand(multibandSplitter, bandNumber)) {
                pushSplitter(manager, splitter, "remove band " + juce::String(bandNumber + 1));
                return true;
            }
        }

        return false;
    }

    bool setCrossoverFrequency(StateManager& manager, size_t index, float val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set crossover frequency";
        std::shared_ptr<SplitterState> splitter = cloneSplitterStateIfNeeded(manager, operation);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter->splitter);

        if (multibandSplitter != nullptr) {
            if (index < SplitterMutators::getNumBands(multibandSplitter) - 1) {
                if (SplitterMutators::setCrossoverFrequency(multibandSplitter, index, val)) {
                    pushSplitter(manager, splitter, operation);
                    return true;
                }
            }
        }

        return false;
    }

    float getCrossoverFrequency(StateManager& manager, size_t index) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter.splitter);

        if (multibandSplitter != nullptr) {
            if (index < SplitterMutators::getNumBands(multibandSplitter) - 1) {
                return SplitterMutators::getCrossoverFrequency(multibandSplitter, index);
            }
        }

        return 0.0f;
    }

    bool setChainCustomName(StateManager& manager, int chainNumber, const juce::String& name) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<SplitterState> splitter = cloneSplitterState(manager);

        if (splitter == nullptr || splitter->splitter == nullptr) {
            return false;
        }

        if (SplitterMutators::setChainCustomName(splitter->splitter, chainNumber, name)) {
            pushSplitter(manager, splitter, "set chain " + juce::String(chainNumber + 1) + " name");
            return true;
        }

        return false;
    }

    juce::String getChainCustomName(StateManager& manager, int chainNumber) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getChainCustomName(splitter.splitter, chainNumber);
        }

        return "";
    }

    std::pair<std::array<float, FFTProvider::NUM_OUTPUTS>, float> getFFTOutputs(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter.splitter);

        if (multibandSplitter != nullptr) {
            std::array<float, FFTProvider::NUM_OUTPUTS> bins;
            const float* outputs = multibandSplitter->fftProvider.getOutputs();

            std::copy(outputs, outputs + FFTProvider::NUM_OUTPUTS, bins.begin());

            return std::make_pair(bins, multibandSplitter->fftProvider.getBinWidth());
        }

        std::array<float, FFTProvider::NUM_OUTPUTS> bins;
        std::fill(bins.begin(), bins.end(), 0.0f);

        return std::make_pair(bins, 0);
    }

    std::shared_ptr<PluginEditorBounds> getPluginEditorBounds(StateManager& manager, int chainNumber, int positionInChain) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            return SplitterMutators::getPluginEditorBounds(splitter.splitter, chainNumber, positionInChain);
        }

        return nullptr;
    }

    void forEachChain(StateManager& manager, std::function<void(int, std::shared_ptr<PluginChain>)> callback) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            for (int chainNumber {0}; chainNumber < splitter.splitter->chains.size(); chainNumber++) {
                callback(chainNumber, splitter.splitter->chains[chainNumber].chain);
            }
        }
    }

    void forEachCrossover(StateManager& manager, std::function<void(float)> callback) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter.splitter);

        if (multibandSplitter != nullptr) {
            for (size_t crossoverNumber {0}; crossoverNumber < SplitterMutators::getNumBands(multibandSplitter) - 1; crossoverNumber++) {
                callback(SplitterMutators::getCrossoverFrequency(multibandSplitter, crossoverNumber));
            }
        }
    }

    void writeSplitterToXml(StateManager& manager, juce::XmlElement* element) {
        std::scoped_lock lock(manager.mutatorsMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        XmlWriter::write(splitter.splitter, element);

        // Store the cached crossover frequencies
        if (splitter.cachedcrossoverFrequencies.has_value()) {
            juce::XmlElement* frequenciesElement = element->createNewChildElement(XML_CACHED_CROSSOVER_FREQUENCIES_STR);
            for (int index {0}; index < splitter.cachedcrossoverFrequencies.value().size(); index++) {
                frequenciesElement->setAttribute(getCachedCrossoverFreqXMLName(index), splitter.cachedcrossoverFrequencies.value()[index]);
            }
        }
    }

    void restoreSplitterFromXml(StateManager& manager,
                                juce::XmlElement* element,
                                std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                                std::function<void(int)> latencyChangeCallback,
                                HostConfiguration config,
                                const PluginConfigurator& pluginConfigurator,
                                juce::Array<juce::PluginDescription> availableTypes,
                                std::function<void(juce::String)> onErrorCallback) {
        std::scoped_lock lock(manager.mutatorsMutex);
        WECore::AudioSpinLock sharedLock(manager.sharedMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        // Restore the cached crossover frequencies first, we need to allow for them to be overwritten
        // by the later call to setSplitType() in the case that we're restoring a multiband split
        juce::XmlElement* frequenciesElement = element->getChildByName(XML_CACHED_CROSSOVER_FREQUENCIES_STR);
        if (frequenciesElement != nullptr) {
            splitter.cachedcrossoverFrequencies = std::vector<float>();
            const int numFrequencies {frequenciesElement->getNumAttributes()};
            for (int index {0}; index < numFrequencies; index++) {
                if (frequenciesElement->hasAttribute(getCachedCrossoverFreqXMLName(index))) {
                    splitter.cachedcrossoverFrequencies.value().push_back(
                        frequenciesElement->getDoubleAttribute(getCachedCrossoverFreqXMLName(index)));
                }
            }
        }

        splitter.splitter = XmlReader::restoreSplitterFromXml(
            element,
            getModulationValueCallback,
            latencyChangeCallback,
            config,
            pluginConfigurator,
            availableTypes,
            onErrorCallback);

        // Make sure prepareToPlay has been called on the splitter as we don't actually know if the host
        // will call it via the PluginProcessor
        if (splitter.splitter != nullptr) {
            SplitterProcessors::prepareToPlay(*splitter.splitter.get(), config.sampleRate, config.blockSize, config.layout);
        }
    }

    void createDefaultSources(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        WECore::AudioSpinLock sharedLock(manager.sharedMutex);

        // No undo/redo needed here
        ModulationMutators::addLfo(manager.getSourcesStateUnsafe());
        ModulationMutators::addEnvelope(manager.getSourcesStateUnsafe());
        ModulationMutators::addRandom(manager.getSourcesStateUnsafe());
    }

    void addLfo(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        ModulationMutators::addLfo(sources);
        pushSources(manager, sources, "add LFO");
    }

    void addEnvelope(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        ModulationMutators::addEnvelope(sources);
        pushSources(manager, sources, "add ENV");
    }

    void addRandom(StateManager& manager) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        ModulationMutators::addRandom(sources);
        pushSources(manager, sources, "add RND");
    }

    void removeModulationSource(StateManager& manager, ModulationSourceDefinition definition) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<StateWrapper> newState = std::make_shared<StateWrapper>(
            cloneSplitterState(manager), cloneSourcesState(manager), "remove modulation source");

        if (newState->splitterState == nullptr ||
            newState->splitterState->splitter == nullptr ||
            newState->modulationSourcesState == nullptr) {
            return;
        }

        if (!ModulationMutators::removeModulationSource(*newState->modulationSourcesState, definition)) {
            return;
        }

        // Iterate through each plugin, remove the source if it has been assigned and renumber ones that
        // are numbered higher
        for (PluginChainWrapper& chain : newState->splitterState->splitter->chains) {
            for (int slotIndex {0}; slotIndex < ChainMutators::getNumSlots(chain.chain); slotIndex++) {
                PluginModulationConfig thisPluginConfig = ChainMutators::getPluginModulationConfig(chain.chain, slotIndex);

                // Iterate through each configured parameter
                for (std::shared_ptr<PluginParameterModulationConfig> parameterConfig : thisPluginConfig.parameterConfigs) {
                    parameterConfig->sources = deleteSourceFromTargetSources(parameterConfig->sources, definition);
                }

                ChainMutators::setPluginModulationConfig(chain.chain, thisPluginConfig, slotIndex);
            }
        }

        pushState(manager, newState);
    }

    void forEachLfo(StateManager& manager, std::function<void(int)> callback) {
        std::scoped_lock lock(manager.mutatorsMutex);
        ModulationSourcesState& state = *manager.getSourcesStateUnsafe();

        for (int lfoIndex {0}; lfoIndex < state.lfos.size(); lfoIndex++) {
            callback(lfoIndex + 1);
        }
    }

    void forEachEnvelope(StateManager& manager, std::function<void(int)> callback) {
        std::scoped_lock lock(manager.mutatorsMutex);
        ModulationSourcesState& state = *manager.getSourcesStateUnsafe();

        for (int envIndex {0}; envIndex < state.envelopes.size(); envIndex++) {
            callback(envIndex + 1);
        }
    }

    void forEachRandom(StateManager& manager, std::function<void(int)> callback) {
        std::scoped_lock lock(manager.mutatorsMutex);
        ModulationSourcesState& state = *manager.getSourcesStateUnsafe();

        for (int rndIndex {0}; rndIndex < state.randomSources.size(); rndIndex++) {
            callback(rndIndex + 1);
        }
    }

    void setLfoTempoSyncSwitch(StateManager& manager, int lfoIndex, bool val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoTempoSyncSwitch(sources, lfoIndex, val)) {
            pushSources(manager, sources, "set LFO " + juce::String(lfoIndex + 1) + " tempo sync");
        }
    }

    void setLfoInvertSwitch(StateManager& manager, int lfoIndex, bool val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoInvertSwitch(sources, lfoIndex, val)) {
            pushSources(manager, sources, "set LFO " + juce::String(lfoIndex + 1) + " invert");
        }
    }

    void setLfoOutputMode(StateManager& manager, int lfoIndex, int val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoOutputMode(sources, lfoIndex, val)) {
            pushSources(manager, sources, "set LFO " + juce::String(lfoIndex + 1) + " output mode");
        }
    }

    void setLfoWave(StateManager& manager, int lfoIndex, int val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoWave(sources, lfoIndex, val)) {
            pushSources(manager, sources, "set LFO " + juce::String(lfoIndex + 1) + " wave");
        }
    }

    void setLfoTempoNumer(StateManager& manager, int lfoIndex, int val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set LFO " + juce::String(lfoIndex + 1) + " tempo numerator";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoTempoNumer(sources, lfoIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void setLfoTempoDenom(StateManager& manager, int lfoIndex, int val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set LFO " + juce::String(lfoIndex + 1) + " tempo denominator";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoTempoDenom(sources, lfoIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void setLfoFreq(StateManager& manager, int lfoIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set LFO " + juce::String(lfoIndex + 1) + " frequency";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoFreq(sources, lfoIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void setLfoDepth(StateManager& manager, int lfoIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set LFO " + juce::String(lfoIndex + 1) + " depth";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoDepth(sources, lfoIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void setLfoManualPhase(StateManager& manager, int lfoIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set LFO " + juce::String(lfoIndex + 1) + " phase";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLfoManualPhase(sources, lfoIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void addSourceToLFOFreq(StateManager& manager, int lfoIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::addSourceToLFOFreq(sources, lfoIndex, source)) {
            pushSources(manager, sources, "add modulation source to LFO " + juce::String(lfoIndex + 1) + " rate");
        }
    }

    void removeSourceFromLFOFreq(StateManager& manager, int lfoIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::removeSourceFromLFOFreq(sources, lfoIndex, source)) {
            pushSources(manager, sources, "remove modulation source from LFO " + juce::String(lfoIndex + 1) + " rate");
        }
    }

    void setLFOFreqModulationAmount(StateManager& manager, int lfoIndex, int sourceIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set LFO " + juce::String(lfoIndex + 1) + " rate modulation amount";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLFOFreqModulationAmount(sources, lfoIndex, sourceIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFOFreqModulationSources(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLFOFreqModulationSources(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    void addSourceToLFODepth(StateManager& manager, int lfoIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::addSourceToLFODepth(sources, lfoIndex, source)) {
            pushSources(manager, sources, "add modulation source to LFO " + juce::String(lfoIndex + 1) + " depth");
        }
    }

    void removeSourceFromLFODepth(StateManager& manager, int lfoIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::removeSourceFromLFODepth(sources, lfoIndex, source)) {
            pushSources(manager, sources, "remove modulation source from LFO " + juce::String(lfoIndex + 1) + " depth");
        }
    }

    void setLFODepthModulationAmount(StateManager& manager, int lfoIndex, int sourceIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set LFO " + juce::String(lfoIndex + 1) + " depth modulation amount";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLFODepthModulationAmount(sources, lfoIndex, sourceIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFODepthModulationSources(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLFODepthModulationSources(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    void addSourceToLFOPhase(StateManager& manager, int lfoIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::addSourceToLFOPhase(sources, lfoIndex, source)) {
            pushSources(manager, sources, "add modulation source to LFO " + juce::String(lfoIndex + 1) + " phase");
        }
    }

    void removeSourceFromLFOPhase(StateManager& manager, int lfoIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::removeSourceFromLFOPhase(sources, lfoIndex, source)) {
            pushSources(manager, sources, "remove modulation source from LFO " + juce::String(lfoIndex + 1) + " phase");
        }
    }

    void setLFOPhaseModulationAmount(StateManager& manager, int lfoIndex, int sourceIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set LFO " + juce::String(lfoIndex + 1) + " phase modulation amount";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setLFOPhaseModulationAmount(sources, lfoIndex, sourceIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFOPhaseModulationSources(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLFOPhaseModulationSources(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    bool getLfoTempoSyncSwitch(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoTempoSyncSwitch(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    bool getLfoInvertSwitch(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoInvertSwitch(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    int getLfoOutputMode(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoOutputMode(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    int getLfoWave(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoWave(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    double getLfoTempoNumer(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoTempoNumer(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    double getLfoTempoDenom(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoTempoDenom(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    double getLfoFreq(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoFreq(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    double getLFOModulatedFreqValue(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLFOModulatedFreqValue(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    double getLfoDepth(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoDepth(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    double getLFOModulatedDepthValue(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLFOModulatedDepthValue(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    double getLfoManualPhase(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLfoManualPhase(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    double getLFOModulatedPhaseValue(StateManager& manager, int lfoIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getLFOModulatedPhaseValue(manager.getSourcesStateUnsafe(), lfoIndex);
    }

    void setEnvAttackTimeMs(StateManager& manager, int envIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set ENV " + juce::String(envIndex + 1) + " attack time";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setEnvAttackTimeMs(sources, envIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void setEnvReleaseTimeMs(StateManager& manager, int envIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set ENV " + juce::String(envIndex + 1) + " release time";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setEnvReleaseTimeMs(sources, envIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void setEnvFilterEnabled(StateManager& manager, int envIndex, bool val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setEnvFilterEnabled(sources, envIndex, val)) {
            pushSources(manager, sources, "set ENV " + juce::String(envIndex + 1) + " filter enabled");
        }
    }

    void setEnvFilterHz(StateManager& manager, int envIndex, double lowCut, double highCut) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set ENV " + juce::String(envIndex + 1) + " filter cutoff";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setEnvFilterHz(sources, envIndex, lowCut, highCut)) {
            pushSources(manager, sources, operation);
        }
    }

    void setEnvAmount(StateManager& manager, int envIndex, float val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set ENV " + juce::String(envIndex + 1) + " amount";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setEnvAmount(sources, envIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void setEnvUseSidechainInput(StateManager& manager, int envIndex, bool val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setEnvUseSidechainInput(sources, envIndex, val)) {
            pushSources(manager, sources, "set ENV " + juce::String(envIndex + 1) + " sidechain input");
        }
    }

    double getEnvAttackTimeMs(StateManager& manager, int envIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getEnvAttackTimeMs(manager.getSourcesStateUnsafe(), envIndex);
    }

    double getEnvReleaseTimeMs(StateManager& manager, int envIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getEnvReleaseTimeMs(manager.getSourcesStateUnsafe(), envIndex);
    }

    bool getEnvFilterEnabled(StateManager& manager, int envIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getEnvFilterEnabled(manager.getSourcesStateUnsafe(), envIndex);
    }

    double getEnvLowCutHz(StateManager& manager, int envIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getEnvLowCutHz(manager.getSourcesStateUnsafe(), envIndex);
    }

    double getEnvHighCutHz(StateManager& manager, int envIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getEnvHighCutHz(manager.getSourcesStateUnsafe(), envIndex);
    }

    float getEnvAmount(StateManager& manager, int envIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getEnvAmount(manager.getSourcesStateUnsafe(), envIndex);
    }

    bool getEnvUseSidechainInput(StateManager& manager, int envIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getEnvUseSidechainInput(manager.getSourcesStateUnsafe(), envIndex);
    }

    double getEnvLastOutput(StateManager& manager, int envIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getEnvLastOutput(manager.getSourcesStateUnsafe(), envIndex);
    }

    void setRandomOutputMode(StateManager& manager, int randomIndex, int val) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setRandomOutputMode(sources, randomIndex, val)) {
            pushSources(manager, sources, "set RND " + juce::String(randomIndex + 1) + " output mode");
        }
    }

    void setRandomFreq(StateManager& manager, int randomIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set RND " + juce::String(randomIndex + 1) + " frequency";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setRandomFreq(sources, randomIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void setRandomDepth(StateManager& manager, int randomIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set RND " + juce::String(randomIndex + 1) + " depth";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setRandomDepth(sources, randomIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    void addSourceToRandomFreq(StateManager& manager, int randomIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::addSourceToRandomFreq(sources, randomIndex, source)) {
            pushSources(manager, sources, "add modulation source to RND " + juce::String(randomIndex + 1) + " frequency");
        }
    }

    void removeSourceFromRandomFreq(StateManager& manager, int randomIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::removeSourceFromRandomFreq(sources, randomIndex, source)) {
            pushSources(manager, sources, "remove modulation source from RND " + juce::String(randomIndex + 1) + " frequency");
        }
    }

    void setRandomFreqModulationAmount(StateManager& manager, int randomIndex, int sourceIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set RND " + juce::String(randomIndex + 1) + " frequency modulation amount";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setRandomFreqModulationAmount(sources, randomIndex, sourceIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    std::vector<std::shared_ptr<PluginParameterModulationSource>> getRandomFreqModulationSources(StateManager& manager, int randomIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getRandomFreqModulationSources(manager.getSourcesStateUnsafe(), randomIndex);
    }

    void addSourceToRandomDepth(StateManager& manager, int randomIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::addSourceToRandomDepth(sources, randomIndex, source)) {
            pushSources(manager, sources, "add modulation source to RND " + juce::String(randomIndex + 1) + " depth");
        }
    }

    void removeSourceFromRandomDepth(StateManager& manager, int randomIndex, ModulationSourceDefinition source) {
        std::scoped_lock lock(manager.mutatorsMutex);
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesState(manager);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::removeSourceFromRandomDepth(sources, randomIndex, source)) {
            pushSources(manager, sources, "remove modulation source from RND " + juce::String(randomIndex + 1) + " depth");
        }
    }

    void setRandomDepthModulationAmount(StateManager& manager, int randomIndex, int sourceIndex, double val) {
        std::scoped_lock lock(manager.mutatorsMutex);

        const juce::String operation = "set RND " + juce::String(randomIndex + 1) + " depth modulation amount";
        std::shared_ptr<ModulationSourcesState> sources = cloneSourcesStateIfNeeded(manager, operation);

        if (sources == nullptr) {
            return;
        }

        if (ModulationMutators::setRandomDepthModulationAmount(sources, randomIndex, sourceIndex, val)) {
            pushSources(manager, sources, operation);
        }
    }

    std::vector<std::shared_ptr<PluginParameterModulationSource>> getRandomDepthModulationSources(StateManager& manager, int randomIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getRandomDepthModulationSources(manager.getSourcesStateUnsafe(), randomIndex);
    }

    int getRandomOutputMode(StateManager& manager, int randomIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getRandomOutputMode(manager.getSourcesStateUnsafe(), randomIndex);
    }

    double getRandomFreq(StateManager& manager, int randomIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getRandomFreq(manager.getSourcesStateUnsafe(), randomIndex);
    }

    double getRandomModulatedFreqValue(StateManager& manager, int randomIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getRandomModulatedFreqValue(manager.getSourcesStateUnsafe(), randomIndex);
    }

    double getRandomDepth(StateManager& manager, int randomIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getRandomDepth(manager.getSourcesStateUnsafe(), randomIndex);
    }

    double getRandomModulatedDepthValue(StateManager& manager, int randomIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getRandomModulatedDepthValue(manager.getSourcesStateUnsafe(), randomIndex);
    }

    double getRandomLastOutput(StateManager& manager, int randomIndex) {
        std::scoped_lock lock(manager.mutatorsMutex);
        return ModulationMutators::getRandomLastOutput(manager.getSourcesStateUnsafe(), randomIndex);
    }

    void resetAllState(StateManager& manager,
                       HostConfiguration config,
                       std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                       std::function<void(int)> latencyChangeCallback) {
        std::scoped_lock lock(manager.mutatorsMutex);

        auto splitterState = std::make_shared<SplitterState>(config, getModulationValueCallback, latencyChangeCallback);
        auto sourcesState = std::make_shared<ModulationSourcesState>(getModulationValueCallback);

        pushState(manager, std::make_shared<StateWrapper>(splitterState, sourcesState, "reset all"));
    }

    void writeSourcesToXml(StateManager& manager, juce::XmlElement* element) {
        std::scoped_lock lock(manager.mutatorsMutex);
        XmlWriter::write(*manager.getSourcesStateUnsafe(), element);
    }

    void restoreSourcesFromXml(StateManager& manager, juce::XmlElement* element, HostConfiguration config) {
        std::scoped_lock lock(manager.mutatorsMutex);
        WECore::AudioSpinLock sharedLock(manager.sharedMutex);

        XmlReader::restoreModulationSourcesFromXml(*manager.getSourcesStateUnsafe(), element, config);
    }

    void undo(StateManager& manager, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout) {
        std::scoped_lock lock(manager.mutatorsMutex);
        WECore::AudioSpinLock sharedLock(manager.sharedMutex);

        if (manager.undoHistory.size() <= 1) {
            return;
        }

        manager.redoHistory.push_back(manager.undoHistory.back());
        manager.undoHistory.pop_back();

        // Enable the latency notifications from the current state
        manager.undoHistory.back()->splitterState->splitter->shouldNotifyProcessorOnLatencyChange = true;
        manager.redoHistory.back()->splitterState->splitter->shouldNotifyProcessorOnLatencyChange = false;

        SplitterProcessors::prepareToPlay(*(manager.undoHistory.back()->splitterState->splitter), sampleRate, samplesPerBlock, layout);
    }

    void redo(StateManager& manager, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout) {
        std::scoped_lock lock(manager.mutatorsMutex);
        WECore::AudioSpinLock sharedLock(manager.sharedMutex);

        if (manager.redoHistory.size() == 0) {
            return;
        }

        manager.undoHistory.push_back(manager.redoHistory.back());
        manager.redoHistory.pop_back();

        // Enable the latency notifications from the current state
        manager.undoHistory.back()->splitterState->splitter->shouldNotifyProcessorOnLatencyChange = true;
        if (manager.redoHistory.size() > 0) {
            manager.redoHistory.back()->splitterState->splitter->shouldNotifyProcessorOnLatencyChange = false;
        }

        SplitterProcessors::prepareToPlay(*(manager.undoHistory.back()->splitterState->splitter), sampleRate, samplesPerBlock, layout);
    }

    std::optional<juce::String> getUndoOperation(const StateManager& manager) {
        if (manager.undoHistory.size() < 2) {
            return {};
        }

        return manager.undoHistory.back()->operation;
    }

    std::optional<juce::String> getRedoOperation(const StateManager& manager) {
        if (manager.redoHistory.size() < 1) {
            return {};
        }

        return manager.redoHistory.back()->operation;
    }
}
