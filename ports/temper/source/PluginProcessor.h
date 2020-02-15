/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "JuceHeader.h"
#include "FaustUIBridge.h"
#include "RestrictionProcessor.h"

#include "faust/dsp/dsp.h"


//==============================================================================
/**
*/
class TemperAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    TemperAudioProcessor();
    ~TemperAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

#if ! JUCE_AUDIOPROCESSOR_NO_GUI
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
#endif

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
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

private:
    AudioProcessorValueTreeState m_params;

    OwnedArray<::dsp> m_dsps;
    ScopedPointer<FaustUIBridge> m_bridge;
    ScopedPointer<RestrictionProcessor> m_restriction;
    std::unique_ptr<juce::dsp::Oversampling<float>> m_oversampler;

    double m_lastKnownSampleRate;
    int m_currentProgram;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TemperAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
