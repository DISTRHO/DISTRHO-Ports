/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"

#include "PluginEditor.h"
#include "ParameterData.h"
#include "AllUtils.h"
#include "PluginUtils.h"
#include "XmlReader.hpp"
#include "XmlWriter.hpp"

namespace {
    // Modulation sources
    const char* XML_MODULATION_SOURCES_STR {"ModulationSources"};

    const char* XML_MACRO_NAMES_STR {"MacroNames"};

    const char* XML_METADATA_STR {"Metadata"};
    const char* XML_METADATA_NAME_STR {"MetadataName"};
    const char* XML_METADATA_FULLPATH_STR {"MetadataFullPath"};
    const char* XML_METADATA_AUTHOR_STR {"MetadataAuthor"};
    const char* XML_METADATA_DESCRIPTION_STR {"MetadataDescription"};

    const char* XML_MAIN_WINDOW_STATE_STR {"MainWindowState"};
    const char* XML_MAIN_WINDOW_BOUNDS_STR {"MainEditorBounds"};
    const char* XML_GRAPH_VIEW_POSITION_STR {"GraphViewScrollPosition"};
    const char* XML_CHAIN_VIEW_POSITIONS_STR {"ChainViewScrollPositions"};
    const char* XML_LFO_BUTTONS_POSITION_STR {"LfoButtonsScrollPosition"};
    const char* XML_ENV_BUTTONS_POSITION_STR {"EnvButtonsScrollPosition"};
    const char* XML_SELECTED_SOURCE_STR {"SelectedSource"};

    juce::String getMacroNameXMLName(int macroNumber) {
        juce::String retVal("MacroName_");
        retVal += juce::String(macroNumber);
        return retVal;
    }

    juce::String getChainPositionXMLName(int chainNumber) {
        juce::String retVal("Chain_");
        retVal += juce::String(chainNumber);
        return retVal;
    }

    // Window states
    const char* XML_PLUGIN_SELECTOR_STATE_STR {"pluginSelectorState"};
    const char* XML_PLUGIN_PARAMETER_SELECTOR_STATE_STR {"pluginParameterSelectorState"};
}

//==============================================================================
SyndicateAudioProcessor::SyndicateAudioProcessor() :
        WECore::JUCEPlugin::CoreAudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                                                .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                                                                .withInput("Sidechain", juce::AudioChannelSet::stereo(), true)),
        pluginScanClient(wrapperType),
        manager({getBusesLayout(), getSampleRate(), getBlockSize()},
                [&](int id, MODULATION_TYPE type) { return getModulationValueForSource(id, type); },
                [&](int newLatencySamples) { onLatencyChange(newLatencySamples); }),
        _editor(nullptr),
        _outputGainLinear(1)
{

    const Utils::Config config = Utils::LoadConfig();
    if (config.enableLogFile) {
        _fileLogger = std::make_unique<MainLogger>(JucePlugin_Name, JucePlugin_VersionString, Utils::PluginLogDirectory);
        juce::Logger::setCurrentLogger(_fileLogger.get());
    } else {
        juce::Logger::setCurrentLogger(&_nullLogger);
    }

    constexpr float PRECISION {0.01f};
    registerPrivateParameter(_splitterParameters, "SplitterParameters");

    // Register the macro parameters
    for (int index {0}; index < macros.size(); index++) {
        registerParameter(macros[index], MACRO_STRS[index], &MACRO, MACRO.defaultValue, PRECISION);
    }

    registerParameter(outputGainLog, OUTPUTGAIN_STR, &OUTPUTGAIN, OUTPUTGAIN.defaultValue, PRECISION);
    registerParameter(outputPan, OUTPUTPAN_STR, &OUTPUTPAN, OUTPUTPAN.defaultValue, PRECISION);

    // Add a default LFO and envelope
    ModelInterface::createDefaultSources(manager);

    // Make sure everything is initialised
    _splitterParameters->setProcessor(this);
    _onParameterUpdate();
    pluginScanClient.restore();

    for (int index {0}; index < macroNames.size(); index++) {
        macroNames[index] = "Macro " + juce::String(index + 1);
    }

    for (auto& env : meterEnvelopes) {
        env.setAttackTimeMs(1);
        env.setReleaseTimeMs(50);
        env.setFilterEnabled(false);
    }

    formatManager.addDefaultFormats();
}

SyndicateAudioProcessor::~SyndicateAudioProcessor()
{
    pluginScanClient.stopScan();

    // Logger must be removed before being deleted
    // (this must be the last thing we do before exiting)
    juce::Logger::setCurrentLogger(nullptr);
}

