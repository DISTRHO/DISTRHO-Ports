#include "catch.hpp"
#include "TestUtils.hpp"
#include "SplitterMutators.hpp"

namespace {
    constexpr int SAMPLE_RATE {44100};
    constexpr int NUM_SAMPLES {64};

    class MutatorTestPluginInstance : public TestUtils::TestPluginInstance {
    public:
        MutatorTestPluginInstance() = default;
    };
}

SCENARIO("SplitterMutators: Chains and slots can be added, replaced, and removed") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("An empty splitter") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        auto splitterSeries = std::make_shared<PluginSplitterSeries>(config, modulationCallback, latencyCallback);

        REQUIRE(splitterSeries->chains[0].chain->latencyListener.calculatedTotalPluginLatency == 0);

        // Catch2 resets the state if we use multiple WHEN clauses. We could use AND_WHEN, but they
        // need to be nested which makes then hard to read, so instead we use a single WHEN.
        WHEN("The splitter is modified") {
            // WHEN("A plugin is added")
            {
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(10);

                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterSeries);
                bool isSuccess = SplitterMutators::insertPlugin(splitter, plugin, 0, 0);

                // THEN("The splitter contains a single chain with a single plugin")
                CHECK(isSuccess);
                CHECK(splitterSeries->chains.size() == 1);
                CHECK(splitterSeries->chains[0].chain->chain.size() == 1);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 0) == plugin);
                CHECK(latencyCalled);
                CHECK(receivedLatency == 10);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A plugin is added at chainNumber > chains.size()")
            {
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(10);

                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterSeries);
                bool isSuccess = SplitterMutators::insertPlugin(splitter, plugin, 10, 0);

                // THEN("Nothing changes")
                CHECK(!isSuccess);
                CHECK(splitterSeries->chains.size() == 1);
                CHECK(splitterSeries->chains[0].chain->chain.size() == 1);
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A gain stage is added")
            {
                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterSeries);
                bool isSuccess = SplitterMutators::insertGainStage(splitter, 0, 1);

                // THEN("The splitter contains a single chain with a plugin and gain stage")
                CHECK(isSuccess);
                CHECK(splitterSeries->chains.size() == 1);
                CHECK(splitterSeries->chains[0].chain->chain.size() == 2);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 1) == nullptr);
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A gain stage is added at chainNumber > chains.size()")
            {
                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterSeries);
                bool isSuccess = SplitterMutators::insertGainStage(splitter, 10, 0);

                // THEN("Nothing changes")
                CHECK(!isSuccess);
                CHECK(splitterSeries->chains.size() == 1);
                CHECK(splitterSeries->chains[0].chain->chain.size() == 2);
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A two chains are added")
            auto splitterParallel = std::make_shared<PluginSplitterParallel>(std::dynamic_pointer_cast<PluginSplitter>(splitterSeries));
            splitterSeries.reset();
            {
                SplitterMutators::addChain(splitterParallel);
                SplitterMutators::addChain(splitterParallel);

                // THEN("The splitter contains a chain with two slots, and two empty chains")
                CHECK(splitterParallel->chains.size() == 3);
                CHECK(splitterParallel->chains[0].chain->chain.size() == 2);
                CHECK(splitterParallel->chains[1].chain->chain.size() == 0);
                CHECK(splitterParallel->chains[2].chain->chain.size() == 0);
                CHECK(latencyCalled);
                CHECK(receivedLatency == 10);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A plugin is replaced in the middle chain")
            {
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(15);

                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);
                bool isSuccess = SplitterMutators::replacePlugin(splitter, plugin, 1, 0);

                // THEN("The plugin is added to the middle chain")
                CHECK(isSuccess);
                CHECK(splitterParallel->chains.size() == 3);
                CHECK(splitterParallel->chains[0].chain->chain.size() == 2);
                CHECK(splitterParallel->chains[1].chain->chain.size() == 1);
                CHECK(splitterParallel->chains[2].chain->chain.size() == 0);
                CHECK(latencyCalled);
                CHECK(receivedLatency == 15);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A plugin changes latency")
            {
                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);
                auto plugin = SplitterMutators::getPlugin(splitter, 1, 0);
                plugin->setLatencySamples(30);

                // Allow the latency message to be sent
                messageManager->runDispatchLoopUntil(10);

                // THEN("The latency is updated")
                CHECK(latencyCalled);
                CHECK(receivedLatency == 30);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A gain stage is removed in the first chain")
            {
                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);
                bool isSuccess = SplitterMutators::removeSlot(splitter, 0, 1);

                // THEN("The first chain has only one plugin left")
                CHECK(isSuccess);
                CHECK(splitterParallel->chains.size() == 3);
                CHECK(splitterParallel->chains[0].chain->chain.size() == 1);
                CHECK(splitterParallel->chains[1].chain->chain.size() == 1);
                CHECK(splitterParallel->chains[2].chain->chain.size() == 0);
                CHECK(latencyCalled);
                CHECK(receivedLatency == 30);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A plugin is removed in the middle chain")
            {
                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);
                bool isSuccess = SplitterMutators::removeSlot(splitter, 1, 0);

                // THEN("The middle chain is empty")
                CHECK(isSuccess);
                CHECK(splitterParallel->chains.size() == 3);
                CHECK(splitterParallel->chains[0].chain->chain.size() == 1);
                CHECK(splitterParallel->chains[1].chain->chain.size() == 0);
                CHECK(splitterParallel->chains[2].chain->chain.size() == 0);
                CHECK(latencyCalled);
                CHECK(receivedLatency == 10);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A chain with a gain stage is removed")
            {
                auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);
                bool isSuccess = SplitterMutators::insertGainStage(splitter, 1, 0);

                REQUIRE(isSuccess);
                REQUIRE(splitterParallel->chains.size() == 3);
                REQUIRE(splitterParallel->chains[1].chain->chain.size() == 1);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;

                isSuccess = SplitterMutators::removeChain(splitterParallel, 1);

                // THEN("The correct chain is removed")
                CHECK(isSuccess);
                CHECK(splitterParallel->chains.size() == 2);
                CHECK(splitterParallel->chains[0].chain->chain.size() == 1);
                CHECK(splitterParallel->chains[1].chain->chain.size() == 0);
                CHECK(latencyCalled);
                CHECK(receivedLatency == 10);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            // WHEN("A chain is removed at chainNumber > chains.size()")
            {
                bool isSuccess = SplitterMutators::removeChain(splitterParallel, 10);

                // THEN("The nothing changes")
                CHECK(!isSuccess);
                CHECK(splitterParallel->chains.size() == 2);
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }

            WHEN("A too many chains are removed")
            {
                bool isSuccess = SplitterMutators::removeChain(splitterParallel, 1);

                REQUIRE(isSuccess);
                REQUIRE(splitterParallel->chains.size() == 1);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;

                isSuccess = SplitterMutators::removeChain(splitterParallel, 0);

                // THEN("The nothing changes")
                CHECK(!isSuccess);
                CHECK(splitterParallel->chains.size() == 1);
                CHECK(splitterParallel->chains[0].chain->chain.size() == 1);
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Reset
                latencyCalled = false;
                receivedLatency = 0;
            }
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("SplitterMutators: Slots can be moved") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A splitter with a single plugin in a single chain") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        auto splitterSeries = std::make_shared<PluginSplitterSeries>(config, modulationCallback, latencyCallback);

        auto plugin = std::make_shared<MutatorTestPluginInstance>();
        plugin->setLatencySamples(10);

        auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterSeries);
        bool isSuccess = SplitterMutators::insertPlugin(splitter, plugin, 0, 0);

        REQUIRE(isSuccess);
        REQUIRE(splitterSeries->chains[0].chain->latencyListener.calculatedTotalPluginLatency == 10);
        REQUIRE(splitterSeries->chains.size() == 1);
        REQUIRE(splitterSeries->chains[0].chain->chain.size() == 1);
        REQUIRE(SplitterMutators::getPlugin(splitter, 0, 0) == plugin);
        REQUIRE(latencyCalled);
        REQUIRE(receivedLatency == 10);

        // Reset
        latencyCalled = false;
        receivedLatency = 0;

        WHEN("The slot is moved to the same position") {
            const bool success {SplitterMutators::moveSlot(splitter, 0, 0, 0, 0)};

            THEN("Nothing changes") {
                CHECK(success);
                CHECK(splitterSeries->chains.size() == 1);
                CHECK(splitterSeries->chains[0].chain->chain.size() == 1);
                CHECK(SplitterMutators::getPlugin(splitterSeries, 0, 0) == plugin);
                CHECK(splitterSeries->chains[0].isSoloed == false);
                CHECK(latencyCalled);
                CHECK(receivedLatency == 10);
            }
        }

        WHEN("The slot is moved to a position > chain.size()") {
            const bool success {SplitterMutators::moveSlot(splitter, 0, 0, 0, 10)};

            THEN("Nothing changes") {
                CHECK(success);
                CHECK(splitterSeries->chains.size() == 1);
                CHECK(splitterSeries->chains[0].chain->chain.size() == 1);
                CHECK(SplitterMutators::getPlugin(splitterSeries, 0, 0) == plugin);
                CHECK(splitterSeries->chains[0].isSoloed == false);
                CHECK(latencyCalled);
                CHECK(receivedLatency == 10);
            }
        }
    }

    GIVEN("A parallel splitter with a two chains") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        auto splitterParallel = std::make_shared<PluginSplitterParallel>(config, modulationCallback, latencyCallback);
        auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);

        // Add one more chain/band
        SplitterMutators::addChain(splitterParallel);

        // Add three plugins and a gain stage to each chain
        std::vector<std::vector<std::shared_ptr<MutatorTestPluginInstance>>> plugins;
        for (int chainNumber {0}; chainNumber < splitterParallel->chains.size(); chainNumber++) {
            plugins.push_back(std::vector<std::shared_ptr<MutatorTestPluginInstance>>());
            int totalLatency {0};

            for (int pluginNumber {0}; pluginNumber < 3; pluginNumber++) {
                auto thisPlugin = std::make_shared<MutatorTestPluginInstance>();
                const int latency {10 * (pluginNumber + 1)};
                totalLatency += latency;
                thisPlugin->setLatencySamples(latency);
                plugins[chainNumber].push_back(thisPlugin);

                bool isSuccess = SplitterMutators::insertPlugin(splitter, thisPlugin, chainNumber, pluginNumber);
                REQUIRE(splitterParallel->chains[chainNumber].chain->latencyListener.calculatedTotalPluginLatency == totalLatency);
                REQUIRE(splitterParallel->chains[chainNumber].chain->chain.size() == pluginNumber + 1);
                REQUIRE(SplitterMutators::getPlugin(splitter, chainNumber, pluginNumber) == thisPlugin);
                REQUIRE(latencyCalled);
                REQUIRE(receivedLatency == (chainNumber == 0 ? totalLatency : 60));
            }

            bool isSuccess = SplitterMutators::insertGainStage(splitter, chainNumber, 3);
            REQUIRE(splitterParallel->chains[chainNumber].chain->chain.size() == 4);
            REQUIRE(SplitterMutators::getPlugin(splitter, chainNumber, 3) == nullptr);
        }

        REQUIRE(splitterParallel->chains.size() == 2);

        // Reset
        latencyCalled = false;
        receivedLatency = 0;

        WHEN("The middle slot is moved to the same position") {
            PluginModulationConfig config;
            config.isActive = true;

            auto paramConfig = std::make_shared<PluginParameterModulationConfig>();
            paramConfig->targetParameterName = "testParam";
            config.parameterConfigs.push_back(paramConfig);

            SplitterMutators::setSlotBypass(splitter, 0, 1, true);
            SplitterMutators::setPluginModulationConfig(splitter, config, 0, 1);
            const bool success {SplitterMutators::moveSlot(splitter, 0, 1, 0, 1)};

            THEN("Nothing changes") {
                CHECK(success);
                REQUIRE(splitterParallel->chains.size() == 2);

                for (int chainNumber {0}; chainNumber < 2; chainNumber++) {
                    CHECK(splitterParallel->chains[chainNumber].chain->chain.size() == 4);
                }

                CHECK(latencyCalled);
                CHECK(receivedLatency == 60);

                // Check bypass
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 0) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 1) == true);
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 2) == false);

                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 0) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 1) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 2) == false);

                // Check the slots
                CHECK(SplitterMutators::getPlugin(splitter, 0, 0) == plugins[0][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 1) == plugins[0][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 2) == plugins[0][2]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 3) == nullptr);

                CHECK(SplitterMutators::getPlugin(splitter, 1, 0) == plugins[1][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 1) == plugins[1][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 2) == plugins[1][2]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 3) == nullptr);

                // Check the modulation config
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 0).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 1).isActive == true);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 1).parameterConfigs[0]->targetParameterName == "testParam");
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 2).isActive == false);

                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 0).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 1).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 2).isActive == false);
            }
        }

        WHEN("A slot is moved from a lower to the next position up in the same chain") {
            PluginModulationConfig config;
            config.isActive = true;

            auto paramConfig = std::make_shared<PluginParameterModulationConfig>();
            paramConfig->targetParameterName = "testParam";
            config.parameterConfigs.push_back(paramConfig);

            SplitterMutators::setSlotBypass(splitter, 0, 1, true);
            SplitterMutators::setPluginModulationConfig(splitter, config, 0, 1);
            const bool success {SplitterMutators::moveSlot(splitter, 0, 1, 0, 2)};

            THEN("Nothing changes") { // Not immediately obvious, but this is correct as the chain is already in the right place
                CHECK(success);
                REQUIRE(splitterParallel->chains.size() == 2);

                for (int chainNumber {0}; chainNumber < 2; chainNumber++) {
                    CHECK(splitterParallel->chains[chainNumber].chain->chain.size() == 4);
                }

                CHECK(latencyCalled);
                CHECK(receivedLatency == 60);

                // Check bypass
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 0) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 1) == true);
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 2) == false);

                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 0) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 1) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 2) == false);

                // Check the slots
                CHECK(SplitterMutators::getPlugin(splitter, 0, 0) == plugins[0][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 1) == plugins[0][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 2) == plugins[0][2]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 3) == nullptr);

                CHECK(SplitterMutators::getPlugin(splitter, 1, 0) == plugins[1][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 1) == plugins[1][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 2) == plugins[1][2]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 3) == nullptr);

                // Check the modulation config
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 0).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 1).isActive == true);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 1).parameterConfigs[0]->targetParameterName == "testParam");
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 2).isActive == false);

                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 0).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 1).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 2).isActive == false);
            }
        }

        WHEN("A slot is moved from a lower to a higher position in the same chain") {
            PluginModulationConfig config;
            config.isActive = true;

            auto paramConfig = std::make_shared<PluginParameterModulationConfig>();
            paramConfig->targetParameterName = "testParam";
            config.parameterConfigs.push_back(paramConfig);

            SplitterMutators::setSlotBypass(splitter, 0, 0, true);
            SplitterMutators::setPluginModulationConfig(splitter, config, 0, 0);
            const bool success {SplitterMutators::moveSlot(splitter, 0, 0, 0, 2)};

            THEN("The slot is moved to the new position") {
                CHECK(success);
                REQUIRE(splitterParallel->chains.size() == 2);

                for (int chainNumber {0}; chainNumber < 2; chainNumber++) {
                    CHECK(splitterParallel->chains[chainNumber].chain->chain.size() == 4);
                }

                CHECK(latencyCalled);
                CHECK(receivedLatency == 60);

                // Check bypass
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 0) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 1) == true);
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 2) == false);

                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 0) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 1) == false);
                CHECK(SplitterMutators::getSlotBypass(splitter, 1, 2) == false);

                // Check the slots
                CHECK(SplitterMutators::getPlugin(splitter, 0, 0) == plugins[0][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 1) == plugins[0][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 2) == plugins[0][2]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 3) == nullptr);

                CHECK(SplitterMutators::getPlugin(splitter, 1, 0) == plugins[1][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 1) == plugins[1][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 2) == plugins[1][2]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 3) == nullptr);

                // Check the modulation config
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 0).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 1).isActive == true);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 1).parameterConfigs[0]->targetParameterName == "testParam");
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 0, 2).isActive == false);

                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 0).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 1).isActive == false);
                CHECK(SplitterMutators::getPluginModulationConfig(splitter, 1, 2).isActive == false);
            }
        }

        WHEN("A slot is moved from a higher to a lower position") {
            SplitterMutators::setGainLinear(splitter, 0, 3, 0.1f);
            SplitterMutators::setPan(splitter, 0, 3, 0.2f);
            const bool success {SplitterMutators::moveSlot(splitter, 0, 3, 0, 0)};

            THEN("The slot is moved to the new position") {
                CHECK(success);
                REQUIRE(splitterParallel->chains.size() == 2);

                for (int chainNumber {0}; chainNumber < 2; chainNumber++) {
                    CHECK(splitterParallel->chains[chainNumber].chain->chain.size() == 4);
                }

                CHECK(latencyCalled);
                CHECK(receivedLatency == 60);

                // Check gain and pan
                CHECK(SplitterMutators::getGainLinear(splitter, 0, 0) == 0.1f);
                CHECK(SplitterMutators::getPan(splitter, 0, 0) == 0.2f);

                // Check the slots
                CHECK(SplitterMutators::getPlugin(splitter, 0, 0) == nullptr);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 1) == plugins[0][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 2) == plugins[0][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 3) == plugins[0][2]);

                CHECK(SplitterMutators::getPlugin(splitter, 1, 0) == plugins[1][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 1) == plugins[1][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 2) == plugins[1][2]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 3) == nullptr);
            }
        }

        WHEN("A slot is moved from one chain to another") {
            SplitterMutators::setGainLinear(splitter, 0, 3, 0.1f);
            SplitterMutators::setPan(splitter, 0, 3, 0.2f);
            const bool success {SplitterMutators::moveSlot(splitter, 0, 3, 1, 0)};

            THEN("The slot is moved to the new position") {
                CHECK(success);
                REQUIRE(splitterParallel->chains.size() == 2);
                CHECK(splitterParallel->chains[0].chain->chain.size() == 3);
                CHECK(splitterParallel->chains[1].chain->chain.size() == 5);

                CHECK(latencyCalled);
                CHECK(receivedLatency == 60);

                // Check gain and pan
                CHECK(SplitterMutators::getGainLinear(splitter, 1, 0) == 0.1f);
                CHECK(SplitterMutators::getPan(splitter, 1, 0) == 0.2f);

                // Check the slots
                CHECK(SplitterMutators::getPlugin(splitter, 0, 0) == plugins[0][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 1) == plugins[0][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 0, 2) == plugins[0][2]);

                CHECK(SplitterMutators::getPlugin(splitter, 1, 0) == nullptr);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 1) == plugins[1][0]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 2) == plugins[1][1]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 3) == plugins[1][2]);
                CHECK(SplitterMutators::getPlugin(splitter, 1, 4) == nullptr);
            }
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("SplitterMutators: Chains can be moved") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A splitter with a single plugin in a single chain") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        auto splitterSeries = std::make_shared<PluginSplitterSeries>(config, modulationCallback, latencyCallback);

        auto plugin = std::make_shared<MutatorTestPluginInstance>();
        plugin->setLatencySamples(10);

        auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterSeries);
        bool isSuccess = SplitterMutators::insertPlugin(splitter, plugin, 0, 0);

        REQUIRE(isSuccess);
        REQUIRE(splitterSeries->chains[0].chain->latencyListener.calculatedTotalPluginLatency == 10);
        REQUIRE(splitterSeries->chains.size() == 1);
        REQUIRE(splitterSeries->chains[0].chain->chain.size() == 1);
        REQUIRE(SplitterMutators::getPlugin(splitter, 0, 0) == plugin);
        REQUIRE(latencyCalled);
        REQUIRE(receivedLatency == 10);

        // Reset
        latencyCalled = false;
        receivedLatency = 0;

        WHEN("The chain is moved to the same position") {
            SplitterMutators::moveChain(splitter, 0, 0);

            THEN("Nothing changes") {
                CHECK(splitterSeries->chains.size() == 1);
                CHECK(splitterSeries->chains[0].chain->chain.size() == 1);
                CHECK(SplitterMutators::getPlugin(splitterSeries, 0, 0) == plugin);
                CHECK(splitterSeries->chains[0].isSoloed == false);
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);
            }
        }

        WHEN("The chain is moved to a position > chains.size()") {
            SplitterMutators::moveChain(splitter, 0, 10);

            THEN("Nothing changes") {
                CHECK(splitterSeries->chains.size() == 1);
                CHECK(splitterSeries->chains[0].chain->chain.size() == 1);
                CHECK(SplitterMutators::getPlugin(splitterSeries, 0, 0) == plugin);
                CHECK(splitterSeries->chains[0].isSoloed == false);
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);
            }
        }
    }

    GIVEN("A multiband splitter with a three chains") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        auto splitterMultiband = std::make_shared<PluginSplitterMultiband>(config, modulationCallback, latencyCallback);
        auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterMultiband);

        // Add one more chain/band
        SplitterMutators::addBand(splitterMultiband);

        // Add a plugin to each chain
        std::vector<std::shared_ptr<MutatorTestPluginInstance>> plugins;
        for (int pluginNumber {0}; pluginNumber < splitterMultiband->chains.size(); pluginNumber++) {
            auto thisPlugin = std::make_shared<MutatorTestPluginInstance>();
            const int latency {10 * (pluginNumber + 1)};
            thisPlugin->setLatencySamples(latency);
            plugins.push_back(thisPlugin);

            bool isSuccess = SplitterMutators::insertPlugin(splitter, thisPlugin, pluginNumber, 0);
            REQUIRE(splitterMultiband->chains[pluginNumber].chain->latencyListener.calculatedTotalPluginLatency == latency);
            REQUIRE(splitterMultiband->chains[pluginNumber].chain->chain.size() == 1);
            REQUIRE(SplitterMutators::getPlugin(splitter, pluginNumber, 0) == thisPlugin);
            REQUIRE(latencyCalled);
            REQUIRE(receivedLatency == latency);
        }

        REQUIRE(splitterMultiband->chains.size() == 3);

        // Reset
        latencyCalled = false;
        receivedLatency = 0;

        WHEN("The middle chain is moved to the same position") {
            SplitterMutators::setChainSolo(splitter, 1, true);
            SplitterMutators::moveChain(splitter, 1, 1);

            THEN("Nothing changes") {
                CHECK(splitterMultiband->chains.size() == 3);

                for (int chainNumber {0}; chainNumber < 3; chainNumber++) {
                    CHECK(splitterMultiband->chains[chainNumber].chain->chain.size() == 1);
                    CHECK(splitterMultiband->crossover->bands[chainNumber].chain == splitterMultiband->chains[chainNumber].chain);
                }

                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Check is soloed as it has to be copied with the wrapper
                CHECK(splitterMultiband->chains[0].isSoloed == false);
                CHECK(splitterMultiband->chains[1].isSoloed == true);
                CHECK(splitterMultiband->chains[2].isSoloed == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 0) == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 1) == true);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 2) == false);

                // Check the plugins to see which chains have moved
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 0, 0) == plugins[0]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 1, 0) == plugins[1]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 2, 0) == plugins[2]);
            }
        }

        WHEN("A chain is moved from a lower to the next position up") {
            SplitterMutators::setChainSolo(splitter, 0, true);
            SplitterMutators::moveChain(splitter, 0, 1);

            THEN("Nothing changes") { // Not immediately obvious, but this is correct as the chain is already in the right place
                CHECK(splitterMultiband->chains.size() == 3);

                for (int chainNumber {0}; chainNumber < 3; chainNumber++) {
                    CHECK(splitterMultiband->chains[chainNumber].chain->chain.size() == 1);
                    CHECK(splitterMultiband->crossover->bands[chainNumber].chain == splitterMultiband->chains[chainNumber].chain);
                }

                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Check is soloed as it has to be copied with the wrapper
                CHECK(splitterMultiband->chains[0].isSoloed == true);
                CHECK(splitterMultiband->chains[1].isSoloed == false);
                CHECK(splitterMultiband->chains[2].isSoloed == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 0) == true);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 1) == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 2) == false);

                // Check the plugins to see which chains have moved
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 0, 0) == plugins[0]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 1, 0) == plugins[1]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 2, 0) == plugins[2]);
            }
        }

        WHEN("The middle chain is moved to a position > chains.size()") {
            SplitterMutators::setChainSolo(splitter, 1, true);
            SplitterMutators::moveChain(splitter, 1, 10);

            THEN("The chain is moved to the end") {
                CHECK(splitterMultiband->chains.size() == 3);

                for (int chainNumber {0}; chainNumber < 3; chainNumber++) {
                    CHECK(splitterMultiband->chains[chainNumber].chain->chain.size() == 1);
                    CHECK(splitterMultiband->crossover->bands[chainNumber].chain == splitterMultiband->chains[chainNumber].chain);
                }

                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Check is soloed as it has to be copied with the wrapper
                CHECK(splitterMultiband->chains[0].isSoloed == false);
                CHECK(splitterMultiband->chains[1].isSoloed == false);
                CHECK(splitterMultiband->chains[2].isSoloed == true);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 0) == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 1) == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 2) == true);

                // Check the plugins to see which chains have moved
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 0, 0) == plugins[0]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 1, 0) == plugins[2]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 2, 0) == plugins[1]);
            }
        }

        WHEN("A chain is moved from a lower to a higher position") {
            SplitterMutators::setChainSolo(splitter, 0, true);
            SplitterMutators::moveChain(splitter, 0, 2);

            THEN("The chain is moved to the new position") {
                CHECK(splitterMultiband->chains.size() == 3);

                for (int chainNumber {0}; chainNumber < 3; chainNumber++) {
                    CHECK(splitterMultiband->chains[chainNumber].chain->chain.size() == 1);
                    CHECK(splitterMultiband->crossover->bands[chainNumber].chain == splitterMultiband->chains[chainNumber].chain);
                }

                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Check is soloed as it has to be copied with the wrapper
                CHECK(splitterMultiband->chains[0].isSoloed == false);
                CHECK(splitterMultiband->chains[1].isSoloed == true);
                CHECK(splitterMultiband->chains[2].isSoloed == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 0) == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 1) == true);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 2) == false);


                // Check the plugins to see which chains have moved
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 0, 0) == plugins[1]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 1, 0) == plugins[0]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 2, 0) == plugins[2]);
            }
        }

        WHEN("A chain is moved from a higher to a lower position") {
            SplitterMutators::setChainSolo(splitter, 2, true);
            SplitterMutators::moveChain(splitter, 2, 0);

            THEN("The chain is moved to the new position") {
                CHECK(splitterMultiband->chains.size() == 3);

                for (int chainNumber {0}; chainNumber < 3; chainNumber++) {
                    CHECK(splitterMultiband->chains[chainNumber].chain->chain.size() == 1);
                    CHECK(splitterMultiband->crossover->bands[chainNumber].chain == splitterMultiband->chains[chainNumber].chain);
                }

                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                // Check is soloed as it has to be copied with the wrapper
                CHECK(splitterMultiband->chains[0].isSoloed == true);
                CHECK(splitterMultiband->chains[1].isSoloed == false);
                CHECK(splitterMultiband->chains[2].isSoloed == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 0) == true);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 1) == false);
                CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 2) == false);

                // Check the plugins to see which chains have moved
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 0, 0) == plugins[2]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 1, 0) == plugins[0]);
                CHECK(SplitterMutators::getPlugin(splitterMultiband, 2, 0) == plugins[1]);
            }
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("SplitterMutators: Modulation config can be set and retrieved") {
    GIVEN("A parallel splitter with two chains") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        auto splitterParallel = std::make_shared<PluginSplitterParallel>(config, modulationCallback, latencyCallback);
        SplitterMutators::addChain(splitterParallel);

        auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);
        SplitterMutators::insertGainStage(splitter, 0, 0);
        SplitterMutators::insertPlugin(splitter, std::make_shared<MutatorTestPluginInstance>(), 0, 1);
        SplitterMutators::insertPlugin(splitter, std::make_shared<MutatorTestPluginInstance>(), 1, 0);

        WHEN("The config is set for a plugin") {
            PluginModulationConfig config;
            config.isActive = true;

            auto paramConfig = std::make_shared<PluginParameterModulationConfig>();
            paramConfig->targetParameterName = "testParam";
            config.parameterConfigs.push_back(paramConfig);

            const bool success {SplitterMutators::setPluginModulationConfig(splitter, config, 1, 0)};

            THEN("The new config can be retrieved") {
                CHECK(success);

                const PluginModulationConfig retrievedConfig {SplitterMutators::getPluginModulationConfig(splitter, 1, 0)};
                CHECK(retrievedConfig.isActive);
                CHECK(retrievedConfig.parameterConfigs[0]->targetParameterName == config.parameterConfigs[0]->targetParameterName);
            }
         }

        WHEN("The config is set for a gain stage") {
            PluginModulationConfig config;
            config.isActive = true;

            auto paramConfig = std::make_shared<PluginParameterModulationConfig>();
            paramConfig->targetParameterName = "testParam";
            config.parameterConfigs.push_back(paramConfig);

            const bool success {SplitterMutators::setPluginModulationConfig(splitter, config, 0, 0)};

            THEN("The configs haven't changed") {
                CHECK(!success);

                for (int chainIndex {0}; chainIndex < splitter->chains.size(); chainIndex++) {
                    for (int slotIndex {0}; slotIndex < splitter->chains[chainIndex].chain->chain.size(); slotIndex++) {
                        const PluginModulationConfig retrievedConfig {SplitterMutators::getPluginModulationConfig(splitter, chainIndex, slotIndex)};
                        CHECK(!retrievedConfig.isActive);
                        CHECK(retrievedConfig.parameterConfigs.size() == 0);
                    }
                }
            }
        }
    }
}

