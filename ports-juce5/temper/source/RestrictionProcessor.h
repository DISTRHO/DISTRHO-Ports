/*
  ==============================================================================

    RestrictionProcessor.h
    Created: 15 Feb 2017 10:03:51pm
    Author:  Nick Thompson

  ==============================================================================
*/

#ifndef RESTRICTIONPROCESSOR_H_INCLUDED
#define RESTRICTIONPROCESSOR_H_INCLUDED

#include "JuceHeader.h"

//==============================================================================
/**
 A simple variant on the JUCE AudioSource for managing the trial version
 restrictions and plugin validation.

 When a source needs to be played, it is first put into a 'prepared' state by a call to
 prepareToPlay(), and then repeated calls will be made to its processBlock() method to
 process the audio data.
 */
class RestrictionProcessor
{
public:
    //==============================================================================
    /** Creates an AudioSource. */
    RestrictionProcessor();

    /** Destructor. */
    virtual ~RestrictionProcessor()      {}

    //==============================================================================
    /** Tells the processor to prepare for playing.
     */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);

    /** Allows the source to release anything it no longer needs after playback has stopped.

     This will be called when the source is no longer going to have its processBlock()
     method called, so it should release any spare memory, etc. that it might have
     allocated during the prepareToPlay() call.
     */
    void releaseResources();

    /** Called repeatedly to fetch subsequent blocks of audio data.

     After calling the prepareToPlay() method, this callback will be made each
     time the audio playback hardware (or whatever other destination the audio
     data is going to) needs another block of data.
     */
    void processBlock (AudioSampleBuffer& buffer);

private:
    //==============================================================================
    ScopedPointer<LinearSmoothedValue<float>> m_smoothing;

    //==============================================================================
    double m_sampleRate;
    double m_delta;
    double m_currentAngle;
};

#endif  // RESTRICTIONPROCESSOR_H_INCLUDED