//==============================================================================
const juce::String SyndicateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SyndicateAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SyndicateAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SyndicateAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SyndicateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SyndicateAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SyndicateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SyndicateAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SyndicateAudioProcessor::getProgramName (int index)
{
    return {};
}

void SyndicateAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SyndicateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    for (auto& env : meterEnvelopes) {
        env.setSampleRate(sampleRate);
    }

    juce::Logger::writeToLog("Setting bus layout:\n" + Utils::busesLayoutToString(getBusesLayout()));
    ModelInterface::prepareToPlay(manager, sampleRate, samplesPerBlock, getBusesLayout());
}

void SyndicateAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    ModelInterface::releaseResources(manager);
}

void SyndicateAudioProcessor::reset() {
    for (auto& env : meterEnvelopes) {
        env.reset();
    }

    ModelInterface::reset(manager);
}

bool SyndicateAudioProcessor::isBusesLayoutSupported(const BusesLayout& layout) const {
    const bool inputEqualsOutput {
        layout.getMainInputChannelSet() == layout.getMainOutputChannelSet()
    };

    const bool isMonoOrStereo {
        layout.getMainInputChannelSet().size() == 1 || layout.getMainInputChannelSet().size() == 2
    };

    return inputEqualsOutput && isMonoOrStereo && !layout.getMainInputChannelSet().isDisabled();
}

void SyndicateAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Send tempo and playhead information to the LFOs
    juce::AudioPlayHead::CurrentPositionInfo mTempoInfo{};
    getPlayHead()->getCurrentPosition(mTempoInfo);

    // Pass the audio through the splitter (this is also the only safe place to pass the playhead through)
    ModelInterface::processBlock(manager, buffer, midiMessages, getPlayHead(), mTempoInfo);

    // Apply the output gain
    for (int channel {0}; channel < getMainBusNumInputChannels(); channel++)
    {
        juce::FloatVectorOperations::multiply(buffer.getWritePointer(channel),
                                              _outputGainLinear,
                                              buffer.getNumSamples());
    }

    // Apply the output pan
    if (canDoStereoSplitTypes(getBusesLayout())) {
        // Stereo input - apply balance
        Utils::processBalance(outputPan->get(), buffer);
    }

    // After processing everything, update the meter envelopes
    for (int sampleIndex {0}; sampleIndex < buffer.getNumSamples(); sampleIndex++) {
        for (int channel {0}; channel < std::min(getMainBusNumInputChannels(), static_cast<int>(meterEnvelopes.size())); channel++) {
            meterEnvelopes[channel].getNextOutput(buffer.getReadPointer(channel)[sampleIndex]);
        }
    }
}

//==============================================================================
bool SyndicateAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SyndicateAudioProcessor::createEditor()
{
    return new SyndicateAudioProcessorEditor (*this);
}

void SyndicateAudioProcessor::addLfo() {
    ModelInterface::addLfo(manager);

    if (_editor != nullptr) {
        _editor->needsModulationBarRebuild();
    }
}

