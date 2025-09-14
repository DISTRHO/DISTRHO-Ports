#include "catch.hpp"
#include "TestUtils.hpp"

#include "PluginSplitter.hpp"
#include "SplitterMutators.hpp"
#include "SplitterProcessors.hpp"

namespace {
    void checkSplitterCommon(PluginSplitter* original, PluginSplitter* clone) {
        // Check the chains
        REQUIRE(original->chains.size() == clone->chains.size());
        for (int chainIndex {0}; chainIndex < original->chains.size(); chainIndex++) {
            PluginChainWrapper originalChain = original->chains[chainIndex];
            PluginChainWrapper clonedChain = clone->chains[chainIndex];

            REQUIRE(originalChain.chain->chain.size() == clonedChain.chain->chain.size());
            CHECK(originalChain.isSoloed == clonedChain.isSoloed);

            for (int slotIndex {0}; slotIndex < originalChain.chain->chain.size(); slotIndex++) {
                std::shared_ptr<ChainSlotBase> originalSlot = originalChain.chain->chain[slotIndex];
                std::shared_ptr<ChainSlotBase> clonedSlot = clonedChain.chain->chain[slotIndex];

                if (std::shared_ptr<ChainSlotPlugin> originalPluginSlot = std::dynamic_pointer_cast<ChainSlotPlugin>(originalSlot)) {
                    // It's a plugin slot, check the plugin
                    auto clonedPluginSlot = std::dynamic_pointer_cast<ChainSlotPlugin>(clonedSlot);
                    REQUIRE(clonedPluginSlot != nullptr);
                    CHECK(clonedPluginSlot->plugin == originalPluginSlot->plugin);
                    CHECK(clonedPluginSlot->modulationConfig != originalPluginSlot->modulationConfig);
                } else {
                    // It's a gain stage, check the values
                    auto originalGainStage = std::dynamic_pointer_cast<ChainSlotGainStage>(originalSlot);
                    auto clonedGainStage = std::dynamic_pointer_cast<ChainSlotGainStage>(clonedSlot);
                    REQUIRE(originalGainStage != nullptr);
                    REQUIRE(clonedGainStage != nullptr);

                    CHECK(clonedGainStage->gain == originalGainStage->gain);
                    CHECK(clonedGainStage->pan == originalGainStage->pan);
                }
            }
        }

        CHECK(original->numChainsSoloed == clone->numChainsSoloed);
        CHECK(original->config.layout == clone->config.layout);
        CHECK(original->config.sampleRate == clone->config.sampleRate);
        CHECK(original->config.blockSize == clone->config.blockSize);
        CHECK(original->getModulationValueCallback(0, MODULATION_TYPE::MACRO) == 1.2f);
        CHECK(clone->getModulationValueCallback(0, MODULATION_TYPE::MACRO) == 1.2f);
        CHECK(clone->shouldNotifyProcessorOnLatencyChange == true);
    }

    void checkBuffers(juce::AudioBuffer<float>& buffer1, juce::AudioBuffer<float>& buffer2) {
        // Check the buffers are the same size
        REQUIRE(buffer1.getNumChannels() == buffer2.getNumChannels());
        REQUIRE(buffer1.getNumSamples() == buffer2.getNumSamples());

        // Check the buffers have the same values
        for (int channelIndex {0}; channelIndex < buffer1.getNumChannels(); channelIndex++) {
            for (int sampleIndex {0}; sampleIndex < buffer1.getNumSamples(); sampleIndex++) {
                CHECK(buffer1.getReadPointer(channelIndex)[sampleIndex] == buffer2.getReadPointer(channelIndex)[sampleIndex]);
            }
        }

        // Mofify one of the buffers, check they're different
        buffer1.getWritePointer(0)[0] = 0.5f;
        CHECK(buffer1.getReadPointer(0)[0] != buffer2.getReadPointer(0)[0]);
    }
}

