#include "catch.hpp"
#include "TestUtils.hpp"

#include "CrossoverState.hpp"
#include "CrossoverMutators.hpp"
#include "CrossoverProcessors.hpp"
#include "ChainMutators.hpp"

SCENARIO("CrossoverState: Clone works correctly") {
    GIVEN("A CrossoverState") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        auto crossover = createDefaultCrossoverState(hostConfig);

        // Set some unique values so we can test for them later
        CrossoverMutators::addBand(crossover);
        CrossoverMutators::setCrossoverFrequency(crossover, 0, 100);
        CrossoverMutators::setCrossoverFrequency(crossover, 1, 200);
        CrossoverMutators::setIsSoloed(crossover, 1, true);

        auto chain = std::make_shared<PluginChain>([](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.2f;
        });
        auto plugin = std::make_shared<TestUtils::TestPluginInstance>();
        ChainMutators::insertPlugin(chain, plugin, 0, hostConfig);
        CrossoverMutators::setPluginChain(crossover, 1, chain);

        // Set the host configuration (and filter states)
        CrossoverProcessors::prepareToPlay(*crossover.get(), hostConfig.sampleRate, hostConfig.blockSize, hostConfig.layout);

        // Populate the buffers
        for (size_t bufferIndex {0}; bufferIndex < crossover->buffers.size(); bufferIndex++) {
            for (size_t channelIndex {0}; channelIndex < crossover->buffers[bufferIndex].getNumChannels(); channelIndex++) {
                for (size_t sampleIndex {0}; sampleIndex < crossover->buffers[bufferIndex].getNumSamples(); sampleIndex++) {
                    crossover->buffers[bufferIndex].getWritePointer(channelIndex)[sampleIndex] = 0.5f;
                }
            }
        }

        REQUIRE(crossover->lowpassFilters.size() == 2);
        REQUIRE(crossover->highpassFilters.size() == 2);
        REQUIRE(crossover->allpassFilters.size() == 1);
        REQUIRE(crossover->buffers.size() == 2);
        REQUIRE(crossover->buffers[0].getNumChannels() == 2);
        REQUIRE(crossover->buffers[0].getNumSamples() == 10);
        REQUIRE(crossover->buffers[1].getNumChannels() == 2);
        REQUIRE(crossover->buffers[1].getNumSamples() == 10);
        REQUIRE(crossover->bands.size() == 3);

        WHEN("It is cloned") {
            CrossoverState* clonedCrossover = crossover->clone();

            THEN("The cloned crossover is equal to the original") {
                CHECK(clonedCrossover->lowpassFilters.size() == crossover->lowpassFilters.size());
                CHECK(clonedCrossover->highpassFilters.size() == crossover->highpassFilters.size());
                CHECK(clonedCrossover->allpassFilters.size() == crossover->allpassFilters.size());
                CHECK(clonedCrossover->buffers.size() == crossover->buffers.size());
                CHECK(clonedCrossover->bands.size() == crossover->bands.size());
                CHECK(clonedCrossover->config.layout == crossover->config.layout);
                CHECK(clonedCrossover->config.sampleRate == crossover->config.sampleRate);
                CHECK(clonedCrossover->config.blockSize == crossover->config.blockSize);
                CHECK(clonedCrossover->numBandsSoloed == crossover->numBandsSoloed);

                // Check the filters
                for (size_t filterIndex {0}; filterIndex < crossover->lowpassFilters.size(); filterIndex++) {
                    CHECK(clonedCrossover->lowpassFilters[filterIndex]->getType() == crossover->lowpassFilters[filterIndex]->getType());
                    CHECK(clonedCrossover->lowpassFilters[filterIndex]->getCutoffFrequency() == crossover->lowpassFilters[filterIndex]->getCutoffFrequency());
                    CHECK(clonedCrossover->highpassFilters[filterIndex]->getType() == crossover->highpassFilters[filterIndex]->getType());
                    CHECK(clonedCrossover->highpassFilters[filterIndex]->getCutoffFrequency() == crossover->highpassFilters[filterIndex]->getCutoffFrequency());
                }

                for (size_t filterIndex {0}; filterIndex < crossover->allpassFilters.size(); filterIndex++) {
                    CHECK(clonedCrossover->allpassFilters[filterIndex]->getType() == crossover->allpassFilters[filterIndex]->getType());
                    CHECK(clonedCrossover->allpassFilters[filterIndex]->getCutoffFrequency() == crossover->allpassFilters[filterIndex]->getCutoffFrequency());
                }

                // Check the buffers
                for (size_t bufferIndex {0}; bufferIndex < crossover->buffers.size(); bufferIndex++) {
                    CHECK(clonedCrossover->buffers[bufferIndex].getNumChannels() == crossover->buffers[bufferIndex].getNumChannels());
                    CHECK(clonedCrossover->buffers[bufferIndex].getNumSamples() == crossover->buffers[bufferIndex].getNumSamples());

                    for (size_t channelIndex {0}; channelIndex < crossover->buffers[bufferIndex].getNumChannels(); channelIndex++) {
                        for (size_t sampleIndex {0}; sampleIndex < crossover->buffers[bufferIndex].getNumSamples(); sampleIndex++) {
                            CHECK(clonedCrossover->buffers[bufferIndex].getReadPointer(channelIndex)[sampleIndex] == 0.5f);
                        }
                    }
                }

                // Check that modifying a cloned buffer doesn't modify the original
                clonedCrossover->buffers[0].getWritePointer(0)[0] = 0.6f;
                CHECK(crossover->buffers[0].getReadPointer(0)[0] == 0.5f);

                // Check the bands
                for (size_t bandIndex {0}; bandIndex < crossover->bands.size(); bandIndex++) {
                    CHECK(clonedCrossover->bands[bandIndex].isSoloed == crossover->bands[bandIndex].isSoloed);
                    CHECK(clonedCrossover->bands[bandIndex].chain == crossover->bands[bandIndex].chain);
                }
            }

            delete clonedCrossover;
        }
    }
}
