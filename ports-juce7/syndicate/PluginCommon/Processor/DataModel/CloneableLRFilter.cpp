#include "CloneableLRFilter.hpp"

//==============================================================================
template <typename SampleType>
CloneableLRFilter<SampleType>::CloneableLRFilter()
{
    update();
}

//==============================================================================
template <typename SampleType>
void CloneableLRFilter<SampleType>::setType (Type newType)
{
    filterType = newType;
}

template <typename SampleType>
void CloneableLRFilter<SampleType>::setCutoffFrequency (SampleType newCutoffFrequencyHz)
{
    jassert (juce::isPositiveAndBelow (newCutoffFrequencyHz, static_cast<SampleType> (sampleRate * 0.5)));

    cutoffFrequency = newCutoffFrequencyHz;
    update();
}

//==============================================================================
template <typename SampleType>
void CloneableLRFilter<SampleType>::prepare (const juce::dsp::ProcessSpec& spec)
{
    jassert (spec.sampleRate > 0);
    jassert (spec.numChannels > 0);

    sampleRate = spec.sampleRate;
    update();

    s1.resize (spec.numChannels);
    s2.resize (spec.numChannels);
    s3.resize (spec.numChannels);
    s4.resize (spec.numChannels);

    reset();
}

template <typename SampleType>
void CloneableLRFilter<SampleType>::reset()
{
    for (auto s : { &s1, &s2, &s3, &s4 })
        std::fill (s->begin(), s->end(), static_cast<SampleType> (0));
}

template <typename SampleType>
void CloneableLRFilter<SampleType>::snapToZero() noexcept
{
    for (auto s : { &s1, &s2, &s3, &s4 })
        for (auto& element : *s)
            juce::dsp::util::snapToZero (element);
}

//==============================================================================
template <typename SampleType>
SampleType CloneableLRFilter<SampleType>::processSample (int channel, SampleType inputValue)
{
    auto yH = (inputValue - (R2 + g) * s1[(size_t) channel] - s2[(size_t) channel]) * h;

    auto yB = g * yH + s1[(size_t) channel];
    s1[(size_t) channel] = g * yH + yB;

    auto yL = g * yB + s2[(size_t) channel];
    s2[(size_t) channel] = g * yB + yL;

    if (filterType == Type::allpass)
        return yL - R2 * yB + yH;

    auto yH2 = ((filterType == Type::lowpass ? yL : yH) - (R2 + g) * s3[(size_t) channel] - s4[(size_t) channel]) * h;

    auto yB2 = g * yH2 + s3[(size_t) channel];
    s3[(size_t) channel] = g * yH2 + yB2;

    auto yL2 = g * yB2 + s4[(size_t) channel];
    s4[(size_t) channel] = g * yB2 + yL2;

    return filterType == Type::lowpass ? yL2 : yH2;
}

template <typename SampleType>
void CloneableLRFilter<SampleType>::processSample (int channel, SampleType inputValue, SampleType &outputLow, SampleType &outputHigh)
{
    auto yH = (inputValue - (R2 + g) * s1[(size_t) channel] - s2[(size_t) channel]) * h;

    auto yB = g * yH + s1[(size_t) channel];
    s1[(size_t) channel] = g * yH + yB;

    auto yL = g * yB + s2[(size_t) channel];
    s2[(size_t) channel] = g * yB + yL;

    auto yH2 = (yL - (R2 + g) * s3[(size_t) channel] - s4[(size_t) channel]) * h;

    auto yB2 = g * yH2 + s3[(size_t) channel];
    s3[(size_t) channel] = g * yH2 + yB2;

    auto yL2 = g * yB2 + s4[(size_t) channel];
    s4[(size_t) channel] = g * yB2 + yL2;

    outputLow = yL2;
    outputHigh = yL - R2 * yB + yH - yL2;
}

template <typename SampleType>
void CloneableLRFilter<SampleType>::update()
{
    g  = (SampleType) std::tan (juce::MathConstants<double>::pi * cutoffFrequency / sampleRate);
    R2 = (SampleType) std::sqrt (2.0);
    h  = (SampleType) (1.0 / (1.0 + R2 * g + g * g));
}

//==============================================================================
template class CloneableLRFilter<float>;
template class CloneableLRFilter<double>;
