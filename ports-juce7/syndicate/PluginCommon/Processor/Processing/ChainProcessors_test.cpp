#include "catch.hpp"
#include "TestUtils.hpp"

#include "ChainMutators.hpp"
#include "ChainProcessors.hpp"

namespace {
    constexpr int NUM_SAMPLES {64};
    constexpr int SAMPLE_RATE {44100};

    class ProcessorTestPluginInstance : public TestUtils::TestPluginInstance {
    public:
        std::optional<std::function<void(double, int)>> onPrepareToPlay;
        std::optional<std::function<void()>> onReleaseResources;
        std::optional<std::function<void()>> onReset;
        std::optional<std::function<void(juce::AudioBuffer<float>&, juce::MidiBuffer&)>> onProcess;

        ProcessorTestPluginInstance() = default;

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override {
            if (onPrepareToPlay.has_value()) {
                onPrepareToPlay.value()(sampleRate, maximumExpectedSamplesPerBlock);
            }
        }

        void releaseResources() override {
            if (onReleaseResources.has_value()) {
                onReleaseResources.value()();
            }
        }

        void reset() override {
            if (onReset.has_value()) {
                onReset.value()();
            }
        }

        void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override {
            if (onProcess.has_value()) {
                onProcess.value()(buffer, midiMessages);
            }
        }
    };
}

SCENARIO("ChainProcessors: Silence in = silence out") {
    GIVEN("Some slots and a buffer of silence") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        // Different chain slots
        const auto chainLayout = GENERATE(
            std::vector<std::string>{},
            std::vector<std::string>{"gain", "plugin"},
            std::vector<std::string>{"plugin", "gain"}
        );

        const bool isChainBypassed = GENERATE(false, true);
        const bool isChainMuted = GENERATE(false, true);

        // Mono and stereo
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

        auto chain = std::make_shared<PluginChain>(modulationCallback);
        chain->isChainBypassed = isChainBypassed;
        chain->isChainMuted = isChainMuted;

        bool didCallPluginProcess {false};
        const int expectedNumChannels {buffer.getNumChannels()};

        for (int slotIndex {0}; slotIndex < chainLayout.size(); slotIndex++) {
            if (chainLayout[slotIndex] == "gain") {
                ChainMutators::insertGainStage(chain, slotIndex, {layout, SAMPLE_RATE, NUM_SAMPLES});

            } else if (chainLayout[slotIndex] == "plugin") {
                auto plugin = std::make_shared<ProcessorTestPluginInstance>();
                plugin->onProcess = [&didCallPluginProcess, expectedNumChannels](juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
                    didCallPluginProcess = true;

                    // TODO Add checks for midi
                    CHECK(buffer.getNumChannels() == expectedNumChannels);
                    CHECK(buffer.getNumSamples() == NUM_SAMPLES);
                };
                ChainMutators::insertPlugin(chain, plugin, slotIndex, hostConfig);
            }
        }

        WHEN("The buffer is processed") {
            juce::MidiBuffer midiBuffer;

            ChainProcessors::prepareToPlay(*(chain.get()), {layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(*(chain.get()), buffer, midiBuffer, nullptr);

            THEN("The buffer contains silence") {
                const bool expectDidCallPluginProcess {
                    !(isChainBypassed || isChainMuted) && ChainMutators::getNumSlots(chain) != 0
                };
                CHECK(didCallPluginProcess == expectDidCallPluginProcess);

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

SCENARIO("ChainProcessors: Gain stage and plugin processing is applied correctly") {
    GIVEN("A gain stage and a buffer of 1's") {
        HostConfiguration hostConfig;
        hostConfig.sampleRate = 44100;
        hostConfig.blockSize = 10;
        hostConfig.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());
        
        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        // Different chain layouts
        auto [chainLayout, expectedOutput] = GENERATE(
            std::make_pair<std::vector<std::string>, float>({}, 1),
            std::make_pair<std::vector<std::string>, float>({"gain", "plugin"}, 0.8),
            std::make_pair<std::vector<std::string>, float>({"plugin", "gain"}, 0.65)
        );

        const bool isChainBypassed = GENERATE(false, true);
        const bool isChainMuted = GENERATE(false, true);

        // Mono and stereo
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

        // Do this after calling GENERATE so it gets reset for each test
        for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
            juce::FloatVectorOperations::fill(buffer.getWritePointer(channelIdx), 1, buffer.getNumSamples());
        }

        auto chain = std::make_shared<PluginChain>(modulationCallback);
        chain->isChainBypassed = isChainBypassed;
        chain->isChainMuted = isChainMuted;

        for (int slotIndex {0}; slotIndex < chainLayout.size(); slotIndex++) {
            if (chainLayout[slotIndex] == "gain") {
                ChainMutators::insertGainStage(chain, slotIndex, {layout, SAMPLE_RATE, NUM_SAMPLES});
                ChainMutators::setGainLinear(chain, slotIndex, 0.5);
                // ChainMutators::setPan(-0.5);

            } else if (chainLayout[slotIndex] == "plugin") {
                auto plugin = std::make_shared<ProcessorTestPluginInstance>();
                plugin->onProcess = [](juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
                    for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                       juce::FloatVectorOperations::add(buffer.getWritePointer(channelIdx), 0.3, buffer.getNumSamples());
                    }
                };
                ChainMutators::insertPlugin(chain, plugin, slotIndex, hostConfig);
            }
        }

        WHEN("The buffer is processed") {
            juce::MidiBuffer midiBuffer;

            ChainProcessors::prepareToPlay(*(chain.get()), {layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainProcessors::processBlock(*(chain.get()), buffer, midiBuffer, nullptr);

            THEN("The buffer contains silence") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        const float finalExpectedOutput {
                            isChainMuted ? 0 : isChainBypassed ? 1 : expectedOutput
                        };
                        CHECK(readPtr[sampleIdx] == Approx(finalExpectedOutput));
                    }
                }
            }
        }
    }
}

SCENARIO("ChainProcessors: Latency is applied correctly") {
    GIVEN("An empty chain and a buffer with a single value") {
        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        const bool isChainBypassed = GENERATE(false, true);
        const bool isChainMuted = GENERATE(false, true);

        // Mono and stereo
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

        // Add a single sample
        for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
            buffer.getWritePointer(channelIdx)[0] = 1;
        }

        auto chain = std::make_shared<PluginChain>(modulationCallback);
        chain->isChainBypassed = isChainBypassed;
        chain->isChainMuted = isChainMuted;

        constexpr int sampleDelay {10};

        WHEN("The latency mutex is locked and the buffer is processed") {
            juce::MidiBuffer midiBuffer;

            ChainProcessors::prepareToPlay(*(chain.get()), {layout, SAMPLE_RATE, NUM_SAMPLES});
            ChainMutators::setRequiredLatency(chain, sampleDelay, {layout, SAMPLE_RATE, NUM_SAMPLES});

            {
                WECore::AudioSpinTryLock lock(chain->latencyCompLineMutex);
                ChainProcessors::processBlock(*(chain.get()), buffer, midiBuffer, nullptr);
            }

            THEN("The buffer is unchanged") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        const float expectedSample {
                            sampleIdx == 0 && !isChainMuted ? 1.0f : 0.0f
                        };
                        CHECK(readPtr[sampleIdx] == expectedSample);
                    }
                }
            }
        }

        WHEN("The buffer is processed") {
            juce::MidiBuffer midiBuffer;

            ChainProcessors::prepareToPlay(*(chain.get()), {layout, SAMPLE_RATE, NUM_SAMPLES});
                ChainMutators::setRequiredLatency(chain, sampleDelay, {layout, SAMPLE_RATE, NUM_SAMPLES});

            ChainProcessors::processBlock(*(chain.get()), buffer, midiBuffer, nullptr);

            THEN("The buffer is delayed correctly") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        const float expectedSample {
                            sampleIdx == sampleDelay && !isChainMuted ? 1.0f : 0.0f
                        };
                        CHECK(readPtr[sampleIdx] == expectedSample);
                    }
                }
            }
        }
    }
}

