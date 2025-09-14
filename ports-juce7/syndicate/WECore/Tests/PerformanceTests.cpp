/*
 *  File:       PerformanceTests.cpp
 *
 *  Created:    26/05/2017
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
#include <ctime>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>

#include "CarveDSP/CarveDSPUnit.h"
#include "SongbirdFilters/SongbirdFilterModule.h"


/**
 * Contains most of the useful stuff for the performance tests.
 * If it gets much larger then it's probably more useful to do some refactoring and move this
 * elsewhere.
 */
namespace {
    /**
     * Contains performance stats
     */
    struct Stats {
        double average;
        double deviation;
    };
    std::ostream& operator<< (std::ostream& stream, const Stats& stats) {
        stream << "Average: " << stats.average << " Deviation: " << stats.deviation;
        return stream;
    }

    /**
     * Contains limits for multiple performance critera
     */
    struct Limits {
        const double INDIVIDUAL;
        const double AVERAGE;
        const double DEVIATION;
    };

    /**
     * Does pretty much what it says.
     * clang complains if this isn't inlined
     */
    inline Stats calcAverageAndDeviation(const std::vector<double>& executionTimes) {
        Stats retVal;

        // calculate the average first
        retVal.average = 0;
        for (double time : executionTimes) {
            retVal.average += time;
        }
        retVal.average = retVal.average / static_cast<double>(executionTimes.size());

        // now calculate deviation
        retVal.deviation = 0;
        for (const double& time : executionTimes) {
            retVal.deviation += std::pow((time - retVal.average), 2);
        }
        retVal.deviation = retVal.deviation / (static_cast<double>(executionTimes.size()) - 1);
        retVal.deviation = std::sqrt(retVal.deviation);

        return retVal;
    }

    bool isNewRun {true};
    inline void appendToResultsFile(const Stats& stats, const std::string& testName) {
        const std::string FILE_PATH("wecore_performance.log");
        std::ofstream outStream;
        outStream.open(FILE_PATH, std::ios_base::app);

        if (isNewRun) {
            isNewRun = false;

            std::time_t now {std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
            outStream << std::endl << std::endl << "**** New Test Run: "
                      << std::put_time(std::localtime(&now), "%F %T");
        }
        outStream << std::endl << testName << ":     " << stats;

    }
}


/*** TESTS BEGIN HERE ***/

SCENARIO("Performance: CarveDSPUnit, 100 buffers of 1024 samples each") {
    GIVEN("A CarveDSPUnit and a buffer of samples") {

        const int NUM_BUFFERS {100};
        std::vector<double> buffer(1024);
        WECore::Carve::CarveDSPUnit<double> mCarve;

        // set the performance limits
        Limits mLimits{0.11, 0.07, 0.01};

        // store the execution time for each buffer
        std::vector<double> executionTimes;

        WHEN("The samples are processed") {
            // turn the unit on
            mCarve.setMode(2);

            for (int nbuf {0}; nbuf < NUM_BUFFERS; nbuf++) {

                // fill the buffer with a sine wave
                int iii {0};
                std::generate(buffer.begin(), buffer.end(), [&iii]{ return std::sin(iii++); });


                // do processing
                const clock_t startTime {clock()};
                for (size_t jjj {0}; jjj < buffer.size(); jjj++) {
                    buffer[jjj] = mCarve.process(buffer[jjj]);
                }
                const clock_t endTime {clock()};

                // calculate the execution time
                const double CLOCKS_PER_MICROSEC {static_cast<double>(CLOCKS_PER_SEC) / 1000};
                const double executionTime {static_cast<double>(endTime - startTime) / CLOCKS_PER_MICROSEC};
                executionTimes.push_back(executionTime);
                CHECK(executionTime < mLimits.INDIVIDUAL);
            }

            THEN("The average and variance are within limits") {
                Stats mStats = calcAverageAndDeviation(executionTimes);
                CHECK(mStats.average < mLimits.AVERAGE);
                CHECK(mStats.deviation < mLimits.DEVIATION);

                appendToResultsFile(mStats, Catch::getResultCapture().getCurrentTestName());
            }
        }
    }
}

SCENARIO("Performance: SongbirdFilterModule (blend mode), 100 buffers of 1024 samples each") {
    GIVEN("A SongbirdFilterModule and a buffer of samples") {

        const int NUM_BUFFERS {100};
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::Songbird::SongbirdFilterModule<double> mSongbird;
        mSongbird.setModMode(false);

        // set the performance limits
        Limits mLimits{1.8, 1.5, 0.12};

        // store the execution time for each buffer
        std::vector<double> executionTimes;

        WHEN("The samples are processed") {

            for (int nbuf {0}; nbuf < NUM_BUFFERS; nbuf++) {

                // fill the buffer with a sine wave
                int iii {0};
                std::generate(leftBuffer.begin(), leftBuffer.end(), [&iii]{ return std::sin(iii++); });
                rightBuffer = leftBuffer;


                // do processing
                const clock_t startTime {clock()};
                mSongbird.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());
                const clock_t endTime {clock()};

                // calculate the execution time
                const double CLOCKS_PER_MICROSEC {static_cast<double>(CLOCKS_PER_SEC) / 1000};
                const double executionTime {static_cast<double>(endTime - startTime) / CLOCKS_PER_MICROSEC};
                executionTimes.push_back(executionTime);
                CHECK(executionTime < mLimits.INDIVIDUAL);
            }

            THEN("The average and variance are within limits") {
                Stats mStats = calcAverageAndDeviation(executionTimes);
                CHECK(mStats.average < mLimits.AVERAGE);
                CHECK(mStats.deviation < mLimits.DEVIATION);

                appendToResultsFile(mStats, Catch::getResultCapture().getCurrentTestName());
            }
        }
    }
}

SCENARIO("Performance: SongbirdFilterModule (freq mode), 100 buffers of 1024 samples each") {
    GIVEN("A SongbirdFilterModule and a buffer of samples") {

        const int NUM_BUFFERS {100};
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::Songbird::SongbirdFilterModule<double> mSongbird;
        mSongbird.setModMode(true);

        // set the performance limits
        Limits mLimits{1.8, 1.5, 0.12};

        // store the execution time for each buffer
        std::vector<double> executionTimes;

        WHEN("The samples are processed") {

            for (int nbuf {0}; nbuf < NUM_BUFFERS; nbuf++) {

                // fill the buffer with a sine wave
                int iii {0};
                std::generate(leftBuffer.begin(), leftBuffer.end(), [&iii]{ return std::sin(iii++); });
                rightBuffer = leftBuffer;


                // do processing
                const clock_t startTime {clock()};
                mSongbird.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());
                const clock_t endTime {clock()};

                // calculate the execution time
                const double CLOCKS_PER_MICROSEC {static_cast<double>(CLOCKS_PER_SEC) / 1000};
                const double executionTime {static_cast<double>(endTime - startTime) / CLOCKS_PER_MICROSEC};
                executionTimes.push_back(executionTime);
                CHECK(executionTime < mLimits.INDIVIDUAL);
            }

            THEN("The average and variance are within limits") {
                Stats mStats = calcAverageAndDeviation(executionTimes);
                CHECK(mStats.average < mLimits.AVERAGE);
                CHECK(mStats.deviation < mLimits.DEVIATION);

                appendToResultsFile(mStats, Catch::getResultCapture().getCurrentTestName());
            }
        }
    }
}
