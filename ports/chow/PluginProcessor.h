/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

//==============================================================================
/**
*/
class ChowAudioProcessor : public AudioProcessor,
                           private AudioProcessorParameter::Listener
{
public:
    //==============================================================================
    ChowAudioProcessor();
    ~ChowAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void parameterValueChanged (int paramIndex, float /*newValue*/) override;
    void parameterGestureChanged (int /*paramIndex*/, bool /*gestureIsStarting*/) override {}

    AudioParameterFloat* threshDB;
    AudioParameterFloat* ratio;
    AudioParameterFloat* inGainDB;
    AudioParameterFloat* outGainDB;

    AudioParameterBool* flip;
    AudioParameterBool* rect;

    //@MAYBELATER: Dry wet param??
    //@MAYBELATER: attack/release stuff??, or at least smooth param changes

    std::unique_ptr<AudioVisualiserComponent> vis;

private:
    int programNum = 0;

    float chow (float x);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowAudioProcessor)
};
