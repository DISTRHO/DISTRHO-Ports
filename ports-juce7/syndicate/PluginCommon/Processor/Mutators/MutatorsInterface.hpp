#pragma once

#include "DataModelInterface.hpp"

namespace ModelInterface {
    bool setSplitType(StateManager& manager, SPLIT_TYPE splitType, HostConfiguration config);
    SPLIT_TYPE getSplitType(StateManager& manager);

    bool replacePlugin(StateManager& manager, std::shared_ptr<juce::AudioPluginInstance> plugin, int chainNumber, int positionInChain);
    bool removeSlot(StateManager& manager, int chainNumber, int positionInChain);
    bool insertGainStage(StateManager& manager, int chainNumber, int positionInChain);

    std::shared_ptr<juce::AudioPluginInstance> getPlugin(StateManager& manager, int chainNumber, int positionInChain);

    bool setGainLinear(StateManager& manager, int chainNumber, int positionInChain, float gain);
    bool setPan(StateManager& manager, int chainNumber, int positionInChain, float pan);
    std::tuple<float, float> getGainLinearAndPan(StateManager& manager, int chainNumber, int positionInChain);
    float getGainStageOutputAmplitude(StateManager& manager, int chainNumber, int positionInChain, int channelNumber);

    PluginModulationConfig getPluginModulationConfig(StateManager& manager, int chainNumber, int positionInChain);
    void setPluginModulationIsActive(StateManager& manager, int chainNumber, int positionInChain, bool val);
    void setModulationTarget(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, juce::String targetName);
    void removeModulationTarget(StateManager& manager, int chainNumber, int positionInChain, int targetNumber);
    void addModulationSourceToTarget(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, ModulationSourceDefinition source);
    void removeModulationSourceFromTarget(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, ModulationSourceDefinition source);
    void setModulationTargetValue(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, float val);
    void setModulationSourceValue(StateManager& manager, int chainNumber, int positionInChain, int targetNumber, int sourceNumber, float val);

    void setSlotBypass(StateManager& manager, int chainNumber, int positionInChain, bool isBypassed);
    bool getSlotBypass(StateManager& manager, int chainNumber, int positionInChain);

    void setChainBypass(StateManager& manager, int chainNumber, bool val);
    void setChainMute(StateManager& manager, int chainNumber, bool val);
    void setChainSolo(StateManager& manager, int chainNumber, bool val);
    bool getChainBypass(StateManager& manager, int chainNumber);
    bool getChainMute(StateManager& manager, int chainNumber);
    bool getChainSolo(StateManager& manager, int chainNumber);

    void moveSlot(StateManager& manager, int fromChainNumber, int fromSlotNumber, int toChainNumber, int toSlotNumber);
    void copySlot(StateManager& manager,
                  std::function<void()> onSuccess,
                  juce::AudioPluginFormatManager& formatManager,
                  int fromChainNumber,
                  int fromSlotNumber,
                  int toChainNumber,
                  int toSlotNumber);

    void moveChain(StateManager& manager, int fromChainNumber, int toChainNumber);
    void copyChain(StateManager& manager,
                   std::function<void()> onSuccess,
                   juce::AudioPluginFormatManager& formatManager,
                   int fromChainNumber,
                   int toChainNumber);

    size_t getNumChains(StateManager& manager);

    bool addParallelChain(StateManager& manager);
    bool removeParallelChain(StateManager& manager, int chainNumber);

    void addCrossoverBand(StateManager& manager);
    bool removeCrossoverBand(StateManager& manager, int bandNumber);
    bool setCrossoverFrequency(StateManager& manager, size_t index, float val);
    float getCrossoverFrequency(StateManager& manager, size_t index);

    bool setChainCustomName(StateManager& manager, int chainNumber, const juce::String& name);
    juce::String getChainCustomName(StateManager& manager, int chainNumber);

    std::pair<std::array<float, FFTProvider::NUM_OUTPUTS>, float> getFFTOutputs(StateManager& manager);

    std::shared_ptr<PluginEditorBounds> getPluginEditorBounds(StateManager& manager, int chainNumber, int positionInChain);

    void forEachChain(StateManager& manager, std::function<void(int, std::shared_ptr<PluginChain>)> callback);
    void forEachCrossover(StateManager& manager, std::function<void(float)> callback);

    void writeSplitterToXml(StateManager& manager, juce::XmlElement* element);
    void restoreSplitterFromXml(
        StateManager& manager, juce::XmlElement* element,
        std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
        std::function<void(int)> latencyChangeCallback,
        HostConfiguration config,
        const PluginConfigurator& pluginConfigurator,
        juce::Array<juce::PluginDescription> availableTypes,
        std::function<void(juce::String)> onErrorCallback);

    void createDefaultSources(StateManager& manager);
    void addLfo(StateManager& manager);
    void addEnvelope(StateManager& manager);
    void addRandom(StateManager& manager);
    void removeModulationSource(StateManager& manager, ModulationSourceDefinition definition);

    void forEachLfo(StateManager& manager, std::function<void(int)> callback);
    void forEachEnvelope(StateManager& manager, std::function<void(int)> callback);
    void forEachRandom(StateManager& manager, std::function<void(int)> callback);

