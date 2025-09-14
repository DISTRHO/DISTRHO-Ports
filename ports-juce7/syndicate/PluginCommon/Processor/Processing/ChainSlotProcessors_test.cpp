#include "catch.hpp"
#include "TestUtils.hpp"

#include "ChainSlotProcessors.hpp"

namespace {
    constexpr int NUM_SAMPLES {64};
    constexpr int SAMPLE_RATE {44100};

    class ProcessorTestPluginInstance : public TestUtils::TestPluginInstance {
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

        std::function<void(juce::AudioBuffer<float>&, juce::MidiBuffer&)> onProcess;

        void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
            onProcess(buffer, midiMessages);
        }

        static ProcessorTestPluginInstance* create() {
            return new ProcessorTestPluginInstance(
                juce::AudioProcessor::BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true));
        }

        static ProcessorTestPluginInstance* createWithSidechain() {
            return new ProcessorTestPluginInstance(
                juce::AudioProcessor::BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                                                       .withInput("Sidechain", juce::AudioChannelSet::stereo(), true));
        }

    private:
        ProcessorTestPluginInstance(BusesProperties buses) : TestUtils::TestPluginInstance(buses) {
            addHostedParameter(std::make_unique<PluginParameter>("param1"));
            addHostedParameter(std::make_unique<PluginParameter>("param2"));
            addHostedParameter(std::make_unique<PluginParameter>("param3"));
        }
    };
}

SCENARIO("ChainProcessors: Gain stage silence in = silence out") {
    GIVEN("A gain stage and a buffer of silence") {
        auto [layout, buffer] = GENERATE(
            std::make_pair<juce::AudioProcessor::BusesLayout, juce::AudioBuffer<float>>(
                TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::mono()),
                juce::AudioBuffer<float>(1, NUM_SAMPLES)
            ),
            std::make_pair<juce::AudioProcessor::BusesLayout, juce::AudioBuffer<float>>(
                TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo()),
                juce::AudioBuffer<float>(2, NUM_SAMPLES)
            )
        );

        buffer.clear();
        ChainSlotGainStage gainStage(1, 0, false, layout);

        WHEN("The buffer is processed") {
            ChainProcessors::prepareToPlay(gainStage, {layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(gainStage, buffer);

            THEN("The buffer contains silence") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == 0.0f);
                    }
                }
            }
        }
    }
}

SCENARIO("ChainProcessors: Gain stage gain is applied correctly") {
    GIVEN("A gain stage and a buffer of 1's") {
        auto [layout, buffer] = GENERATE(
            std::make_pair<juce::AudioProcessor::BusesLayout, juce::AudioBuffer<float>>(
                TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::mono()),
                juce::AudioBuffer<float>(1, NUM_SAMPLES)
            ),
            std::make_pair<juce::AudioProcessor::BusesLayout, juce::AudioBuffer<float>>(
                TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo()),
                juce::AudioBuffer<float>(2, NUM_SAMPLES)
            )
        );

        const float gain = GENERATE(0, 0.5, 1, 2);

        // Do this after calling GENERATE for the gain so it gets reset for each test
        for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
            juce::FloatVectorOperations::fill(buffer.getWritePointer(channelIdx), 1, buffer.getNumSamples());
        }

        ChainSlotGainStage gainStage(gain, 0, false, layout);

        WHEN("The buffer is processed") {
            ChainProcessors::prepareToPlay(gainStage, {layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(gainStage, buffer);

            THEN("The buffer has the correct gain applied") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == gain);
                    }
                }
            }
        }
    }
}

SCENARIO("ChainProcessors: Gain stage gain isn't applied when bypassed") {
    GIVEN("A gain stage with a gain other than 1 but bypassed and a buffer of 1's") {
        auto [layout, buffer] = GENERATE(
            std::make_pair<juce::AudioProcessor::BusesLayout, juce::AudioBuffer<float>>(
                TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::mono()),
                juce::AudioBuffer<float>(1, NUM_SAMPLES)
            ),
            std::make_pair<juce::AudioProcessor::BusesLayout, juce::AudioBuffer<float>>(
                TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo()),
                juce::AudioBuffer<float>(2, NUM_SAMPLES)
            )
        );

        for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
            juce::FloatVectorOperations::fill(buffer.getWritePointer(channelIdx), 1, buffer.getNumSamples());
        }

        ChainSlotGainStage gainStage(0.5, 0, true, layout);

        WHEN("The buffer is processed") {
            ChainProcessors::prepareToPlay(gainStage, {layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(gainStage, buffer);

            THEN("The buffer is unchanged") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == 1.0f);
                    }
                }
            }
        }
    }
}

