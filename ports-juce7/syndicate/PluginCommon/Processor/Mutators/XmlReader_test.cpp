#include "catch.hpp"

#include "XmlReader.hpp"
#include "XmlConsts.hpp"
#include "XmlWriter.hpp"
#include "TestUtils.hpp"
#include "SplitterMutators.hpp"
#include "ModulationMutators.hpp"

namespace {
    // Use values that would never be sensible  defaults so we know if they were actually set
    constexpr int NUM_SAMPLES {10};
    constexpr int SAMPLE_RATE {2000};

    class XMLTestPluginInstance : public TestUtils::TestPluginInstance {
    public:
        juce::PluginDescription description;
        std::string retrievedData;

        bool shouldSetLayoutSuccessfully;

        XMLTestPluginInstance() : shouldSetLayoutSuccessfully(true) { }

        XMLTestPluginInstance(const juce::PluginDescription& newDescription) : description(newDescription) {
            shouldSetLayoutSuccessfully = description.name == "failLayout" ? false : true;
        }

        void getStateInformation(juce::MemoryBlock& destData) override {
            const std::string testString("testPluginData");
            destData.append(testString.c_str(), testString.size());
         }
        void setStateInformation(const void* data, int sizeInBytes) override { retrievedData = std::string(static_cast<const char*>(data), sizeInBytes); }

    protected:
        bool isBusesLayoutSupported(const BusesLayout& arr) const override { return shouldSetLayoutSuccessfully; }
    };
}

SCENARIO("XmlReader: Can restore PluginSplitter") {
    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;

        PluginConfigurator configurator;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        bool latencyCalled {false};
        auto latencyCallback = [&latencyCalled](int) {
            latencyCalled = true;
        };

        auto errorCallback = [](juce::String) {
            // Do nothing
        };

        WHEN("Asked to restore a PluginSplitter from it") {
            const juce::Array<juce::PluginDescription> pluginTypes;
            std::shared_ptr<PluginSplitter> splitter = XmlReader::restoreSplitterFromXml(
                &e, modulationCallback, latencyCallback, config, configurator, pluginTypes, errorCallback);

            THEN("A PluginSplitter with default values is created") {
                CHECK(std::dynamic_pointer_cast<PluginSplitterSeries>(splitter) != nullptr);
                CHECK(splitter->chains.size() == 0);
                CHECK(splitter->numChainsSoloed == 0);
                CHECK(splitter->config.sampleRate == SAMPLE_RATE);
                CHECK(splitter->config.blockSize == NUM_SAMPLES);
                CHECK(latencyCalled);
            }
        }
    }

    GIVEN("An XmlElement with three chains, one soloed") {
        juce::XmlElement e("test");

        HostConfiguration config;
        config.sampleRate = SAMPLE_RATE;
        config.blockSize = NUM_SAMPLES;
        config.layout = TestUtils::createLayoutWithChannels(
            juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo());

        PluginConfigurator configurator;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        bool latencyCalled {false};
        auto latencyCallback = [&latencyCalled](int) {
            latencyCalled = true;
        };

        auto errorCallback = [](juce::String) {
            // Do nothing
        };

        const juce::String splitTypeString = GENERATE(
            juce::String(XML_SPLIT_TYPE_SERIES_STR),
            juce::String(XML_SPLIT_TYPE_PARALLEL_STR),
            juce::String(XML_SPLIT_TYPE_MULTIBAND_STR),
            juce::String(XML_SPLIT_TYPE_LEFTRIGHT_STR),
            juce::String(XML_SPLIT_TYPE_MIDSIDE_STR)
        );

        // Build the XML elements
        e.setAttribute(XML_SPLIT_TYPE_STR, splitTypeString);
        juce::XmlElement* chainsElement = e.createNewChildElement(XML_CHAINS_STR);

        // First chain
        auto chain1 = std::make_shared<PluginChain>(modulationCallback);
        ChainMutators::insertGainStage(chain1, 0, config);

        juce::XmlElement* chainElement1 = chainsElement->createNewChildElement(getChainXMLName(0));
        chainElement1->setAttribute(XML_IS_CHAIN_SOLOED_STR, true);
        XmlWriter::write(chain1, chainElement1);

        // Second chain
        auto chain2 = std::make_shared<PluginChain>(modulationCallback);

        ChainMutators::insertGainStage(chain2, 0, config);
        ChainMutators::insertGainStage(chain2, 1, config);

        juce::XmlElement* chainElement2 = chainsElement->createNewChildElement(getChainXMLName(1));
        chainElement2->setAttribute(XML_IS_CHAIN_SOLOED_STR, false);
        XmlWriter::write(chain2, chainElement2);

        // Third chain
        auto chain3 = std::make_shared<PluginChain>(modulationCallback);
        ChainMutators::insertGainStage(chain3, 0, config);

        juce::XmlElement* chainElement3 = chainsElement->createNewChildElement(getChainXMLName(2));
        chainElement3->setAttribute(XML_IS_CHAIN_SOLOED_STR, true);
        XmlWriter::write(chain3, chainElement3);

        if (splitTypeString == XML_SPLIT_TYPE_MULTIBAND_STR) {
            juce::XmlElement* crossoverElement = e.createNewChildElement(XML_CROSSOVERS_STR);
            crossoverElement->setAttribute(getCrossoverXMLName(0), 2500);
            crossoverElement->setAttribute(getCrossoverXMLName(1), 4000);
        }

        WHEN("Asked to restore a PluginSplitter from it") {
            const juce::Array<juce::PluginDescription> pluginTypes;
            std::shared_ptr<PluginSplitter> splitter = XmlReader::restoreSplitterFromXml(
                &e, modulationCallback, latencyCallback, config, configurator, pluginTypes, errorCallback);

            THEN("A PluginSplitter with the correct values is created") {
                if (splitTypeString == XML_SPLIT_TYPE_SERIES_STR) {
                    CHECK(std::dynamic_pointer_cast<PluginSplitterSeries>(splitter) != nullptr);
                } else if (splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR) {
                    CHECK(std::dynamic_pointer_cast<PluginSplitterParallel>(splitter) != nullptr);
                } else if (splitTypeString == XML_SPLIT_TYPE_MULTIBAND_STR) {
                    auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter);
                    REQUIRE(multibandSplitter != nullptr);
                    CHECK(SplitterMutators::getCrossoverFrequency(multibandSplitter, 0) == Approx(2500.0));
                    CHECK(SplitterMutators::getCrossoverFrequency(multibandSplitter, 1) == Approx(4000.0));
                } else if (splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR) {
                    CHECK(std::dynamic_pointer_cast<PluginSplitterLeftRight>(splitter) != nullptr);
                } else if (splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR) {
                    CHECK(std::dynamic_pointer_cast<PluginSplitterMidSide>(splitter) != nullptr);
                }

                CHECK(splitter->chains.size() == 3);
                CHECK(splitter->chains[0].chain->chain.size() == 1);
                CHECK(splitter->chains[1].chain->chain.size() == 2);
                CHECK(splitter->chains[2].chain->chain.size() == 1);
                CHECK(splitter->config.sampleRate == SAMPLE_RATE);
                CHECK(splitter->config.blockSize == NUM_SAMPLES);
                CHECK(latencyCalled);
                CHECK(splitter->numChainsSoloed == 2);
            }
        }
    }
}

