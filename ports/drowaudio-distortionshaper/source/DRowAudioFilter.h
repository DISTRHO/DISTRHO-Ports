/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#ifndef _DROWAUDIOFILTER_H_
#define _DROWAUDIOFILTER_H_

#include "Parameters.h"

//==============================================================================
/**
    A reverb filter that impliments a Schoeder/Moorer model with adjustable
	pre-delay, early reflections and reverb tail.
*/
class DRowAudioFilter  : public AudioProcessor,
                         public ChangeBroadcaster
{
public:
    //==============================================================================
    DRowAudioFilter();
    ~DRowAudioFilter();

    bool hasEditor() const { return true; }

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

	void processBlock (AudioSampleBuffer& buffer,
                       MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (const int channelIndex) const;
    const String getOutputChannelName (const int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;

    //==============================================================================
    int getNumPrograms()                                        { return 0; }
    int getCurrentProgram()                                     { return 0; }
    void setCurrentProgram (int index)                          { }
    const String getProgramName (int index)                     { return String::empty; }
    void changeProgramName (int index, const String& newName)   { }

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);

    bool silenceInProducesSilenceOut() const { return false; }
    double getTailLengthSeconds() const { return 0.0; }

    //==============================================================================
	// Custom Methods
	void setupParams();
	void updateParameters();

	// AU Compatibility Methods
	double getScaledParameter(int index);
	void setScaledParameter(int index, float newValue);
	void setScaledParameterNotifyingHost(int index, float newValue);
	double getParameterMin(int index);
	double getParameterMax(int index);
	double getParameterDefault(int index);
	ParameterUnit getParameterUnit(int index);
	double getParameterStep(int index);
	double getParameterSkewFactor(int index);
	void smoothParameters();
	PluginParameter* getParameterPointer(int index);

    //==============================================================================
    juce_UseDebuggingNewOperator

	float* getDistortionBuffer()	{	return distortionBuffer;		}
	int getDistortionBufferSize()	{	return distortionBufferSize;	}

private:

	PluginParameter params[noParams];

	double currentSampleRate;

	static const int distortionBufferSize = 1024;
	static const int distortionBufferMax = distortionBufferSize - 1;
//	float *distortionBuffer;
	HeapBlock <float> distortionBuffer;

	ScopedPointer <OnePoleFilter> inFilterL;
	ScopedPointer <OnePoleFilter> inFilterR;
	ScopedPointer <OnePoleFilter> outFilterL;
	ScopedPointer <OnePoleFilter> outFilterR;

	void parameterChanged (int index, float newValue);
	void refillBuffer();
};

#endif //_DROWAUDIOFILTER_H_
