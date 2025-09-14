#include "catch.hpp"
#include "TestUtils.hpp"

#include "ChainSlots.hpp"

SCENARIO("ChainSlotGainStage: Clone works correctly") {
    GIVEN("A gain stage slot") {
        ChainSlotGainStage gainStage(0.5f, 0.0f, false, juce::AudioProcessor::BusesLayout());

        WHEN("It is cloned") {
            ChainSlotGainStage* clonedGainStage = gainStage.clone();

            THEN("The cloned gain stage is equal to the original") {
                CHECK(clonedGainStage->isBypassed == gainStage.isBypassed);
                CHECK(clonedGainStage->gain == gainStage.gain);
                CHECK(clonedGainStage->pan == gainStage.pan);
                CHECK(clonedGainStage->numMainChannels == gainStage.numMainChannels);

                // We don't need to copy the meter envelopes, but they should be configured exactly
                // the same
                CHECK(clonedGainStage->meterEnvelopes.size() == gainStage.meterEnvelopes.size());
                for (int envIndex {0}; envIndex < clonedGainStage->meterEnvelopes.size(); envIndex++) {
                    auto& clonedEnvelope = clonedGainStage->meterEnvelopes[envIndex];
                    auto& originalEnvelope = gainStage.meterEnvelopes[envIndex];
                    CHECK(clonedEnvelope.getAttackTimeMs() == originalEnvelope.getAttackTimeMs());
                    CHECK(clonedEnvelope.getReleaseTimeMs() == originalEnvelope.getReleaseTimeMs());
                    CHECK(clonedEnvelope.getFilterEnabled() == originalEnvelope.getFilterEnabled());
                }
            }

            delete clonedGainStage;
        }
    }
}

SCENARIO("ChainSlotPlugin: Clone works correctly") {
    GIVEN("A plugin slot with some basic modulation") {
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

        REQUIRE(pluginSlot->spareSCBuffer != nullptr);
        REQUIRE(pluginSlot->spareSCBuffer->getNumChannels() == 4);
        REQUIRE(pluginSlot->spareSCBuffer->getNumSamples() == 10);

        auto modulationConfig = std::make_shared<PluginModulationConfig>();
        modulationConfig->isActive = true;

        auto parameterModulationConfig = std::make_shared<PluginParameterModulationConfig>();
        parameterModulationConfig->targetParameterName = "test param";
        parameterModulationConfig->restValue = 0.75f;

        auto source = std::make_shared<PluginParameterModulationSource>();
        source->definition = ModulationSourceDefinition(3, MODULATION_TYPE::LFO);
        source->modulationAmount = 0.8f;

        parameterModulationConfig->sources.push_back(source);
        modulationConfig->parameterConfigs.push_back(parameterModulationConfig);
        pluginSlot->modulationConfig = modulationConfig;

        WHEN("It is cloned") {
            ChainSlotPlugin* clonedPluginSlot = pluginSlot->clone();

            THEN("The cloned plugin slot is equal to the original") {
                CHECK(clonedPluginSlot->isBypassed == pluginSlot->isBypassed);
                CHECK(clonedPluginSlot->plugin == pluginSlot->plugin); // Should be the same shared pointer
                CHECK(clonedPluginSlot->modulationConfig != pluginSlot->modulationConfig); // Should be a different shared pointer
                CHECK(clonedPluginSlot->getModulationValueCallback(0, MODULATION_TYPE::MACRO) == 1.2f);
                CHECK(clonedPluginSlot->editorBounds == pluginSlot->editorBounds); // Should be the same shared pointer
                CHECK(clonedPluginSlot->spareSCBuffer != pluginSlot->spareSCBuffer); // Should be a different shared pointer
                CHECK(clonedPluginSlot->spareSCBuffer->getNumChannels() == pluginSlot->spareSCBuffer->getNumChannels());
                CHECK(clonedPluginSlot->spareSCBuffer->getNumSamples() == pluginSlot->spareSCBuffer->getNumSamples());

                // Check all the modulation
                auto clonedModulationConfig = clonedPluginSlot->modulationConfig;
                CHECK(clonedModulationConfig->isActive == modulationConfig->isActive);
                CHECK(clonedModulationConfig->parameterConfigs.size() == modulationConfig->parameterConfigs.size());

                auto clonedParameterModulationConfig = clonedModulationConfig->parameterConfigs[0];
                CHECK(clonedParameterModulationConfig != parameterModulationConfig); // Should be a different shared pointer
                CHECK(clonedParameterModulationConfig->targetParameterName == parameterModulationConfig->targetParameterName);
                CHECK(clonedParameterModulationConfig->restValue == parameterModulationConfig->restValue);
                CHECK(clonedParameterModulationConfig->sources.size() == parameterModulationConfig->sources.size());

                auto clonedSource = clonedParameterModulationConfig->sources[0];
                CHECK(clonedSource != source); // Should be a different shared pointer
                CHECK(clonedSource->definition == source->definition);
                CHECK(clonedSource->modulationAmount == source->modulationAmount);
            }

            delete clonedPluginSlot;
        }
    }
}