SCENARIO("XmlReader: Can restore PluginChain") {
    const juce::Array<juce::PluginDescription> pluginTypes;

    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        HostConfiguration config;
        config.sampleRate = 44100;
        config.blockSize = 64;

        PluginConfigurator configurator;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        auto errorCallback = [](juce::String) {
            // Do nothing
        };

        WHEN("Asked to restore a PluginChain from it") {
            auto chain = XmlReader::restoreChainFromXml(
                &e, config, configurator, modulationCallback, pluginTypes, errorCallback);

            THEN("A PluginChain with default values is created") {
                CHECK(chain->isChainBypassed == false);
                CHECK(chain->isChainMuted == false);
                CHECK(chain->chain.size() == 0);
                CHECK(chain->getModulationValueCallback(0, MODULATION_TYPE::MACRO) == 1.234f);
            }
        }
    }

    GIVEN("An XmlElement that has an invalid plugin element") {
        juce::XmlElement e("test");
        e.setAttribute(XML_IS_CHAIN_BYPASSED_STR, false);
        e.setAttribute(XML_IS_CHAIN_MUTED_STR, false);

        auto* pluginsElement = e.createNewChildElement(XML_PLUGINS_STR);
        auto* invalidPluginElement = pluginsElement->createNewChildElement(getSlotXMLName(0));
        invalidPluginElement->setAttribute(XML_SLOT_TYPE_STR, "some invalid type");

        HostConfiguration config;
        config.sampleRate = 44100;
        config.blockSize = 64;

        PluginConfigurator configurator;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        auto errorCallback = [](juce::String) {
            // Do nothing
        };

        WHEN("Asked to restore a PluginChain from it") {
            auto chain = XmlReader::restoreChainFromXml(
                &e, config, configurator, modulationCallback, pluginTypes, errorCallback);

            THEN("A PluginChain with default values is created") {
                CHECK(chain->isChainBypassed == false);
                CHECK(chain->isChainMuted == false);
                CHECK(chain->chain.size() == 0);
                CHECK(chain->getModulationValueCallback(0, MODULATION_TYPE::MACRO) == 1.234f);
            }
        }
    }

    GIVEN("An XmlElement that has attributes set correctly") {
        const bool isChainBypassed = GENERATE(false, true);
        const bool isChainMuted = GENERATE(false, true);
        const bool includeCustomName = GENERATE(false, true); // Test for backwards compatibility as this was added in 1.3.0

        // Only test loading gain stages - we can't test loading a real plugin here
        const int numGainStages = GENERATE(0, 1, 2);

        juce::XmlElement e("test");

        HostConfiguration config;
        config.sampleRate = 44100;
        config.blockSize = 64;

        PluginConfigurator configurator;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            // Return something unique we can test for later
            return 1.234f;
        };

        auto errorCallback = [](juce::String) {
            // Do nothing
        };

        e.setAttribute(XML_IS_CHAIN_BYPASSED_STR, isChainBypassed);
        e.setAttribute(XML_IS_CHAIN_MUTED_STR, isChainMuted);
        if (includeCustomName) {
            e.setAttribute(XML_CHAIN_CUSTOM_NAME_STR, "testChainName");
        }

        auto* pluginsElement = e.createNewChildElement(XML_PLUGINS_STR);

        for (int index {0}; index < numGainStages; index++) {
            auto* gainStageElement = pluginsElement->createNewChildElement(getSlotXMLName(index));
            gainStageElement->setAttribute(XML_SLOT_TYPE_STR, XML_SLOT_TYPE_GAIN_STAGE_STR);
            gainStageElement->setAttribute(XML_SLOT_IS_BYPASSED_STR, index % 2 == 0);
            gainStageElement->setAttribute(XML_GAIN_STAGE_GAIN_STR, 0.1 * index);
            gainStageElement->setAttribute(XML_GAIN_STAGE_PAN_STR, 0.21 * index);
        }

        WHEN("Asked to restore a PluginChain from it") {
            auto chain = XmlReader::restoreChainFromXml(
                &e, config, configurator, modulationCallback, pluginTypes, errorCallback);

            THEN("A PluginChain with default values is created") {
                CHECK(chain->isChainBypassed == isChainBypassed);
                CHECK(chain->isChainMuted == isChainMuted);
                CHECK(chain->customName == (includeCustomName ? "testChainName" : ""));
                CHECK(chain->chain.size() == numGainStages);
                CHECK(chain->getModulationValueCallback(0, MODULATION_TYPE::MACRO) == 1.234f);

                for (int index {0}; index < numGainStages; index++) {
                    CHECK(chain->chain[index]->isBypassed == (index % 2 == 0));

                    auto gainStage = std::dynamic_pointer_cast<ChainSlotGainStage>(chain->chain[index]);
                    REQUIRE(gainStage != nullptr);
                    CHECK(gainStage->gain == Approx(0.1 * index));
                    CHECK(gainStage->pan == Approx(0.21 * index));
                }
            }
        }
    }
}

