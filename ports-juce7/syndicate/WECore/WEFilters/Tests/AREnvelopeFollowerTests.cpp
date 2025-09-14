/*
 *  File:       AREnvelopeFollowerTests.cpp
 *
 *  Created:    03/06/2017
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
#include "WEFilters/AREnvelopeFollowerSquareLaw.h"

SCENARIO("AREnvelopeFollowerSquareLaw: Parameters can be set and retrieved correctly") {
    GIVEN("A new AREnvelopeFollower object") {
        WECore::AREnv::AREnvelopeFollowerSquareLaw mEnv;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mEnv.getAttackTimeMs() == Approx(20.0));
                CHECK(mEnv.getReleaseTimeMs() == Approx(200.0));
                CHECK(mEnv.getFilterEnabled() == false);
                CHECK(mEnv.getLowCutHz() == Approx(0.0));
                CHECK(mEnv.getHighCutHz() == Approx(20000.0));
            }
        }

        WHEN("All parameters are changed to unique values") {
            mEnv.setAttackTimeMs(21.0);
            mEnv.setReleaseTimeMs(22.0);
            mEnv.setFilterEnabled(true);
            mEnv.setLowCutHz(23.0);
            mEnv.setHighCutHz(24.0);

            THEN("They all get their correct unique values") {
                CHECK(mEnv.getAttackTimeMs() == Approx(21.0));
                CHECK(mEnv.getReleaseTimeMs() == Approx(22.0));
                CHECK(mEnv.getFilterEnabled() == true);
                CHECK(mEnv.getLowCutHz() == Approx(23.0));
                CHECK(mEnv.getHighCutHz() == Approx(24.0));
            }
        }
    }
}

SCENARIO("AREnvelopeFollowerSquareLaw: Parameters enforce their bounds correctly") {
    GIVEN("A new AREnvelopeFollower object") {
        WECore::AREnv::AREnvelopeFollowerSquareLaw mEnv;

        WHEN("All parameter values are too low") {
            mEnv.setAttackTimeMs(0);
            mEnv.setReleaseTimeMs(0);
            mEnv.setLowCutHz(-1);
            mEnv.setHighCutHz(-1);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mEnv.getAttackTimeMs() == Approx(0.1));
                CHECK(mEnv.getReleaseTimeMs() == Approx(0.1));
                CHECK(mEnv.getLowCutHz() == Approx(0.0));
                CHECK(mEnv.getHighCutHz() == Approx(0.0));
            }
        }

        WHEN("All parameter values are too high") {
            mEnv.setAttackTimeMs(10001);
            mEnv.setReleaseTimeMs(10001);
            mEnv.setLowCutHz(20001);
            mEnv.setHighCutHz(20001);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mEnv.getAttackTimeMs() == Approx(10000));
                CHECK(mEnv.getReleaseTimeMs() == Approx(10000));
                CHECK(mEnv.getLowCutHz() == Approx(20000.0));
                CHECK(mEnv.getHighCutHz() == Approx(20000.0));
            }
        }
    }
}
