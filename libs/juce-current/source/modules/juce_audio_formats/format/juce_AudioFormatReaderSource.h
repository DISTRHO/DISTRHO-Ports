/*
  ==============================================================================

   This file is part of the JUCE 7 technical preview.
   Copyright (c) 2022 - Raw Material Software Limited

   You may use this code under the terms of the GPL v3
   (see www.gnu.org/licenses).

   For the technical preview this file cannot be licensed commercially.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
/**
    A type of AudioSource that will read from an AudioFormatReader.

    @see PositionableAudioSource, AudioTransportSource, BufferingAudioSource

    @tags{Audio}
*/
class JUCE_API  AudioFormatReaderSource  : public PositionableAudioSource
{
public:
    //==============================================================================
    /** Creates an AudioFormatReaderSource for a given reader.

        @param sourceReader                     the reader to use as the data source - this must
                                                not be null
        @param deleteReaderWhenThisIsDeleted    if true, the reader passed-in will be deleted
                                                when this object is deleted; if false it will be
                                                left up to the caller to manage its lifetime
    */
    AudioFormatReaderSource (AudioFormatReader* sourceReader,
                             bool deleteReaderWhenThisIsDeleted);

    /** Destructor. */
    ~AudioFormatReaderSource() override;

    //==============================================================================
    /** Toggles loop-mode.

        If set to true, it will continuously loop the input source. If false,
        it will just emit silence after the source has finished.

        @see isLooping
    */
    void setLooping (bool shouldLoop) override;

    /** Returns whether loop-mode is turned on or not. */
    bool isLooping() const override                             { return looping; }

    /** Returns the reader that's being used. */
    AudioFormatReader* getAudioFormatReader() const noexcept    { return reader; }

    //==============================================================================
    /** Implementation of the AudioSource method. */
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;

    /** Implementation of the AudioSource method. */
    void releaseResources() override;

    /** Implementation of the AudioSource method. */
    void getNextAudioBlock (const AudioSourceChannelInfo&) override;

    //==============================================================================
    /** Implements the PositionableAudioSource method. */
    void setNextReadPosition (int64 newPosition) override;

    /** Implements the PositionableAudioSource method. */
    int64 getNextReadPosition() const override;

    /** Implements the PositionableAudioSource method. */
    int64 getTotalLength() const override;

private:
    //==============================================================================
    OptionalScopedPointer<AudioFormatReader> reader;

    int64 nextPlayPos;
    bool looping;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioFormatReaderSource)
};

} // namespace juce
