#include "catch.hpp"
#include "TestUtils.hpp"

#include "CloneableLRFilter.hpp"

SCENARIO("CloneableLRFilter: Clone works correctly") {
    GIVEN("A CloneableLRFilter") {
        CloneableLRFilter<float> filter;

        // Set some unique values so we can test for them later
        filter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
        filter.setCutoffFrequency(1000);

        // Populate the state with some unique values so we can test for them later
        constexpr int numFilterChannels {2};
        filter.prepare({48000, 64, numFilterChannels});
        juce::AudioBuffer<float> buffer(numFilterChannels, 64);

        for (int channel {0}; channel < numFilterChannels; ++channel) {
            for (int sample {0}; sample < buffer.getNumSamples(); ++sample) {
                buffer.setSample(channel, sample, (sample % 10) / 10.0f);
            }
        }

        juce::dsp::AudioBlock<float> block(juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(0, numFilterChannels));
        juce::dsp::ProcessContextReplacing context(block);
        filter.process(context);

        WHEN("It is cloned") {
            auto clonedFilter = filter.clone();

            THEN("The cloned filter is equal to the original") {
                CHECK(clonedFilter->g == filter.g);
                CHECK(clonedFilter->R2 == filter.R2);
                CHECK(clonedFilter->h == filter.h);
                CHECK(clonedFilter->s1 == filter.s1);
                CHECK(clonedFilter->s2 == filter.s2);
                CHECK(clonedFilter->s3 == filter.s3);
                CHECK(clonedFilter->s4 == filter.s4);
                CHECK(clonedFilter->sampleRate == filter.sampleRate);
                CHECK(clonedFilter->cutoffFrequency == filter.cutoffFrequency);
                CHECK(clonedFilter->filterType == filter.filterType);
            }
            
            delete clonedFilter;
        }
    }
}