#include "catch.hpp"
#include "TestUtils.hpp"

#include "ChainMutators.hpp"
#include "ChainProcessors.hpp"

namespace {
    constexpr int SAMPLE_RATE {44100};
    constexpr int NUM_SAMPLES {64};

    class MutatorTestPluginInstance : public TestUtils::TestPluginInstance {
    public:
        class PluginParameter : public Parameter {
        public:
            float value;
            juce::String name;

            PluginParameter(juce::String newName) : value(0), name(newName) { }

            float getValue() const override { return value; }
            void setValue(float newValue) override { value = newValue; }
            float getDefaultValue() const override { return 0; }
            juce::String getName(int maximumStringLength) const override { return name; }
            juce::String getLabel() const override { return name; }
            juce::String getParameterID() const override { return name; }
        };

        juce::AudioProcessorListener* addedListener;
        juce::AudioProcessorListener* removedListener;

        MutatorTestPluginInstance() : addedListener(nullptr), removedListener(nullptr) {
            addHostedParameter(std::make_unique<PluginParameter>("param1"));
            addHostedParameter(std::make_unique<PluginParameter>("param2"));
            addHostedParameter(std::make_unique<PluginParameter>("param3"));
        }

        void addListener(juce::AudioProcessorListener* newListener) {
            addedListener = newListener;
            TestUtils::TestPluginInstance::addListener(newListener);
        }

        void removeListener(juce::AudioProcessorListener* listenerToRemove) override {
            removedListener = listenerToRemove;
            TestUtils::TestPluginInstance::removeListener(listenerToRemove);
        }
    };
}

