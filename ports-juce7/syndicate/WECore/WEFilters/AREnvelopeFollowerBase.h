/*
 *  File:       AREnveloperFollowerBase.h
 *
 *  Created:    27/05/2017
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

#include "AREnvelopeFollowerParameters.h"
#include "ModulationSource.h"
#include "TPTSVFilter.h"

namespace WECore::AREnv {
    /**
     * Base class for an envelope follower with controls for attack and release times, with optional
     * low/high cut filters before the envelope stage.
     */
    class AREnvelopeFollowerBase : public ModulationSource<double> {
    public:
        AREnvelopeFollowerBase() : _attackTimeMs(Parameters::ATTACK_MS.defaultValue),
                                   _releaseTimeMs(Parameters::RELEASE_MS.defaultValue),
                                   _filterEnabled(Parameters::FILTER_ENABLED.defaultValue) {
            // call this here rather than setting it in initialiser list so that the coefficients get
            // setup
            reset();
            setSampleRate(44100);

            _lowCutFilter.setCutoff(Parameters::LOW_CUT.defaultValue);
            _highCutFilter.setCutoff(Parameters::HIGH_CUT.defaultValue);
            _lowCutFilter.setMode(TPTSVF::Parameters::FILTER_MODE.HIGHPASS);
            _highCutFilter.setMode(TPTSVF::Parameters::FILTER_MODE.LOWPASS);
        }

        virtual ~AREnvelopeFollowerBase() override = default;

        /** @name Setter Methods */
        /** @{ */

        /**
         * Sets the sample rate the envelope will operate at.
         * It is recommended that you call this at some point before calling clockUpdateEnvelope.
         *
         * @param[in]   sampleRate  The sample rate in Hz
         */
        inline void setSampleRate(double sampleRate);

        /**
         * Sets the attack time of the envelope.
         *
         * @see ATTACK_MS for valid values
         *
         * @param[in]   time    Attack time in milliseconds
         */
        inline void setAttackTimeMs(double time);

        /**
         * Sets the release time of the envelope.
         *
         * @see RELEASE_MS for valid values
         *
         * @param[in]   time    Release time in milliseconds
         */
        inline void setReleaseTimeMs(double time);

        /**
         * Sets enables or disables the filter in front of the envelope.
         *
         * @see FILTER_ENABLED for valid values
         *
         * @param[in]   isEnabled    Set to true to enabled the filter.
         */
        void setFilterEnabled(bool isEnabled) { _filterEnabled = isEnabled; }

        /**
         * Sets the low cut applied before the envelope.
         *
         * @see LOW_CUT for valid values
         *
         * @param[in]   freq    Cutoff frequency in Hz
         */
        void setLowCutHz(double freq) { _lowCutFilter.setCutoff(freq); }

        /**
         * Sets the high cut applied before the envelope.
         *
         * @see HIGH_CUT for valid values
         *
         * @param[in]   freq    Cutoff frequency in Hz
         */
        void setHighCutHz(double freq) { _highCutFilter.setCutoff(freq); }
        /** @} */

        /** @name Getter Methods */
        /** @{ */

        /**
         * @see     setAttackTimeMs
         */
        double getAttackTimeMs() const { return _attackTimeMs; }

        /**
         * @see     setReleaseTimeMs
         */
        double getReleaseTimeMs() const { return _releaseTimeMs; }

        /**
         * @see     setFilterEnabled
         */
        bool getFilterEnabled() const { return _filterEnabled; }

        /**
         * @see     setLowCutHz
         */
        double getLowCutHz() const { return _lowCutFilter.getCutoff(); }

        /**
         * @see     setHighCutHz
         */
        double getHighCutHz() const { return _highCutFilter.getCutoff(); }
        /** @} */

    protected:
        double _envVal;

        double _attackTimeMs;
        double _releaseTimeMs;

        double _attackCoef;
        double _releaseCoef;

        bool _filterEnabled;
        TPTSVF::TPTSVFilter<double> _lowCutFilter;
        TPTSVF::TPTSVFilter<double> _highCutFilter;

        double _sampleRate;

        /**
         * Applies the filters if needed, then calls _envGetNextOutputImpl.
         */
        inline double _getNextOutputImpl(double inSample) override;

        virtual inline double _envGetNextOutputImpl(double inSample) = 0;

        inline void _resetImpl() override;

        double _calcCoef(double timeMs) {
            return exp(log(0.01) / (timeMs * _sampleRate * 0.001));
        }
    };

    void AREnvelopeFollowerBase::setSampleRate(double sampleRate) {
        _sampleRate = sampleRate;
        _attackCoef = _calcCoef(_attackTimeMs);
        _releaseCoef = _calcCoef(_releaseTimeMs);

        _lowCutFilter.setSampleRate(sampleRate);
        _highCutFilter.setSampleRate(sampleRate);
    }

    void AREnvelopeFollowerBase::setAttackTimeMs(double time) {
        _attackTimeMs = Parameters::ATTACK_MS.BoundsCheck(time);
        _attackCoef = _calcCoef(_attackTimeMs);
    }

    void AREnvelopeFollowerBase::setReleaseTimeMs(double time) {
        _releaseTimeMs = Parameters::RELEASE_MS.BoundsCheck(time);
        _releaseCoef = _calcCoef(_releaseTimeMs);
    }

    double AREnvelopeFollowerBase::_getNextOutputImpl(double inSample) {
        if (_filterEnabled) {
            _lowCutFilter.processBlock(&inSample, 1);
            _highCutFilter.processBlock(&inSample, 1);
        }

        return _envGetNextOutputImpl(inSample);
    }

    void AREnvelopeFollowerBase::_resetImpl() {
        _envVal = 0;
        _lowCutFilter.reset();
        _highCutFilter.reset();
    }
}
