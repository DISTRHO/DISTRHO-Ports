/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#if ! JUCE_AUDIOPROCESSOR_NO_GUI
#include "PluginEditor.h"
#endif
#include "TemperDsp.hpp"

const int kOversampleFactor = 3;

//==============================================================================
TemperAudioProcessor::TemperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  ),
m_params (*this, nullptr)
#else
: m_params (*this, nullptr)
#endif
{
    m_bridge = new FaustUIBridge(m_params);
    m_lastKnownSampleRate = 0.0;
    m_currentProgram = -1;

    // Initialize the dsp units
    for (int i = 0; i < getTotalNumInputChannels(); ++i)
    {
        TemperDsp* dsp = new TemperDsp();
        dsp->buildUserInterface(m_bridge);
        m_dsps.add(dsp);
    }

    // Initialize the AudioProcessorValueTreeState root
    ValueTree root (Identifier("TEMPER"));
    m_params.state = root;
}

TemperAudioProcessor::~TemperAudioProcessor()
{
}

//==============================================================================
const String TemperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TemperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TemperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double TemperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TemperAudioProcessor::getNumPrograms()
{
    return 5;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TemperAudioProcessor::getCurrentProgram()
{
    return m_currentProgram;
}

void TemperAudioProcessor::setCurrentProgram (int index)
{
    switch (index) {
        case 0:
            setStateInformation(BinaryData::DefaultPreset_xml,
                                BinaryData::DefaultPreset_xmlSize);
            break;
        case 1:
            setStateInformation(BinaryData::StubbedToePreset_xml,
                                BinaryData::StubbedToePreset_xmlSize);
            break;
        case 2:
            setStateInformation(BinaryData::BeeStingPreset_xml,
                                BinaryData::BeeStingPreset_xmlSize);
            break;
        case 3:
            setStateInformation(BinaryData::MorningAtTheDMVPreset_xml,
                                BinaryData::MorningAtTheDMVPreset_xmlSize);
            break;
        case 4:
            setStateInformation(BinaryData::FlyingUnitedPreset_xml,
                                BinaryData::FlyingUnitedPreset_xmlSize);
            break;
        default:
            break;
    }

    m_currentProgram = index;
}

const String TemperAudioProcessor::getProgramName (int index)
{
    switch (index) {
        case 0:
            return String("Default");
        case 1:
            return String("Stubbed Toe");
        case 2:
            return String("Bee Sting");
        case 3:
            return String("Morning at the DMV");
        case 4:
            return String("Flying United");
        default:
            return String();
    }
}

void TemperAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void TemperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto filterType = juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR;
    m_oversampler = std::unique_ptr<juce::dsp::Oversampling<float>>(new juce::dsp::Oversampling<float>(getTotalNumInputChannels(),
                                                                    kOversampleFactor, filterType, false));

    // Re-initialize the dsp modules at the upsampled rate.
    if (m_lastKnownSampleRate == 0.0)
        for (int i = 0; i < m_dsps.size(); ++i)
            m_dsps.getUnchecked(i)->init(sampleRate * pow(2, kOversampleFactor));
    else
        for (int i = 0; i < m_dsps.size(); ++i)
            m_dsps.getUnchecked(i)->instanceConstants(sampleRate * pow(2, kOversampleFactor));

    m_oversampler->initProcessing(static_cast<size_t> (samplesPerBlock));
    m_lastKnownSampleRate = sampleRate;

    setLatencySamples(static_cast<int>(m_oversampler->getLatencyInSamples()));
}

void TemperAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TemperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TemperAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

#if ! JUCE_AUDIOPROCESSOR_NO_GUI
    TemperAudioProcessorEditor* editor = static_cast<TemperAudioProcessorEditor*>(getActiveEditor());
#endif

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

#if ! JUCE_AUDIOPROCESSOR_NO_GUI
    // Push input buffer into the Pre spectroscope component.
    if (editor)
        editor->m_vizPre->pushBuffer(buffer);
#endif

    // Now the guts of the processing; oversampling and applying the Faust dsp module.
    const int numInputChannels = buffer.getNumChannels();
    const int numInputSamples = buffer.getNumSamples();

    juce::dsp::AudioBlock<float> block (buffer.getArrayOfWritePointers(),
                                        numInputChannels,
                                        numInputSamples);

    juce::dsp::AudioBlock<float> oversampledBlock = m_oversampler->processSamplesUp(block);

    // Run the faust processors on each channel of the oversampled block.
    for (int i = 0; i < numInputChannels; ++i)
    {
        auto* processor = m_dsps.getUnchecked(i);
        auto* data = oversampledBlock.getChannelPointer(i);

        int len = static_cast<int>(oversampledBlock.getNumSamples());

        processor->compute(len, &data, &data);
    }

    m_oversampler->processSamplesDown(block);

#if ! JUCE_AUDIOPROCESSOR_NO_GUI
    // Push resulting buffer into the Post spectroscope component.
    if (editor)
        editor->m_vizPost->pushBuffer(buffer);
#endif
}

#if ! JUCE_AUDIOPROCESSOR_NO_GUI
//==============================================================================
bool TemperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* TemperAudioProcessor::createEditor()
{
    return new TemperAudioProcessorEditor (*this, m_params);
}
#endif

//==============================================================================
void TemperAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ScopedPointer<XmlElement> xml (m_params.state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TemperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
        if (xmlState->hasTagName (m_params.state.getType()))
            m_params.state = ValueTree::fromXml (*xmlState);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TemperAudioProcessor();
}