SCENARIO("ChainProcessors: Plugin methods are called correctly") {
    GIVEN("A chain of plugins") {
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
        auto plugin1 = std::make_shared<ProcessorTestPluginInstance>();
        auto plugin2 = std::make_shared<ProcessorTestPluginInstance>();
        ChainMutators::insertPlugin(chain, plugin1, 0, hostConfig);
        ChainMutators::insertPlugin(chain, plugin2, 1, hostConfig);

        WHEN("prepareToPlay is called") {
            bool calledPrepareToPlay1 {false};
            plugin1->onPrepareToPlay = [&calledPrepareToPlay1](double sampleRate, int samplesPerBlock) {
                calledPrepareToPlay1 = true;
                CHECK(sampleRate == SAMPLE_RATE);
                CHECK(samplesPerBlock == NUM_SAMPLES);
            };

            bool calledPrepareToPlay2 {false};
            plugin2->onPrepareToPlay = [&calledPrepareToPlay2](double sampleRate, int samplesPerBlock) {
                calledPrepareToPlay2 = true;
                CHECK(sampleRate == SAMPLE_RATE);
                CHECK(samplesPerBlock == NUM_SAMPLES);
            };

            ChainProcessors::prepareToPlay(*(chain.get()), {layout, SAMPLE_RATE, NUM_SAMPLES});

            THEN("The plugin's prepareToPlay is called with the correct arguments") {
                CHECK(calledPrepareToPlay1);
                CHECK(calledPrepareToPlay2);
            }
        }

        WHEN("releaseResources is called") {
            bool calledReleaseResources1 {false};
            plugin1->onReleaseResources = [&calledReleaseResources1]() {
                calledReleaseResources1 = true;
            };

            bool calledReleaseResources2 {false};
            plugin2->onReleaseResources = [&calledReleaseResources2]() {
                calledReleaseResources2 = true;
            };

            ChainProcessors::releaseResources(*(chain.get()));

            THEN("The plugin's releaseResources is called with the correct arguments") {
                CHECK(calledReleaseResources1);
                CHECK(calledReleaseResources2);
            }
        }

        WHEN("reset is called") {
            bool calledReset1 {false};
            plugin1->onReset = [&calledReset1]() {
                calledReset1 = true;
            };

            bool calledReset2 {false};
            plugin2->onReset = [&calledReset2]() {
                calledReset2 = true;
            };

            ChainProcessors::reset(*(chain.get()));

            THEN("The plugin's reset is called with the correct arguments") {
                CHECK(calledReset1);
                CHECK(calledReset2);
            }
        }
    }
}