void SyndicateAudioProcessor::setLfoTempoSyncSwitch(int lfoIndex, bool val) {
    ModelInterface::setLfoTempoSyncSwitch(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setLfoInvertSwitch(int lfoIndex, bool val) {
    ModelInterface::setLfoInvertSwitch(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setLfoOutputMode(int lfoIndex, int val) {
    ModelInterface::setLfoOutputMode(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setLfoWave(int lfoIndex, int val) {
    ModelInterface::setLfoWave(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setLfoTempoNumer(int lfoIndex, int val) {
    ModelInterface::setLfoTempoNumer(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setLfoTempoDenom(int lfoIndex, int val) {
    ModelInterface::setLfoTempoDenom(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setLfoFreq(int lfoIndex, double val) {
    ModelInterface::setLfoFreq(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setLfoDepth(int lfoIndex, double val) {
    ModelInterface::setLfoDepth(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setLfoManualPhase(int lfoIndex, double val) {
    ModelInterface::setLfoManualPhase(manager, lfoIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::addEnvelope() {
    ModelInterface::addEnvelope(manager);

    if (_editor != nullptr) {
        _editor->needsModulationBarRebuild();
    }
}

void SyndicateAudioProcessor::setEnvAttackTimeMs(int envIndex, double val) {
    ModelInterface::setEnvAttackTimeMs(manager, envIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setEnvReleaseTimeMs(int envIndex, double val) {
    ModelInterface::setEnvReleaseTimeMs(manager, envIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setEnvFilterEnabled(int envIndex, bool val) {
    ModelInterface::setEnvFilterEnabled(manager, envIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setEnvFilterHz(int envIndex, double lowCut, double highCut) {
    ModelInterface::setEnvFilterHz(manager, envIndex, lowCut, highCut);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setEnvAmount(int envIndex, float val) {
    ModelInterface::setEnvAmount(manager, envIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setEnvUseSidechainInput(int envIndex, bool val) {
    ModelInterface::setEnvUseSidechainInput(manager, envIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::addRandomSource() {
    ModelInterface::addRandom(manager);

    if (_editor != nullptr) {
        _editor->needsModulationBarRebuild();
    }
}

void SyndicateAudioProcessor::setRandomOutputMode(int randomIndex, int val) {
    ModelInterface::setRandomOutputMode(manager, randomIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setRandomFreq(int randomIndex, double val) {
    ModelInterface::setRandomFreq(manager, randomIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setRandomDepth(int randomIndex, double val) {
    ModelInterface::setRandomDepth(manager, randomIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

float SyndicateAudioProcessor::getModulationValueForSource(int id, MODULATION_TYPE type) {
    // TODO This method may be called multiple times for each buffer, could be optimised
    if (type == MODULATION_TYPE::MACRO) {
        const int index {id - 1};
        if (index < macros.size()) {
            return macros[index]->get();
        }
    } else if (type == MODULATION_TYPE::LFO) {
        return ModelInterface::getLfoModulationValue(manager, id);
    } else if (type == MODULATION_TYPE::ENVELOPE) {
        return ModelInterface::getEnvelopeModulationValue(manager, id);
    } else if (type == MODULATION_TYPE::RANDOM) {
        return ModelInterface::getRandomModulationValue(manager, id);
    }

    return 0.0f;
}

void SyndicateAudioProcessor::removeModulationSource(ModulationSourceDefinition definition) {
    ModelInterface::removeModulationSource(manager, definition);

    // Make sure any changes to assigned sources are reflected in the UI
    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::setSplitType(SPLIT_TYPE splitType) {
    if (ModelInterface::setSplitType(manager, splitType, {getBusesLayout(), getSampleRate(), getBlockSize()})) {
        // For graph state changes we need to make sure the processor has updated its state first,
        // then the UI can rebuild based on the processor state
        if (_editor != nullptr) {
            _editor->needsGraphRebuild();
        }
    }
}

void SyndicateAudioProcessor::setChainBypass(int chainNumber, bool val) {
    ModelInterface::setChainBypass(manager, chainNumber, val);

    if (_editor != nullptr) {
        _editor->needsChainButtonsRefresh();
    }
}

void SyndicateAudioProcessor::setChainMute(int chainNumber, bool val) {
    ModelInterface::setChainMute(manager, chainNumber, val);

    if (_editor != nullptr) {
        _editor->needsChainButtonsRefresh();
    }
}

void SyndicateAudioProcessor::setChainSolo(int chainNumber, bool val) {
    ModelInterface::setChainSolo(manager, chainNumber, val);

    if (_editor != nullptr) {
        _editor->needsChainButtonsRefresh();
    }
}

void SyndicateAudioProcessor::setChainCustomName(int chainNumber, const juce::String& name) {
    ModelInterface::setChainCustomName(manager, chainNumber, name);

    if (_editor != nullptr) {
        _editor->needsChainButtonsRefresh();
    }

}

void SyndicateAudioProcessor::setSlotBypass(int chainNumber, int positionInChain, bool bypass) {
    ModelInterface::setSlotBypass(manager, chainNumber, positionInChain, bypass);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setSlotGainLinear(int chainNumber, int positionInChain, float gain) {
    ModelInterface::setGainLinear(manager, chainNumber, positionInChain, gain);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setSlotPan(int chainNumber, int positionInChain, float pan) {
    ModelInterface::setPan(manager, chainNumber, positionInChain, pan);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setPluginModulationIsActive(int chainNumber, int pluginNumber, bool val) {
    ModelInterface::setPluginModulationIsActive(manager, chainNumber, pluginNumber, val);

    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::setModulationTarget(int chainNumber, int pluginNumber, int targetNumber, juce::String targetName) {
    ModelInterface::setModulationTarget(manager, chainNumber, pluginNumber, targetNumber, targetName);

    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::removeModulationTarget(int chainNumber, int pluginNumber, int targetNumber) {
    ModelInterface::removeModulationTarget(manager, chainNumber, pluginNumber, targetNumber);

    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::addModulationSourceToTarget(int chainNumber, int pluginNumber, int targetNumber, ModulationSourceDefinition source) {
    ModelInterface::addModulationSourceToTarget(manager, chainNumber, pluginNumber, targetNumber, source);

    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::removeModulationSourceFromTarget(int chainNumber, int pluginNumber, int targetNumber, ModulationSourceDefinition source) {
    ModelInterface::removeModulationSourceFromTarget(manager, chainNumber, pluginNumber, targetNumber, source);

    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::setModulationTargetValue(int chainNumber, int pluginNumber, int targetNumber, float val) {
    ModelInterface::setModulationTargetValue(manager, chainNumber, pluginNumber, targetNumber, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::setModulationSourceValue(int chainNumber, int pluginNumber, int targetNumber, int sourceNumber, float val) {
    ModelInterface::setModulationSourceValue(manager, chainNumber, pluginNumber, targetNumber, sourceNumber, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::addSourceToLFOFreq(int lfoIndex, ModulationSourceDefinition source) {
    ModelInterface::addSourceToLFOFreq(manager, lfoIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::removeSourceFromLFOFreq(int lfoIndex, ModulationSourceDefinition source) {
    ModelInterface::removeSourceFromLFOFreq(manager, lfoIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::setLFOFreqModulationAmount(int lfoIndex, int sourceIndex, double val) {
    ModelInterface::setLFOFreqModulationAmount(manager, lfoIndex, sourceIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::addSourceToLFODepth(int lfoIndex, ModulationSourceDefinition source) {
    ModelInterface::addSourceToLFODepth(manager, lfoIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::removeSourceFromLFODepth(int lfoIndex, ModulationSourceDefinition source) {
    ModelInterface::removeSourceFromLFODepth(manager, lfoIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::setLFODepthModulationAmount(int lfoIndex, int sourceIndex, double val) {
    ModelInterface::setLFODepthModulationAmount(manager, lfoIndex, sourceIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::addSourceToLFOPhase(int lfoIndex, ModulationSourceDefinition source) {
    ModelInterface::addSourceToLFOPhase(manager, lfoIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::removeSourceFromLFOPhase(int lfoIndex, ModulationSourceDefinition source) {
    ModelInterface::removeSourceFromLFOPhase(manager, lfoIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::setLFOPhaseModulationAmount(int lfoIndex, int sourceIndex, double val) {
    ModelInterface::setLFOPhaseModulationAmount(manager, lfoIndex, sourceIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::addSourceToRandomFreq(int randomIndex, ModulationSourceDefinition source) {
    ModelInterface::addSourceToRandomFreq(manager, randomIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::removeSourceFromRandomFreq(int randomIndex, ModulationSourceDefinition source) {
    ModelInterface::removeSourceFromRandomFreq(manager, randomIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::setRandomFreqModulationAmount(int randomIndex, int sourceIndex, double val) {
    ModelInterface::setRandomFreqModulationAmount(manager, randomIndex, sourceIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::addSourceToRandomDepth(int randomIndex, ModulationSourceDefinition source) {
    ModelInterface::addSourceToRandomDepth(manager, randomIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::removeSourceFromRandomDepth(int randomIndex, ModulationSourceDefinition source) {
    ModelInterface::removeSourceFromRandomDepth(manager, randomIndex, source);

    if (_editor != nullptr) {
        _editor->needsSelectedModulationSourceRebuild();
    }
}

void SyndicateAudioProcessor::setRandomDepthModulationAmount(int randomIndex, int sourceIndex, double val) {
    ModelInterface::setRandomDepthModulationAmount(manager, randomIndex, sourceIndex, val);

    if (_editor != nullptr) {
        _editor->needsUndoRedoRefresh();
    }
}

void SyndicateAudioProcessor::addParallelChain() {
    if (ModelInterface::addParallelChain(manager)) {
        if (_editor != nullptr) {
            _editor->needsGraphRebuild();
        }
    }
}

void SyndicateAudioProcessor::removeParallelChain(int chainNumber) {
    if (ModelInterface::removeParallelChain(manager, chainNumber)) {
        if (_editor != nullptr) {
            _editor->needsGraphRebuild();
        }
    }
}

void SyndicateAudioProcessor::addCrossoverBand() {
    ModelInterface::addCrossoverBand(manager);
    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::removeCrossoverBand(int bandNumber) {
    if (ModelInterface::removeCrossoverBand(manager, bandNumber)) {
        if (_editor != nullptr) {
            _editor->needsGraphRebuild();
        }
    }
}

void SyndicateAudioProcessor::setCrossoverFrequency(size_t index, float val) {
    // Changing the frequency of one crossover may affect others if they also need to be
    // moved - so we set the splitter first, it will update all the frequencies internally,
    // then update the parameter
    if (ModelInterface::setCrossoverFrequency(manager, index, val)) {
        _splitterParameters->triggerUpdate();
    }
}

bool SyndicateAudioProcessor::onPluginSelectedByUser(std::shared_ptr<juce::AudioPluginInstance> plugin,
                                                     int chainNumber,
                                                     int pluginNumber) {

    juce::Logger::writeToLog("SyndicateAudioProcessor::onPluginSelectedByUser: Loading plugin");

    if (pluginConfigurator.configure(plugin,
                                     {getBusesLayout(), getSampleRate(), getBlockSize()})) {
        juce::Logger::writeToLog("SyndicateAudioProcessor::onPluginSelectedByUser: Plugin configured");

        // Hand the plugin over to the splitter
        if (ModelInterface::replacePlugin(manager, std::move(plugin), chainNumber, pluginNumber)) {
            // Ideally we'd like to handle plugin selection like any other parameter - just update the
            // parameter and just action the update in the callback
            // We can't do that though as the splitters are stateful, but we still update the parameter
            // so the UI also gets the update - need to do this last though as the UI pulls its state
            // from the splitter
            if (_editor != nullptr) {
                _editor->needsGraphRebuild();
            }

            return true;
        } else {
            juce::Logger::writeToLog("SyndicateAudioProcessor::onPluginSelectedByUser: Failed to insert new plugin");
        }
    } else {
        juce::Logger::writeToLog("SyndicateAudioProcessor::onPluginSelectedByUser: Failed to configure plugin");
    }

    return false;
}

void SyndicateAudioProcessor::removePlugin(int chainNumber, int pluginNumber) {
    juce::Logger::writeToLog("Removing slot from graph: " + juce::String(chainNumber) + " " + juce::String(pluginNumber));

    if (ModelInterface::removeSlot(manager, chainNumber, pluginNumber)) {
        if (_editor != nullptr) {
            _editor->needsGraphRebuild();
        }
    }
}

void SyndicateAudioProcessor::insertGainStage(int chainNumber, int pluginNumber) {
    juce::Logger::writeToLog("Inserting gain stage: " + juce::String(chainNumber) + " " + juce::String(pluginNumber));

    if (ModelInterface::insertGainStage(manager, chainNumber, pluginNumber)) {
        if (_editor != nullptr) {
            _editor->needsGraphRebuild();
        }
    }
}

void SyndicateAudioProcessor::copySlot(int fromChainNumber, int fromSlotNumber, int toChainNumber, int toSlotNumber) {
    auto onSuccess = [&]() {
        if (_editor != nullptr) {
            _editor->needsGraphRebuild();
        }
    };

    ModelInterface::copySlot(manager, onSuccess, formatManager, fromChainNumber, fromSlotNumber, toChainNumber, toSlotNumber);
}

void SyndicateAudioProcessor::moveSlot(int fromChainNumber, int fromSlotNumber, int toChainNumber, int toSlotNumber) {
    ModelInterface::moveSlot(manager, fromChainNumber, fromSlotNumber, toChainNumber, toSlotNumber);

    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::moveChain(int fromChainNumber, int toChainNumber) {
    ModelInterface::moveChain(manager, fromChainNumber, toChainNumber);

    if (_editor != nullptr) {
        _editor->needsGraphRebuild();
    }
}

void SyndicateAudioProcessor::copyChain(int fromChainNumber, int toChainNumber) {
    auto onSuccess = [&]() {
        if (_editor != nullptr) {
            _editor->needsGraphRebuild();
        }
    };

    ModelInterface::copyChain(manager, onSuccess, formatManager, fromChainNumber, toChainNumber);
}

void SyndicateAudioProcessor::resetAllState() {
    ModelInterface::resetAllState(manager,
                                  {getBusesLayout(), getSampleRate(), getBlockSize()},
                                  [&](int id, MODULATION_TYPE type) { return getModulationValueForSource(id, type); },
                                  [&](int newLatencySamples) { onLatencyChange(newLatencySamples); });

    ModelInterface::createDefaultSources(manager);
    ModelInterface::prepareToPlay(manager, getSampleRate(), getBlockSize(), getBusesLayout());

    if (_editor != nullptr) {
        _editor->needsToRefreshAll();
    }
}

void SyndicateAudioProcessor::undo() {
    ModelInterface::undo(manager, getSampleRate(), getBlockSize(), getBusesLayout());

    if (_editor != nullptr) {
        _editor->needsToRefreshAll();
    }
}

void SyndicateAudioProcessor::redo() {
    ModelInterface::redo(manager, getSampleRate(), getBlockSize(), getBusesLayout());

    if (_editor != nullptr) {
        _editor->needsToRefreshAll();
    }
}

void SyndicateAudioProcessor::setPresetMetadata(const PresetMetadata& newMetadata) {
    presetMetadata = newMetadata;

    if (_editor != nullptr) {
        _editor->needsImportExportRefresh();
    }
}

void SyndicateAudioProcessor::onLatencyChange(int newLatencySamples) {
    setLatencySamples(newLatencySamples);
}

std::vector<juce::String> SyndicateAudioProcessor::_provideParamNamesForMigration() {
    // No parameters to migrate
    return std::vector<juce::String>();
}

void SyndicateAudioProcessor::_migrateParamValues(std::vector<float>& /*paramValues*/) {
    // Do nothing - no parameters to migrate
}

void SyndicateAudioProcessor::_onParameterUpdate() {
    _outputGainLinear = WECore::CoreMath::dBToLinear(outputGainLog->get());
}

void SyndicateAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
#ifdef DEMO_BUILD
    juce::Logger::writeToLog("Not saving state - demo build");
#else
    WECore::JUCEPlugin::CoreAudioProcessor::getStateInformation(destData);
#endif
}

void SyndicateAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
#ifdef DEMO_BUILD
    juce::Logger::writeToLog("Not restoring state - demo build");
#else
    WECore::JUCEPlugin::CoreAudioProcessor::setStateInformation(data, sizeInBytes);

    // Some DAWs can open the UI before loading the state, so we need to make sure the UI is updated
    if (_editor != nullptr) {
        _editor->needsToRefreshAll();
    }
#endif
}

void SyndicateAudioProcessor::SplitterParameters::restoreFromXml(juce::XmlElement* element) {
    juce::Logger::writeToLog("Restoring plugin state from XML");

    if (_processor != nullptr) {
        juce::XmlElement* splitterElement = element->getChildByName(XML_SPLITTER_STR);
        if (splitterElement != nullptr) {
            // Restore the splitter first as we need to know how many chains there are
            _restoreSplitterFromXml(splitterElement);
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_SPLITTER_STR));
        }

        juce::XmlElement* modulationElement = element->getChildByName(XML_MODULATION_SOURCES_STR);
        if (modulationElement != nullptr) {
            // Restore the modulation source parameters
            _restoreModulationSourcesFromXml(modulationElement);
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_MODULATION_SOURCES_STR));
        }

        juce::XmlElement* macroNamesElement = element->getChildByName(XML_MACRO_NAMES_STR);
        if (macroNamesElement != nullptr) {
            // Restore the macro names
            _restoreMacroNamesFromXml(macroNamesElement);
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_MACRO_NAMES_STR));
        }

        juce::XmlElement* metadataElement = element->getChildByName(XML_METADATA_STR);
        if (metadataElement != nullptr) {
            // Restore the metadata
            _restoreMetadataFromXml(metadataElement);
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_METADATA_STR));
        }

        juce::XmlElement* pluginSelectorElement = element->getChildByName(XML_PLUGIN_SELECTOR_STATE_STR);
        if (pluginSelectorElement != nullptr) {
            // Restore the plugin selector window state
            _processor->pluginSelectorState.restoreFromXml(pluginSelectorElement);
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_PLUGIN_SELECTOR_STATE_STR));
        }

        juce::XmlElement* pluginParameterSelectorElement = element->getChildByName(XML_PLUGIN_PARAMETER_SELECTOR_STATE_STR);
        if (pluginParameterSelectorElement != nullptr) {
            // Restore the plugin parameter selector window state
            _processor->pluginParameterSelectorState.restoreFromXml(pluginParameterSelectorElement);
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_PLUGIN_PARAMETER_SELECTOR_STATE_STR));
        }

        juce::XmlElement* mainWindowElement = element->getChildByName(XML_MAIN_WINDOW_STATE_STR);
        if (mainWindowElement != nullptr) {
            // Restore the main window state
            _restoreMainWindowStateFromXml(mainWindowElement);
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_MAIN_WINDOW_STATE_STR));
        }
    } else {
        juce::Logger::writeToLog("Restore failed - no processor");
    }
}

void SyndicateAudioProcessor::SplitterParameters::writeToXml(juce::XmlElement* element) {
    juce::Logger::writeToLog("Writing plugin state to XML");

    if (_processor != nullptr) {
        // Store the splitter
        juce::XmlElement* splitterElement = element->createNewChildElement(XML_SPLITTER_STR);
        _writeSplitterToXml(splitterElement);

        // Store the LFOs/envelopes
        juce::XmlElement* modulationElement = element->createNewChildElement(XML_MODULATION_SOURCES_STR);
        _writeModulationSourcesToXml(modulationElement);

        // Store the macro names
        juce::XmlElement* macroNamesElement = element->createNewChildElement(XML_MACRO_NAMES_STR);
        _writeMacroNamesToXml(macroNamesElement);

        // Store the metadata
        juce::XmlElement* metadataElement = element->createNewChildElement(XML_METADATA_STR);
        _writeMetadataToXml(metadataElement);

        // Store window states
        juce::XmlElement* mainWindowElement = element->createNewChildElement(XML_MAIN_WINDOW_STATE_STR);
        _writeMainWindowStateToXml(mainWindowElement);

        juce::XmlElement* pluginSelectorElement = element->createNewChildElement(XML_PLUGIN_SELECTOR_STATE_STR);
        _processor->pluginSelectorState.writeToXml(pluginSelectorElement);

        juce::XmlElement* pluginParameterSelectorElement = element->createNewChildElement(XML_PLUGIN_PARAMETER_SELECTOR_STATE_STR);
        _processor->pluginParameterSelectorState.writeToXml(pluginParameterSelectorElement);
    } else {
        juce::Logger::writeToLog("Writing failed - no processor");
    }
}

void SyndicateAudioProcessor::SplitterParameters::_restoreSplitterFromXml(juce::XmlElement* element) {
    SyndicateAudioProcessor* tmpProcessor = _processor;

    ModelInterface::restoreSplitterFromXml(
        _processor->manager,
        element,
        [tmpProcessor](int id, MODULATION_TYPE type) { return tmpProcessor->getModulationValueForSource(id, type); },
        [tmpProcessor](int newLatencySamples) { tmpProcessor->onLatencyChange(newLatencySamples); },
        {_processor->getBusesLayout(), _processor->getSampleRate(), _processor->getBlockSize()},
        _processor->pluginConfigurator,
        _processor->pluginScanClient.getPluginTypes(),
        [&](juce::String errorText) { _processor->restoreErrors.push_back(errorText); }
    );
}

void SyndicateAudioProcessor::SplitterParameters::_restoreModulationSourcesFromXml(juce::XmlElement* element) {
    ModelInterface::restoreSourcesFromXml(
        _processor->manager,
        element,
        {_processor->getBusesLayout(), _processor->getSampleRate(), _processor->getBlockSize()}
    );
}

void SyndicateAudioProcessor::SplitterParameters::_restoreMacroNamesFromXml(juce::XmlElement* element) {
    for (int index {0}; index < _processor->macroNames.size(); index++) {
        if (element->hasAttribute(getMacroNameXMLName(index))) {
            _processor->macroNames[index] = element->getStringAttribute(getMacroNameXMLName(index));
        } else {
            juce::Logger::writeToLog("Missing macro name attribute: " + getMacroNameXMLName(index));
        }
    }
}

void SyndicateAudioProcessor::SplitterParameters::_restoreMetadataFromXml(juce::XmlElement* element) {
    if (element->hasAttribute(XML_METADATA_NAME_STR)) {
        _processor->presetMetadata.name = element->getStringAttribute(XML_METADATA_NAME_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_METADATA_NAME_STR));
    }

    if (element->hasAttribute(XML_METADATA_FULLPATH_STR)) {
        _processor->presetMetadata.fullPath = element->getStringAttribute(XML_METADATA_FULLPATH_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_METADATA_FULLPATH_STR));
    }

    if (element->hasAttribute(XML_METADATA_AUTHOR_STR)) {
        _processor->presetMetadata.author = element->getStringAttribute(XML_METADATA_AUTHOR_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_METADATA_AUTHOR_STR));
    }

    if (element->hasAttribute(XML_METADATA_DESCRIPTION_STR)) {
        _processor->presetMetadata.description = element->getStringAttribute(XML_METADATA_DESCRIPTION_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_METADATA_DESCRIPTION_STR));
    }
}

void SyndicateAudioProcessor::SplitterParameters::_restoreMainWindowStateFromXml(juce::XmlElement* element) {
    if (element->hasAttribute(XML_MAIN_WINDOW_BOUNDS_STR)) {
        const juce::String boundsString = element->getStringAttribute(XML_MAIN_WINDOW_BOUNDS_STR);
        _processor->mainWindowState.bounds = juce::Rectangle<int>::fromString(boundsString);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MAIN_WINDOW_BOUNDS_STR));
    }

    if (element->hasAttribute(XML_GRAPH_VIEW_POSITION_STR)) {
        _processor->mainWindowState.graphViewScrollPosition = element->getIntAttribute(XML_GRAPH_VIEW_POSITION_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_GRAPH_VIEW_POSITION_STR));
    }

    juce::XmlElement* chainScrollPositionsElement = element->getChildByName(XML_CHAIN_VIEW_POSITIONS_STR);
    if (chainScrollPositionsElement != nullptr) {
        std::vector<int> chainViewScrollPositions;

        const int numChains {chainScrollPositionsElement->getNumAttributes()};
        for (int index {0}; index < numChains; index++) {
            if (chainScrollPositionsElement->hasAttribute(getChainPositionXMLName(index))) {
                chainViewScrollPositions.push_back(chainScrollPositionsElement->getIntAttribute(getChainPositionXMLName(index)));
            }
        }

        _processor->mainWindowState.chainViewScrollPositions = chainViewScrollPositions;
    }

    if (element->hasAttribute(XML_LFO_BUTTONS_POSITION_STR)) {
        _processor->mainWindowState.lfoButtonsScrollPosition = element->getIntAttribute(XML_LFO_BUTTONS_POSITION_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_LFO_BUTTONS_POSITION_STR));
    }

    if (element->hasAttribute(XML_ENV_BUTTONS_POSITION_STR)) {
        _processor->mainWindowState.envButtonsScrollPosition = element->getIntAttribute(XML_ENV_BUTTONS_POSITION_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_ENV_BUTTONS_POSITION_STR));
    }

    juce::XmlElement* selectedSourceElement = element->getChildByName(XML_SELECTED_SOURCE_STR);
    if (selectedSourceElement != nullptr) {
        _processor->mainWindowState.selectedModulationSource = ModulationSourceDefinition(1, MODULATION_TYPE::LFO);
        _processor->mainWindowState.selectedModulationSource.value().restoreFromXml(selectedSourceElement);
    } else {
        juce::Logger::writeToLog("Missing element " + juce::String(XML_SELECTED_SOURCE_STR));
    }
}

void SyndicateAudioProcessor::SplitterParameters::_writeSplitterToXml(juce::XmlElement* element) {
    ModelInterface::writeSplitterToXml(_processor->manager, element);
}

void SyndicateAudioProcessor::SplitterParameters::_writeModulationSourcesToXml(juce::XmlElement* element) {
    ModelInterface::writeSourcesToXml(_processor->manager, element);
}

void SyndicateAudioProcessor::SplitterParameters::_writeMacroNamesToXml(juce::XmlElement* element) {
    for (int index {0}; index < _processor->macroNames.size(); index++) {
        element->setAttribute(getMacroNameXMLName(index), _processor->macroNames[index]);
    }
}

void SyndicateAudioProcessor::SplitterParameters::_writeMetadataToXml(juce::XmlElement* element) {
    element->setAttribute(XML_METADATA_NAME_STR, _processor->presetMetadata.name);
    element->setAttribute(XML_METADATA_FULLPATH_STR, _processor->presetMetadata.fullPath);
    element->setAttribute(XML_METADATA_AUTHOR_STR, _processor->presetMetadata.author);
    element->setAttribute(XML_METADATA_DESCRIPTION_STR, _processor->presetMetadata.description);
}

void SyndicateAudioProcessor::SplitterParameters::_writeMainWindowStateToXml(juce::XmlElement* element) {
    // Store the main window bounds
    element->setAttribute(XML_MAIN_WINDOW_BOUNDS_STR, _processor->mainWindowState.bounds.toString());

    // Store scroll positions
    element->setAttribute(XML_GRAPH_VIEW_POSITION_STR, _processor->mainWindowState.graphViewScrollPosition);

    juce::XmlElement* chainScrollPositionsElement = element->createNewChildElement(XML_CHAIN_VIEW_POSITIONS_STR);
    for (int index {0}; index < _processor->mainWindowState.chainViewScrollPositions.size(); index++) {
        chainScrollPositionsElement->setAttribute(
            getChainPositionXMLName(index), _processor->mainWindowState.chainViewScrollPositions[index]
        );
    }

    element->setAttribute(XML_LFO_BUTTONS_POSITION_STR, _processor->mainWindowState.lfoButtonsScrollPosition);
    element->setAttribute(XML_ENV_BUTTONS_POSITION_STR, _processor->mainWindowState.envButtonsScrollPosition);

    if (_processor->mainWindowState.selectedModulationSource.has_value()) {
        juce::XmlElement* selectedSourceElement = element->createNewChildElement(XML_SELECTED_SOURCE_STR);
        _processor->mainWindowState.selectedModulationSource.value().writeToXml(selectedSourceElement);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SyndicateAudioProcessor();
}