SCENARIO("XmlReader: Can determine if an XmlElement is a gain stage or a plugin") {
    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        WHEN("Checked as a plugin") {
            THEN("It's not a plugin") {
                CHECK_FALSE(XmlReader::XmlElementIsPlugin(&e));
            }
        }

        WHEN("Checked as a gain stage") {
            THEN("It's not a gain stage") {
                CHECK_FALSE(XmlReader::XmlElementIsGainStage(&e));
            }
        }
    }

    GIVEN("An XmlElement that has an invalid value in the slot type") {
        juce::XmlElement e("test");
        e.setAttribute(XML_SLOT_TYPE_STR, "some invalid value");

        WHEN("Checked as a plugin") {
            THEN("It's not a plugin") {
                CHECK_FALSE(XmlReader::XmlElementIsPlugin(&e));
            }
        }

        WHEN("Checked as a gain stage") {
            THEN("It's not a gain stage") {
                CHECK_FALSE(XmlReader::XmlElementIsGainStage(&e));
            }
        }
    }

    GIVEN("An XmlElement that is a gain stage") {
        juce::XmlElement e("test");
        e.setAttribute(XML_SLOT_TYPE_STR, XML_SLOT_TYPE_GAIN_STAGE_STR);

        WHEN("Checked as a plugin") {
            THEN("It's not a plugin") {
                CHECK_FALSE(XmlReader::XmlElementIsPlugin(&e));
            }
        }

        WHEN("Checked as a gain stage") {
            THEN("It's a gain stage") {
                CHECK(XmlReader::XmlElementIsGainStage(&e));
            }
        }
    }

    GIVEN("An XmlElement that is a plugin") {
        juce::XmlElement e("test");
        e.setAttribute(XML_SLOT_TYPE_STR, XML_SLOT_TYPE_PLUGIN_STR);

        WHEN("Checked as a plugin") {
            THEN("It's a plugin") {
                CHECK(XmlReader::XmlElementIsPlugin(&e));
            }
        }

        WHEN("Checked as a gain stage") {
            THEN("It's not a gain stage") {
                CHECK_FALSE(XmlReader::XmlElementIsGainStage(&e));
            }
        }
    }
}

