#pragma once

#include "DataModelInterface.hpp"

namespace ModulationMutators {
    // LFOs
    void addLfo(std::shared_ptr<ModelInterface::ModulationSourcesState> sources);
    bool setLfoTempoSyncSwitch(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, bool val);
    bool setLfoInvertSwitch(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, bool val);
    bool setLfoOutputMode(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, int val);
    bool setLfoWave(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, int val);
    bool setLfoTempoNumer(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, int val);
    bool setLfoTempoDenom(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, int val);
    bool setLfoFreq(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, double val);
    bool setLfoDepth(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, double val);
    bool setLfoManualPhase(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, double val);
    bool addSourceToLFOFreq(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, ModulationSourceDefinition source);
    bool removeSourceFromLFOFreq(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, ModulationSourceDefinition source);
    bool setLFOFreqModulationAmount(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFOFreqModulationSources(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    bool addSourceToLFODepth(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, ModulationSourceDefinition source);
    bool removeSourceFromLFODepth(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, ModulationSourceDefinition source);
    bool setLFODepthModulationAmount(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFODepthModulationSources(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    bool addSourceToLFOPhase(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, ModulationSourceDefinition source);
    bool removeSourceFromLFOPhase(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, ModulationSourceDefinition source);
    bool setLFOPhaseModulationAmount(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getLFOPhaseModulationSources(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);

    bool getLfoTempoSyncSwitch(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    bool getLfoInvertSwitch(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    int getLfoOutputMode(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    int getLfoWave(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    double getLfoTempoNumer(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    double getLfoTempoDenom(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    double getLfoFreq(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    double getLFOModulatedFreqValue(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    double getLfoDepth(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    double getLFOModulatedDepthValue(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    double getLfoManualPhase(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);
    double getLFOModulatedPhaseValue(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int lfoIndex);

    // Envelopes
    void addEnvelope(std::shared_ptr<ModelInterface::ModulationSourcesState> sources);
    bool setEnvAttackTimeMs(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex, double val);
    bool setEnvReleaseTimeMs(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex, double val);
    bool setEnvFilterEnabled(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex, bool val);
    bool setEnvFilterHz(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex, double lowCut, double highCut);
    bool setEnvAmount(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex, float val);
    bool setEnvUseSidechainInput(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex, bool val);

    double getEnvAttackTimeMs(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex);
    double getEnvReleaseTimeMs(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex);
    bool getEnvFilterEnabled(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex);
    double getEnvLowCutHz(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex);
    double getEnvHighCutHz(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex);
    float getEnvAmount(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex);
    bool getEnvUseSidechainInput(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex);
    double getEnvLastOutput(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int envIndex);

    // Random
    void addRandom(std::shared_ptr<ModelInterface::ModulationSourcesState> sources);
    bool setRandomOutputMode(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, int val);
    bool setRandomFreq(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, double val);
    bool setRandomDepth(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, double val);
    bool addSourceToRandomFreq(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, ModulationSourceDefinition source);
    bool removeSourceFromRandomFreq(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, ModulationSourceDefinition source);
    bool setRandomFreqModulationAmount(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getRandomFreqModulationSources(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex);
    bool addSourceToRandomDepth(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, ModulationSourceDefinition source);
    bool removeSourceFromRandomDepth(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, ModulationSourceDefinition source);
    bool setRandomDepthModulationAmount(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex, int sourceIndex, double val);
    std::vector<std::shared_ptr<PluginParameterModulationSource>> getRandomDepthModulationSources(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex);

    int getRandomOutputMode(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex);
    double getRandomFreq(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex);
    double getRandomModulatedFreqValue(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex);
    double getRandomDepth(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex);
    double getRandomModulatedDepthValue(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex);
    double getRandomLastOutput(std::shared_ptr<ModelInterface::ModulationSourcesState> sources, int randomIndex);

    bool removeModulationSource(ModelInterface::ModulationSourcesState& state, ModulationSourceDefinition definition);
}
