#include "catch.hpp"
#include "TestUtils.hpp"

#include "CloneableDelayLine.hpp"

SCENARIO("CloneableDelayLine: Clone works correctly") {
    GIVEN("A CloneableDelayLine") {
        CloneableDelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> delayLine(0);

        // Set some unique values so we can test for them later
        delayLine.setDelay(1000);

        // Populate the state with some unique values so we can test for them later
        constexpr int numFilterChannels {2};
        delayLine.prepare({48000, 64, numFilterChannels});
        juce::AudioBuffer<float> buffer(numFilterChannels, 64);

        for (int channel {0}; channel < numFilterChannels; ++channel) {
            for (int sample {0}; sample < buffer.getNumSamples(); ++sample) {
                buffer.setSample(channel, sample, (sample % 10) / 10.0f);
            }
        }

        juce::dsp::AudioBlock<float> block(juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(0, numFilterChannels));
        juce::dsp::ProcessContextReplacing context(block);
        delayLine.process(context);

        WHEN("It is cloned") {
            auto clonedDelayLine = delayLine.clone();

            THEN("The cloned delay line is equal to the original") {
                CHECK(clonedDelayLine->sampleRate == delayLine.sampleRate);
                CHECK(clonedDelayLine->bufferData == delayLine.bufferData);
                CHECK(clonedDelayLine->v == delayLine.v);
                CHECK(clonedDelayLine->writePos == delayLine.writePos);
                CHECK(clonedDelayLine->readPos == delayLine.readPos);
                CHECK(clonedDelayLine->delay == delayLine.delay);
                CHECK(clonedDelayLine->delayFrac == delayLine.delayFrac);
                CHECK(clonedDelayLine->delayInt == delayLine.delayInt);
                CHECK(clonedDelayLine->totalSize == delayLine.totalSize);
                CHECK(clonedDelayLine->alpha == delayLine.alpha);
            }

            delete clonedDelayLine;
        }
    }
}