SCENARIO("XmlReader: Can restore ChainSlotGainStage") {
    GIVEN("An XmlElement that has no attributes") {
        juce::AudioProcessor::BusesLayout layout;
        juce::XmlElement e("test");

        WHEN("Asked to restore a ChainSlotGainStage from it") {
            auto gainStage = XmlReader::restoreChainSlotGainStage(&e, layout);

            THEN("A ChainSlotGainStage with default values is created") {
                CHECK(gainStage->gain == 1.0f);
                CHECK(gainStage->pan == 0.0f);
                CHECK(gainStage->isBypassed == false);
                CHECK(gainStage->numMainChannels == 0);
            }
        }
    }

    GIVEN("An XmlElement that has attributes set correctly") {
        juce::AudioProcessor::BusesLayout layout = GENERATE(
            TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::mono()),
            TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::stereo()));

        juce::XmlElement e("test");
        e.setAttribute(XML_SLOT_TYPE_STR, XML_SLOT_TYPE_GAIN_STAGE_STR);
        e.setAttribute(XML_GAIN_STAGE_GAIN_STR, 0.5f);
        e.setAttribute(XML_GAIN_STAGE_PAN_STR, 0.6f);
        e.setAttribute(XML_SLOT_IS_BYPASSED_STR, true);

        WHEN("Asked to restore a ChainSlotGainStage from it") {
            auto gainStage = XmlReader::restoreChainSlotGainStage(&e, layout);

            THEN("A ChainSlotGainStage with the correct values is created") {
                CHECK(gainStage->gain == 0.5f);
                CHECK(gainStage->pan == 0.6f);
                CHECK(gainStage->isBypassed == true);
                CHECK(gainStage->numMainChannels == layout.getMainInputChannels());
            }
        }
    }
}

SCENARIO("XmlReader: Can restore ChainSlotPlugin") {
    auto createDefaultTestData = []() {
        return std::make_tuple<
            std::function<float(int, MODULATION_TYPE)>,
            HostConfiguration,
            PluginConfigurator,
            XmlReader::LoadPluginFunction,
            std::function<void(juce::String)>
        >(
            [](int, MODULATION_TYPE) { return 0.0f; },
            HostConfiguration{TestUtils::createLayoutWithInputChannels(juce::AudioChannelSet::mono()), 60000, 100}, // Deliberately use values the code would never default to so we can check they are actually set correctly
            PluginConfigurator(),
            [](const juce::PluginDescription& description, const HostConfiguration& config) {
                return std::make_tuple<std::unique_ptr<juce::AudioPluginInstance>, juce::String>(
                    std::make_unique<XMLTestPluginInstance>(description), ""
                );
            },
            [](juce::String errorMsg) { }
        );
    };

    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        WHEN("Asked to restore a ChainSlotPlugin from it") {
            auto [modulationCallback, config, configurator, loadPlugin, onError] =
                createDefaultTestData();

            auto slot = XmlReader::restoreChainSlotPlugin(
                &e, modulationCallback, config, configurator, loadPlugin, onError);

            THEN("A ChainSlotPlugin isn't created") {
                CHECK(slot == nullptr);
            }
        }
    }

    GIVEN("An XmlElement that has an empty description element") {
        juce::XmlElement e("test");
        auto empty = e.createNewChildElement("empty");

        WHEN("Asked to restore a ChainSlotPlugin from it") {
            auto [modulationCallback, config, configurator, loadPlugin, onError] =
                createDefaultTestData();

            auto slot = XmlReader::restoreChainSlotPlugin(
                &e, modulationCallback, config, configurator, loadPlugin, onError);

            THEN("A ChainSlotPlugin isn't created") {
                CHECK(slot == nullptr);
            }
        }
    }

    GIVEN("An XmlElement that has a valid plugin description element") {
        juce::XmlElement e("test");
        juce::PluginDescription description;
        description.name = "testPlugin";
        auto descriptionElement = description.createXml();
        e.addChildElement(descriptionElement.release());

        WHEN("Asked to restore a ChainSlotPlugin from it but loadPlugin fails") {
            auto [modulationCallback, config, configurator, loadPlugin, onError] =
                createDefaultTestData();

            loadPlugin = [](const juce::PluginDescription& description, const HostConfiguration& config) {
                return std::make_tuple<std::unique_ptr<juce::AudioPluginInstance>, juce::String>(
                    nullptr, "Test won't load plugin"
                );
            };

            onError = [](juce::String errorMsg) {
                CHECK(errorMsg == "Failed to restore plugin: Test won't load plugin");
            };

            auto slot = XmlReader::restoreChainSlotPlugin(
                &e, modulationCallback, config, configurator, loadPlugin, onError);

            THEN("A ChainSlotPlugin isn't created") {
                CHECK(slot == nullptr);
            }
        }
    }

    GIVEN("An XmlElement that has a valid plugin description element (that secretly signals to the test plugin to fail attempts to set layout)") {
        juce::XmlElement e("test");
        juce::PluginDescription description;
        description.name = "failLayout";
        auto descriptionElement = description.createXml();
        e.addChildElement(descriptionElement.release());

        WHEN("Asked to restore a ChainSlotPlugin from it but it can't find a valid layout") {
            auto [modulationCallback, config, configurator, loadPlugin, onError] =
                createDefaultTestData();

            onError = [](juce::String errorMsg) {
                CHECK(errorMsg == "Failed to restore TestPlugin as it may be a mono only plugin being restored into a stereo instance of Syndicate or vice versa");
            };

            auto slot = XmlReader::restoreChainSlotPlugin(
                &e, modulationCallback, config, configurator, loadPlugin, onError);

            THEN("A ChainSlotPlugin isn't created") {
                CHECK(slot == nullptr);
            }
        }
    }

    GIVEN("An XmlElement that has all the correct data") {
        const bool isBypassed = GENERATE(false, true);

        juce::XmlElement e("test");

        e.setAttribute(XML_SLOT_IS_BYPASSED_STR, isBypassed);

        juce::PluginDescription description;
        description.name = "TestPlugin";
        auto descriptionElement = description.createXml();
        e.addChildElement(descriptionElement.release());

        const juce::Rectangle<int> bounds(150, 200);
        e.setAttribute(XML_PLUGIN_EDITOR_BOUNDS_STR, bounds.toString());

        const juce::Rectangle<int> displayArea(2000, 1000);
        e.setAttribute(XML_DISPLAY_AREA_STR, displayArea.toString());

        const std::string testString("testPluginData");
        juce::MemoryBlock block(testString.size(), true);
        block.copyFrom(testString.c_str(), 0, testString.size());
        e.setAttribute(XML_PLUGIN_DATA_STR, block.toBase64Encoding());

        auto configElement = e.createNewChildElement(XML_MODULATION_CONFIG_STR);
        configElement->setAttribute(XML_MODULATION_IS_ACTIVE_STR, true);
        auto parameterConfigElement1 = configElement->createNewChildElement("ParamConfig_0");
        parameterConfigElement1->setAttribute(XML_MODULATION_TARGET_PARAMETER_NAME_STR, "testParam1");
        auto parameterConfigElement2 = configElement->createNewChildElement("ParamConfig_1");
        parameterConfigElement2->setAttribute(XML_MODULATION_TARGET_PARAMETER_NAME_STR, "testParam2");

        WHEN("Asked to restore a ChainSlotPlugin from it") {
            auto [modulationCallback, config, configurator, loadPlugin, onError] =
                createDefaultTestData();

            auto slot = XmlReader::restoreChainSlotPlugin(
                &e, modulationCallback, config, configurator, loadPlugin, onError);

            THEN("A ChainSlotPlugin is created with the correct fields") {
                REQUIRE(slot != nullptr);
                CHECK(slot->isBypassed == isBypassed);
                CHECK(slot->editorBounds->value().editorBounds == bounds);
                CHECK(slot->editorBounds->value().displayArea == displayArea);

                auto plugin = dynamic_cast<XMLTestPluginInstance*>(slot->plugin.get());
                REQUIRE(plugin != nullptr);
                CHECK(plugin->retrievedData == testString);

                CHECK(slot->plugin->getSampleRate() == 60000);
                CHECK(slot->plugin->getBlockSize() == 100);

                CHECK(slot->modulationConfig->isActive);
                CHECK(slot->modulationConfig->parameterConfigs.size() == 2);
                CHECK(slot->modulationConfig->parameterConfigs[0]->targetParameterName == "testParam1");
                CHECK(slot->modulationConfig->parameterConfigs[1]->targetParameterName == "testParam2");
            }
        }
    }
}

