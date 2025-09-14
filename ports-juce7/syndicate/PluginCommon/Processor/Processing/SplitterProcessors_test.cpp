#include "catch.hpp"
#include "TestUtils.hpp"
#include "SplitterMutators.hpp"
#include "SplitterProcessors.hpp"

namespace {
    constexpr int NUM_SAMPLES {10};
    constexpr int SAMPLE_RATE {2000};

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

SCENARIO("SplitterProcessors: Silence in = silence out") {
    GIVEN("A splitter and a buffer of silence") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        auto latencyCallback = [](int) {
            // Do nothing
        };

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

        config.layout = layout;

        // Do this after calling GENERATE so it gets reset for each test
        for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
            juce::FloatVectorOperations::fill(buffer.getWritePointer(channelIdx), 0, buffer.getNumSamples());
        }

        const juce::String splitTypeString = GENERATE(
            juce::String(XML_SPLIT_TYPE_SERIES_STR),
            juce::String(XML_SPLIT_TYPE_PARALLEL_STR),
            // juce::String(XML_SPLIT_TYPE_MULTIBAND_STR), // TODO multiband crashes on vDSP_destroy_fftsetup
            juce::String(XML_SPLIT_TYPE_LEFTRIGHT_STR),
            juce::String(XML_SPLIT_TYPE_MIDSIDE_STR)
        );

        std::shared_ptr<PluginSplitter> splitter;

        if (splitTypeString == XML_SPLIT_TYPE_SERIES_STR) {
            auto splitterSeries = std::make_shared<PluginSplitterSeries>(config, modulationCallback, latencyCallback);
            splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterSeries);
        } else if (splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR) {
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

        const bool isChainSoloed = GENERATE(false, true);
        SplitterMutators::setChainSolo(splitter, 0, isChainSoloed);

        // Add a gain stage to the first chain
        SplitterMutators::insertGainStage(splitter, 0, 0);
        SplitterMutators::setGainLinear(splitter, 0, 0, 0.5);
        SplitterMutators::setPan(splitter, 0, 0, -0.5);

        // Add a plugin to the second chain
        auto plugin = std::make_shared<ProcessorTestPluginInstance>();
        plugin->onProcess = [](juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
            for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                juce::FloatVectorOperations::multiply(buffer.getWritePointer(channelIdx), 0.3, buffer.getNumSamples());
            }
        };
        SplitterMutators::insertPlugin(splitter, plugin, 1, 0);

        WHEN("The buffer is processed") {
            juce::MidiBuffer midiBuffer;

            if (buffer.getNumChannels() == 1 &&
                (splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR || splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR)) {
                // Skip these - can't use mono for these split types
            } else {
                SplitterProcessors::prepareToPlay(*(splitter.get()), SAMPLE_RATE, NUM_SAMPLES, layout);
                SplitterProcessors::processBlock(*(splitter.get()), buffer, midiBuffer, nullptr);
            }


            THEN("The buffer contains silence") {
                for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                    const auto readPtr = buffer.getReadPointer(channelIdx);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readPtr[sampleIdx] == Approx(0.0f));
                    }
                }
            }
        }
    }
}

