/*
 *  File:       DSPUnitProcessingTests.cpp
 *
 *  Created:    08/09/2018
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
#include "CarveDSP/CarveDSPUnit.h"
#include "CarveDSP/Tests/TestData.h"

SCENARIO("CarveDSPUnit: Silence in = silence out") {
    GIVEN("A CarveDSPUnit and a buffer of silent samples") {
        std::vector<double> buffer(1024);
        WECore::Carve::CarveDSPUnit<double> mCarve;

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0);

            // turn the unit on
            mCarve.setMode(2);

            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }

            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(0.0));
                }
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Sine Default") {
    GIVEN("A CarveDSPUnit and a buffer of sine samples") {
        std::vector<double> buffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::Carve::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::Carve::CarveDSPUnit<double> mCarve;

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::generate(buffer.begin(),
                          buffer.end(),
                          [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );

            // turn the unit on
            mCarve.setMode(2);

            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Parabolic Soft Default") {
    GIVEN("A CarveDSPUnit and a buffer of sine samples") {
        std::vector<double> buffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::Carve::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::Carve::CarveDSPUnit<double> mCarve;

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::generate(buffer.begin(),
                          buffer.end(),
                          [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );

            // turn the unit on
            mCarve.setMode(3);

            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Parabolic Hard Default") {
    GIVEN("A CarveDSPUnit and a buffer of sine samples") {
        std::vector<double> buffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::Carve::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::Carve::CarveDSPUnit<double> mCarve;

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::generate(buffer.begin(),
                          buffer.end(),
                          [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );

            // turn the unit on
            mCarve.setMode(4);

            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Asymetric Sine Default") {
    GIVEN("A CarveDSPUnit and a buffer of sine samples") {
        std::vector<double> buffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::Carve::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::Carve::CarveDSPUnit<double> mCarve;

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::generate(buffer.begin(),
                          buffer.end(),
                          [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );

            // turn the unit on
            mCarve.setMode(5);

            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Exponent Default") {
    GIVEN("A CarveDSPUnit and a buffer of sine samples") {
        std::vector<double> buffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::Carve::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::Carve::CarveDSPUnit<double> mCarve;

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::generate(buffer.begin(),
                          buffer.end(),
                          [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );

            // turn the unit on
            mCarve.setMode(6);

            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Clipper Default") {
    GIVEN("A CarveDSPUnit and a buffer of sine samples") {
        std::vector<double> buffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::Carve::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::Carve::CarveDSPUnit<double> mCarve;

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::generate(buffer.begin(),
                          buffer.end(),
                          [iii = 0]() mutable {return std::sin(WECore::CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );

            // turn the unit on
            mCarve.setMode(7);

            // do processing
            for (size_t iii {0}; iii < buffer.size(); iii++) {
                buffer[iii] = mCarve.process(buffer[iii]);
            }

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(buffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                }
            }
        }
    }
}
