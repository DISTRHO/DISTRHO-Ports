/*
 ==============================================================================

 This file is part of the JUCETICE project - Copyright 2009 by Lucio Asnaghi.

 JUCETICE is based around the JUCE library - "Jules' Utility Class Extensions"
 Copyright 2007 by Julian Storer.

 ------------------------------------------------------------------------------

 JUCE and JUCETICE can be redistributed and/or modified under the terms of
 the GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.

 JUCE and JUCETICE are distributed in the hope that they will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with JUCE and JUCETICE; if not, visit www.gnu.org/licenses or write to
 Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA 02111-1307 USA

 ==============================================================================

   @author  justin
   @tweaker Lucio Asnaghi

 ==============================================================================
*/

#ifndef __JUCETICE_AUDIOSOURCEPROCESSOR_HEADER__
#define __JUCETICE_AUDIOSOURCEPROCESSOR_HEADER__

class AudioSourceProcessor : public AudioProcessor
{
public:

    AudioSourceProcessor (AudioSource* const inputSource,
                         const bool deleteInputWhenDeleted);

    ~AudioSourceProcessor ();

    void prepareToPlay (double sampleRate, int estimatedSamplesPerBlock);
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    void releaseResources();

    const String getName() const;
    const String getInputChannelName (const int channelIndex) const;
    const String getOutputChannelName (const int channelIndex) const;

    bool isInputChannelStereoPair (int index)   const             { return false; }
    bool isOutputChannelStereoPair (int index)   const            { return false; }
    bool acceptsMidi() const                                      { return false; }
    bool producesMidi() const                                     { return false; }

    int getNumParameters()                                        { return 0; }
    const String getParameterName (int parameterIndex)            { return String::empty; }
    float getParameter (int parameterIndex)                       { return 0.0; }
    const String getParameterText (int parameterIndex)            { return String::empty; }
    void setParameter (int parameterIndex, float newValue)        { }

    int getNumPrograms()                                          { return 0; }
    int getCurrentProgram()                                       { return 0; }
    void setCurrentProgram (int index)                            { }
    const String getProgramName (int index)                       { return String::empty; }
    void changeProgramName (int index, const String& newName)     { }

    void getStateInformation (MemoryBlock& destData)              { }
    void setStateInformation (const void* data,int sizeInBytes)   { }

    AudioProcessorEditor* createEditor()                          { return 0; }

private:

    AudioSource* const input;
    const bool deleteInputWhenDeleted;
}; 

#endif

