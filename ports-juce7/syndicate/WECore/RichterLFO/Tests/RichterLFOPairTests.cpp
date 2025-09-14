/*
 *  File:       RichterLFOPairTests.cpp
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
#include "RichterLFO/RichterLFOPair.h"

SCENARIO("RichterLFOPair: Parameters can be set and retrieved correctly") {
    GIVEN("A new RichterLFOPair object") {
        WECore::Richter::RichterLFOPair mLFOPair;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mLFOPair.LFO.getBypassSwitch() == false);
                CHECK(mLFOPair.LFO.getPhaseSyncSwitch() == true);
                CHECK(mLFOPair.LFO.getTempoSyncSwitch() == false);
                CHECK(mLFOPair.LFO.getInvertSwitch() == false);
                CHECK(mLFOPair.LFO.getWave() == 1);
                CHECK(mLFOPair.LFO.getOutputMode() == 2);
                CHECK(mLFOPair.LFO.getDepth() == Approx(0.5));
                CHECK(mLFOPair.LFO.getFreq() == Approx(2.0));
                CHECK(mLFOPair.LFO.getManualPhase() == Approx(0.0));
                CHECK(mLFOPair.LFO.getTempoNumer() == Approx(1.0));
                CHECK(mLFOPair.LFO.getTempoDenom() == Approx(1.0));

                CHECK(mLFOPair.MOD->getBypassSwitch() == false);
                CHECK(mLFOPair.MOD->getPhaseSyncSwitch() == true);
                CHECK(mLFOPair.MOD->getTempoSyncSwitch() == false);
                CHECK(mLFOPair.MOD->getInvertSwitch() == false);
                CHECK(mLFOPair.MOD->getWave() == 1);
                CHECK(mLFOPair.MOD->getOutputMode() == 2);
                CHECK(mLFOPair.MOD->getDepth() == Approx(0.5));
                CHECK(mLFOPair.MOD->getFreq() == Approx(2.0));
                CHECK(mLFOPair.MOD->getManualPhase() == Approx(0.0));
                CHECK(mLFOPair.MOD->getTempoNumer() == Approx(1.0));
                CHECK(mLFOPair.MOD->getTempoDenom() == Approx(1.0));
            }
        }

        WHEN("All parameters are changed to unique values") {
            mLFOPair.LFO.setBypassSwitch(true);
            mLFOPair.LFO.setPhaseSyncSwitch(false);
            mLFOPair.LFO.setTempoSyncSwitch(true);
            mLFOPair.LFO.setInvertSwitch(true);
            mLFOPair.LFO.setWave(2);
            mLFOPair.LFO.setOutputMode(1);
            mLFOPair.LFO.setDepth(0.1);
            mLFOPair.LFO.setFreq(3);
            mLFOPair.LFO.setManualPhase(0.5);
            mLFOPair.LFO.setTempoNumer(2);
            mLFOPair.LFO.setTempoDenom(3);

            mLFOPair.MOD->setBypassSwitch(true);
            mLFOPair.MOD->setPhaseSyncSwitch(true);
            mLFOPair.MOD->setTempoSyncSwitch(true);
            mLFOPair.MOD->setInvertSwitch(true);
            mLFOPair.MOD->setWave(3);
            mLFOPair.MOD->setOutputMode(1);
            mLFOPair.MOD->setDepth(0.5);
            mLFOPair.MOD->setFreq(6);
            mLFOPair.MOD->setManualPhase(0.7);
            mLFOPair.MOD->setTempoNumer(3);
            mLFOPair.MOD->setTempoDenom(4);

            THEN("They all get their correct unique values") {
                CHECK(mLFOPair.LFO.getBypassSwitch() == true);
                CHECK(mLFOPair.LFO.getPhaseSyncSwitch() == false);
                CHECK(mLFOPair.LFO.getTempoSyncSwitch() == true);
                CHECK(mLFOPair.LFO.getInvertSwitch() == true);
                CHECK(mLFOPair.LFO.getWave() == 2);
                CHECK(mLFOPair.LFO.getOutputMode() == 1);
                CHECK(mLFOPair.LFO.getDepth() == Approx(0.1));
                CHECK(mLFOPair.LFO.getFreq() == Approx(3.0));
                CHECK(mLFOPair.LFO.getManualPhase() == Approx(0.5));
                CHECK(mLFOPair.LFO.getTempoNumer() == Approx(2.0));
                CHECK(mLFOPair.LFO.getTempoDenom() == Approx(3.0));

                CHECK(mLFOPair.MOD->getBypassSwitch() == true);
                CHECK(mLFOPair.MOD->getPhaseSyncSwitch() == true);
                CHECK(mLFOPair.MOD->getTempoSyncSwitch() == true);
                CHECK(mLFOPair.MOD->getInvertSwitch() == true);
                CHECK(mLFOPair.MOD->getWave() == 3);
                CHECK(mLFOPair.MOD->getOutputMode() == 1);
                CHECK(mLFOPair.MOD->getDepth() == Approx(0.5));
                CHECK(mLFOPair.MOD->getFreq() == Approx(6.0));
                CHECK(mLFOPair.MOD->getManualPhase() == Approx(0.7));
                CHECK(mLFOPair.MOD->getTempoNumer() == Approx(3.0));
                CHECK(mLFOPair.MOD->getTempoDenom() == Approx(4.0));
            }
        }
    }
}

SCENARIO("RichterLFOPair: Parameters enforce their bounds correctly") {
    GIVEN("A new RichterLFOPair object") {
        WECore::Richter::RichterLFOPair mLFOPair;

        WHEN("All parameter values are too low") {
            mLFOPair.LFO.setWave(-5);
            mLFOPair.LFO.setOutputMode(-5);
            mLFOPair.LFO.setDepth(-5);
            mLFOPair.LFO.setFreq(-5);
            mLFOPair.LFO.setManualPhase(-5);
            mLFOPair.LFO.setTempoNumer(-5);
            mLFOPair.LFO.setTempoDenom(-5);

            mLFOPair.MOD->setWave(-5);
            mLFOPair.MOD->setOutputMode(-5);
            mLFOPair.MOD->setDepth(-5);
            mLFOPair.MOD->setFreq(-5);
            mLFOPair.MOD->setManualPhase(-5);
            mLFOPair.MOD->setTempoNumer(-5);
            mLFOPair.MOD->setTempoDenom(-5);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mLFOPair.LFO.getWave() == 1);
                CHECK(mLFOPair.LFO.getOutputMode() == 1);
                CHECK(mLFOPair.LFO.getDepth() == Approx(0.0));
                CHECK(mLFOPair.LFO.getFreq() == Approx(0.0));
                CHECK(mLFOPair.LFO.getManualPhase() == Approx(0.0));
                CHECK(mLFOPair.LFO.getTempoNumer() == Approx(1.0));
                CHECK(mLFOPair.LFO.getTempoDenom() == Approx(1.0));

                CHECK(mLFOPair.MOD->getWave() == 1);
                CHECK(mLFOPair.MOD->getOutputMode() == 1);
                CHECK(mLFOPair.MOD->getDepth() == Approx(0.0));
                CHECK(mLFOPair.MOD->getFreq() == Approx(0.0));
                CHECK(mLFOPair.MOD->getManualPhase() == Approx(0.0));
                CHECK(mLFOPair.MOD->getTempoNumer() == Approx(1.0));
                CHECK(mLFOPair.MOD->getTempoDenom() == Approx(1.0));
            }
        }

        WHEN("All parameter values are too high") {
            mLFOPair.LFO.setWave(100);
            mLFOPair.LFO.setOutputMode(100);
            mLFOPair.LFO.setDepth(100);
            mLFOPair.LFO.setFreq(100);
            mLFOPair.LFO.setManualPhase(10000);
            mLFOPair.LFO.setTempoNumer(100);
            mLFOPair.LFO.setTempoDenom(100);

            mLFOPair.MOD->setWave(100);
            mLFOPair.MOD->setOutputMode(100);
            mLFOPair.MOD->setDepth(100);
            mLFOPair.MOD->setFreq(100);
            mLFOPair.MOD->setManualPhase(10000);
            mLFOPair.MOD->setTempoNumer(100);
            mLFOPair.MOD->setTempoDenom(100);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mLFOPair.LFO.getWave() == 4);
                CHECK(mLFOPair.LFO.getOutputMode() == 2);
                CHECK(mLFOPair.LFO.getDepth() == Approx(1.0));
                CHECK(mLFOPair.LFO.getFreq() == Approx(20.0));
                CHECK(mLFOPair.LFO.getManualPhase() == Approx(360.0));
                CHECK(mLFOPair.LFO.getTempoNumer() == Approx(32.0));
                CHECK(mLFOPair.LFO.getTempoDenom() == Approx(32.0));

                CHECK(mLFOPair.MOD->getWave() == 4);
                CHECK(mLFOPair.MOD->getOutputMode() == 2);
                CHECK(mLFOPair.MOD->getDepth() == Approx(1.0));
                CHECK(mLFOPair.MOD->getFreq() == Approx(20.0));
                CHECK(mLFOPair.MOD->getManualPhase() == Approx(360.0));
                CHECK(mLFOPair.MOD->getTempoNumer() == Approx(32.0));
                CHECK(mLFOPair.MOD->getTempoDenom() == Approx(32.0));
            }
        }
    }
}