SCENARIO("ChainMutators: Slots can be added, replaced, and removed") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("An empty chain") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());
        
        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 0.0f;
        };

        const auto layout = TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo());

        auto chain = std::make_shared<PluginChain>(modulationCallback);
        REQUIRE(chain->latencyListener.calculatedTotalPluginLatency == 0);

        // Catch2 resets the state if we use multiple WHEN clauses. We could use AND_WHEN, but they
        // need to be nested which makes then hard to read, so instead we use a single WHEN.
        WHEN("The chain is modified") {
            // WHEN("A plugin is added")
            {
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(10);
                ChainMutators::insertPlugin(chain, plugin, 0, hostConfig);

                // THEN("The chain contains a single plugin")
                CHECK(chain->chain.size() == 1);
                CHECK(ChainMutators::getNumSlots(chain) == 1);
                CHECK(ChainMutators::getPlugin(chain, 0) == plugin);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 10);
                CHECK(plugin->addedListener == &chain->latencyListener);
            }

             // WHEN("A plugin is added at position > chains.size()")
            {
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(15);
                ChainMutators::insertPlugin(chain, plugin, 5, hostConfig);

                // THEN("The new plugin is added at the end")
                CHECK(chain->chain.size() == 2);
                CHECK(ChainMutators::getNumSlots(chain) == 2);
                CHECK(ChainMutators::getPlugin(chain, 1) == plugin);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 25);
                CHECK(plugin->addedListener == &chain->latencyListener);
            }

            // WHEN("A plugin is added in the middle")
            {
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(20);
                ChainMutators::insertPlugin(chain, plugin, 1, hostConfig);

                // THEN("The new plugin is added in the correct place")
                CHECK(chain->chain.size() == 3);
                CHECK(ChainMutators::getNumSlots(chain) == 3);
                CHECK(ChainMutators::getPlugin(chain, 1) == plugin);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 45);
                CHECK(plugin->addedListener == &chain->latencyListener);
            }

            // WHEN("A gain stage is added in the middle")
            {
                ChainMutators::insertGainStage(chain, 2, {layout, SAMPLE_RATE, NUM_SAMPLES});

                // THEN("The new gain stage is added in the correct place")
                CHECK(chain->chain.size() == 4);
                CHECK(ChainMutators::getNumSlots(chain) == 4);
                CHECK(ChainMutators::getPlugin(chain, 2) == nullptr);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 45);
            }

            // WHEN("A gain stage is added at position > chains.size()")
            {
                ChainMutators::insertGainStage(chain, 10, {layout, SAMPLE_RATE, NUM_SAMPLES});

                // THEN("The new gain stage is added at the end")
                CHECK(chain->chain.size() == 5);
                CHECK(ChainMutators::getNumSlots(chain) == 5);
                CHECK(ChainMutators::getPlugin(chain, 4) == nullptr);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 45);
            }

            // WHEN("A plugin is replaced")
            {
                REQUIRE(ChainMutators::getPlugin(chain, 1) != nullptr);
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(25);

                auto oldPlugin = std::dynamic_pointer_cast<MutatorTestPluginInstance>(ChainMutators::getPlugin(chain, 1));
                ChainMutators::replacePlugin(chain, plugin, 1, hostConfig);

                // THEN("The new plugin is in the correct place")
                CHECK(chain->chain.size() == 5);
                CHECK(ChainMutators::getNumSlots(chain) == 5);
                CHECK(ChainMutators::getPlugin(chain, 1) == plugin);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 50);
                CHECK(oldPlugin->removedListener == &chain->latencyListener);
                CHECK(plugin->addedListener == &chain->latencyListener);
            }

            // WHEN("A gain stage is replaced with a plugin")
            {
                REQUIRE(ChainMutators::getPlugin(chain, 2) == nullptr);
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(30);
                ChainMutators::replacePlugin(chain, plugin, 2, hostConfig);

                // THEN("The new plugin is in the correct place")
                CHECK(chain->chain.size() == 5);
                CHECK(ChainMutators::getNumSlots(chain) == 5);
                CHECK(ChainMutators::getPlugin(chain, 2) == plugin);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 80);
                CHECK(plugin->addedListener == &chain->latencyListener);
            }

            // WHEN("A plugin is replaced at position > chains.size()")
            {
                auto plugin = std::make_shared<MutatorTestPluginInstance>();
                plugin->setLatencySamples(35);
                ChainMutators::replacePlugin(chain, plugin, 10, hostConfig);

                // THEN("The new plugin is added at the end")
                CHECK(chain->chain.size() == 6);
                CHECK(ChainMutators::getNumSlots(chain) == 6);
                CHECK(ChainMutators::getPlugin(chain, 5) == plugin);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 115);
                CHECK(plugin->addedListener == &chain->latencyListener);
            }

            // WHEN("A plugin is removed")
            {
                REQUIRE(ChainMutators::getPlugin(chain, 2) != nullptr);

                // Figure out what we expect the chain to look like before making changes
                auto expectedChain = chain->chain;
                expectedChain.erase(expectedChain.begin() + 2);

                auto oldPlugin = std::dynamic_pointer_cast<MutatorTestPluginInstance>(ChainMutators::getPlugin(chain, 2));
                const bool success {ChainMutators::removeSlot(chain, 2)};

                // THEN("The plugin is removed from the correct place")
                CHECK(success);
                CHECK(chain->chain.size() == 5);
                CHECK(ChainMutators::getNumSlots(chain) == 5);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 85);
                CHECK(oldPlugin->removedListener == &chain->latencyListener);

                for (int slotIndex {0}; slotIndex < expectedChain.size(); slotIndex++) {
                    CHECK(expectedChain[slotIndex] == chain->chain[slotIndex]);
                }
            }

            // WHEN("A gain stage is removed")
            {
                REQUIRE(ChainMutators::getPlugin(chain, 3) == nullptr);

                // Figure out what we expect the chain to look like before making changes
                auto expectedChain = chain->chain;
                expectedChain.erase(expectedChain.begin() + 3);

                const bool success {ChainMutators::removeSlot(chain, 3)};

                // THEN("The plugin is removed from the correct place")
                CHECK(success);
                CHECK(chain->chain.size() == 4);
                CHECK(ChainMutators::getNumSlots(chain) == 4);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 85);

                for (int slotIndex {0}; slotIndex < expectedChain.size(); slotIndex++) {
                    CHECK(expectedChain[slotIndex] == chain->chain[slotIndex]);
                }
            }

            // WHEN("A plugin is removed at position > chains.size()")
            {
                REQUIRE(ChainMutators::getNumSlots(chain) == 4);
                const bool success {ChainMutators::removeSlot(chain, 4)};

                // THEN("No plugins are removed")
                CHECK(!success);
                CHECK(chain->chain.size() == 4);
                CHECK(ChainMutators::getNumSlots(chain) == 4);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 85);
            }

            // WHEN("A plugin changes latency")
            {
                auto plugin = ChainMutators::getPlugin(chain, 0);
                REQUIRE(plugin != nullptr);

                plugin->setLatencySamples(40);

                // Allow the latency message to be sent
                messageManager->runDispatchLoopUntil(10);

                // THEN("The new latency is calculated")
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 115);
            }
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("ChainMutators: Modulation config can be set and retrieved") {
    GIVEN("A chain with three plugins and a gain stage") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());
        
        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 0.0f;
        };

        const auto layout = TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo());

        auto chain = std::make_shared<PluginChain>(modulationCallback);
        ChainMutators::insertPlugin(chain, std::make_shared<MutatorTestPluginInstance>(), 0, hostConfig);
        ChainMutators::insertGainStage(chain, 1, {layout, SAMPLE_RATE, NUM_SAMPLES});
        ChainMutators::insertPlugin(chain, std::make_shared<MutatorTestPluginInstance>(), 2, hostConfig);
        ChainMutators::insertPlugin(chain, std::make_shared<MutatorTestPluginInstance>(), 3, hostConfig);

        WHEN("The config is set for a plugin") {
            PluginModulationConfig config;
            config.isActive = true;

            auto paramConfig = std::make_shared<PluginParameterModulationConfig>();
            paramConfig->targetParameterName = "testParam";
            config.parameterConfigs.push_back(paramConfig);

            const bool success {ChainMutators::setPluginModulationConfig(chain, config, 2)};

            THEN("The new config can be retrieved") {
                CHECK(success);

                const PluginModulationConfig retrievedConfig {ChainMutators::getPluginModulationConfig(chain, 2)};
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

            const bool success {ChainMutators::setPluginModulationConfig(chain, config, 1)};

            THEN("The configs haven't changed") {
                CHECK(!success);

                for (int slotIndex {0}; slotIndex < ChainMutators::getNumSlots(chain); slotIndex++) {
                    const PluginModulationConfig retrievedConfig {ChainMutators::getPluginModulationConfig(chain, slotIndex)};
                    CHECK(!retrievedConfig.isActive);
                    CHECK(retrievedConfig.parameterConfigs.size() == 0);
                }
            }
        }
    }
}

