/*
 *  File:       SongbirdFilterModuleTests.cpp
 *
 *  Created:    07/01/2017
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
#include "SongbirdFilters/SongbirdFilterModule.h"
#include "SongbirdFilters/Tests/TestData.h"

#include <algorithm>

SCENARIO("SongbirdFilterModule: Parameters can be set and retrieved correctly") {
    GIVEN("A new SongbirdFilterModule object") {
        WECore::Songbird::SongbirdFilterModule<double> mSongbird;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mSongbird.getVowel1() == 1);
                CHECK(mSongbird.getVowel2() == 2);
                CHECK(mSongbird.getFilterPosition() == 0.5);
                CHECK(mSongbird.getMix() == 1.0);
                CHECK(mSongbird.getAirGain() == 0.5);
                CHECK(mSongbird.getModMode() == true);
            }
        }

        WHEN("All parameters are changed to unique values") {
            mSongbird.setVowel1(3);
            mSongbird.setVowel2(4);
            mSongbird.setFilterPosition(0.03);
            mSongbird.setMix(0.04);
            mSongbird.setAirGain(0.05);
            mSongbird.setModMode(true);

            THEN("They all get their correct unique values") {
                CHECK(mSongbird.getVowel1() == 3);
                CHECK(mSongbird.getVowel2() == 4);
                CHECK(mSongbird.getFilterPosition() == Approx(0.03));
                CHECK(mSongbird.getMix() == Approx(0.04));
                CHECK(mSongbird.getAirGain() == Approx(0.05));
                CHECK(mSongbird.getModMode() == true);
            }
        }
    }
}

SCENARIO("SongbirdFilterModule: Parameters enforce their bounds correctly") {
    GIVEN("A new SongbirdFilterModule object") {
        WECore::Songbird::SongbirdFilterModule<double> mSongbird;

        WHEN("All parameter values are too low") {
            mSongbird.setVowel1(-5);
            mSongbird.setVowel2(-5);
            mSongbird.setFilterPosition(-5);
            mSongbird.setMix(-5);
            mSongbird.setAirGain(-5);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mSongbird.getVowel1() == 1);
                CHECK(mSongbird.getVowel2() == 1);
                CHECK(mSongbird.getFilterPosition() == Approx(0.0));
                CHECK(mSongbird.getMix() == Approx(0.0));
                CHECK(mSongbird.getAirGain() == Approx(0.0));
            }
        }

        WHEN("All parameter values are too high") {
            mSongbird.setVowel1(1000);
            mSongbird.setVowel2(1000);
            mSongbird.setFilterPosition(1000);
            mSongbird.setMix(1000);
            mSongbird.setAirGain(1000);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mSongbird.getVowel1() == 5);
                CHECK(mSongbird.getVowel2() == 5);
                CHECK(mSongbird.getFilterPosition() == Approx(1.0));
                CHECK(mSongbird.getMix() == Approx(1.0));
                CHECK(mSongbird.getAirGain() == Approx(1.0));
            }
        }
    }
}

SCENARIO("SongbirdFilterModule: Silence in = silence out") {
    GIVEN("A SongbirdFilterModule and a buffer of silent samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::Songbird::SongbirdFilterModule<double> mSongbird;

        // fill the buffer
        std::fill(leftBuffer.begin(), leftBuffer.end(), 0);
        std::fill(rightBuffer.begin(), rightBuffer.end(), 0);

        WHEN("The silence samples are processed") {

            // do processing
            mSongbird.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is silence") {
                for (size_t iii {0}; iii < leftBuffer.size(); iii++) {
                    CHECK(leftBuffer[iii] == Approx(0.0));
                    CHECK(rightBuffer[iii] == Approx(0.0));
                }
            }
        }
    }
}

SCENARIO("SongbirdFilterModule: Freq mode") {
    GIVEN("A SongbirdFilterModule and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        const std::vector<double>& expectedOutputLeft =
                TestData::Songbird::Data.at(Catch::getResultCapture().getCurrentTestName() + "-left");
        const std::vector<double>& expectedOutputRight =
                TestData::Songbird::Data.at(Catch::getResultCapture().getCurrentTestName() + "-right");

        WECore::Songbird::SongbirdFilterModule<double> mSongbird;

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        // fill the buffers, phase shift the right one so that they're not identical
        std::generate(leftBuffer.begin(),
                      leftBuffer.end(),
                      [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );
        std::generate(rightBuffer.begin(),
                      rightBuffer.end(),
                      [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE) + WECore::CoreMath::LONG_PI);} );
        WHEN("The parameters are set and samples are processed") {
            // Set freq mode
            mSongbird.setVowel1(1);
            mSongbird.setVowel2(2);
            mSongbird.setAirGain(1);
            mSongbird.setModMode(true);

            mSongbird.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is as expected") {
                for (size_t iii {0}; iii < leftBuffer.size(); iii++) {
                    CHECK(leftBuffer[iii] == Approx(expectedOutputLeft[iii]).margin(0.00001));
                    CHECK(rightBuffer[iii] == Approx(expectedOutputRight[iii]).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("SongbirdFilterModule: Blend mode") {
    GIVEN("A SongbirdFilterModule and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        const std::vector<double>& expectedOutputLeft =
                TestData::Songbird::Data.at(Catch::getResultCapture().getCurrentTestName() + "-left");
        const std::vector<double>& expectedOutputRight =
                TestData::Songbird::Data.at(Catch::getResultCapture().getCurrentTestName() + "-right");

        WECore::Songbird::SongbirdFilterModule<double> mSongbird;

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        // fill the buffers, phase shift the right one so that they're not identical
        std::generate(leftBuffer.begin(),
                      leftBuffer.end(),
                      [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );
        std::generate(rightBuffer.begin(),
                      rightBuffer.end(),
                      [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE) + WECore::CoreMath::LONG_PI);} );
        WHEN("The parameters are set and samples are processed") {
            // Set blend mode
            mSongbird.setVowel1(1);
            mSongbird.setVowel2(2);
            mSongbird.setAirGain(1);
            mSongbird.setModMode(false);

            mSongbird.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is as expected") {
                for (size_t iii {0}; iii < leftBuffer.size(); iii++) {
                    CHECK(leftBuffer[iii] == Approx(expectedOutputLeft[iii]).margin(0.00001));
                    CHECK(rightBuffer[iii] == Approx(expectedOutputRight[iii]).margin(0.00001));
                }
            }
        }
    }
}
