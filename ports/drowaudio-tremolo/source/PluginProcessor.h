/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINPROCESSOR_H_CC1BF768__
#define __PLUGINPROCESSOR_H_CC1BF768__

#include "Common.h"
#include "Common/ParameterUpdater.h"


//==============================================================================
/**
*/
class TremoloAudioProcessor :   public AudioProcessor,
                                public Value::Listener,
                                public ParameterUpdater::Listener,
                                public ChangeBroadcaster
{
public:
    //==============================================================================
    TremoloAudioProcessor();
    ~TremoloAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

    bool silenceInProducesSilenceOut() const { return true; }
    double getTailLengthSeconds() const { return 0.0; }

    //==============================================================================
    /** Returns a PluginParameter so any UIs that have been created can reference
        the underlying Values.
     */
    PluginParameter* getParameterObject (int index);

    /** Quicker way of getting the Value object of a PluginParameter.
     */
    Value& getParameterValueObject (int index);

    /** This will be called when one of our PluginParameter values changes.
        We go through them one by one to see which one has changed and respond to
        it accordingly.
     */
    void valueChanged (Value& changedValue);

    //==============================================================================
    Buffer& getTremoloBuffer (int index);

private:
    //==============================================================================
    OwnedArray<PluginParameter> parameters;
    Value dummyValue;

    Buffer tremoloBufferL, tremoloBufferR, dummyBuffer;

    double currentSampleRate;
    float tremoloBufferPosition;
    float tremoloBufferIncriment;

    ParameterUpdater parameterUpdater;

    //==============================================================================
    void parameterUpdated (int index);

    void fillBuffer (float* bufferToFill, float phaseAngleRadians);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TremoloAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_CC1BF768__
