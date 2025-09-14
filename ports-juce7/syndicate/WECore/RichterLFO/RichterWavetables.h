/*
 *  File:       RichterWavetables.h
 *
 *  Created:    13/07/2020
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

namespace WECore::Richter {

    /**
     * Singleton which contains the wavetables used by the Richter LFOs.
     */
    class Wavetables {
    public:

        /**
         * Number of samples in each of the wavetables.
         */
        static constexpr int SIZE {2000};

        static const Wavetables* getInstance() {
            static Wavetables instance;
            return &instance;
        }

        const double* getSine() const { return _sineTable; }
        const double* getSquare() const { return _squareTable; }
        const double* getSaw() const { return _sawTable; }
        const double* getSidechain() const { return _sidechainTable; }

    private:
        double _sineTable[SIZE];
        double _squareTable[SIZE];
        double _sawTable[SIZE];
        double _sidechainTable[SIZE];

        /**
         * Populates the available wavetables
         */
        inline Wavetables();
    };

    Wavetables::Wavetables() {

        // Sine wavetable
        for (int idx = 0; idx < Wavetables::SIZE; idx++) {
            const double radians {idx * CoreMath::DOUBLE_TAU / Wavetables::SIZE};

            // Just a conventional sine
            _sineTable[idx] = sin(radians);
        }

        // Square wavetable
        for (int idx = 0; idx < Wavetables::SIZE; idx++) {
            const double radians {(idx * CoreMath::DOUBLE_TAU / Wavetables::SIZE) + 0.32};

            // The fourier series for a square wave produces a very sharp square with some overshoot
            // and ripple, so this actually uses slightly lower amplitudes for each harmonic than
            // would normally be used.
            //
            // Because the harmonics are lower amplitude, it needs to be scaled up by 1.2 to reach
            // a range of -1 to +1
            _squareTable[idx] =
            (
                sin(radians) +
                (0.3/1.0) * sin(3 * radians) +
                (0.3/2.0) * sin(5 * radians) +
                (0.3/4.0) * sin(7 * radians) +
                (0.3/8.0) * sin(9 * radians) +
                (0.3/16.0) * sin(11 * radians) +
                (0.3/32.0) * sin(13 * radians)
            ) * 1.2;
        }

        // Saw wavetable
        for (int idx = 0; idx < Wavetables::SIZE; idx++) {
            const double radians {(idx * CoreMath::DOUBLE_TAU / Wavetables::SIZE) + CoreMath::DOUBLE_PI};

            // Conventional fourier series for a saw wave, scaled to fit -1 to 1
            _sawTable[idx] =
            (
                sin(radians) -
                (1.0/2.0) * sin(2 * radians) +
                (1.0/3.0) * sin(3 * radians) -
                (1.0/4.0) * sin(4 * radians) +
                (1.0/6.0) * sin(5 * radians) -
                (1.0/8.0) * sin(6 * radians) +
                (1.0/12.0) * sin(7 * radians) -
                (1.0/16.0) * sin(8 * radians) +
                (1.0/24.0) * sin(9 * radians) -
                (1.0/32.0) * sin(10 * radians) +
                (1.0/48.0) * sin(11 * radians) -
                (1.0/64.0) * sin(12 * radians) +
                (1.0/96.0) * sin(13 * radians) -
                (1.0/128.0) * sin(14 * radians)
            ) * (2.0 / 3.0);
        }

        // Sidechain wavetable
        for (int idx = 0; idx < Wavetables::SIZE; idx++) {
            const double radians {idx * CoreMath::DOUBLE_TAU / Wavetables::SIZE};

            _sidechainTable[idx] =
            (
                radians < 0.4497 ?
                    -2 * sin(pow(0.2 * radians - 0.8245, 6) * 10) + 1 :
                    -2 * sin(pow(0.15 * radians - 0.802, 6) * 10) + 1
            );
        }
    }
}