SCENARIO("PluginSplitterSeries: Clone works correctly") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A splitter with one plugin slot and one gain stage") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for it later
            return 1.2f;
        };

        int latencyCalled {0};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled++;
            receivedLatency = latency;
        };

        auto splitter = std::make_shared<PluginSplitterSeries>(hostConfig, modulationCallback, latencyCallback);

        auto plugin = std::make_shared<TestUtils::TestPluginInstance>();
        SplitterMutators::insertPlugin(splitter, plugin, 0, 0);
        SplitterMutators::insertGainStage(splitter, 0, 1);

        // Set some unique values so we can test for them later
        SplitterMutators::setChainSolo(splitter, 0, true);

        // Reset before starting the tests
        latencyCalled = 0;
        receivedLatency = 0;

        WHEN("It is cloned") {
            PluginSplitterSeries* clonedSplitter = splitter->clone();
            splitter->shouldNotifyProcessorOnLatencyChange = false;

            THEN("The cloned splitter is equal to the original") {
                checkSplitterCommon(splitter.get(), clonedSplitter);

                // Check that when a plugin updates its latency, the latency callback is called from
                // the new splitter only
                plugin->setLatencySamples(15);
                messageManager->runDispatchLoopUntil(10);
                CHECK(latencyCalled == 1);
                CHECK(receivedLatency == 15);
            }

            delete clonedSplitter;
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("PluginSplitterParallel: Clone works correctly") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A splitter with one plugin slot and one gain stage") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for it later
            return 1.2f;
        };

        int latencyCalled {0};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled++;
            receivedLatency = latency;
        };

        auto splitter = std::make_shared<PluginSplitterParallel>(hostConfig, modulationCallback, latencyCallback);

        auto plugin = std::make_shared<TestUtils::TestPluginInstance>();
        SplitterMutators::insertPlugin(splitter, plugin, 0, 0);
        SplitterMutators::addChain(splitter);
        SplitterMutators::insertGainStage(splitter, 1, 1);

        // Set some unique values so we can test for them later
        SplitterMutators::setChainSolo(splitter, 0, true);

        // Initialise the buffers
        SplitterProcessors::prepareToPlay(*splitter.get(), hostConfig.sampleRate, hostConfig.blockSize, hostConfig.layout);

        // Reset before starting the tests
        latencyCalled = 0;
        receivedLatency = 0;

        WHEN("It is cloned") {
            PluginSplitterParallel* clonedSplitter = splitter->clone();
            splitter->shouldNotifyProcessorOnLatencyChange = false;

            THEN("The cloned splitter is equal to the original") {
                checkSplitterCommon(splitter.get(), clonedSplitter);

                // Check that when a plugin updates its latency, the latency callback is called from
                // the new splitter only
                plugin->setLatencySamples(15);
                messageManager->runDispatchLoopUntil(10);
                CHECK(latencyCalled == 1);
                CHECK(receivedLatency == 15);

                // Check that the buffers are equal
                checkBuffers(*splitter->inputBuffer, *clonedSplitter->inputBuffer);
                checkBuffers(*splitter->outputBuffer, *clonedSplitter->outputBuffer);
            }

            delete clonedSplitter;
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("PluginSplitterMultiband: Clone works correctly") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A splitter with one plugin slot and one gain stage") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for it later
            return 1.2f;
        };

        int latencyCalled {0};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled++;
            receivedLatency = latency;
        };

        auto splitter = std::make_shared<PluginSplitterMultiband>(hostConfig, modulationCallback, latencyCallback);
        // Workaround for having constructed the multiband splitter directly
        CrossoverMutators::setPluginChain(splitter->crossover, 0, splitter->chains[0].chain);
        CrossoverMutators::setPluginChain(splitter->crossover, 1, splitter->chains[1].chain);

        auto plugin = std::make_shared<TestUtils::TestPluginInstance>();
        SplitterMutators::insertPlugin(splitter, plugin, 0, 0);
        SplitterMutators::addBand(splitter);
        SplitterMutators::insertGainStage(splitter, 1, 1);

        // Set some unique values so we can test for them later
        SplitterMutators::setChainSolo(splitter, 0, true);
        SplitterMutators::setCrossoverFrequency(splitter, 0, 150);
        SplitterMutators::setCrossoverFrequency(splitter, 1, 250);

        // Initialise the buffers
        SplitterProcessors::prepareToPlay(*splitter.get(), hostConfig.sampleRate, hostConfig.blockSize, hostConfig.layout);

        // Reset before starting the tests
        latencyCalled = 0;
        receivedLatency = 0;

        // Get a reference to the original crossover state so we can check which splitter it's in
        // after cloning
        std::shared_ptr<PluginChain> originalChain = splitter->chains[0].chain;

        REQUIRE(splitter->chains.size() == 3);
        REQUIRE(splitter->chains[0].chain->chain.size() == 1);
        REQUIRE(splitter->chains[1].chain->chain.size() == 1);

        WHEN("It is cloned") {
            PluginSplitterMultiband* clonedSplitter = splitter->clone();
            splitter->shouldNotifyProcessorOnLatencyChange = false;

            THEN("The cloned splitter is equal to the original") {
                checkSplitterCommon(splitter.get(), clonedSplitter);

                // Check that when a plugin updates its latency, the latency callback is called from
                // the new splitter only
                plugin->setLatencySamples(15);
                messageManager->runDispatchLoopUntil(10);
                CHECK(latencyCalled == 1);
                CHECK(receivedLatency == 15);

                // Check the crossover is equal but not the same
                CHECK(splitter->crossover != clonedSplitter->crossover);
                CHECK(splitter->crossover->bands[0].chain == originalChain);
                CHECK(clonedSplitter->crossover->bands[0].chain != originalChain);
                CHECK(splitter->crossover->lowpassFilters.size() == clonedSplitter->crossover->lowpassFilters.size());
                CHECK(splitter->crossover->highpassFilters.size() == clonedSplitter->crossover->highpassFilters.size());
                CHECK(splitter->crossover->allpassFilters.size() == clonedSplitter->crossover->allpassFilters.size());
                CHECK(splitter->crossover->buffers.size() == clonedSplitter->crossover->buffers.size());
                CHECK(splitter->crossover->bands.size() == clonedSplitter->crossover->bands.size());
                CHECK(splitter->crossover->config.layout == clonedSplitter->crossover->config.layout);
                CHECK(splitter->crossover->config.sampleRate == clonedSplitter->crossover->config.sampleRate);
                CHECK(splitter->crossover->config.blockSize == clonedSplitter->crossover->config.blockSize);
                CHECK(splitter->crossover->numBandsSoloed == clonedSplitter->crossover->numBandsSoloed);

                for (int filterIndex {0}; filterIndex < splitter->crossover->lowpassFilters.size(); filterIndex++) {
                    CHECK(splitter->crossover->lowpassFilters[filterIndex]->getType() == clonedSplitter->crossover->lowpassFilters[filterIndex]->getType());
                    CHECK(splitter->crossover->lowpassFilters[filterIndex]->getCutoffFrequency() == clonedSplitter->crossover->lowpassFilters[filterIndex]->getCutoffFrequency());
                    CHECK(splitter->crossover->highpassFilters[filterIndex]->getType() == clonedSplitter->crossover->highpassFilters[filterIndex]->getType());
                    CHECK(splitter->crossover->highpassFilters[filterIndex]->getCutoffFrequency() == clonedSplitter->crossover->highpassFilters[filterIndex]->getCutoffFrequency());
                }

                for (int filterIndex {0}; filterIndex < splitter->crossover->allpassFilters.size(); filterIndex++) {
                    CHECK(splitter->crossover->allpassFilters[filterIndex]->getType() == clonedSplitter->crossover->allpassFilters[filterIndex]->getType());
                    CHECK(splitter->crossover->allpassFilters[filterIndex]->getCutoffFrequency() == clonedSplitter->crossover->allpassFilters[filterIndex]->getCutoffFrequency());
                }
            }

            delete clonedSplitter;
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("PluginSplitterLeftRight: Clone works correctly") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A splitter with one plugin slot and one gain stage") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for it later
            return 1.2f;
        };

        int latencyCalled {0};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled++;
            receivedLatency = latency;
        };

        auto splitter = std::make_shared<PluginSplitterLeftRight>(hostConfig, modulationCallback, latencyCallback);

        auto plugin = std::make_shared<TestUtils::TestPluginInstance>();
        SplitterMutators::insertPlugin(splitter, plugin, 0, 0);
        SplitterMutators::insertGainStage(splitter, 1, 1);

        // Set some unique values so we can test for them later
        SplitterMutators::setChainSolo(splitter, 0, true);

        // Initialise the buffers
        SplitterProcessors::prepareToPlay(*splitter.get(), hostConfig.sampleRate, hostConfig.blockSize, hostConfig.layout);

        // Reset before starting the tests
        latencyCalled = 0;
        receivedLatency = 0;

        WHEN("It is cloned") {
            PluginSplitterLeftRight* clonedSplitter = splitter->clone();
            splitter->shouldNotifyProcessorOnLatencyChange = false;

            THEN("The cloned splitter is equal to the original") {
                checkSplitterCommon(splitter.get(), clonedSplitter);

                // Check that when a plugin updates its latency, the latency callback is called from
                // the new splitter only
                plugin->setLatencySamples(15);
                messageManager->runDispatchLoopUntil(10);
                CHECK(latencyCalled == 1);
                CHECK(receivedLatency == 15);

                // Check that the buffers are equal
                checkBuffers(*splitter->leftBuffer, *clonedSplitter->leftBuffer);
                checkBuffers(*splitter->rightBuffer, *clonedSplitter->rightBuffer);
            }

            delete clonedSplitter;
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("PluginSplitterMidSide: Clone works correctly") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A splitter with one plugin slot and one gain stage") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for it later
            return 1.2f;
        };

        int latencyCalled {0};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled++;
            receivedLatency = latency;
        };

        auto splitter = std::make_shared<PluginSplitterMidSide>(hostConfig, modulationCallback, latencyCallback);

        auto plugin = std::make_shared<TestUtils::TestPluginInstance>();
        SplitterMutators::insertPlugin(splitter, plugin, 0, 0);
        SplitterMutators::insertGainStage(splitter, 1, 1);

        // Set some unique values so we can test for them later
        SplitterMutators::setChainSolo(splitter, 0, true);

        // Initialise the buffers
        SplitterProcessors::prepareToPlay(*splitter.get(), hostConfig.sampleRate, hostConfig.blockSize, hostConfig.layout);

        // Reset before starting the tests
        latencyCalled = 0;
        receivedLatency = 0;

        WHEN("It is cloned") {
            PluginSplitterMidSide* clonedSplitter = splitter->clone();

            THEN("The cloned splitter is equal to the original") {
                checkSplitterCommon(splitter.get(), clonedSplitter);
                splitter->shouldNotifyProcessorOnLatencyChange = false;

                // Check that when a plugin updates its latency, the latency callback is called from
                // the new splitter only
                plugin->setLatencySamples(15);
                messageManager->runDispatchLoopUntil(10);
                CHECK(latencyCalled == 1);
                CHECK(receivedLatency == 15);

                // Check that the buffers are equal
                checkBuffers(*splitter->midBuffer, *clonedSplitter->midBuffer);
                checkBuffers(*splitter->sideBuffer, *clonedSplitter->sideBuffer);
            }

            delete clonedSplitter;
        }
    }

    juce::MessageManager::deleteInstance();
}