SCENARIO("SplitterProcessors: Gain stage and plugin processing is applied correctly") {
    GIVEN("A splitter and a buffer of 1's") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        auto latencyCallback = [](int) {
            // Do nothing
        };

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

        config.layout = layout;

        const juce::String splitTypeString = GENERATE(
            juce::String(XML_SPLIT_TYPE_SERIES_STR),
            juce::String(XML_SPLIT_TYPE_PARALLEL_STR),
            // juce::String(XML_SPLIT_TYPE_MULTIBAND_STR), // TODO multiband crashes on vDSP_destroy_fftsetup
            juce::String(XML_SPLIT_TYPE_LEFTRIGHT_STR),
            juce::String(XML_SPLIT_TYPE_MIDSIDE_STR)
        );

        std::shared_ptr<PluginSplitter> splitter;

        if (splitTypeString == XML_SPLIT_TYPE_SERIES_STR) {
            auto splitterSeries = std::make_shared<PluginSplitterSeries>(config, modulationCallback, latencyCallback);
            splitter = std::dynamic_pointer_cast<PluginSplitter>(splitterSeries);
        } else if (splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR) {
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

        const bool isChainSoloed = GENERATE(false, true);
        SplitterMutators::setChainSolo(splitter, 0, isChainSoloed);

        // Add a gain stage to the first chain
        SplitterMutators::insertGainStage(splitter, 0, 0);
        SplitterMutators::setGainLinear(splitter, 0, 0, 0.5);
        SplitterMutators::setPan(splitter, 0, 0, -0.5);

        // Add a plugin to the second chain
        auto plugin = std::make_shared<ProcessorTestPluginInstance>();
        plugin->onProcess = [](juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
            for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
                juce::FloatVectorOperations::add(buffer.getWritePointer(channelIdx), 0.3, buffer.getNumSamples());
            }
        };
        SplitterMutators::insertPlugin(splitter, plugin, 1, 0);

        // Do this after calling GENERATE so it gets reset for each test
        for (int channelIdx {0}; channelIdx < buffer.getNumChannels(); channelIdx++) {
            juce::FloatVectorOperations::fill(buffer.getWritePointer(channelIdx), 1, buffer.getNumSamples());
        }

        WHEN("The buffer is processed") {
            juce::MidiBuffer midiBuffer;

            if (buffer.getNumChannels() == 1 &&
                (splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR || splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR)) {
                // Skip these - can't use mono for these split types
            } else {
                SplitterProcessors::prepareToPlay(*(splitter.get()), SAMPLE_RATE, NUM_SAMPLES, layout);
                SplitterProcessors::processBlock(*(splitter.get()), buffer, midiBuffer, nullptr);
            }

            THEN("The buffer contains silence") {
                if (buffer.getNumChannels() == 1) {
                    // Mono
                    if (!(splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR || splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR)) {
                        const float expectedSample {
                            splitTypeString == XML_SPLIT_TYPE_SERIES_STR || isChainSoloed ? 0.5f :
                            splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR ? 1.8f : 0.0f
                        };

                        const auto readPtr = buffer.getReadPointer(0);

                        for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                            CHECK(readPtr[sampleIdx] == Approx(expectedSample));
                        }
                    }
                } else {
                    // Stereo
                    const float expectedLeftSample {
                        splitTypeString == XML_SPLIT_TYPE_SERIES_STR || isChainSoloed ? 0.5f :
                        splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR ? 1.8f :
                        splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR ? 0.8f :
                        splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR ? 0.8f : 0.0f
                    };

                    const float expectedRightSample {
                        splitTypeString == XML_SPLIT_TYPE_SERIES_STR ? 0.25f :
                        splitTypeString == XML_SPLIT_TYPE_SERIES_STR ? 0.25f :
                        splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR && !isChainSoloed ? 1.55f :
                        splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR && isChainSoloed ? 0.25f :
                        splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR && !isChainSoloed ? 1.3f :
                        splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR && isChainSoloed ? 0.0f :
                        splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR && !isChainSoloed ? 0.2f :
                        splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR && isChainSoloed ? 0.5f : 0.0f
                    };

                    const auto readLeftPtr = buffer.getReadPointer(0);
                    const auto readRightPtr = buffer.getReadPointer(1);

                    for (int sampleIdx {0}; sampleIdx < buffer.getNumSamples(); sampleIdx++) {
                        CHECK(readLeftPtr[sampleIdx] == Approx(expectedLeftSample));
                        CHECK(readRightPtr[sampleIdx] == Approx(expectedRightSample));
                    }
                }
            }
        }
    }
}

