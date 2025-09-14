/*
 *  File:       TPTSVFilter.h
 *
 *  Created:    22/12/2016
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

#include "General/CoreMath.h"
#include "WEFilters/TPTSVFilterParameters.h"

namespace WECore::TPTSVF {
    /**
     * A state variable filter from a topology-preserving transform.
     *
     * To use this class, simply call reset, and the process methods as necessary, using the provided
     * getter and setter methods to manipulate parameters.
     *
     * Internally relies on the parameters provided in TPTSVFilterParameters.h
     *
     * Based on a talk given by Ivan Cohen: https://www.youtube.com/watch?v=esjHXGPyrhg
     */
    template <typename T>
    class TPTSVFilter {
        static_assert(std::is_floating_point<T>::value,
                      "Must be provided with a floating point template type");

    public:
        TPTSVFilter() : _sampleRate(44100),
                        _cutoffHz(Parameters::CUTOFF.defaultValue),
                        _q(Parameters::Q.defaultValue),
                        _gain(Parameters::GAIN.defaultValue),
                        _s1(0),
                        _s2(0),
                        _g(0),
                        _h(0),
                        _mode(Parameters::FILTER_MODE.BYPASS) {
            _calculateCoefficients();
        }

        virtual ~TPTSVFilter() = default;

        /**
         * Applies the filtering to a buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param[out]  inSamples   Pointer to the first sample of the left channel's buffer
         * @param[in]   numSamples  Number of samples in the buffer
         */
        void processBlock(T* inSamples, size_t numSamples);

        /**
         * Resets filter coefficients.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        void reset() {
            _s1 = 0;
            _s2 = 0;
        }

        /** @name Getter Methods */
        /** @{ */

        int getMode() const { return _mode; }
        double getCutoff() const { return _cutoffHz; }
        double getQ() const { return _q; }
        double getGain() const { return _gain; }

        /** @} */

        /** @name Setter Methods */
        /** @{ */

        void setMode(int val) { _mode = Parameters::FILTER_MODE.BoundsCheck(val); }
        inline void setCutoff(double val);
        inline void setQ(double val);
        void setGain(double val) { _gain = Parameters::GAIN.BoundsCheck(val); }
        inline void setSampleRate(double val);

        /** @} */

        TPTSVFilter clone() const {
            return TPTSVFilter(*this);
        }

    private:
        double  _sampleRate,
                _cutoffHz,
                _q,
                _gain;

        T _s1,
          _s2,
          _g,
          _h;

        int _mode;

        void _calculateCoefficients();

        TPTSVFilter(const TPTSVFilter& other) {
            _sampleRate = other._sampleRate;
            _cutoffHz = other._cutoffHz;
            _q = other._q;
            _gain = other._gain;

            _s1 = other._s1;
            _s2 = other._s2;
            _g = other._g;
            _h = other._h;

            _mode = other._mode;
        }
    };

    template <typename T>
    void TPTSVFilter<T>::processBlock(T* inSamples, size_t numSamples) {

        if (_mode != Parameters::FILTER_MODE.BYPASS) {

            for (size_t idx {0}; idx < numSamples; idx++) {
                const T sample {inSamples[idx]};

                const T yH {static_cast<T>(_h * (sample - (1.0f / _q + _g) * _s1 - _s2))};

                const T yB {static_cast<T>(_g * yH + _s1)};
                _s1 = _g * yH + yB;

                const T yL {static_cast<T>(_g * yB + _s2)};
                _s2 = _g * yB + yL;

                switch (_mode) {
                    case Parameters::ModeParameter::PEAK:
                        inSamples[idx] = yB * static_cast<T>(_gain);
                        break;

                    case Parameters::ModeParameter::HIGHPASS:
                        inSamples[idx] = yH * static_cast<T>(_gain);
                        break;

                    default:
                        inSamples[idx] = yL * static_cast<T>(_gain);
                        break;
                }
            }
        }
    }

    template <typename T>
    void TPTSVFilter<T>::setCutoff(double val) {
        _cutoffHz = Parameters::CUTOFF.BoundsCheck(val);
        _calculateCoefficients();
    }

    template <typename T>
    void TPTSVFilter<T>::setQ(double val) {
        _q = Parameters::Q.BoundsCheck(val);
        _calculateCoefficients();
    }

    template <typename T>
    void TPTSVFilter<T>::setSampleRate(double val) {
        _sampleRate = val;
        _calculateCoefficients();
    }

    template <typename T>
    void TPTSVFilter<T>::_calculateCoefficients() {
        _g  = static_cast<T>(std::tan(CoreMath::DOUBLE_PI * _cutoffHz / _sampleRate));
        _h  = static_cast<T>(1.0 / (1 + _g / _q + _g * _g));
    }
}