SCENARIO("XmlReader: Can restore PluginModulationConfig") {
    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        WHEN("Asked to restore a PluginModulationConfig from it") {
            auto config = XmlReader::restorePluginModulationConfig(&e);

            THEN("A PluginModulationConfig with default values is created") {
                CHECK_FALSE(config->isActive);
                CHECK(config->parameterConfigs.size() == 0);
            }
        }
    }

    GIVEN("An XmlElement that has attributes set correctly") {
        const bool isActive = GENERATE(false, true);

        juce::XmlElement e("test");
        e.setAttribute(XML_MODULATION_IS_ACTIVE_STR, isActive);

        auto config1 = e.createNewChildElement("ParamConfig_0");
        config1->setAttribute(XML_MODULATION_TARGET_PARAMETER_NAME_STR, "testParam1");

        auto config2 = e.createNewChildElement("ParamConfig_1");
        config2->setAttribute(XML_MODULATION_TARGET_PARAMETER_NAME_STR, "testParam2");

        WHEN("Asked to restore a PluginModulationConfig from it") {
            auto config = XmlReader::restorePluginModulationConfig(&e);

            THEN("A PluginModulationConfig with the correct values is created") {
                CHECK(config->isActive == isActive);
                CHECK(config->parameterConfigs.size() == 2);
                CHECK(config->parameterConfigs[0]->targetParameterName == "testParam1");
                CHECK(config->parameterConfigs[1]->targetParameterName == "testParam2");
            }
        }
    }
}

