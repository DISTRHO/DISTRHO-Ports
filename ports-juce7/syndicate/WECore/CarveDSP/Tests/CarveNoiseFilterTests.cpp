/*
 *  File:       CarveNoiseFilterTests.cpp
 *
 *  Created:    20/05/2017
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

#include "catch.hpp"
#include "CarveDSP/CarveNoiseFilter.h"

SCENARIO("CarveNoiseFilter: Silence in = silence out") {
    GIVEN("A CarveNoiseFilter and a buffer of silent samples") {
        std::vector<float> buffer(1024);
        WECore::Carve::NoiseFilter<float> mFilter(20, 20000);
        mFilter.setSampleRate(48000);

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0);

            mFilter.Process1in1out(&buffer[0], buffer.size());

            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(0.0f).margin(0.00001));
                }
            }
        }
    }
}
