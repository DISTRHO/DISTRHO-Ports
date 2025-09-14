#include "catch.hpp"

#include "TestUtils.hpp"

namespace {
    class ConfigTestPluginInstance : public TestUtils::TestPluginInstance {
    public:
        bool isPrepared;
        std::function<bool(const juce::AudioProcessor::BusesLayout&)> onIsBusesLayoutSupported;

        ConfigTestPluginInstance(std::function<bool(const juce::AudioProcessor::BusesLayout&)> newOnIsBusesLayoutSupported) : isPrepared(false), onIsBusesLayoutSupported(newOnIsBusesLayoutSupported) {}

        void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) override {
            TestUtils::TestPluginInstance::prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock);
            isPrepared = true;
        }

    protected:
        bool isBusesLayoutSupported(const BusesLayout& arr) const override {
            return onIsBusesLayoutSupported(arr);
        }
    };

    HostConfiguration getHostConfig(std::string name) {
        HostConfiguration config;

        if (name == "mono") {
            config.layout.inputBuses.add(juce::AudioChannelSet::mono());
            config.layout.outputBuses.add(juce::AudioChannelSet::mono());
            config.sampleRate = 80000;
            config.blockSize = 100;
        } else if (name == "monoSC") {
            config.layout.inputBuses.add(juce::AudioChannelSet::mono());
            config.layout.inputBuses.add(juce::AudioChannelSet::mono());
            config.layout.outputBuses.add(juce::AudioChannelSet::mono());
            config.sampleRate = 85000;
            config.blockSize = 150;
        } else if (name == "stereo") {
            config.layout.inputBuses.add(juce::AudioChannelSet::stereo());
            config.layout.outputBuses.add(juce::AudioChannelSet::stereo());
            config.sampleRate = 90000;
            config.blockSize = 200;
        } else if (name == "stereoSC") {
            config.layout.inputBuses.add(juce::AudioChannelSet::stereo());
            config.layout.inputBuses.add(juce::AudioChannelSet::stereo());
            config.layout.outputBuses.add(juce::AudioChannelSet::stereo());
            config.sampleRate = 95000;
            config.blockSize = 250;
        }

        return config;
    }

    std::vector<juce::AudioProcessor::BusesLayout> getExpectedLayouts(std::string name) {
        std::vector<juce::AudioProcessor::BusesLayout> layouts;

        if (name == "mono") {
            juce::AudioProcessor::BusesLayout mono;
            mono.inputBuses.add(juce::AudioChannelSet::mono());
            mono.outputBuses.add(juce::AudioChannelSet::mono());
            layouts.push_back(mono);

            juce::AudioProcessor::BusesLayout monoSC;
            monoSC.inputBuses.add(juce::AudioChannelSet::mono());
            monoSC.inputBuses.add(juce::AudioChannelSet::mono());
            monoSC.outputBuses.add(juce::AudioChannelSet::mono());
            layouts.push_back(monoSC);
        } else if (name == "monoSC") {
            juce::AudioProcessor::BusesLayout monoSC;
            monoSC.inputBuses.add(juce::AudioChannelSet::mono());
            monoSC.inputBuses.add(juce::AudioChannelSet::mono());
            monoSC.outputBuses.add(juce::AudioChannelSet::mono());
            layouts.push_back(monoSC);

            juce::AudioProcessor::BusesLayout mono;
            mono.inputBuses.add(juce::AudioChannelSet::mono());
            mono.outputBuses.add(juce::AudioChannelSet::mono());
            layouts.push_back(mono);
        } else if (name == "stereo") {
            juce::AudioProcessor::BusesLayout stereo;
            stereo.inputBuses.add(juce::AudioChannelSet::stereo());
            stereo.outputBuses.add(juce::AudioChannelSet::stereo());
            layouts.push_back(stereo);

            juce::AudioProcessor::BusesLayout stereoSC;
            stereoSC.inputBuses.add(juce::AudioChannelSet::stereo());
            stereoSC.inputBuses.add(juce::AudioChannelSet::stereo());
            stereoSC.outputBuses.add(juce::AudioChannelSet::stereo());
            layouts.push_back(stereoSC);
        } else if (name == "stereoSC") {
            juce::AudioProcessor::BusesLayout stereoSC;
            stereoSC.inputBuses.add(juce::AudioChannelSet::stereo());
            stereoSC.inputBuses.add(juce::AudioChannelSet::stereo());
            stereoSC.outputBuses.add(juce::AudioChannelSet::stereo());
            layouts.push_back(stereoSC);

            juce::AudioProcessor::BusesLayout stereo;
            stereo.inputBuses.add(juce::AudioChannelSet::stereo());
            stereo.outputBuses.add(juce::AudioChannelSet::stereo());
            layouts.push_back(stereo);
        }

        return layouts;
    }
}

SCENARIO("PluginConfigurator: Can configure the plugin correctly") {
    PluginConfigurator configurator;

    GIVEN("A host configuration and a plugin") {
        typedef std::tuple<std::string, bool> TestData;

        auto [hostConfigString, shouldSucceed] = GENERATE(
            TestData("mono", true),
            TestData("monoSC", true),
            TestData("stereo", true),
            TestData("stereoSC", true),
            TestData("mono", false),
            TestData("monoSC", false),
            TestData("stereo", false),
            TestData("stereoSC", false)
        );

        const HostConfiguration hostConfig = getHostConfig(hostConfigString);

        std::vector<juce::AudioProcessor::BusesLayout> testedLayouts;
        auto plugin = std::make_shared<ConfigTestPluginInstance>([hostConfig, shouldSucceed = shouldSucceed, &testedLayouts](const juce::AudioProcessor::BusesLayout& layout) {
            testedLayouts.push_back(layout);
            return shouldSucceed;
        });

        WHEN("Asked to configure it") {
            const bool success {configurator.configure(plugin, hostConfig)};

            THEN("The plugin is configured") {
                CHECK(success == shouldSucceed);
                CHECK(plugin->isPrepared == shouldSucceed);

                const auto expectedLayouts = getExpectedLayouts(hostConfigString);
                if (shouldSucceed) {
                    CHECK(plugin->getSampleRate() == hostConfig.sampleRate);
                    CHECK(plugin->getBlockSize() == hostConfig.blockSize);

                    // Just check the first layout as the configurator will stop after the first success
                    CHECK(testedLayouts[0] == expectedLayouts[0]);
                } else {
                    // We can check each layout as the configurator will try all of them
                    CHECK(testedLayouts == expectedLayouts);
                }
            }
        }
    }
}