SCENARIO("XmlReader: Can restore PluginParameterModulationConfig") {
    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        WHEN("Asked to restore a PluginParameterModulationConfig from it") {
            auto config = XmlReader::restorePluginParameterModulationConfig(&e);

            THEN("A PluginParameterModulationConfig with default values is created") {
                CHECK(config->targetParameterName == "");
                CHECK(config->restValue == 0);
                CHECK(config->sources.size() == 0);
            }
        }
    }

    GIVEN("An XmlElement that has attributes set correctly") {
        juce::XmlElement e("test");
        e.setAttribute(XML_MODULATION_TARGET_PARAMETER_NAME_STR, "testName");
        e.setAttribute(XML_MODULATION_REST_VALUE_STR, 0.5);

        auto source1 = e.createNewChildElement("Source_0");
        source1->setAttribute(XML_MODULATION_SOURCE_AMOUNT, -0.5);

        auto source2 = e.createNewChildElement("Source_1");
        source2->setAttribute(XML_MODULATION_SOURCE_AMOUNT, 0.5);

        WHEN("Asked to restore a PluginParameterModulationConfig from it") {
            auto config = XmlReader::restorePluginParameterModulationConfig(&e);

            THEN("A PluginParameterModulationConfig with the correct values is created") {
                CHECK(config->targetParameterName == "testName");
                CHECK(config->restValue == 0.5);
                CHECK(config->sources.size() == 2);
                CHECK(config->sources[0]->modulationAmount == -0.5f);
                CHECK(config->sources[1]->modulationAmount == 0.5f);
            }
        }
    }
}

SCENARIO("XmlReader: Can restore PluginParameterModulationSource") {
    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        WHEN("Asked to restore a PluginParameterModulationSource from it") {
            auto source = XmlReader::restorePluginParameterModulationSource(&e);

            THEN("A PluginParameterModulationSource with default values is created") {
                CHECK(source->definition == ModulationSourceDefinition(0, MODULATION_TYPE::MACRO));
                CHECK(source->modulationAmount == 0.0f);
            }
        }
    }

    GIVEN("An XmlElement that has attributes set correctly") {
        const float amount = GENERATE(-1, 0.5, 1);

        juce::XmlElement e("test");
        e.setAttribute(XML_MODULATION_SOURCE_ID, 1);
        e.setAttribute(XML_MODULATION_SOURCE_TYPE, "lfo");
        e.setAttribute(XML_MODULATION_SOURCE_AMOUNT, amount);

        WHEN("Asked to restore a PluginParameterModulationSource from it") {
            auto source = XmlReader::restorePluginParameterModulationSource(&e);

            THEN("A PluginParameterModulationSource with the correct values is created") {
                CHECK(source->definition == ModulationSourceDefinition(1, MODULATION_TYPE::LFO));
                CHECK(source->modulationAmount == amount);
            }
        }
    }
}

SCENARIO("XmlReader: Can restore ModulationSourceDefinition") {
    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        WHEN("Asked to restore a ModulationSourceDefinition from it") {
            ModulationSourceDefinition definition(0, MODULATION_TYPE::MACRO);
            definition.restoreFromXml(&e);

            THEN("Nothing is changed") {
                CHECK(definition.id == 0);
                CHECK(definition.type == MODULATION_TYPE::MACRO);
            }
        }
    }

    GIVEN("An XmlElement that has attributes set correctly") {
        juce::XmlElement e("test");

        const int modulationId = GENERATE(1, 2, 3);

        auto [modulationType, modulationTypeString] = GENERATE(
            std::pair<MODULATION_TYPE, juce::String>(MODULATION_TYPE::MACRO, "macro"),
            std::pair<MODULATION_TYPE, juce::String>(MODULATION_TYPE::LFO, "lfo"),
            std::pair<MODULATION_TYPE, juce::String>(MODULATION_TYPE::ENVELOPE, "envelope"),
            std::pair<MODULATION_TYPE, juce::String>(MODULATION_TYPE::RANDOM, "random")
        );

        e.setAttribute(XML_MODULATION_SOURCE_ID, modulationId);
        e.setAttribute(XML_MODULATION_SOURCE_TYPE, modulationTypeString);

        WHEN("Asked to restore a PluginParameterModulationSource from it") {
            ModulationSourceDefinition definition(0, MODULATION_TYPE::MACRO);
            definition.restoreFromXml(&e);

            THEN("A PluginParameterModulationSource with the correct values is created") {
                CHECK(definition.id == modulationId);
                CHECK(definition.type == modulationType);
            }
        }
    }
}


