#include "catch.hpp"
#include "TestUtils.hpp"

#include "CloneableSources.hpp"

SCENARIO("CloneableLFO: Clone works correctly") {
    GIVEN("A CloneableLFO") {
        ModelInterface::CloneableLFO lfo;
        const auto freqSourceLFO = std::make_shared<ModelInterface::CloneableLFO>();
        const auto depthSourceLFO = std::make_shared<ModelInterface::CloneableLFO>();
        const auto phaseSourceLFO = std::make_shared<ModelInterface::CloneableLFO>();

        // Set some unique values so we can test for them later
        lfo.setBypassSwitch(true);
        lfo.setPhaseSyncSwitch(true);
        lfo.setTempoSyncSwitch(true);
        lfo.setInvertSwitch(true);
        lfo.setWave(WECore::Richter::Parameters::WAVE.SQUARE);
        lfo.setOutputMode(WECore::Richter::Parameters::OUTPUTMODE.UNIPOLAR);
        lfo.setTempoNumer(2);
        lfo.setTempoDenom(3);
        lfo.setFreq(4.5);
        lfo.setDepth(0.7);
        lfo.setManualPhase(250);
        lfo.setSampleRate(48000);
        lfo.addFreqModulationSource(freqSourceLFO);
        lfo.setFreqModulationAmount(0, 0.3);
        lfo.addDepthModulationSource(depthSourceLFO);
        lfo.setDepthModulationAmount(0, 0.4);
        lfo.addPhaseModulationSource(phaseSourceLFO);
        lfo.setPhaseModulationAmount(0, 0.5);

        // Set up some internal state
        lfo.prepareForNextBuffer(110, 5);
        lfo.getNextOutput(0.5);
        lfo.getNextOutput(0.5);
        lfo.getNextOutput(0.5);

        WHEN("It is cloned") {
            ModelInterface::CloneableLFO* clonedLFO = lfo.clone();

            THEN("The cloned LFO is equal to the original") {
                CHECK(clonedLFO->getBypassSwitch() == lfo.getBypassSwitch());
                CHECK(clonedLFO->getPhaseSyncSwitch() == lfo.getPhaseSyncSwitch());
                CHECK(clonedLFO->getTempoSyncSwitch() == lfo.getTempoSyncSwitch());
                CHECK(clonedLFO->getInvertSwitch() == lfo.getInvertSwitch());
                CHECK(clonedLFO->getWave() == lfo.getWave());
                CHECK(clonedLFO->getOutputMode() == lfo.getOutputMode());
                CHECK(clonedLFO->getTempoNumer() == lfo.getTempoNumer());
                CHECK(clonedLFO->getTempoDenom() == lfo.getTempoDenom());
                CHECK(clonedLFO->getFreq() == lfo.getFreq());
                CHECK(clonedLFO->getDepth() == lfo.getDepth());
                CHECK(clonedLFO->getManualPhase() == lfo.getManualPhase());
                // CHECK(clonedLFO->getSampleRate() == lfo.getSampleRate());
                CHECK(clonedLFO->getLastOutput() == lfo.getLastOutput());
                CHECK(clonedLFO->getNextOutput(0.5) == lfo.getNextOutput(0.5));
                REQUIRE(clonedLFO->getFreqModulationSources().size() == 1);
                CHECK(clonedLFO->getFreqModulationSources()[0].source == freqSourceLFO);
                CHECK(clonedLFO->getFreqModulationSources()[0].amount == 0.3);
                REQUIRE(clonedLFO->getFreqModulationSources().size() == 1);
                CHECK(clonedLFO->getDepthModulationSources()[0].source == depthSourceLFO);
                CHECK(clonedLFO->getDepthModulationSources()[0].amount == 0.4);
                REQUIRE(clonedLFO->getPhaseModulationSources().size() == 1);
                CHECK(clonedLFO->getPhaseModulationSources()[0].source == phaseSourceLFO);
                CHECK(clonedLFO->getPhaseModulationSources()[0].amount == 0.5);
            }

            delete clonedLFO;
        }
    }
}

SCENARIO("CloneableEnvelopeFollower: Clone works correctly") {
    GIVEN("A CloneableEnvelopeFollower") {
        ModelInterface::CloneableEnvelopeFollower envelope;

        // Set some unique values so we can test for them later
        envelope.setSampleRate(48000);
        envelope.setAttackTimeMs(1.2);
        envelope.setReleaseTimeMs(3.4);
        envelope.setFilterEnabled(true);
        envelope.setLowCutHz(21);
        envelope.setHighCutHz(530);

        // Set up some internal state
        envelope.getNextOutput(0.4);
        envelope.getNextOutput(0.5);
        envelope.getNextOutput(0.8);

        WHEN("It is cloned") {
            ModelInterface::CloneableEnvelopeFollower* clonedEnvelope = envelope.clone();

            THEN("The cloned envelope is equal to the original") {
                CHECK(clonedEnvelope->getAttackTimeMs() == envelope.getAttackTimeMs());
                CHECK(clonedEnvelope->getReleaseTimeMs() == envelope.getReleaseTimeMs());
                CHECK(clonedEnvelope->getFilterEnabled() == envelope.getFilterEnabled());
                CHECK(clonedEnvelope->getLowCutHz() == envelope.getLowCutHz());
                CHECK(clonedEnvelope->getHighCutHz() == envelope.getHighCutHz());
                // CHECK(clonedEnvelope->getSampleRate() == envelope.getSampleRate());
                CHECK(clonedEnvelope->getLastOutput() == envelope.getLastOutput());
                CHECK(clonedEnvelope->getNextOutput(0.5) == envelope.getNextOutput(0.5));
            }

            delete clonedEnvelope;
        }
    }
}
