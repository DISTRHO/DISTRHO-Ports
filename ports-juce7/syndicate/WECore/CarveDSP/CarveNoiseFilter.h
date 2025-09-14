/*
 *	File:		CarveNoiseFilter.h
 *
 *	Version:	2.0.0
 *
 *	Created:	02/06/2016
 *
 *	This file is part of WECore.
 *
 *  WECore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WECore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WECore.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "WEFilters/TPTSVFilter.h"

/**
 * A simple bandpass filter which can process mono or stereo signals.
 * Was initially created to remove frequencies at the extremes of the human
 * hearing range to clean up audio but can fulfil any typical bandpass
 * filter purpose.
 *
 * The cutoff frequencies cannot be changed once the object is constructed.
 *
 * Has methods for processing either a mono or stereo buffer of samples.
 *
 * @see setSampleRate   - recommended to call before performing any processing
 */
namespace WECore::Carve {

    template <typename T>
    class NoiseFilter {
    public:

        /**
         * Defaults the sample rate. It is recommended to call setSampleRate manually
         * before attempting any processing.
         *
         * @param   lowCutHz    Everything below this frequency will be cut
         * @param   highCutHz   Everything above this frequency will be cut
         */
        NoiseFilter(double lowCutHz, double highCutHz);

        virtual ~NoiseFilter() {}

        /**
         * Configures the filters for the correct sample rate. Ensure this is
         * called before attempting to process audio.
         *
         * @param   sampleRate  The sample rate the filter should be configured for
         */
        inline void setSampleRate(double sampleRate);

        /**
         * Resets all filters.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        inline void reset();

        /**
         * Applies the filtering to a mono buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param   inSample    Pointer to the first sample of the buffer
         * @param   numSamples  Number of samples in the buffer
         */
        inline void Process1in1out(T* inSample, size_t numSamples);

        /**
         * Applies the filtering to a stereo buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param   inLeftSample    Pointer to the first sample of the left channel's buffer
         * @param   inRightSample   Pointer to the first sample of the right channel's buffer
         * @param   numSamples      Number of samples in the buffer. The left and right buffers
         *                          must be the same size.
         */
        inline void Process2in2out(T *inLeftSample, T *inRightSample, size_t numSamples);

    private:
        WECore::TPTSVF::TPTSVFilter<T> _monoLowCutFilter;
        WECore::TPTSVF::TPTSVFilter<T> _leftLowCutFilter;
        WECore::TPTSVF::TPTSVFilter<T> _rightLowCutFilter;

        WECore::TPTSVF::TPTSVFilter<T> _monoHighCutFilter;
        WECore::TPTSVF::TPTSVFilter<T> _leftHighCutFilter;
        WECore::TPTSVF::TPTSVFilter<T> _rightHighCutFilter;

        double _lowCutHz,
               _highCutHz;
    };

    template <typename T>
    NoiseFilter<T>::NoiseFilter(double lowCutHz, double highCutHz) : _lowCutHz(lowCutHz),
                                                                     _highCutHz(highCutHz) {
        setSampleRate(44100);

        auto setupLowCutFilter = [lowCutHz](TPTSVF::TPTSVFilter<T>& filter) {
            filter.setMode(WECore::TPTSVF::Parameters::ModeParameter::HIGHPASS);
            filter.setCutoff(lowCutHz);
            filter.setQ(1);
            filter.setGain(1);
        };

        auto setupHighCutFilter = [highCutHz](TPTSVF::TPTSVFilter<T>& filter) {
            filter.setMode(WECore::TPTSVF::Parameters::ModeParameter::LOWPASS);
            filter.setCutoff(highCutHz);
            filter.setQ(1);
            filter.setGain(1);
        };

        setupLowCutFilter(_monoLowCutFilter);
        setupLowCutFilter(_leftLowCutFilter);
        setupLowCutFilter(_rightLowCutFilter);

        setupHighCutFilter(_monoHighCutFilter);
        setupHighCutFilter(_leftHighCutFilter);
        setupHighCutFilter(_rightHighCutFilter);
    }

    template <typename T>
    void NoiseFilter<T>::setSampleRate(double sampleRate) {
        _monoLowCutFilter.setSampleRate(sampleRate);
        _leftLowCutFilter.setSampleRate(sampleRate);
        _rightLowCutFilter.setSampleRate(sampleRate);

        _monoHighCutFilter.setSampleRate(sampleRate);
        _leftHighCutFilter.setSampleRate(sampleRate);
        _rightHighCutFilter.setSampleRate(sampleRate);
    }

    template <typename T>
    void NoiseFilter<T>::reset() {
        _monoLowCutFilter.reset();
        _leftLowCutFilter.reset();
        _rightLowCutFilter.reset();

        _monoHighCutFilter.reset();
        _leftHighCutFilter.reset();
        _rightHighCutFilter.reset();
    }

    template <typename T>
    void NoiseFilter<T>::Process1in1out(T* inSample, size_t numSamples) {
        _monoLowCutFilter.processBlock(inSample, numSamples);
        _monoHighCutFilter.processBlock(inSample, numSamples);
    }

    template <typename T>
    void NoiseFilter<T>::Process2in2out(T *inLeftSample, T *inRightSample, size_t numSamples) {
        _leftLowCutFilter.processBlock(inLeftSample, numSamples);
        _leftHighCutFilter.processBlock(inLeftSample, numSamples);

        _rightLowCutFilter.processBlock(inRightSample, numSamples);
        _rightHighCutFilter.processBlock(inRightSample, numSamples);
    }
}
