/*
 *  File:       CarveDSPUnitTests.cpp
 *
 *  Created:    26/12/2016
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

SCENARIO("CarveDSPUnit: Parameters can be set and retrieved correctly") {
    GIVEN("A new CarveDSPUnit object") {
        WECore::Carve::CarveDSPUnit<double> mCarve;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mCarve.getMode() == 2);
                CHECK(mCarve.getPreGain() == Approx(1.0));
                CHECK(mCarve.getPostGain() == Approx(0.5));
                CHECK(mCarve.getTweak() == Approx(0.0));
            }
        }

        WHEN("All parameters are changed to unique values") {
            mCarve.setMode(2);
            mCarve.setPreGain(0.02);
            mCarve.setPostGain(0.03);
            mCarve.setTweak(0.04);

            THEN("They all get their correct unique values") {
                CHECK(mCarve.getMode() == 2);
                CHECK(mCarve.getPreGain() == Approx(0.02));
                CHECK(mCarve.getPostGain() == Approx(0.03));
                CHECK(mCarve.getTweak() == Approx(0.04));
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Parameters enforce their bounds correctly") {
    GIVEN("A new CarveDSPUnit object") {
        WECore::Carve::CarveDSPUnit<double> mCarve;

        WHEN("All parameter values are too low") {
            mCarve.setMode(-5);
            mCarve.setPreGain(-5);
            mCarve.setPostGain(-5);
            mCarve.setTweak(-5);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mCarve.getMode() == 1);
                CHECK(mCarve.getPreGain() == Approx(0.0));
                CHECK(mCarve.getPostGain() == Approx(0.0));
                CHECK(mCarve.getTweak() == Approx(0.0));
            }
        }

        WHEN("All parameter values are too high") {
            mCarve.setMode(10);
            mCarve.setPreGain(5);
            mCarve.setPostGain(5);
            mCarve.setTweak(5);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mCarve.getMode() == 7);
                CHECK(mCarve.getPreGain() == Approx(2.0));
                CHECK(mCarve.getPostGain() == Approx(2.0));
                CHECK(mCarve.getTweak() == Approx(1.0));
            }
        }
    }
}

SCENARIO("CarveDSPUnit: Parameter combinations that should result in silence output for any input") {
    GIVEN("A new CarveDSPUnit object and a buffer of 0.5fs") {
        std::vector<double> buffer(1024);
        WECore::Carve::CarveDSPUnit<double> mCarve;

        WHEN("The unit is turned off") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0.5);

            // turn the unit off
            mCarve.setMode(1);

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

        WHEN("Unit is on but has 0 pregain") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0.5);

            // turn the unit on, set pregain
            mCarve.setMode(2);
            mCarve.setPreGain(0);

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

        WHEN("Unit is on but has 0 postgain") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0.5);

            // turn the unit on, set pregain and postgain
            mCarve.setMode(2);
            mCarve.setPreGain(1);
            mCarve.setPostGain(0);

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

