/*
 *  File:       StereoWidthProcessorTests.cpp
 *
 *  Created:    05/12/2020
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
#include "General/CoreMath.h"
#include "TestUtils.h"
#include "WEFilters/StereoWidthProcessor.h"

SCENARIO("StereoWidthProcessor: Parameters can be set and retrieved correctly") {
    GIVEN("A new StereoWidthProcessor object") {

        WECore::StereoWidth::StereoWidthProcessor<double> processor;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(processor.getWidth() == Approx(1.0));
            }
        }

        WHEN("All parameters are changed to unique values") {
            processor.setWidth(1.5);

            THEN("They all get their correct unique values") {
                CHECK(processor.getWidth() == Approx(1.5));
            }
        }
    }
}

SCENARIO("StereoWidthProcessor: Parameters enforce their bounds correctly") {
    GIVEN("A new StereoWidthProcessor object") {

        WECore::StereoWidth::StereoWidthProcessor<double> processor;

        WHEN("All parameter values are too low") {
            processor.setWidth(-10);

            THEN("Parameters enforce their lower bounds") {
                CHECK(processor.getWidth() == Approx(0.0));
            }
        }

        WHEN("All parameter values are too high") {
            processor.setWidth(10);


            THEN("Parameters enforce their upper bounds") {
                CHECK(processor.getWidth() == Approx(2.0));

            }
        }
    }
}

SCENARIO("StereoWidthProcessor: Silence in = silence out") {
    GIVEN("A StereoWidthProcessor and a buffer of silent samples") {

        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::StereoWidth::StereoWidthProcessor<double> processor;

        // Fill the buffer
        std::fill(leftBuffer.begin(), leftBuffer.end(), 0);
        std::fill(rightBuffer.begin(), rightBuffer.end(), 0);

        WHEN("The silence samples are processed") {

            // Do processing
            processor.process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is silence") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(0.0));
                    CHECK(rightBuffer[index] == Approx(0.0));
                }
            }
        }
    }
}

SCENARIO("StereoWidthProcessor: Neutral width position") {
    GIVEN("A StereoWidthProcessor and a buffer of sine samples") {

        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::StereoWidth::StereoWidthProcessor<double> processor;

        // Fill the buffers
        WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
        std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

        // Set the expected output
        std::vector<double> expectedOutput(1024);
        std::copy(leftBuffer.begin(), leftBuffer.end() , expectedOutput.begin());

        WHEN("The samples are processed") {
            // Do processing
            processor.process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is silence") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(expectedOutput[index]));
                    CHECK(rightBuffer[index] == Approx(expectedOutput[index]));
                }
            }
        }
    }
}

SCENARIO("StereoWidthProcessor: Stereo to full mono") {
    GIVEN("A StereoWidthProcessor and a buffer of sine samples") {

        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::StereoWidth::StereoWidthProcessor<double> processor;

        // Fill the buffers with different frequency sines
        WECore::TestUtils::generateSine(leftBuffer, 44100, 1000);
        WECore::TestUtils::generateSine(leftBuffer, 44100, 1500);

        WHEN("The width is set to mono and samples are processed") {

            processor.setWidth(0);

            // Do processing
            processor.process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is the same for each channel") {
                for (size_t index {0}; index < leftBuffer.size(); index++) {
                    CHECK(leftBuffer[index] == Approx(rightBuffer[index]));
                }
            }
        }
    }
}
