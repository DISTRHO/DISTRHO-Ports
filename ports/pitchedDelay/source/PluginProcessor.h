/*
==============================================================================

  This file was auto-generated!

  It contains the basic startup code for a Juce application.

==============================================================================
*/

#ifndef __PLUGINPROCESSOR_H_DCFCCE65__
#define __PLUGINPROCESSOR_H_DCFCCE65__

#include "JuceHeader.h"
#include "dsp/delaytabdsp.h"
#include "dsp/BandLimit.h"

#ifdef DEBUG
#define NUMDELAYTABS 2
#else
#define NUMDELAYTABS 5
#endif
#define PITCHRANGESEMITONES 12


//==============================================================================
/**
*/
class PitchedDelayAudioProcessor  : public AudioProcessor
{
public:
	enum
	{
		kDryVolume,
		kMasterVolume,

		kNumParameters
	};

	enum Presets
	{		
		kPresetAllSeconds,		
		kPresetAll1_2,
		kPresetAll1_2T,
		kPresetAll1_4,
		kPresetAll1_4T,
		kPresetAll1_8,
		kPresetAll1_8T,
		kPresetAll1_16,

		kNumPresets
	};


	//==============================================================================
	PitchedDelayAudioProcessor();
	~PitchedDelayAudioProcessor();

	//==============================================================================
	void prepareToPlay (double sampleRate, int samplesPerBlock);
	void releaseResources();

	void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

	//==============================================================================
	AudioProcessorEditor* createEditor();
	bool hasEditor() const;

	double getTailLengthSeconds() const { return 0.0; }

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
	bool silenceInProducesSilenceOut() const;
	//==============================================================================
	int getNumPrograms();
	int getCurrentProgram();
	void setCurrentProgram (int index);
	const String getProgramName (int index);
	void changeProgramName (int index, const String& newName);

	//==============================================================================
	void getStateInformation (MemoryBlock& destData);
	void setStateInformation (const void* data, int sizeInBytes);

	DelayTabDsp* getDelay(int index)
	{
		return delays[index];
	}



	int getNumDelays()
	{
		return delays.size();
	}

	int getNumDelayParameters() { return delays.size() * delays[0]->getNumParameters(); }

	int currentTab;

	bool showTooltips;

private:	

	float params[kNumParameters];
	SampleDelay latencyCompensation;

	OwnedArray<DelayTabDsp> delays;

	OwnedArray<DownSampler2x> downSamplers;
	OwnedArray<UpSampler2x> upSamplers;
	HeapBlock<float> overSampleBuffers;
	HeapBlock<float*> osBufferL;
	HeapBlock<float*> osBufferR;

  //==============================================================================
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchedDelayAudioProcessor);
};



#endif  // __PLUGINPROCESSOR_H_DCFCCE65__
