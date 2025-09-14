/*
 *  File:       TPTSVFilterTests.cpp
 *
 *  Created:    09/05/2017
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
#include "WEFilters/TPTSVFilter.h"

SCENARIO("TPTSVFilter: Parameters can be set and retrieved correctly") {
    GIVEN("A new TPTSVFilter object") {
        WECore::TPTSVF::TPTSVFilter<double> mFilter;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mFilter.getMode() == 1);
                CHECK(mFilter.getCutoff() == Approx(20.0));
                CHECK(mFilter.getQ() == Approx(0.5));
                CHECK(mFilter.getGain() == Approx(1.0));
            }
        }

        WHEN("All parameters are changed to unique values") {
            mFilter.setMode(2);
            mFilter.setCutoff(21);
            mFilter.setQ(0.6);
            mFilter.setGain(1.1);

            THEN("They all get their correct unique values") {
                CHECK(mFilter.getMode() == 2);
                CHECK(mFilter.getCutoff() == Approx(21.0));
                CHECK(mFilter.getQ() == Approx(0.6));
                CHECK(mFilter.getGain() == Approx(1.1));
            }
        }
    }
}

SCENARIO("TPTSVFilter: Parameters enforce their bounds correctly") {
    GIVEN("A new TPTSVFilter object") {
        WECore::TPTSVF::TPTSVFilter<double> mFilter;

        WHEN("All parameter values are too low") {
            mFilter.setMode(-1);
            mFilter.setCutoff(-1);
            mFilter.setQ(0);
            mFilter.setGain(-1);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mFilter.getMode() == 1);
                CHECK(mFilter.getCutoff() == Approx(0.0));
                CHECK(mFilter.getQ() == Approx(0.1));
                CHECK(mFilter.getGain() == Approx(0.0));
            }
        }

        WHEN("All parameter values are too high") {
            mFilter.setMode(5);
            mFilter.setCutoff(20001);
            mFilter.setQ(21);
            mFilter.setGain(3);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mFilter.getMode() == 4);
                CHECK(mFilter.getCutoff() == Approx(20000.0));
                CHECK(mFilter.getQ() == Approx(20.0));
                CHECK(mFilter.getGain() == Approx(2.0));
            }
        }
    }
}

SCENARIO("TPTSVFilter: Silence in = silence out") {
    GIVEN("A TPTSVFilter and a buffer of silent samples") {
        std::vector<double> buffer(1024);
        WECore::TPTSVF::TPTSVFilter<double> mFilter;

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0);

            // do processing
            mFilter.processBlock(&buffer[0], buffer.size());

            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(0.0));
                }
            }
        }
    }
}

SCENARIO("TPTSVFilter: Clone works correctly") {
    GIVEN("A TPTSVFilter and a buffer of samples") {
        auto generateBuffer = []() {
            std::vector<double> buffer(1024);
            std::fill(buffer.begin(), buffer.end(), 0);
            std::fill(buffer.begin() + 300, buffer.end(), 0.5);
            std::fill(buffer.begin() + 600, buffer.end(), 0.7);
            return buffer;
        };

        std::vector<double> initialBuffer = generateBuffer();
        WECore::TPTSVF::TPTSVFilter<double> filter;

        // Set some unique values so we can test for them later
        filter.setMode(WECore::TPTSVF::Parameters::FILTER_MODE.HIGHPASS);
        filter.setCutoff(1001);
        filter.setQ(1.1);
        filter.setGain(0.8);
        filter.setSampleRate(48000);

        // Set up some internal state
        filter.processBlock(&initialBuffer[0], initialBuffer.size());

        WHEN("It is cloned") {
            WECore::TPTSVF::TPTSVFilter<double> clonedFilter = filter.clone();

            THEN("The cloned filter is equal to the original") {
                CHECK(clonedFilter.getMode() == filter.getMode());
                CHECK(clonedFilter.getCutoff() == Approx(filter.getCutoff()));
                CHECK(clonedFilter.getQ() == Approx(filter.getQ()));
                CHECK(clonedFilter.getGain() == Approx(filter.getGain()));
                // CHECK(clonedFilter.getSampleRate() == Approx(filter.getSampleRate()));

                // Check internal state
                std::vector<double> buffer1 = generateBuffer();
                filter.processBlock(&buffer1[0], buffer1.size());

                std::vector<double> buffer2 = generateBuffer();
                clonedFilter.processBlock(&buffer2[0], buffer2.size());

                for (size_t index {0}; index < buffer1.size(); index++) {
                    CHECK(buffer1[index] == Approx(buffer2[index]));
                }
            }
        }
    }
}
