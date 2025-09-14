/*
 *  File:       SongbirdFormantFilter.h
 *
 *  Created:    16/07/2016
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
 */

#pragma once

#include "SongbirdFilters/Formant.h"
#include "WEFilters/TPTSVFilter.h"
#include <algorithm>
#include <vector>

namespace WECore::Songbird {
    /**
     * A class containing a vector of bandpass filters to produce a vowel sound.
     *
     * Supports only mono audio processing. For stereo processing, you must create
     * two objects of this type. (Do not reuse a single object for both channels)
     *
     * To use this class, simply call setFormants, reset, and process as necessary.
     *
     * @see setFormants - must be called before performing any processing
     * @see Formant     - Formant objects are required for operation of this class
     */
    template <typename T, size_t NUM_FORMANTS>
    class SongbirdFormantFilter {
        static_assert(std::is_floating_point<T>::value,
                "Must be provided with a floating point template type");

    public:
        /**
         * Creates and stores the appropriate number of filters.
         */
        SongbirdFormantFilter() {
            for (size_t iii {0}; iii < NUM_FORMANTS; iii++) {
                _filters[iii].setMode(TPTSVF::Parameters::FILTER_MODE.PEAK);
                _filters[iii].setQ(10);
            }
        }

        virtual ~SongbirdFormantFilter() = default;

        /**
         * Applies the filtering to a mono buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param   inSample   Sample to process
         */
        inline T process(T inSample);

        /**
         * Sets the properties of each bandpass filter contained in the object.
         *
         * @param   formants    An array of Formants, the size of which must equal the
         *                      number of bandpass filters in the object.
         *
         * @return  A boolean value, true if the formants have been applied to the filters
         *          correctly, false if the operation failed
         *
         * @see     Formant - This object is used to as a convenient container of all the
         *                    parameters which can be supplied to a bandpass filter.
         */
        inline bool setFormants(const std::array<Formant, NUM_FORMANTS>& formants);

        /**
         * Sets the sample rate which the filters will be operating on.
         */
        inline void setSampleRate(double val);

        /**
         * Resets all filters.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        inline void reset();

    private:
        std::array<TPTSVF::TPTSVFilter<T>, NUM_FORMANTS> _filters;
    };

    template <typename T, size_t NUM_FORMANTS>
    T SongbirdFormantFilter<T, NUM_FORMANTS>::process(T inSample) {

        T outSample {0};

        // Perform the filtering for each formant peak
        for (size_t filterNumber {0}; filterNumber < _filters.size(); filterNumber++) {

            T tempSample {inSample};

            _filters[filterNumber].processBlock(&tempSample, 1);

            outSample += tempSample;
        }

        return outSample;
    }

    template <typename T, size_t NUM_FORMANTS>
    bool SongbirdFormantFilter<T, NUM_FORMANTS>::setFormants(
            const std::array<Formant, NUM_FORMANTS>& formants) {

        bool retVal {false};

        // if the correct number of formants have been supplied,
        // apply them to each filter in turn
        if (_filters.size() == formants.size()) {
            retVal = true;

            for (size_t iii {0}; iii < _filters.size(); iii++) {
                _filters[iii].setCutoff(formants[iii].frequency);

                double gainAbs = CoreMath::dBToLinear(formants[iii].gaindB);
                _filters[iii].setGain(gainAbs);
            }
        }

        return retVal;
    }

    template <typename T, size_t NUM_FORMANTS>
    void SongbirdFormantFilter<T, NUM_FORMANTS>::setSampleRate(double val) {
        for (TPTSVF::TPTSVFilter<T>& filter : _filters) {
            filter.setSampleRate(val);
        }
    }

    template <typename T, size_t NUM_FORMANTS>
    void SongbirdFormantFilter<T, NUM_FORMANTS>::reset() {
        for (TPTSVF::TPTSVFilter<T>& filter : _filters) {
            filter.reset();
        }
    }
}