SCENARIO("SplitterProcessors: Chain methods are called correctly") {
    GIVEN("A parallel splitter and an empty buffer") {
        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;
        config.layout = TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo());

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        auto latencyCallback = [](int) {
            // Do nothing
        };

        auto splitterParallel = std::make_shared<PluginSplitterParallel>(config, modulationCallback, latencyCallback);
        SplitterMutators::addChain(splitterParallel);

        auto plugin1 = std::make_shared<ProcessorTestPluginInstance>();
        auto plugin2 = std::make_shared<ProcessorTestPluginInstance>();

        SplitterMutators::insertPlugin(splitterParallel, plugin1, 0, 0);
        SplitterMutators::insertPlugin(splitterParallel, plugin2, 1, 0);

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

            // Set the buffers to something random so we can check if they get reset
            splitterParallel->inputBuffer.reset(new juce::AudioBuffer<float>(1, 5));
            splitterParallel->outputBuffer.reset(new juce::AudioBuffer<float>(3, 84));

            SplitterProcessors::prepareToPlay(*(splitterParallel.get()), SAMPLE_RATE, NUM_SAMPLES, config.layout);

            THEN("Each chain's prepareToPlay is called with the correct arguments") {
                CHECK(calledPrepareToPlay1);
                CHECK(calledPrepareToPlay2);

                CHECK(splitterParallel->config.sampleRate == SAMPLE_RATE);
                CHECK(splitterParallel->config.blockSize == NUM_SAMPLES);

                CHECK(splitterParallel->inputBuffer->getNumChannels() == 2);
                CHECK(splitterParallel->inputBuffer->getNumSamples() == NUM_SAMPLES);
                CHECK(splitterParallel->outputBuffer->getNumChannels() == 2);
                CHECK(splitterParallel->outputBuffer->getNumSamples() == NUM_SAMPLES);
            }

            WHEN("The splitter is converted to series and prepareToPlay is called") {
                calledPrepareToPlay1 = false;
                calledPrepareToPlay2 = false;

                auto splitterSeries = std::make_shared<PluginSplitterSeries>(splitterParallel);
                SplitterProcessors::prepareToPlay(*(splitterSeries.get()), SAMPLE_RATE, NUM_SAMPLES, config.layout);

                THEN("Each chain's prepareToPlay is called with the correct arguments") {
                    CHECK(calledPrepareToPlay1);
                    CHECK(calledPrepareToPlay2);
                    CHECK(splitterSeries->config.sampleRate == SAMPLE_RATE);
                    CHECK(splitterSeries->config.blockSize == NUM_SAMPLES);
                }
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

            SplitterProcessors::releaseResources(*(splitterParallel.get()));

            THEN("Each chain's releaseResources is called") {
                CHECK(calledReleaseResources1);
                CHECK(calledReleaseResources2);
            }

            WHEN("The splitter is converted to series and releaseResources is called") {
                calledReleaseResources1 = false;
                calledReleaseResources2 = false;

                auto splitterSeries = std::make_shared<PluginSplitterSeries>(splitterParallel);
                SplitterProcessors::releaseResources(*(splitterSeries.get()));

                THEN("Each chain's releaseResources is called") {
                    CHECK(calledReleaseResources1);
                    CHECK(calledReleaseResources2);
                }
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

            SplitterProcessors::reset(*(splitterParallel.get()));

            THEN("Each chain's reset is called") {
                CHECK(calledReset1);
                CHECK(calledReset2);
            }

            WHEN("The splitter is converted to series and reset is called") {
                calledReset1 = false;
                calledReset2 = false;

                auto splitterSeries = std::make_shared<PluginSplitterSeries>(splitterParallel);
                SplitterProcessors::reset(*(splitterSeries.get()));

                THEN("Each chain's releaseResources is called") {
                    CHECK(calledReset1);
                    CHECK(calledReset2);
                }
            }
        }
    }
}