SCENARIO("ChainMutators: Slot parameters can be modified and retrieved") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A chain with two plugins and two gain stages") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());
        
        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 0.0f;
        };

        const auto layout = TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo());

        auto chain = std::make_shared<PluginChain>(modulationCallback);

        REQUIRE(chain->latencyListener.calculatedTotalPluginLatency == 0);

        {
            auto plugin = std::make_shared<MutatorTestPluginInstance>();
            plugin->setLatencySamples(10);
            ChainMutators::insertPlugin(chain, plugin, 0, hostConfig);
        }

        ChainMutators::insertGainStage(chain, 1, {layout, SAMPLE_RATE, NUM_SAMPLES});

        {
            auto plugin = std::make_shared<MutatorTestPluginInstance>();
            plugin->setLatencySamples(15);
            ChainMutators::insertPlugin(chain, plugin, 2, hostConfig);
        }

        ChainMutators::insertGainStage(chain, 3, {layout, SAMPLE_RATE, NUM_SAMPLES});

        REQUIRE(chain->latencyListener.calculatedTotalPluginLatency == 25);

        WHEN("A plugin is bypassed") {
            const bool success {ChainMutators::setSlotBypass(chain, 2, true)};

            // Allow the latency message to be sent
            messageManager->runDispatchLoopUntil(10);

            THEN("The plugin is bypassed correctly") {
                CHECK(success);
                CHECK(!ChainMutators::getSlotBypass(chain, 0));
                CHECK(!ChainMutators::getSlotBypass(chain, 1));
                CHECK(ChainMutators::getSlotBypass(chain, 2));
                CHECK(!ChainMutators::getSlotBypass(chain, 3));
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 10);
            }
        }

        WHEN("A gain stage is bypassed") {
            const bool success {ChainMutators::setSlotBypass(chain, 1, true)};

            // Allow the latency message to be sent (though we don't expect one for a gain stage change)
            messageManager->runDispatchLoopUntil(10);

            THEN("The gain stage is bypassed correctly") {
                CHECK(success);
                CHECK(!ChainMutators::getSlotBypass(chain, 0));
                CHECK(ChainMutators::getSlotBypass(chain, 1));
                CHECK(!ChainMutators::getSlotBypass(chain, 2));
                CHECK(!ChainMutators::getSlotBypass(chain, 3));
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 25);
            }
        }

        WHEN("An out of bounds slot is bypassed") {
            const bool success {ChainMutators::setSlotBypass(chain, 10, true)};

            // Allow the latency message to be sent (though we don't expect one here)
            messageManager->runDispatchLoopUntil(10);

            THEN("Nothing is bypassed") {
                CHECK(!success);
                CHECK(!ChainMutators::getSlotBypass(chain, 0));
                CHECK(!ChainMutators::getSlotBypass(chain, 1));
                CHECK(!ChainMutators::getSlotBypass(chain, 2));
                CHECK(!ChainMutators::getSlotBypass(chain, 3));
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 25);
            }
        }

        WHEN("Gain and pan is set for a plugin slot") {
            const bool gainSuccess {ChainMutators::setGainLinear(chain, 2, 0.5)};
            const bool panSuccess {ChainMutators::setPan(chain, 2, -0.5)};

            THEN("Nothing is changed") {
                CHECK(!gainSuccess);
                CHECK(!panSuccess);
                CHECK(ChainMutators::getGainLinear(chain, 2) == 0.0f);
                CHECK(ChainMutators::getPan(chain, 2) == 0.0f);
            }
        }

        WHEN("Gain and pan is set for a gain stage slot") {
            const bool gainSuccess {ChainMutators::setGainLinear(chain, 1, 0.5)};
            const bool panSuccess {ChainMutators::setPan(chain, 1, -0.5)};

            THEN("The gain stage is bypassed correctly") {
                CHECK(gainSuccess);
                CHECK(panSuccess);
                CHECK(ChainMutators::getGainLinear(chain, 1) == 0.5f);
                CHECK(ChainMutators::getPan(chain, 1) == -0.5f);
            }
        }

        WHEN("Gain and pan is set for an out of bounds slot") {
            const bool gainSuccess {ChainMutators::setGainLinear(chain, 10, 0.5)};
            const bool panSuccess {ChainMutators::setPan(chain, 10, -0.5)};

            THEN("Nothing is bypassed") {
                CHECK(!gainSuccess);
                CHECK(!panSuccess);
            }
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("ChainMutators: The chain can be bypassed and muted") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("An empty chain") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());
        
        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        const auto layout = TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo());

        auto chain = std::make_shared<PluginChain>(modulationCallback);

        REQUIRE(chain->latencyListener.calculatedTotalPluginLatency == 0);

        auto plugin = std::make_shared<MutatorTestPluginInstance>();
        plugin->setLatencySamples(10);
        ChainMutators::insertPlugin(chain, plugin, 0, hostConfig);

        REQUIRE(chain->latencyListener.calculatedTotalPluginLatency == 10);

        WHEN("The chain is bypassed") {
            REQUIRE(!chain->isChainBypassed);
            ChainMutators::setChainBypass(chain, true);

            // Allow the latency message to be sent
            messageManager->runDispatchLoopUntil(10);

            THEN("The bypass is set correctly") {
                CHECK(chain->isChainBypassed);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 0);
            }
        }

        WHEN("The chain is muted") {
            REQUIRE(!chain->isChainBypassed);
            ChainMutators::setChainMute(chain, true);

            // Allow the latency message to be sent (though we don't expect one for mute)
            messageManager->runDispatchLoopUntil(10);

            THEN("The mute is set correctly") {
                CHECK(chain->isChainMuted);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 10);
            }
        }
    }

    juce::MessageManager::deleteInstance();
}

SCENARIO("ChainMutators: Chains are removed from plugins as latency listeners on destruction") {
    GIVEN("A plugin chain with a single plugin") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());
        
        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        auto chain = std::make_shared<PluginChain>(modulationCallback);
        auto plugin = std::make_shared<MutatorTestPluginInstance>();

        ChainMutators::insertPlugin(chain, plugin, 0, hostConfig);

        WHEN("The chain is deleted") {
            // Get a pointer to the chain as a AudioProcessorListener before resetting
            auto listener = &chain->latencyListener;
            chain.reset();

            THEN("The chain is removed from the plugin as a listener") {
                CHECK(plugin->removedListener == listener);
            }
        }
    }
}