SCENARIO("ChainProcessors: Gain stage panning is applied correctly") {
    GIVEN("A gain stage panned left and a buffer of 1's") {

        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add(juce::AudioChannelSet::stereo());

        juce::AudioBuffer<float> buffer(2, NUM_SAMPLES);

        const float pan = GENERATE(-1, 1);

        // Do this after calling GENERATE for the gain so it gets reset for each test
        for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
            juce::FloatVectorOperations::fill(buffer.getWritePointer(channelIdx), 1, buffer.getNumSamples());
        }

        ChainSlotGainStage gainStage(1, pan, false, layout);

        WHEN("The buffer is processed") {
            ChainProcessors::prepareToPlay(gainStage, {layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(gainStage, buffer);

            THEN("The buffer has been panned correctly") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    const bool bufferShouldBeZero {
                        (pan == -1 && channelIdx == 1) || (pan == 1 && channelIdx == 0)
                    };

                    const float expectedValue {bufferShouldBeZero ? 0.0f : 1.0f};

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == expectedValue);
                    }
                }
            }
        }
    }
}

SCENARIO("ChainProcessors: Gain stage panning isn't applied when bypassed") {
    GIVEN("A gain stage panned left and a buffer of 1's") {

        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add(juce::AudioChannelSet::stereo());

        juce::AudioBuffer<float> buffer(2, NUM_SAMPLES);

        const float pan = GENERATE(-1, 1);

        // Do this after calling GENERATE for the gain so it gets reset for each test
        for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
            juce::FloatVectorOperations::fill(buffer.getWritePointer(channelIdx), 1, buffer.getNumSamples());
        }

        ChainSlotGainStage gainStage(1, pan, true, layout);

        WHEN("The buffer is processed") {
            ChainProcessors::prepareToPlay(gainStage, {layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(gainStage, buffer);

            THEN("The buffer is unchanged ") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == 1.0f);
                    }
                }
            }
        }
    }
}

SCENARIO("ChainProcessors: Plugin isn't applied when bypassed") {
    GIVEN("A plugin but bypassed and a buffer of 1's") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        juce::AudioBuffer<float> buffer(2, NUM_SAMPLES);
        juce::FloatVectorOperations::fill(buffer.getWritePointer(0), 1, buffer.getNumSamples());
        juce::FloatVectorOperations::fill(buffer.getWritePointer(1), 1, buffer.getNumSamples());

        std::shared_ptr<ProcessorTestPluginInstance> plugin(ProcessorTestPluginInstance::create());
        plugin->onProcess = [](juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
            // This should never be called while bypassed
            CHECK(false);
        };

        ChainSlotPlugin slot(plugin,
                             true,
                             [](int, MODULATION_TYPE) { return 0.0f; },
                             hostConfig);

        WHEN("The buffer is processed") {
            juce::MidiBuffer midiBuffer;

            ChainProcessors::prepareToPlay(slot, {hostConfig.layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(slot, buffer, midiBuffer, nullptr);

            THEN("The buffer is unchanged") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == 1.0f);
                    }
                }
            }
        }
    }
}