SCENARIO("SplitterMutators: Slot parameters can be modified and retrieved") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A splitter with two chains") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        const juce::String splitTypeString = GENERATE(
            // We don't need series
            juce::String(XML_SPLIT_TYPE_PARALLEL_STR),
            juce::String(XML_SPLIT_TYPE_MULTIBAND_STR),
            juce::String(XML_SPLIT_TYPE_LEFTRIGHT_STR),
            juce::String(XML_SPLIT_TYPE_MIDSIDE_STR)
        );

        std::shared_ptr<PluginSplitter> splitter;

        if (splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR) {
            auto splitterParallel = std::make_shared<PluginSplitterParallel>(config, modulationCallback, latencyCallback);
            SplitterMutators::addChain(splitterParallel);
            splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);
        } else if (splitTypeString == XML_SPLIT_TYPE_MULTIBAND_STR) {
            auto splitterMultiband = std::make_shared<PluginSplitterMultiband>(config, modulationCallback, latencyCallback);
            splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterMultiband);
        } else if (splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR) {
            auto splitterLeftRight = std::make_shared<PluginSplitterLeftRight>(config, modulationCallback, latencyCallback);
            splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterLeftRight);
        } else if (splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR) {
            auto splitterMidSide = std::make_shared<PluginSplitterMidSide>(config, modulationCallback, latencyCallback);
            splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterMidSide);
        }

        REQUIRE(splitter->chains.size() == 2);
        REQUIRE(splitter->chains[0].chain->latencyListener.calculatedTotalPluginLatency == 0);
        REQUIRE(splitter->chains[1].chain->latencyListener.calculatedTotalPluginLatency == 0);


        SplitterMutators::insertGainStage(splitter, 0, 0);

        {
            auto plugin = std::make_shared<MutatorTestPluginInstance>();
            plugin->setLatencySamples(20);
            SplitterMutators::insertPlugin(splitter, plugin, 0, 1);
        }

        {
            auto plugin = std::make_shared<MutatorTestPluginInstance>();
            plugin->setLatencySamples(15);
            SplitterMutators::insertPlugin(splitter, plugin, 1, 0);
        }

        REQUIRE(receivedLatency == 20);

        // Reset
        latencyCalled = false;
        receivedLatency = 0;

        WHEN("A plugin is bypassed") {
            const bool success {SplitterMutators::setSlotBypass(splitter, 0, 1, true)};

            // Allow the latency message to be sent
            messageManager->runDispatchLoopUntil(10);

            THEN("The plugin is bypassed correctly") {
                CHECK(success);
                CHECK(!SplitterMutators::getSlotBypass(splitter, 0, 0));
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 1));
                CHECK(!SplitterMutators::getSlotBypass(splitter, 1, 0));
                CHECK(latencyCalled);
                CHECK(receivedLatency == 15);
            }
        }

        WHEN("A gain stage is bypassed") {
            const bool success {SplitterMutators::setSlotBypass(splitter, 0, 0, true)};

            // Allow the latency message to be sent (though we don't expect one for a gain stage change)
            messageManager->runDispatchLoopUntil(10);

            THEN("The gain stage is bypassed correctly") {
                CHECK(success);
                CHECK(SplitterMutators::getSlotBypass(splitter, 0, 0));
                CHECK(!SplitterMutators::getSlotBypass(splitter, 0, 1));
                CHECK(!SplitterMutators::getSlotBypass(splitter, 1, 0));
                CHECK(latencyCalled);
                CHECK(receivedLatency == 20);
            }
        }

        WHEN("An out of bounds slot is bypassed") {
            const bool success {SplitterMutators::setSlotBypass(splitter, 10, 0, true)};

            // Allow the latency message to be sent (though we don't expect one here)
            messageManager->runDispatchLoopUntil(10);

            THEN("Nothing is bypassed") {
                CHECK(!success);
                CHECK(!SplitterMutators::getSlotBypass(splitter, 0, 0));
                CHECK(!SplitterMutators::getSlotBypass(splitter, 0, 1));
                CHECK(!SplitterMutators::getSlotBypass(splitter, 1, 0));
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);
            }
        }

        WHEN("A chain is soloed") {
            const bool success {SplitterMutators::setChainSolo(splitter, 0, true)};

            // Allow the latency message to be sent
            messageManager->runDispatchLoopUntil(10);

            THEN("The chain is soloed correctly") {
                CHECK(success);
                CHECK(SplitterMutators::getChainSolo(splitter, 0));
                CHECK(!SplitterMutators::getChainSolo(splitter, 1));
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);

                if (splitTypeString == XML_SPLIT_TYPE_MULTIBAND_STR) {
                    // Make sure the crossover state matches
                    auto splitterMultiband = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter);
                    REQUIRE(splitterMultiband != nullptr);
                    CHECK(CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 0));
                    CHECK(!CrossoverMutators::getIsSoloed(splitterMultiband->crossover, 1));
                }
            }
        }

        WHEN("An out of bounds chain is soloed") {
            const bool success {SplitterMutators::setChainSolo(splitter, 10, true)};

            // Allow the latency message to be sent
            messageManager->runDispatchLoopUntil(10);

            THEN("Nothing is soloed") {
                CHECK(!success);
                CHECK(!SplitterMutators::getChainSolo(splitter, 0));
                CHECK(!SplitterMutators::getChainSolo(splitter, 1));
                CHECK(!latencyCalled);
                CHECK(receivedLatency == 0);
            }
        }

        WHEN("Gain and pan is set for a plugin slot") {
            const bool gainSuccess {SplitterMutators::setGainLinear(splitter, 0, 1, 0.5)};
            const bool panSuccess {SplitterMutators::setPan(splitter, 0, 1, -0.5)};

            THEN("Nothing is changed") {
                CHECK(!gainSuccess);
                CHECK(!panSuccess);
                CHECK(SplitterMutators::getGainLinear(splitter, 0, 1) == 0.0f);
                CHECK(SplitterMutators::getPan(splitter, 0, 1) == 0.0f);
            }
        }

        WHEN("Gain and pan is set for a gain stage slot") {
            const bool gainSuccess {SplitterMutators::setGainLinear(splitter, 0, 0, 0.5)};
            const bool panSuccess {SplitterMutators::setPan(splitter, 0, 0, -0.5)};

            THEN("The gain stage is bypassed correctly") {
                CHECK(gainSuccess);
                CHECK(panSuccess);
                CHECK(SplitterMutators::getGainLinear(splitter, 0, 0) == 0.5f);
                CHECK(SplitterMutators::getPan(splitter, 0, 0) == -0.5f);
            }
        }

        WHEN("Gain and pan is set for an out of bounds chain") {
            const bool gainSuccess {SplitterMutators::setGainLinear(splitter, 10, 0, 0.5)};
            const bool panSuccess {SplitterMutators::setPan(splitter, 10, 0, -0.5)};

            THEN("Nothing is bypassed") {
                CHECK(!gainSuccess);
                CHECK(!panSuccess);
            }
        }

        if (splitTypeString == XML_SPLIT_TYPE_MULTIBAND_STR) {
            // Make sure the crossover state matches
            auto splitterMultiband = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter);
            REQUIRE(splitterMultiband != nullptr);

            WHEN("Crossover frequency is set") {
                const bool success = SplitterMutators::setCrossoverFrequency(splitterMultiband, 0, 2500);

                THEN("the crossover frequency is set correctly") {
                    CHECK(success);
                    CHECK(SplitterMutators::getCrossoverFrequency(splitterMultiband, 0) == 2500);
                }
            }

            WHEN("Crossover frequency is set for an out of bounds crossover") {
                const bool success = SplitterMutators::setCrossoverFrequency(splitterMultiband, 4, 4000);

                THEN("The default value is unchanged") {
                    CHECK(!success);
                    CHECK(SplitterMutators::getCrossoverFrequency(splitterMultiband, 0) == 1000);
                }
            }
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("SplitterMutators: Chain custom names can be set and retrieved") {
    GIVEN("A parallel splitter with two chains") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        auto splitterParallel = std::make_shared<PluginSplitterParallel>(config, modulationCallback, latencyCallback);
        SplitterMutators::addChain(splitterParallel);

        auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);

        WHEN("A custom name is set for a chain") {
            const bool success {SplitterMutators::setChainCustomName(splitter, 0, "Test Chain")};

            THEN("The new name can be retrieved") {
                CHECK(success);
                CHECK(SplitterMutators::getChainCustomName(splitter, 0) == "Test Chain");
                CHECK(SplitterMutators::getChainCustomName(splitter, 1) == "");
            }
        }

        WHEN("A custom name is set for an out of bounds chain") {
            const bool success {SplitterMutators::setChainCustomName(splitter, 10, "Test Chain")};

            THEN("The default name is unchanged") {
                CHECK(!success);
                CHECK(SplitterMutators::getChainCustomName(splitter, 0) == "");
                CHECK(SplitterMutators::getChainCustomName(splitter, 1) == "");
            }
        }
    }
}


