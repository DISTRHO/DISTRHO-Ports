/*
 *  File:       TPTSVFilterTests.cpp
 *
 *  Created:    04/12/2020
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
#include "WEFilters/SimpleCompressor.h"

namespace Comp = WECore::SimpleCompressor;

SCENARIO("SimpleCompressor: Parameters can be set and retrieved correctly") {
    GIVEN("A new SimpleCompressor object") {

        Comp::SimpleCompressor<double> compressor;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(compressor.getDirection() == Comp::Direction::DOWNWARD);
                CHECK(compressor.getAttack() == Approx(10.0));
                CHECK(compressor.getRelease() == Approx(100.0));
                CHECK(compressor.getThreshold() == Approx(0.0));
                CHECK(compressor.getRatio() == Approx(2.0));
                CHECK(compressor.getKneeWidth() == Approx(2.0));
            }
        }

        WHEN("All parameters are changed to unique values") {
            compressor.setDirection(Comp::Direction::UPWARD);
            compressor.setAttack(0.11);
            compressor.setRelease(1.1);
            compressor.setThreshold(-10.0);
            compressor.setRatio(1.2);
            compressor.setKneeWidth(1.3);

            THEN("They all get their correct unique values") {
                CHECK(compressor.getDirection() == Comp::Direction::UPWARD);
                CHECK(compressor.getAttack() == Approx(0.11));
                CHECK(compressor.getRelease() == Approx(1.1));
                CHECK(compressor.getThreshold() == Approx(-10.0));
                CHECK(compressor.getRatio() == Approx(1.2));
                CHECK(compressor.getKneeWidth() == Approx(1.3));
            }
        }
    }
}

SCENARIO("SimpleCompressor: Parameters enforce their bounds correctly") {
    GIVEN("A new SimpleCompressor object") {

        Comp::SimpleCompressor<double> compressor;

        WHEN("All parameter values are too low") {
            compressor.setAttack(-10);
            compressor.setRelease(-10);
            compressor.setThreshold(-100);
            compressor.setRatio(-10);
            compressor.setKneeWidth(-10);

            THEN("Parameters enforce their lower bounds") {
                CHECK(compressor.getAttack() == Approx(0.1));
                CHECK(compressor.getRelease() == Approx(1.0));
                CHECK(compressor.getThreshold() == Approx(-60.0));
                CHECK(compressor.getRatio() == Approx(1.0));
                CHECK(compressor.getKneeWidth() == Approx(1.0));
            }
        }

        WHEN("All parameter values are too high") {
            compressor.setAttack(1000);
            compressor.setRelease(10000);
            compressor.setThreshold(10);
            compressor.setRatio(100);
            compressor.setKneeWidth(100);

            THEN("Parameters enforce their upper bounds") {
                CHECK(compressor.getAttack() == Approx(500));
                CHECK(compressor.getRelease() == Approx(5000));
                CHECK(compressor.getThreshold() == Approx(0));
                CHECK(compressor.getRatio() == Approx(30));
                CHECK(compressor.getKneeWidth() == Approx(10));
            }
        }
    }
}

SCENARIO("SimpleCompressor: Silence in = silence out") {
    GIVEN("A SimpleCompressor and a buffer of silent samples") {

        std::vector<double> buffer(1024);
        Comp::SimpleCompressor<double> compressor;

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0);

            // do processing
            compressor.process1in1out(&buffer[0], buffer.size());

            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(0.0));
                }
            }
        }
    }
}
