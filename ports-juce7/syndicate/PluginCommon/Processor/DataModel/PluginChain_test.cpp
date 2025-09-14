#include "catch.hpp"
#include "TestUtils.hpp"

#include "PluginChain.hpp"

SCENARIO("PluginChain: Clone works correctly") {
    auto messageManager = juce::MessageManager::getInstance();

    GIVEN("A chain with one plugin slot and one gain stage") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());
        
        auto plugin = std::make_shared<TestUtils::TestPluginInstance>();
        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.2f;
        };
        auto pluginSlot = std::make_shared<ChainSlotPlugin>(plugin, false, modulationCallback, hostConfig);

        auto chain = std::make_shared<PluginChain>(modulationCallback);
        plugin->setLatencySamples(15);
        chain->chain.push_back(pluginSlot);
        plugin->addListener(&chain->latencyListener);
        chain->latencyListener.onPluginChainUpdate();
        REQUIRE(chain->latencyListener.calculatedTotalPluginLatency == 15);

        auto gainStage = std::make_shared<ChainSlotGainStage>(0.5f, 0.0f, false, juce::AudioProcessor::BusesLayout());
        chain->chain.push_back(gainStage);

        // Change the values so we can test for them
        // chain->isBypassed = true; // Can't test for this one as it sets the latency to 0 so would stop us testing the latency listener
        chain->isChainMuted = true;
        chain->latencyCompLine.reset(new CloneableDelayLineType(100));
        chain->latencyCompLine->setDelay(50);
        chain->latencyCompLine->prepare({44100, 10, 1});
        chain->latencyCompLine->pushSample(0, 0.75f);

        WHEN("It is cloned") {
            PluginChain* clonedChain = chain->clone();

            THEN("The cloned chain is equal to the original") {
                CHECK(clonedChain->isChainBypassed == chain->isChainBypassed);
                CHECK(clonedChain->isChainMuted == chain->isChainMuted);
                CHECK(clonedChain->getModulationValueCallback(0, MODULATION_TYPE::MACRO) == 1.2f);

                // Check the plugin slot
                auto clonedPluginSlot = std::dynamic_pointer_cast<ChainSlotPlugin>(clonedChain->chain[0]);
                CHECK(clonedPluginSlot != nullptr);
                CHECK(clonedPluginSlot->isBypassed == pluginSlot->isBypassed);
                CHECK(clonedPluginSlot->plugin == pluginSlot->plugin); // Should be the same shared pointer
                CHECK(clonedPluginSlot->modulationConfig != pluginSlot->modulationConfig); // Should be a different shared pointer

                // Check the gain stage
                auto clonedGainStage = std::dynamic_pointer_cast<ChainSlotGainStage>(clonedChain->chain[1]);
                CHECK(clonedGainStage != nullptr);
                CHECK(clonedGainStage->isBypassed == gainStage->isBypassed);
                CHECK(clonedGainStage->gain == gainStage->gain);
                CHECK(clonedGainStage->pan == gainStage->pan);
                CHECK(clonedGainStage->numMainChannels == gainStage->numMainChannels);

                // Check the delay line
                CHECK(clonedChain->latencyCompLine != chain->latencyCompLine);
                CHECK(chain->latencyCompLine != nullptr);
                CHECK(clonedChain->latencyCompLine->getMaximumDelayInSamples() == 100);
                CHECK(clonedChain->latencyCompLine->getDelay() == 50);
                CHECK(clonedChain->latencyCompLine->popSample(0, 0) == 0.75f);

                // Check the latency listener
                CHECK(clonedChain->latencyListener.calculatedTotalPluginLatency == 15);

                // Check the listeners for both chains are still connected
                plugin->setLatencySamples(30);
                messageManager->runDispatchLoopUntil(10);
                CHECK(chain->latencyListener.calculatedTotalPluginLatency == 30);
                CHECK(clonedChain->latencyListener.calculatedTotalPluginLatency == 30);
            }

            delete clonedChain;
        }
    }

    juce::MessageManager::deleteInstance();
}
