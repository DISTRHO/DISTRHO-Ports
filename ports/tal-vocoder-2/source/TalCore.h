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

#include "./engine/VocoderEngine.h"
#include "./engine/Params.h"
#include "TalPreset.h"

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

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

	void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    bool hasEditor() const                  { return true; }
    AudioProcessorEditor* createEditor();

    // vst
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

    bool silenceInProducesSilenceOut() const { return false; }
    double getTailLengthSeconds() const { return 0.0; }

    bool doesClip();

    // vst preset handling
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    // preset chunk save load
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

    // midi
    bool getNextEvent(MidiBuffer::Iterator *midiIterator, const int samplePos);
	void processMidiPerSample (MidiBuffer::Iterator *midiIterator, int samplePos);

    void setNewSampleRate();

private:

	VocoderEngine *engine;
	float sampleRate;

	TalPreset **talPresets;
	int curProgram;

	// Midi values
	int midiEventPos;
	bool hasMoreMidiMessages;

    MidiMessage *nextMidiMessage;
    MidiMessage *midiMessage;
	bool hasMidiMessage;

    int numberOfPrograms;

    CriticalSection myCriticalSectionBuffer;
};
#endif