    // LFOs
    void setLfoTempoSyncSwitch(StateManager& manager, int lfoIndex, bool val);
    void setLfoInvertSwitch(StateManager& manager, int lfoIndex, bool val);
    void setLfoOutputMode(StateManager& manager, int lfoIndex, int val);
    void setLfoWave(StateManager& manager, int lfoIndex, int val);
    void setLfoTempoNumer(StateManager& manager, int lfoIndex, int val);
    void setLfoTempoDenom (StateManager& manager, int lfoIndex, int val);
    void setLfoFreq(StateManager& manager, int lfoIndex, double val);
    void setLfoDepth(StateManager& manager, int lfoIndex, double val);
    void setLfoManualPhase(StateManager& manager, int lfoIndex, double val);
    void addSourceToLFOFreq(StateManager& manager, int lfoIndex, ModulationSourceDefinition source);
    void removeSourceFromLFOFreq(StateManager& manager, int lfoIndex, ModulationSourceDefinition source);
    void setLFOFreqModulationAmount(StateManager& manager, int lfoIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFOFreqModulationSources(StateManager& manager, int lfoIndex);
    void addSourceToLFODepth(StateManager& manager, int lfoIndex, ModulationSourceDefinition source);
    void removeSourceFromLFODepth(StateManager& manager, int lfoIndex, ModulationSourceDefinition source);
    void setLFODepthModulationAmount(StateManager& manager, int lfoIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFODepthModulationSources(StateManager& manager, int lfoIndex);
    void addSourceToLFOPhase(StateManager& manager, int lfoIndex, ModulationSourceDefinition source);
    void removeSourceFromLFOPhase(StateManager& manager, int lfoIndex, ModulationSourceDefinition source);
    void setLFOPhaseModulationAmount(StateManager& manager, int lfoIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFOPhaseModulationSources(StateManager& manager, int lfoIndex);

    bool getLfoTempoSyncSwitch(StateManager& manager, int lfoIndex);
    bool getLfoInvertSwitch(StateManager& manager, int lfoIndex);
    int getLfoOutputMode(StateManager& manager, int lfoIndex);
    int getLfoWave(StateManager& manager, int lfoIndex);
    double getLfoTempoNumer(StateManager& manager, int lfoIndex);
    double getLfoTempoDenom(StateManager& manager, int lfoIndex);
    double getLfoFreq(StateManager& manager, int lfoIndex);
    double getLFOModulatedFreqValue(StateManager& manager, int lfoIndex);
    double getLfoDepth(StateManager& manager, int lfoIndex);
    double getLFOModulatedDepthValue(StateManager& manager, int lfoIndex);
    double getLfoManualPhase(StateManager& manager, int lfoIndex);
    double getLFOModulatedPhaseValue(StateManager& manager, int lfoIndex);

    // Envelopes
    void setEnvAttackTimeMs(StateManager& manager, int envIndex, double val);
    void setEnvReleaseTimeMs(StateManager& manager, int envIndex, double val);
    void setEnvFilterEnabled(StateManager& manager, int envIndex, bool val);
    void setEnvFilterHz(StateManager& manager, int envIndex, double lowCut, double highCut);
    void setEnvAmount(StateManager& manager, int envIndex, float val);
    void setEnvUseSidechainInput(StateManager& manager, int envIndex, bool val);

    double getEnvAttackTimeMs(StateManager& manager, int envIndex);
    double getEnvReleaseTimeMs(StateManager& manager, int envIndex);
    bool getEnvFilterEnabled(StateManager& manager, int envIndex);
    double getEnvLowCutHz(StateManager& manager, int envIndex);
    double getEnvHighCutHz(StateManager& manager, int envIndex);
    float getEnvAmount(StateManager& manager, int envIndex);
    bool getEnvUseSidechainInput(StateManager& manager, int envIndex);
    double getEnvLastOutput(StateManager& manager, int envIndex);

    // Random
    void setRandomOutputMode(StateManager& manager, int randomIndex, int val);
    void setRandomFreq(StateManager& manager, int randomIndex, double val);
    void setRandomDepth(StateManager& manager, int randomIndex, double val);
    void addSourceToRandomFreq(StateManager& manager, int randomIndex, ModulationSourceDefinition source);
    void removeSourceFromRandomFreq(StateManager& manager, int randomIndex, ModulationSourceDefinition source);
    void setRandomFreqModulationAmount(StateManager& manager, int randomIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getRandomFreqModulationSources(StateManager& manager, int randomIndex);
    void addSourceToRandomDepth(StateManager& manager, int randomIndex, ModulationSourceDefinition source);
    void removeSourceFromRandomDepth(StateManager& manager, int randomIndex, ModulationSourceDefinition source);
    void setRandomDepthModulationAmount(StateManager& manager, int randomIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getRandomDepthModulationSources(StateManager& manager, int randomIndex);

    int getRandomOutputMode(StateManager& manager, int randomIndex);
    double getRandomFreq(StateManager& manager, int randomIndex);
    double getRandomModulatedFreqValue(StateManager& manager, int randomIndex);
    double getRandomDepth(StateManager& manager, int randomIndex);
    double getRandomModulatedDepthValue(StateManager& manager, int randomIndex);
    double getRandomLastOutput(StateManager& manager, int randomIndex);

    void resetAllState(StateManager& manager,
                       HostConfiguration config,
                       std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                       std::function<void(int)> latencyChangeCallback);

    void writeSourcesToXml(StateManager& manager, juce::XmlElement* element);
    void restoreSourcesFromXml(StateManager& manager, juce::XmlElement* element, HostConfiguration config);

    void undo(StateManager& manager, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout);
    void redo(StateManager& manager, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout);

    std::optional<juce::String> getUndoOperation(const StateManager& manager);
    std::optional<juce::String> getRedoOperation(const StateManager& manager);
}