SCENARIO("SplitterMutators: PluginEditorBounds can be retrieved") {
    GIVEN("A parallel splitter with two chains") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        bool latencyCalled {false};
        int receivedLatency {0};
        auto latencyCallback = [&latencyCalled, &receivedLatency](int latency) {
            latencyCalled = true;
            receivedLatency = latency;
        };

        auto splitterParallel = std::make_shared<PluginSplitterParallel>(config, modulationCallback, latencyCallback);
        SplitterMutators::addChain(splitterParallel);
        REQUIRE(splitterParallel->chains[0].chain->latencyListener.calculatedTotalPluginLatency == 0);
        REQUIRE(splitterParallel->chains[1].chain->latencyListener.calculatedTotalPluginLatency == 0);

        auto splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterParallel);
        SplitterMutators::insertGainStage(splitter, 0, 0);
        SplitterMutators::insertPlugin(splitter, std::make_shared<MutatorTestPluginInstance>(), 1, 0);

        auto& pluginBounds = std::dynamic_pointer_cast<ChainSlotPlugin>(splitter->chains[1].chain->chain[0])->editorBounds;
        pluginBounds.reset(new PluginEditorBounds());
        *(pluginBounds.get()) = PluginEditorBoundsContainer(
            juce::Rectangle<int>(150, 200),
            juce::Rectangle<int>(2000, 1000));

        WHEN("Editor bounds are retrieved for a plugin") {
            auto bounds = SplitterMutators::getPluginEditorBounds(splitter, 1, 0);

            THEN("Bounds are retrieved correctly") {
                CHECK(bounds->value().editorBounds == juce::Rectangle<int>(150, 200));
                CHECK(bounds->value().displayArea == juce::Rectangle<int>(2000, 1000));
            }
        }

        WHEN("Editor bounds are retrieved for a gain stage") {
            auto bounds = SplitterMutators::getPluginEditorBounds(splitter, 0, 0);

            THEN("Bounds aren't retrieved") {
                CHECK(!bounds->has_value());
            }
        }

        WHEN("Editor bounds are retrieved for an out of bounds slot") {
            auto bounds = SplitterMutators::getPluginEditorBounds(splitter, 10, 0);

            THEN("Bounds aren't retrieved") {
                CHECK(!bounds->has_value());
            }
        }
    }
}
