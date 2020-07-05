/*
  ==============================================================================

    SpectroscopeComponent.cpp
    Created: 8 Apr 2017 12:46:51pm
    Author:  Nick Thompson

  ==============================================================================
*/

#include "JuceHeader.h"
#include "SpectroscopeComponent.h"

//==============================================================================
SpectroscopeComponent::SpectroscopeComponent()
:   m_fifoIndex(0),
    m_fftBlockReady(false),
    m_forwardFFT(kFFTOrder),
    m_window(kFFTSize, juce::dsp::WindowingFunction<float>::hann),
    m_strokeColour(Colours::white),
    m_fillStartColour(Colours::white.withAlpha(0.2f)),
    m_fillStopColour(Colours::white.withAlpha(0.8f))
{
    zeromem(m_outputData, sizeof(m_outputData));
    setSize(700, 200);
    startTimerHz(30);
}

SpectroscopeComponent::~SpectroscopeComponent()
{
    stopTimer();
}

void SpectroscopeComponent::paint (Graphics& g)
{
    const float width = (float) getWidth();
    const float height = (float) getHeight();

    // Clear the drawing target
    g.setColour(Colours::transparentBlack);
    g.fillAll();

    // The values in the output bins after the FFT have a range that I don't understand
    // and isn't explained in the docs. It seems that if I scale down by the size of the
    // fft buffer, I get somewhat reasonable results on the graph. But in examples I've
    // seen, we would just divide here by the maximum value in the bins at the time of
    // drawing. Seeing as that would be inconsistent between frames, I'm defaulting to the
    // size of the fft here unless the max value in the bins is larger.
    Range<float> maxBin = FloatVectorOperations::findMinAndMax(m_outputData, kOutputSize);
    const float scale = 1.0f / jmax((float) kFFTSize, maxBin.getEnd());

    g.setColour(m_fillStartColour);

    for (int i = 0; i < kOutputSize; ++i)
    {
        float x = std::log10 (1 + 39 * ((i + 1.0f) / kOutputSize)) / std::log10 (40.0f) * width;

        const float yMag = scale * m_outputData[i];
        const float yDecibel = Decibels::gainToDecibels(yMag);
        const float y = jmap(yDecibel, -90.0f, -12.0f, height, 0.0f);

        g.drawVerticalLine((int) x, y, height);
    }
}

void SpectroscopeComponent::resized()
{
}

void SpectroscopeComponent::timerCallback()
{
    if (m_fftBlockReady)
    {
        // Compute the frequency transform
        m_window.multiplyWithWindowingTable(m_fftData, kFFTSize);
        m_forwardFFT.performFrequencyOnlyForwardTransform(m_fftData);

        // Copy the frequency bins into the output data buffer, taking
        // max(output[i], fftData[i]) for each bin. Note that after computing the
        // FrequencyOnlyForwardTransform on an array A of size N, A[N/2, N) is full
        // of zeros, and A[0, N/4) is a mirror of A[N/4, N/2). Therefore we only copy
        // kFFTSize / 2 samples into the output data buffer here.
        FloatVectorOperations::max(m_outputData, m_outputData, m_fftData, kOutputSize);

        m_fftBlockReady = false;
    }

    // Decay the output bin magnitudes
    for (int i = 0; i < kOutputSize; ++i)
        m_outputData[i] *= 0.707f;

    repaint();
}

void SpectroscopeComponent::pushBuffer(AudioSampleBuffer &buffer)
{
    if (buffer.getNumChannels() > 0)
    {
        const int numSamples = buffer.getNumSamples();
        const float* channelData = buffer.getReadPointer(0);

        for (int i = 0; i < numSamples; ++i)
            pushSample(channelData[i]);
    }
}

inline void SpectroscopeComponent::pushSample(float sample)
{
    // When we wrap around the fifo table, we copy the data into the
    // FFT buffer and prepare to perform the transform.
    if (m_fifoIndex == kFFTSize)
    {
        if (!m_fftBlockReady)
        {
            zeromem(m_fftData, sizeof(m_fftData));
            memcpy(m_fftData, m_fifo, sizeof(m_fifo));
            m_fftBlockReady = true;
        }

        m_fifoIndex = 0;
    }

    m_fifo[m_fifoIndex++] = sample;
}

void SpectroscopeComponent::setColours(Colour strokeColour, Colour fillStartColour, Colour fillStopColour)
{
    m_strokeColour = strokeColour;
    m_fillStartColour = fillStartColour;
    m_fillStopColour = fillStopColour;
}
