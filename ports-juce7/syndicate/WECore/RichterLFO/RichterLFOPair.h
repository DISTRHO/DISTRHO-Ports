/*
 *  File:       RichterLFOPair.h
 *
 *  Created:    18/05/2015
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

#include "RichterLFO.h"
#include "WEFilters/ModulationSource.h"

namespace WECore::Richter {
    /**
     *
     * A convenience class that allows a simple implementation of an LFO that has
     * been paired with a MOD oscillator to modulate its depth and frequency. If you use
     * this class, you will never need to interact directly with either of the contained
     * LFOs for anything other than getting or setting parameter values.
     *
     * This class has been created as the LFO relies on the MOD being ready before
     * it can perform certain operations, which means there are method calls to
     * each oscillator which must be interleaved carefully.
     *
     * The following example shows how to set up this object for audio at 120bpm and 44.1kHz:
     * @code
     * RichterLFOPair lfoPair;
     * lfoPair.prepareForNextBuffer(120, 0, 44100);
     * @endcode
     *
     * Then the class can be used to process a buffer as follows:
     * (where numSamples is the size of the buffer)
     * @code
     * for (size_t iii {0}; iii < numSamples; iii++) {
     *     buffer[iii] = buffer[iii] * lfoPair.getNextOutput();
     * }
     * @endcode
     *
     * getNextOutput must be called once for each sample. Even if there is a sample in the buffer
     * which you do not wish to apply processing to, getNextOutput must still be called otherwise
     * subsequent samples will have the wrong gain calculation applied.
     */

    class RichterLFOPair : public ModulationSource<double> {
    public:
        inline RichterLFOPair();
        virtual ~RichterLFOPair() override = default;
        RichterLFOPair operator= (RichterLFOPair& other) = delete;
        RichterLFOPair(RichterLFOPair& other) = delete;

        /**
         * Prepares for processing the next buffer of samples. For example if using JUCE, you
         * would call this in your processBlock() method before doing any processing.
         *
         * This calls various protected methods of each of the oscillators in a specific order
         * to ensure calculations are done correctly.
         *
         * @param   bpm             Current bpm of the host.
         * @param   timeInSeconds   Position of the host DAW's playhead at the start of
         *                          playback.
         */
        inline void prepareForNextBuffer(double bpm, double timeInSeconds);

        /**
         * Sets the sample rate for both LFOs.
         *
         * @param   sampleRate      Current sample rate of the host
         */
        inline void setSampleRate(double sampleRate);

        RichterLFO LFO;
        std::shared_ptr<RichterLFO> MOD;

    private:
        /**
         * Returns a gain value which is intended to be multiplied with a single sample to apply the
         * tremolo effect.
         *
         * Note: Calling this method will advance the oscillators internal counters by one
         *       sample. Calling this method will return a different value each time.
         *
         * @return  The value of the RichterLFO's output at this moment, a value between 0 and 1.
         */
        inline double _getNextOutputImpl(double inSample) override;

        /**
         * Call each oscillator's reset method.
         */
        inline void _resetImpl() override;
    };

    RichterLFOPair::RichterLFOPair() {
        MOD = std::make_shared<RichterLFO>();
        LFO.addFreqModulationSource(MOD);
        LFO.addDepthModulationSource(MOD);
    }

    void RichterLFOPair::prepareForNextBuffer(double bpm,
                                              double timeInSeconds) {
        LFO.prepareForNextBuffer(bpm, timeInSeconds);
        MOD->prepareForNextBuffer(bpm, timeInSeconds);
    }

    void RichterLFOPair::setSampleRate(double sampleRate) {
        LFO.setSampleRate(sampleRate);
        MOD->setSampleRate(sampleRate);
    }

    void RichterLFOPair::_resetImpl() {
        LFO.reset();
        MOD->reset();
    }

    double RichterLFOPair::_getNextOutputImpl(double /*inSample*/) {
        double retVal {1};

        // Advance the modulation LFO state
        MOD->getNextOutput(0);

        // Always call getNextOutput regardless of bypassed state
        const double tempGain {LFO.getNextOutput(0)};

        if (LFO.getBypassSwitch()) {
            // The output of the LFO is a value in the range -1:1, we need to convert this into a
            // gain in the range 0:1 and make sure the value is 1 when the depth is 0
            retVal = (tempGain / 2) + (2 - LFO.getDepth()) / 2;
        }

        return retVal;
    }
}
