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

#ifndef TALCORE_H
#define TALCORE_H

#include "./engine/AudioUtils.h"
#include "./engine/SynthEngine.h"
#include "./engine/Params.h"
#include "TalPreset.h"
#include "./EnvelopeEditor/EnvelopePresetUtility.h"

//==============================================================================
/**
    A simple plugin filter that just applies a gain change to the audio
    passing through it.

*/

class TalCore  : public AudioProcessor,
                 public ChangeBroadcaster
{
public:
    //==============================================================================
    TalCore();
    ~TalCore();

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

    bool silenceInProducesSilenceOut() const { return false; }
    double getTailLengthSeconds() const { return 0.0; }

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
	void getStateInformation (MemoryBlock& destData);
	void getCurrentProgramStateInformation (MemoryBlock& destData);
	void setStateInformation (const void* data, int sizeInBytes);
	void setCurrentProgramStateInformation (const void* data, int sizeInBytes);

    void setStateInformationFromXml(XmlElement* xmlState);
    XmlElement* getCurrentProgramStateInformationAsXml();

    void getXmlPrograms(XmlElement *programList, int programNumber);
    void setXmlPrograms(XmlElement *programList, int programNumber, float version);

    String getStateInformationString ();
    void setStateInformationString (const String& data);

	void processMidiPerSample (MidiBuffer::Iterator *midiIterator, int samplePos);
    bool getNextEvent (MidiBuffer::Iterator *midiIterator, int samplePos);

    EnvelopeEditor* getEnvelopeEditor();
    void envelopeChanged();

    float getBpm();

private:
	TalPreset **talPresets;
	SynthEngine *engine;

	float sampleRate;
	int curProgram;

	// Midi values
	int midiEventPos;
	bool hasMidiMessage;
	bool hasMidiMessageInBuffer;

	float currentNumberOfVoicesNormalized;

    MidiMessage *nextMidiMessage;
    MidiMessage *midiMessage;

    AudioPlayHead::CurrentPositionInfo pos;

    int numPrograms;

    float bpm;

	AudioUtils audioUtils;
};
#endif