SCENARIO("ChainProcessors: Plugin is applied correctly with modulation") {
    GIVEN("A plugin with three parameters (two which are modulated) and a buffer of 1's") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        juce::AudioBuffer<float> buffer(2, NUM_SAMPLES);
        juce::FloatVectorOperations::fill(buffer.getWritePointer(0), 1, buffer.getNumSamples());
        juce::FloatVectorOperations::fill(buffer.getWritePointer(1), 1, buffer.getNumSamples());

        bool didCallProcess {false};
        std::shared_ptr<ProcessorTestPluginInstance> plugin(ProcessorTestPluginInstance::create());
        plugin->onProcess = [&didCallProcess](juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
            didCallProcess = true;

            // TODO Add checks for midi
            CHECK(buffer.getNumChannels() == 2);
            CHECK(buffer.getNumSamples() == 64);

            juce::FloatVectorOperations::fill(buffer.getWritePointer(0), 0.1, buffer.getNumSamples());
            juce::FloatVectorOperations::fill(buffer.getWritePointer(1), 0.2, buffer.getNumSamples());
        };

        bool didCallModulation {false};
        ChainSlotPlugin slot(plugin,
                             false,
                             [&didCallModulation](int id, MODULATION_TYPE type) {
            didCallModulation = true;
            if (type == MODULATION_TYPE::MACRO) {
                CHECK(id == 1);
                return 0.13f;
            } else if (type == MODULATION_TYPE::LFO) {
                CHECK(id == 2);
                return 0.23f;
            } else if (type == MODULATION_TYPE::ENVELOPE) {
                CHECK(id == 3);
                return 0.33f;
            }

            CHECK(false);

            return 0.0f;
        },
        hostConfig);

        // Configure the plugin without a sidechain input
        juce::AudioProcessor::BusesLayout pluginLayout;
        pluginLayout.inputBuses.add(juce::AudioChannelSet::stereo());
        pluginLayout.outputBuses.add(juce::AudioChannelSet::stereo());
        REQUIRE(plugin->setBusesLayout(pluginLayout));

        // Set up the modulation
        slot.modulationConfig->isActive = true;
        slot.modulationConfig->parameterConfigs.push_back(std::make_shared<PluginParameterModulationConfig>());
        slot.modulationConfig->parameterConfigs.push_back(std::make_shared<PluginParameterModulationConfig>());

        // The first parameter has 1 modulation source
        slot.modulationConfig->parameterConfigs[0]->targetParameterName = "param1";
        slot.modulationConfig->parameterConfigs[0]->restValue = 0.11;
        slot.modulationConfig->parameterConfigs[0]->sources.push_back(
            std::make_shared<PluginParameterModulationSource>(ModulationSourceDefinition(1, MODULATION_TYPE::MACRO), 0.12));

        // The second parameter has 2 modulation sources
        slot.modulationConfig->parameterConfigs[1]->targetParameterName = "param2";
        slot.modulationConfig->parameterConfigs[1]->restValue = 0.21;
        slot.modulationConfig->parameterConfigs[1]->sources.push_back(
            std::make_shared<PluginParameterModulationSource>(ModulationSourceDefinition(2, MODULATION_TYPE::LFO), 0.22));
        slot.modulationConfig->parameterConfigs[1]->sources.push_back(
            std::make_shared<PluginParameterModulationSource>(ModulationSourceDefinition(3, MODULATION_TYPE::ENVELOPE), 0.32));

        slot.plugin->getHostedParameter(2)->setValue(0.5);

        WHEN("The buffer is processed") {
            juce::MidiBuffer midiBuffer;

            ChainProcessors::prepareToPlay(slot, {hostConfig.layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(slot, buffer, midiBuffer, nullptr);

            THEN("The buffer is processed and modulation is applied correctly") {
                CHECK(didCallProcess);

                // Check the buffer
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);
                    const float expectedValue = channelIdx == 0 ? 0.1 : 0.2;

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == expectedValue);
                    }
                }

                // Check the parameter modulation
                CHECK(slot.plugin->getHostedParameter(0)->getValue() == Approx(0.11 + 0.12 * 0.13));
                CHECK(slot.plugin->getHostedParameter(1)->getValue() == Approx(0.21 + 0.22 * 0.23 + 0.32 * 0.33));
                CHECK(slot.plugin->getHostedParameter(2)->getValue() == Approx(0.5));
            }
        }
    }
}

SCENARIO("ChainProcessors: Spare SC buffer is used when plugin is expecting a sidechain input and Syndicate can't provide one") {
    GIVEN("A plugin with three parameters (two which are modulated) and a buffer of 1's") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        juce::AudioBuffer<float> buffer(2, NUM_SAMPLES);
        juce::FloatVectorOperations::fill(buffer.getWritePointer(0), 1, buffer.getNumSamples());
        juce::FloatVectorOperations::fill(buffer.getWritePointer(1), 1, buffer.getNumSamples());

        bool didCallProcess {false};
        std::shared_ptr<ProcessorTestPluginInstance> plugin(ProcessorTestPluginInstance::createWithSidechain());
        plugin->onProcess = [&didCallProcess](juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
            didCallProcess = true;

            // TODO Add checks for midi
            CHECK(buffer.getNumChannels() == 4);
            CHECK(buffer.getNumSamples() == 64);

            juce::FloatVectorOperations::fill(buffer.getWritePointer(0), 0.1, buffer.getNumSamples());
            juce::FloatVectorOperations::fill(buffer.getWritePointer(1), 0.2, buffer.getNumSamples());
        };

        // Configure the plugin to expect a sidechain input
        juce::AudioProcessor::BusesLayout pluginLayout;
        pluginLayout.inputBuses.add(juce::AudioChannelSet::stereo());
        pluginLayout.inputBuses.add(juce::AudioChannelSet::stereo());
        pluginLayout.outputBuses.add(juce::AudioChannelSet::stereo());
        REQUIRE(plugin->setBusesLayout(pluginLayout));

        ChainSlotPlugin slot(plugin, false, [](int id, MODULATION_TYPE type) { return 0.0f; }, hostConfig);
        REQUIRE(slot.spareSCBuffer->getNumChannels() == 4);

        WHEN("The buffer is processed") {
            juce::MidiBuffer midiBuffer;

            ChainProcessors::prepareToPlay(slot, {hostConfig.layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(slot, buffer, midiBuffer, nullptr);

            THEN("The buffer is processed and modulation is applied correctly") {
                CHECK(didCallProcess);

                // Check the buffer
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);
                    const float expectedValue = channelIdx == 0 ? 0.1 : 0.2;

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == expectedValue);
                    }
                }
            }
        }
    }
}
