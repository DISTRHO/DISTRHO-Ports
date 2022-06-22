/*
  ==============================================================================

    RestrictionProcessor.cpp
    Created: 15 Feb 2017 10:03:51pm
    Author:  Nick Thompson

  ==============================================================================
*/

#include "RestrictionProcessor.h"

const double kFreq = 0.05; // Twenty second period.
const double kDuty = 0.75; // Fifteen seconds of audio, five seconds of silence.

//==============================================================================
RestrictionProcessor::RestrictionProcessor()
{
    m_smoothing = new LinearSmoothedValue<float>();
}

//==============================================================================
void RestrictionProcessor::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    m_sampleRate = sampleRate;
    m_delta = kFreq / m_sampleRate;
    m_currentAngle = 0.0;
    m_smoothing->reset(sampleRate, 1.0);
}

void RestrictionProcessor::releaseResources() {}

void RestrictionProcessor::processBlock(AudioSampleBuffer &buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    float** channelData = buffer.getArrayOfWritePointers();

    for (int j = 0; j < numSamples; ++j)
    {
        const float targetGain = static_cast<float>(m_currentAngle <= kDuty);

        m_smoothing->setValue(targetGain);
        const float gain = m_smoothing->getNextValue();

        for (int i = 0; i < numChannels; ++i)
        {
            const float in = channelData[i][j];
            channelData[i][j] = gain * in;
        }

        m_currentAngle = fmod(m_currentAngle + m_delta, 1.0);
    }
}
