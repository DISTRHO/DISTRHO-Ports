#include "CloneableDelayLine.hpp"

/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

//==============================================================================
template <typename SampleType, typename InterpolationType>
CloneableDelayLine<SampleType, InterpolationType>::CloneableDelayLine()
    : CloneableDelayLine (0)
{
}

template <typename SampleType, typename InterpolationType>
CloneableDelayLine<SampleType, InterpolationType>::CloneableDelayLine (int maximumDelayInSamples)
{
    jassert (maximumDelayInSamples >= 0);

    sampleRate = 44100.0;

    setMaximumDelayInSamples (maximumDelayInSamples);
}

//==============================================================================
template <typename SampleType, typename InterpolationType>
void CloneableDelayLine<SampleType, InterpolationType>::setDelay (SampleType newDelayInSamples)
{
    auto upperLimit = (SampleType) getMaximumDelayInSamples();
    jassert (juce::isPositiveAndNotGreaterThan (newDelayInSamples, upperLimit));

    delay     = juce::jlimit ((SampleType) 0, upperLimit, newDelayInSamples);
    delayInt  = static_cast<int> (std::floor (delay));
    delayFrac = delay - (SampleType) delayInt;

    updateInternalVariables();
}

template <typename SampleType, typename InterpolationType>
SampleType CloneableDelayLine<SampleType, InterpolationType>::getDelay() const
{
    return delay;
}

//==============================================================================
template <typename SampleType, typename InterpolationType>
void CloneableDelayLine<SampleType, InterpolationType>::prepare (const juce::dsp::ProcessSpec& spec)
{
    jassert (spec.numChannels > 0);

    bufferData.setSize ((int) spec.numChannels, totalSize, false, false, true);

    writePos.resize (spec.numChannels);
    readPos.resize  (spec.numChannels);

    v.resize (spec.numChannels);
    sampleRate = spec.sampleRate;

    reset();
}

template <typename SampleType, typename InterpolationType>
void CloneableDelayLine<SampleType, InterpolationType>::setMaximumDelayInSamples (int maxDelayInSamples)
{
    jassert (maxDelayInSamples >= 0);
    totalSize = juce::jmax (4, maxDelayInSamples + 2);
    bufferData.setSize ((int) bufferData.getNumChannels(), totalSize, false, false, true);
    reset();
}

template <typename SampleType, typename InterpolationType>
void CloneableDelayLine<SampleType, InterpolationType>::reset()
{
    for (auto vec : { &writePos, &readPos })
        std::fill (vec->begin(), vec->end(), 0);

    std::fill (v.begin(), v.end(), static_cast<SampleType> (0));

    bufferData.clear();
}

//==============================================================================
template <typename SampleType, typename InterpolationType>
void CloneableDelayLine<SampleType, InterpolationType>::pushSample (int channel, SampleType sample)
{
    bufferData.setSample (channel, writePos[(size_t) channel], sample);
    writePos[(size_t) channel] = (writePos[(size_t) channel] + totalSize - 1) % totalSize;
}

template <typename SampleType, typename InterpolationType>
SampleType CloneableDelayLine<SampleType, InterpolationType>::popSample (int channel, SampleType delayInSamples, bool updateReadPointer)
{
    if (delayInSamples >= 0)
        setDelay (delayInSamples);

    auto result = interpolateSample (channel);

    if (updateReadPointer)
        readPos[(size_t) channel] = (readPos[(size_t) channel] + totalSize - 1) % totalSize;

    return result;
}

//==============================================================================
template class CloneableDelayLine<float,  juce::dsp::DelayLineInterpolationTypes::None>;
template class CloneableDelayLine<double, juce::dsp::DelayLineInterpolationTypes::None>;
template class CloneableDelayLine<float,  juce::dsp::DelayLineInterpolationTypes::Linear>;
template class CloneableDelayLine<double, juce::dsp::DelayLineInterpolationTypes::Linear>;
template class CloneableDelayLine<float,  juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>;
template class CloneableDelayLine<double, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>;
template class CloneableDelayLine<float,  juce::dsp::DelayLineInterpolationTypes::Thiran>;
template class CloneableDelayLine<double, juce::dsp::DelayLineInterpolationTypes::Thiran>;
