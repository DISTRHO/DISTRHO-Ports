/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChowAudioProcessor::ChowAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    vis.reset (new AudioVisualiserComponent (1));

    addParameter (threshDB = new AudioParameterFloat (String ("thresh"), String ("Threshold"),
                                                    -100.0f, 0.0f, -27.0f));
    
    addParameter (ratio = new AudioParameterFloat (String ("ratio"), String ("Ratio"),
                                                   1.0f, 50.0f, 10.0f));

    addParameter (inGainDB = new AudioParameterFloat (String ("inGaindB"), String ("Input Gain"),
                                                      -30.0f, 6.0f, 0.0f));

    addParameter (outGainDB = new AudioParameterFloat (String ("outGaindB"), String ("Output Gain"),
                                                       -30.0f, 6.0f, 0.0f));

    addParameter (flip = new AudioParameterBool (String ("flip"), String ("Flip"), false));
    addParameter (rect = new AudioParameterBool (String ("rect"), String ("Rect"), false));

    threshDB->addListener (this);
    ratio->addListener (this);
    inGainDB->addListener (this);
    outGainDB->addListener (this);
    flip->addListener (this);
    rect->addListener (this);
}

ChowAudioProcessor::~ChowAudioProcessor()
{
}

void ChowAudioProcessor::parameterValueChanged (int paramIndex, float /*newValue*/)
{
    auto* editor = dynamic_cast<ChowAudioProcessorEditor*> (getActiveEditor());
    if (editor != nullptr)
    {
        auto* floatParam = dynamic_cast<AudioParameterFloat*> (getParameters()[paramIndex]);
        if (floatParam != nullptr)
        {
            if (Slider* slider = editor->getSliderForParam (floatParam))
                slider->setValue (dynamic_cast<AudioParameterFloat*> (getParameters()[paramIndex])->get(), dontSendNotification);
            return;
        }

        auto* boolParam = dynamic_cast<AudioParameterBool*> (getParameters()[paramIndex]);
        if (boolParam != nullptr)
        {
            if (Button* button = editor->getButtonForParam (boolParam))
                button->setToggleState (dynamic_cast<AudioParameterBool*> (getParameters()[paramIndex])->get(), dontSendNotification);
        }
    }
}

//==============================================================================
const String ChowAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ChowAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ChowAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ChowAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ChowAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ChowAudioProcessor::getNumPrograms()
{
    return 5;
}

int ChowAudioProcessor::getCurrentProgram()
{
    return programNum;
}

void ChowAudioProcessor::setCurrentProgram (int index)
{
    programNum = index;

    switch (index)
    {
    case 0:
        *threshDB = -27.0f;
        *ratio = 10.0f;
        *inGainDB = 0.0f;
        *outGainDB = 0.0f;
        *flip = false;
        *rect = false;
        return;

    case 1:
        *threshDB = 0.0f;
        *ratio = 1.0f;
        *flip = false;
        *rect = false;
        return;

    case 2:
        *threshDB = -100.0f;
        *ratio = 50.0f;
        *flip = false;
        *rect = false;
        return;

    case 3:
        *threshDB = -100.0f;
        *ratio = 0.0f;
        *flip = false;
        *rect = true;
        return;

    case 4:
        *threshDB = -100.0f;
        *ratio = 0.0f;
        *flip = true;
        *rect = true;
        return;

    default:
        programNum = 0;
        return;
    }
}

const String ChowAudioProcessor::getProgramName (int index)
{
    switch (index)
    {
    case 0:
        return String ("Default");

    case 1:
        return String ("Clean");

    case 2:
        return String ("Smash");

    case 3:
        return String ("Rect");

    case 4:
        return String ("Flip Rect");

    default:
        return ("Nien");
    }
}

void ChowAudioProcessor::changeProgramName (int /*index*/, const String& /*newName*/)
{
}

//==============================================================================
void ChowAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    setRateAndBufferSizeDetails (sampleRate, samplesPerBlock);
    vis->clear();
}

void ChowAudioProcessor::releaseResources()
{
    vis->clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ChowAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

float ChowAudioProcessor::chow (float x)
{
    float y = x;

    const float threshGain = Decibels::decibelsToGain (threshDB->get());

    if (! *flip && x > threshGain)
    {
        y = threshGain;
        
        if (! *rect)
            y += ((x - threshGain) / ratio->get());
    }

    else if (*flip && x < -threshGain)
    {
        y = -threshGain;
        
        if (! *rect)
            y += ((x + threshGain) / ratio->get());
    }

    //set y
    return y;
}

void ChowAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    ScopedNoDenormals noDenormals;

    buffer.applyGain (Decibels::decibelsToGain (inGainDB->get()));
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* x = buffer.getWritePointer (channel);
        for (int n = 0; n < buffer.getNumSamples(); n++)
            x[n] = chow (x[n]);
    }
    buffer.applyGain (Decibels::decibelsToGain (outGainDB->get()));

    vis->pushBuffer (buffer.getArrayOfReadPointers(), 1, buffer.getNumSamples());
}

//==============================================================================
bool ChowAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* ChowAudioProcessor::createEditor()
{
    return new ChowAudioProcessorEditor (*this);
}

//==============================================================================
void ChowAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml (new XmlElement ("ChowXmlData"));

    xml->setAttribute ("threshDB", (double) *threshDB);
    xml->setAttribute ("ratio", (double) *ratio);
    xml->setAttribute ("inGainDB", (double) *inGainDB);
    xml->setAttribute ("outGainDB", (double) *outGainDB);
    xml->setAttribute ("flip", *flip);

    copyXmlToBinary (*xml, destData);
}

void ChowAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName ("ChowXmlData"))
        {
            *threshDB = (float) xmlState->getDoubleAttribute ("threshDB", 0.0);
            *ratio = (float) xmlState->getDoubleAttribute ("ratio", 10.0);
            *inGainDB = (float) xmlState->getDoubleAttribute ("inGainDB", 0.0);
            *outGainDB = (float) xmlState->getDoubleAttribute ("outGainDB", 0.0);
            *flip = xmlState->getBoolAttribute ("flip", false);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChowAudioProcessor();
}
