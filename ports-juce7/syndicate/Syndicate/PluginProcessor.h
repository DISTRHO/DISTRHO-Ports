/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "CoreJUCEPlugin/CoreAudioProcessor.h"
#include "CoreJUCEPlugin/CustomParameter.h"
#include "MainLogger.h"
#include "NullLogger.hpp"
#include "PluginScanClient.h"
#include "PluginSelectorState.h"
#include "PluginParameterSelectorState.h"
#include "ParameterData.h"
#include "PluginConfigurator.hpp"
#include "ModelInterface.hpp"
#include "PresetMetadata.hpp"

class SyndicateAudioProcessorEditor;

struct MainWindowState {
    juce::Rectangle<int> bounds;
    int graphViewScrollPosition;
    std::vector<int> chainViewScrollPositions;
    int lfoButtonsScrollPosition;
    int envButtonsScrollPosition;
    int rndButtonsScrollPosition;
    std::optional<ModulationSourceDefinition> selectedModulationSource;

    MainWindowState() : bounds(0, 0, 860, 620),
                        graphViewScrollPosition(0),
                        lfoButtonsScrollPosition(0),
                        envButtonsScrollPosition(0) {}
};

//==============================================================================
/**
*/
class SyndicateAudioProcessor : public WECore::JUCEPlugin::CoreAudioProcessor
{
public:
    PluginScanClient pluginScanClient;
    PluginSelectorState pluginSelectorState; // TODO convert this to a custom parameter
    PluginParameterSelectorState pluginParameterSelectorState;
    ModelInterface::StateManager manager;
    PluginConfigurator pluginConfigurator;
    std::array<juce::String, NUM_MACROS> macroNames;
    std::array<WECore::AREnv::AREnvelopeFollowerSquareLaw, 2> meterEnvelopes;
    std::vector<juce::String> restoreErrors; // Populated during restore, displayed and cleared when the UI is opened
    juce::AudioPluginFormatManager formatManager;
    MainWindowState mainWindowState;
    PresetMetadata presetMetadata;

    //==============================================================================
    SyndicateAudioProcessor();
    ~SyndicateAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    // Public parameters
    juce::AudioParameterFloat* outputGainLog;
    juce::AudioParameterFloat* outputPan;
    std::array<juce::AudioParameterFloat*, NUM_MACROS> macros;

    void setEditor(SyndicateAudioProcessorEditor* editor) { _editor = editor; }
    void removeEditor() { _editor = nullptr; }

    // Sources
    void addLfo();
    void setLfoTempoSyncSwitch(int lfoIndex, bool val);
    void setLfoInvertSwitch(int lfoIndex, bool val);
    void setLfoOutputMode(int lfoIndex, int val);
    void setLfoWave(int lfoIndex, int val);
    void setLfoTempoNumer(int lfoIndex, int val);
    void setLfoTempoDenom (int lfoIndex, int val);
    void setLfoFreq(int lfoIndex, double val);
    void setLfoDepth(int lfoIndex, double val);
    void setLfoManualPhase(int lfoIndex, double val);

    void addEnvelope();
    void setEnvAttackTimeMs(int envIndex, double val);
    void setEnvReleaseTimeMs(int envIndex, double val);
    void setEnvFilterEnabled(int envIndex, bool val);
    void setEnvFilterHz(int envIndex, double lowCut, double highCut);
    void setEnvAmount(int envIndex, float val);
    void setEnvUseSidechainInput(int envIndex, bool val);

    void addRandomSource();
    void setRandomOutputMode(int randomIndex, int val);
    void setRandomFreq(int randomIndex, double val);
    void setRandomDepth(int randomIndex, double val);

    float getModulationValueForSource(int id, MODULATION_TYPE type);
    void removeModulationSource(ModulationSourceDefinition definition);

    void setSplitType(SPLIT_TYPE splitType);
    SPLIT_TYPE getSplitType() { return ModelInterface::getSplitType(manager); }

    void setChainBypass(int chainNumber, bool val);
    void setChainMute(int chainNumber, bool val);
    void setChainSolo(int chainNumber, bool val);
    void setChainCustomName(int chainNumber, const juce::String& name);

    void setSlotBypass(int chainNumber, int positionInChain, bool bypass);
    void setSlotGainLinear(int chainNumber, int positionInChain, float gain);
    void setSlotPan(int chainNumber, int positionInChain, float pan);

    // Modulation tray
    void setPluginModulationIsActive(int chainNumber, int pluginNumber, bool val);
    void setModulationTarget(int chainNumber, int pluginNumber, int targetNumber, juce::String targetName);
    void removeModulationTarget(int chainNumber, int pluginNumber, int targetNumber);
    void addModulationSourceToTarget(int chainNumber, int pluginNumber, int targetNumber, ModulationSourceDefinition source);
    void removeModulationSourceFromTarget(int chainNumber, int pluginNumber, int targetNumber, ModulationSourceDefinition source);
    void setModulationTargetValue(int chainNumber, int pluginNumber, int targetNumber, float val);
    void setModulationSourceValue(int chainNumber, int pluginNumber, int targetNumber, int sourceNumber, float val);

