/*
 *  File:       SimpleCompressor.h
 *
 *  Created:    03/12/2020
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
#include "EffectsProcessor.h"
#include "SimpleCompressorParameters.h"

namespace WECore::SimpleCompressor {

    enum class Direction {
        UPWARD,
        DOWNWARD
    };

    /**
     * Simple implementation of a feedforward log domain compressor supporting both upward and
     * downward compression types.
     */
    template <typename SampleType>
    class SimpleCompressor : public EffectsProcessor1in1out<SampleType> {
    public:
        SimpleCompressor();
        virtual ~SimpleCompressor() override = default;

        /** @name Setter Methods */
        /** @{ */

        /**
         * Sets the sample rate being used.
         *
         * @param   val The sample rate
         */
        inline void setSampleRate(double val);

        /**
         * Sets whether the compressor operates in upward or downward mode.
         *
         * @param   val The mode to be applied
         */
        inline void setDirection(Direction val);

        /**
         * Sets the attack time in milliseconds.
         *
         * @param   val The attack to be applied.
         *
         * @see     ATTACK for valid values
         */
        inline void setAttack(double val);

        /**
         * Sets the release time in milliseconds.
         *
         * @param   val The release to be applied.
         *
         * @see     RELEASE for valid values
         */
        inline void setRelease(double val);

        /**
         * Sets the threshold in dB.
         *
         * @param   val The threshold to be applied.
         *
         * @see     THRESHOLD for valid values
         */
        void setThreshold(double val) { _threshold = Parameters::THRESHOLD.BoundsCheck(val); }

        /**
         * Sets the compression ratio.
         *
         * @param   val The ratio to be applied.
         *
         * @see     RATIO for valid values
         */
        void setRatio(double val) { _ratio = Parameters::RATIO.BoundsCheck(val); }

        /**
         * Sets the knee width.
         *
         * @param   val The knee width to be applied.
         *
         * @see     KNEE_WIDTH for valid values
         */
        void setKneeWidth(double val) { _kneeWidth = Parameters::KNEE_WIDTH.BoundsCheck(val); }

        /** @} */

        /** @name Getter Methods */
        /** @{ */

        /**
         * @see setDirection
         */
        Direction getDirection() const { return _direction; }

        /**
         * @see setAttack
         */
        double getAttack() const { return _attackMs; }

        /**
         * @see setRelease
         */
        double getRelease() const { return _releaseMs; }

        /**
         * @see setThreshold
         */
        double getThreshold() const { return _threshold; }

        /**
         * @see setRatio
         */
        double getRatio() const { return _ratio; }

        /**
         * @see setKneeWidth
         */
        double getKneeWidth() const { return _kneeWidth; }

        /**
         * Returns the last gain value applied to an input signal.
         */
        SampleType getGainApplied() const { return _gainApplied; }

        /** @} */

        inline virtual void process1in1out(SampleType* inSamples,
                                           size_t numSamples) override;

        inline virtual void reset() override;

    private:
        Direction _direction;

        double _sampleRate;

        double _attackMs;
        double _attackCoef;
        double _releaseMs;
        double _releaseCoef;
        double _threshold;
        double _ratio;
        double _kneeWidth;

        SampleType _levelDetectorState;
        SampleType _gainApplied;

        double _calcCoef(double timeMs) { return exp(log(0.01) / (timeMs * _sampleRate * 0.001)); }

        inline SampleType _computeLevel(SampleType inSample);

        inline SampleType _computeGain(SampleType inSample);
    };

    template <typename SampleType>
    SimpleCompressor<SampleType>::SimpleCompressor() : _direction(Direction::DOWNWARD),
                                                       _attackMs(Parameters::ATTACK_MS.defaultValue),
                                                       _releaseMs(Parameters::RELEASE_MS.defaultValue),
                                                       _threshold(Parameters::THRESHOLD.defaultValue),
                                                       _ratio(Parameters::RATIO.defaultValue),
                                                       _kneeWidth(Parameters::KNEE_WIDTH.defaultValue),
                                                       _levelDetectorState(0),
                                                       _gainApplied(0) {
        // Call this here rather than setting it in initialiser list so that the coefficients get
        // setup
        setSampleRate(44100);
    }

    template <typename SampleType>
    void SimpleCompressor<SampleType>::setSampleRate(double val) {
        if (!WECore::CoreMath::compareFloatsEqual(_sampleRate, val)) {
            _sampleRate = val;

            _attackCoef = _calcCoef(_attackMs);
            _releaseCoef = _calcCoef(_releaseMs);

            reset();
        }
    }

    template <typename SampleType>
    void SimpleCompressor<SampleType>::setDirection(Direction val) {
        if (_direction != val) {
            _direction = val;
            reset();
        }
    }

    template <typename SampleType>
    void SimpleCompressor<SampleType>::setAttack(double val) {
        _attackMs = Parameters::ATTACK_MS.BoundsCheck(val);
        _attackCoef = _calcCoef(_attackMs);
    }

    template <typename SampleType>
    void SimpleCompressor<SampleType>::setRelease(double val) {
        _releaseMs = Parameters::RELEASE_MS.BoundsCheck(val);
        _releaseCoef = _calcCoef(_releaseMs);
    }

    template <typename SampleType>
    void SimpleCompressor<SampleType>::process1in1out(SampleType* inSamples,
                                                      size_t numSamples) {

        for (size_t index {0}; index < numSamples; index++) {

            // Rectify the input and convert to dB
            const SampleType absdB {static_cast<SampleType>(
                CoreMath::linearTodB(std::abs(inSamples[index]))
            )};

            // Compute gain
            const SampleType gainComp {_computeGain(absdB)};

            // Take the difference and pass to the level detection
            const SampleType level {_computeLevel(absdB - gainComp)};

            // Convert to linear and apply the gain
            _gainApplied = static_cast<SampleType>(CoreMath::dBToLinear(-level));

            inSamples[index] = inSamples[index] * _gainApplied;
        }
    }

    template <typename SampleType>
    void SimpleCompressor<SampleType>::reset() {
        _levelDetectorState = 0;
        _gainApplied = 0;
    }

    template <typename SampleType>
    SampleType SimpleCompressor<SampleType>::_computeLevel(SampleType inSample) {

        if (inSample > _levelDetectorState) {
            _levelDetectorState = _attackCoef * _levelDetectorState + (1 - _attackCoef) * inSample;
        } else {
            _levelDetectorState = _releaseCoef * _levelDetectorState + (1 - _releaseCoef) * inSample;
        }

        return _levelDetectorState;
    }

    template <typename SampleType>
    SampleType SimpleCompressor<SampleType>::_computeGain(SampleType inSample) {

        SampleType retVal {inSample};

        if (_direction == Direction::DOWNWARD) {

            // Downward compression
            if (inSample - _threshold < -_kneeWidth / 2) {
                // Level is below threshold and knee - do nothing

            } else if (std::abs(inSample - _threshold) <= (_kneeWidth / 2)) {
                // Level is within the range of the knee
                retVal = inSample + (1 / _ratio - 1)
                        * std::pow(inSample - _threshold + (_kneeWidth / 2), 2);

            } else {
                // Level is above threshold and knee
                retVal = _threshold + (inSample - _threshold) / _ratio;
            }

        } else {

            // Upward compression
            if (inSample - _threshold < -_kneeWidth / 2) {
                // Level is below theshold and knee
                retVal = _threshold - (_threshold - inSample) / _ratio;

            } else if (std::abs(inSample - _threshold) <= (_kneeWidth / 2)) {
                // Level is within the range of the knee
                retVal = inSample + (1 - 1 / _ratio) \
                         * std::pow(inSample - _threshold - _kneeWidth / 2, 2) / (2 * _kneeWidth);

            } else {
                // Level is above threshold and knee - do nothing
            }

        }

        return retVal;
    }
}