SCENARIO("XmlReader: Can restore ModulationSourcesState") {
    GIVEN("An XmlElement that has no attributes") {
        juce::XmlElement e("test");

        HostConfiguration config;
        config.sampleRate = 44100;
        config.blockSize = 64;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };


        WHEN("Asked to restore a ModulationSourcesState from it") {
            ModelInterface::ModulationSourcesState state(modulationCallback);
            XmlReader::restoreModulationSourcesFromXml(state, &e, config);

            THEN("Nothing is restored") {
                CHECK(state.lfos.size() == 0);
                CHECK(state.envelopes.size() == 0);
                CHECK(state.randomSources.size() == 0);
            }
        }
    }

    GIVEN("An XmlElement with an LFO and an envelope follower") {
        juce::XmlElement e("test");

        HostConfiguration config;
        config.sampleRate = 44100;
        config.blockSize = 64;

        auto modulationCallback = [](int, MODULATION_TYPE) {
            return 0.0f;
        };

        // LFOs
        {
            auto lfosElement = e.createNewChildElement(XML_LFOS_STR);
            auto thisLfoElement = lfosElement->createNewChildElement("LFO_0");
            thisLfoElement->setAttribute(XML_LFO_BYPASS_STR, true);
            thisLfoElement->setAttribute(XML_LFO_PHASE_SYNC_STR, true);
            thisLfoElement->setAttribute(XML_LFO_TEMPO_SYNC_STR, true);
            thisLfoElement->setAttribute(XML_LFO_INVERT_STR, true);
            thisLfoElement->setAttribute(XML_LFO_WAVE_STR, 2);
            thisLfoElement->setAttribute(XML_LFO_TEMPO_NUMER_STR, 3);
            thisLfoElement->setAttribute(XML_LFO_TEMPO_DENOM_STR, 4);
            thisLfoElement->setAttribute(XML_LFO_FREQ_STR, 0.5);
            thisLfoElement->setAttribute(XML_LFO_DEPTH_STR, 0.6);
            thisLfoElement->setAttribute(XML_LFO_MANUAL_PHASE_STR, 100);
            thisLfoElement->setAttribute(XML_LFO_OUTPUT_MODE_STR, WECore::Richter::Parameters::OUTPUTMODE.UNIPOLAR);

            juce::XmlElement* freqModSourcesElement = thisLfoElement->createNewChildElement(XML_LFO_FREQ_MODULATION_SOURCES_STR);
            juce::XmlElement* thisFreqSourceElement = freqModSourcesElement->createNewChildElement("Source_0");
            thisFreqSourceElement->setAttribute(XML_MODULATION_SOURCE_ID, 2);
            thisFreqSourceElement->setAttribute(XML_MODULATION_SOURCE_TYPE, "lfo");
            thisFreqSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, 0.3);

            juce::XmlElement* depthModSourcesElement = thisLfoElement->createNewChildElement(XML_LFO_DEPTH_MODULATION_SOURCES_STR);
            juce::XmlElement* thisDepthSourceElement = depthModSourcesElement->createNewChildElement("Source_0");
            thisDepthSourceElement->setAttribute(XML_MODULATION_SOURCE_ID, 3);
            thisDepthSourceElement->setAttribute(XML_MODULATION_SOURCE_TYPE, "envelope");
            thisDepthSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, 0.4);

            juce::XmlElement* phaseModsourcesElement = thisLfoElement->createNewChildElement(XML_LFO_PHASE_MODULATION_SOURCES_STR);
            juce::XmlElement* thisPhaseSourceElement = phaseModsourcesElement->createNewChildElement("Source_0");
            thisPhaseSourceElement->setAttribute(XML_MODULATION_SOURCE_ID, 4);
            thisPhaseSourceElement->setAttribute(XML_MODULATION_SOURCE_TYPE, "macro");
            thisPhaseSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, 0.5);
        }

        // Envelopes
        {
            auto envelopesElement = e.createNewChildElement(XML_ENVELOPES_STR);
            auto thisEnvelopeElement = envelopesElement->createNewChildElement("Envelope_0");
            thisEnvelopeElement->setAttribute(XML_ENV_ATTACK_TIME_STR, 0.1);
            thisEnvelopeElement->setAttribute(XML_ENV_RELEASE_TIME_STR, 0.2);
            thisEnvelopeElement->setAttribute(XML_ENV_FILTER_ENABLED_STR, true);
            thisEnvelopeElement->setAttribute(XML_ENV_LOW_CUT_STR, 100);
            thisEnvelopeElement->setAttribute(XML_ENV_HIGH_CUT_STR, 200);
            thisEnvelopeElement->setAttribute(XML_ENV_AMOUNT_STR, 0.3);
            thisEnvelopeElement->setAttribute(XML_ENV_USE_SIDECHAIN_INPUT_STR, true);
        }

        // Randoms
        {
            auto randomsElement = e.createNewChildElement(XML_RANDOMS_STR);
            auto thisRandomElement = randomsElement->createNewChildElement("Random_0");
            thisRandomElement->setAttribute(XML_RANDOM_OUTPUT_MODE_STR, WECore::Perlin::Parameters::OUTPUTMODE.UNIPOLAR);
            thisRandomElement->setAttribute(XML_RANDOM_FREQ_STR, 0.1);
            thisRandomElement->setAttribute(XML_RANDOM_DEPTH_STR, 0.2);

            juce::XmlElement* freqModSourcesElement = thisRandomElement->createNewChildElement(XML_RANDOM_FREQ_MODULATION_SOURCES_STR);
            juce::XmlElement* thisFreqSourceElement = freqModSourcesElement->createNewChildElement("Source_0");
            thisFreqSourceElement->setAttribute(XML_MODULATION_SOURCE_ID, 5);
            thisFreqSourceElement->setAttribute(XML_MODULATION_SOURCE_TYPE, "envelope");
            thisFreqSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, 0.6);

            juce::XmlElement* depthModSourcesElement = thisRandomElement->createNewChildElement(XML_RANDOM_DEPTH_MODULATION_SOURCES_STR);
            juce::XmlElement* thisDepthSourceElement = depthModSourcesElement->createNewChildElement("Source_0");
            thisDepthSourceElement->setAttribute(XML_MODULATION_SOURCE_ID, 6);
            thisDepthSourceElement->setAttribute(XML_MODULATION_SOURCE_TYPE, "macro");
            thisDepthSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, 0.7);
        }

        WHEN("Asked to restore a ModulationSourcesState from it") {
            auto state = std::make_shared<ModelInterface::ModulationSourcesState>(modulationCallback);
            XmlReader::restoreModulationSourcesFromXml(*state.get(), &e, config);

            THEN("The LFO and envelope are restored") {
                // LFOs
                {
                    REQUIRE(state->lfos.size() == 1);
                    auto lfo = state->lfos[0];
                    CHECK(lfo->getBypassSwitch() == true);
                    CHECK(lfo->getPhaseSyncSwitch() == true);
                    CHECK(lfo->getTempoSyncSwitch() == true);
                    CHECK(lfo->getInvertSwitch() == true);
                    CHECK(lfo->getWave() == 2);
                    CHECK(lfo->getTempoNumer() == 3);
                    CHECK(lfo->getTempoDenom() == 4);
                    CHECK(lfo->getFreq() == 0.5);
                    CHECK(lfo->getDepth() == 0.6);
                    CHECK(lfo->getManualPhase() == 100);
                    CHECK(lfo->getOutputMode() == WECore::Richter::Parameters::OUTPUTMODE.UNIPOLAR);

                    std::vector<std::shared_ptr<PluginParameterModulationSource>> freqModSources =  ModulationMutators::getLFOFreqModulationSources(state, 0);
                    REQUIRE(freqModSources.size() == 1);
                    CHECK(freqModSources[0]->definition == ModulationSourceDefinition(2, MODULATION_TYPE::LFO));
                    CHECK(freqModSources[0]->modulationAmount == 0.3f);

                    std::vector<std::shared_ptr<PluginParameterModulationSource>> depthModSources =  ModulationMutators::getLFODepthModulationSources(state, 0);
                    REQUIRE(depthModSources.size() == 1);
                    CHECK(depthModSources[0]->definition == ModulationSourceDefinition(3, MODULATION_TYPE::ENVELOPE));
                    CHECK(depthModSources[0]->modulationAmount == 0.4f);

                    std::vector<std::shared_ptr<PluginParameterModulationSource>> phaseModSources =  ModulationMutators::getLFOPhaseModulationSources(state, 0);
                    REQUIRE(phaseModSources.size() == 1);
                    CHECK(phaseModSources[0]->definition == ModulationSourceDefinition(4, MODULATION_TYPE::MACRO));
                    CHECK(phaseModSources[0]->modulationAmount == 0.5f);
                }

                // Envelopes
                {
                    REQUIRE(state->envelopes.size() == 1);
                    auto envelope = state->envelopes[0];
                    CHECK(envelope->envelope->getAttackTimeMs() == 0.1);
                    CHECK(envelope->envelope->getReleaseTimeMs() == 0.2);
                    CHECK(envelope->envelope->getFilterEnabled() == true);
                    CHECK(envelope->envelope->getLowCutHz() == 100);
                    CHECK(envelope->envelope->getHighCutHz() == 200);
                    CHECK(envelope->amount == 0.3f);
                    CHECK(envelope->useSidechainInput == true);
                }

                // Randoms
                {
                    REQUIRE(state->randomSources.size() == 1);
                    auto random = state->randomSources[0];
                    CHECK(random->getOutputMode() == WECore::Perlin::Parameters::OUTPUTMODE.UNIPOLAR);
                    CHECK(random->getFreq() == 0.1);
                    CHECK(random->getDepth() == 0.2);

                    std::vector<std::shared_ptr<PluginParameterModulationSource>> freqModSources =  ModulationMutators::getRandomFreqModulationSources(state, 0);
                    REQUIRE(freqModSources.size() == 1);
                    CHECK(freqModSources[0]->definition == ModulationSourceDefinition(5, MODULATION_TYPE::ENVELOPE));
                    CHECK(freqModSources[0]->modulationAmount == 0.6f);

                    std::vector<std::shared_ptr<PluginParameterModulationSource>> depthModSources =  ModulationMutators::getRandomDepthModulationSources(state, 0);
                    REQUIRE(depthModSources.size() == 1);
                    CHECK(depthModSources[0]->definition == ModulationSourceDefinition(6, MODULATION_TYPE::MACRO));
                    CHECK(depthModSources[0]->modulationAmount == 0.7f);
                }
            }
        }
    }
}