    // Parameter modulation
    void addSourceToLFOFreq(int lfoIndex, ModulationSourceDefinition source);
    void removeSourceFromLFOFreq(int lfoIndex, ModulationSourceDefinition source);
    void setLFOFreqModulationAmount(int lfoIndex, int sourceIndex, double val);

    void addSourceToLFODepth(int lfoIndex, ModulationSourceDefinition source);
    void removeSourceFromLFODepth(int lfoIndex, ModulationSourceDefinition source);
    void setLFODepthModulationAmount(int lfoIndex, int sourceIndex, double val);

    void addSourceToLFOPhase(int lfoIndex, ModulationSourceDefinition source);
    void removeSourceFromLFOPhase(int lfoIndex, ModulationSourceDefinition source);
    void setLFOPhaseModulationAmount(int lfoIndex, int sourceIndex, double val);

    void addSourceToRandomFreq(int randomIndex, ModulationSourceDefinition source);
    void removeSourceFromRandomFreq(int randomIndex, ModulationSourceDefinition source);
    void setRandomFreqModulationAmount(int randomIndex, int sourceIndex, double val);

    void addSourceToRandomDepth(int randomIndex, ModulationSourceDefinition source);
    void removeSourceFromRandomDepth(int randomIndex, ModulationSourceDefinition source);
    void setRandomDepthModulationAmount(int randomIndex, int sourceIndex, double val);

    // Parallel Split
    void addParallelChain();
    void removeParallelChain(int chainNumber);

    // Multiband Split
    void addCrossoverBand();
    void removeCrossoverBand(int bandNumber);
    void setCrossoverFrequency(size_t index, float val);

    // Plugin events
    bool onPluginSelectedByUser(std::shared_ptr<juce::AudioPluginInstance> plugin,
                                int chainNumber,
                                int pluginNumber);

    void removePlugin(int chainNumber, int pluginNumber);

    void insertGainStage(int chainNumber, int pluginNumber);

    void copySlot(int fromChainNumber, int fromSlotNumber, int toChainNumber, int toSlotNumber);

    void moveSlot(int fromChainNumber, int fromSlotNumber, int toChainNumber, int toSlotNumber);

    void moveChain(int fromChainNumber, int toChainNumber);

    void copyChain(int fromChainNumber, int toChainNumber);

    void resetAllState();

    void undo();
    void redo();

    void setPresetMetadata(const PresetMetadata& newMetadata);

    /**
     * Called by a splitter when its latency has changed, so this processor can update the latency
     * it reports back to the host.
     */
    void onLatencyChange(int newLatencySamples);

    /**
     * Override so we can disable conventional save/restore in the demo but still allow it manually using the
     * import/export buttons.
     */
    virtual void getStateInformation(juce::MemoryBlock& destData) override;
    virtual void setStateInformation(const void* data, int sizeInBytes) override;

private:
    /**
     * Provides a way for the processor to trigger UI updates, and also manages saving and restoring
     * parameter state for the splitter.
     */
    class SplitterParameters : public WECore::JUCEPlugin::CustomParameter {
    public:
        SplitterParameters() : _processor(nullptr) { }
        ~SplitterParameters() = default;

        void triggerUpdate() { _updateListener(); }

        void setProcessor(SyndicateAudioProcessor* processor) { _processor = processor; }

        void restoreFromXml(juce::XmlElement* element) override;
        void writeToXml(juce::XmlElement* element) override;

    private:
        SyndicateAudioProcessor* _processor;

        void _restoreSplitterFromXml(juce::XmlElement* element);
        void _restoreModulationSourcesFromXml(juce::XmlElement* element);
        void _restoreMacroNamesFromXml(juce::XmlElement* element);
        void _restoreMetadataFromXml(juce::XmlElement* element);
        void _restoreMainWindowStateFromXml(juce::XmlElement* element);

        void _writeSplitterToXml(juce::XmlElement* element);
        void _writeModulationSourcesToXml(juce::XmlElement* element);
        void _writeMacroNamesToXml(juce::XmlElement* element);
        void _writeMetadataToXml(juce::XmlElement* element);
        void _writeMainWindowStateToXml(juce::XmlElement* element);
    };

    std::unique_ptr<MainLogger> _fileLogger;
    NullLogger _nullLogger;
    SyndicateAudioProcessorEditor* _editor;
    double _outputGainLinear;

    SplitterParameters* _splitterParameters;


    std::vector<juce::String> _provideParamNamesForMigration() override;
    void _migrateParamValues(std::vector<float>& paramValues) override;

    void _onParameterUpdate() override;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SyndicateAudioProcessor)
};
