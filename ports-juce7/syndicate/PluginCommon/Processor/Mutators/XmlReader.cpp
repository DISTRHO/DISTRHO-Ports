#include "XmlReader.hpp"
#include "XmlConsts.hpp"
#include "ChainSlotProcessors.hpp"
#include "ChainProcessors.hpp"
#include "SplitterMutators.hpp"
#include "SplitTypes.hpp"
#include "RichterLFO/RichterLFO.h"

namespace {
    int comparePluginDescriptionAgainstTarget(const juce::PluginDescription& description, const juce::PluginDescription& target) {
        // Format is most important for compatibility when loading settings
        if (description.pluginFormatName == target.pluginFormatName) {
            return 2;
        }

        if (description.version == target.version) {
            return 1;
        }

        return 0;
    }

    class AvailableTypesSorter {
    public:
        AvailableTypesSorter(const juce::PluginDescription& target) : _target(target) {}

        int compareElements(const juce::PluginDescription& a, const juce::PluginDescription& b) {
            return comparePluginDescriptionAgainstTarget(b, _target) - comparePluginDescriptionAgainstTarget(a, _target);
        }

    private:
        const juce::PluginDescription& _target;
    };

    juce::String pluginTypesToString(const juce::Array<juce::PluginDescription>& types) {
        juce::String retVal;
        for (const juce::PluginDescription& type : types) {
            retVal += type.pluginFormatName + " " + type.version + ", ";
        }

        return retVal;
    }
}

namespace XmlReader {
    std::shared_ptr<PluginSplitter> restoreSplitterFromXml(
            juce::XmlElement* element,
            std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
            std::function<void(int)> latencyChangeCallback,
            HostConfiguration configuration,
            const PluginConfigurator& pluginConfigurator,
            juce::Array<juce::PluginDescription> availableTypes,
            std::function<void(juce::String)> onErrorCallback) {

        // Default to series
        SPLIT_TYPE splitType = SPLIT_TYPE::SERIES;

        if (element->hasAttribute(XML_SPLIT_TYPE_STR)) {
            const juce::String splitTypeString = element->getStringAttribute(XML_SPLIT_TYPE_STR);
            juce::Logger::writeToLog("Restoring split type: " + splitTypeString);

            // We need to check if the split type we're attempting to restore is supported.
            // For example in Logic when switching from a stereo to mono plugin we may have saved to XML
            // using a left/right split in a 2in2out configuration but will be restoring into a 1in1out
            // configuration.
            // In that case we move to a parallel split type.
            splitType = stringToSplitType(splitTypeString);
            const bool isExpecting2in2out {splitType == SPLIT_TYPE::LEFTRIGHT || splitType == SPLIT_TYPE::MIDSIDE};

            if (isExpecting2in2out && !canDoStereoSplitTypes(configuration.layout)) {
                // Migrate to parallel
                splitType = SPLIT_TYPE::PARALLEL;
            }
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SPLIT_TYPE_STR));
        }

        std::shared_ptr<PluginSplitter> splitter;
        if (splitType == SPLIT_TYPE::SERIES) {
            splitter = std::make_shared<PluginSplitterSeries>(configuration, getModulationValueCallback, latencyChangeCallback);
        } else if (splitType == SPLIT_TYPE::PARALLEL) {
            splitter = std::make_shared<PluginSplitterParallel>(configuration, getModulationValueCallback, latencyChangeCallback);
        } else if (splitType == SPLIT_TYPE::MULTIBAND) {
            splitter = std::make_shared<PluginSplitterMultiband>(configuration, getModulationValueCallback, latencyChangeCallback);
        } else if (splitType == SPLIT_TYPE::LEFTRIGHT) {
            splitter = std::make_shared<PluginSplitterLeftRight>(configuration, getModulationValueCallback, latencyChangeCallback);
        } else if (splitType == SPLIT_TYPE::MIDSIDE) {
            splitter = std::make_shared<PluginSplitterMidSide>(configuration, getModulationValueCallback, latencyChangeCallback);
        }

        // Reset state
        while (!splitter->chains.empty()) {
            splitter->chains.erase(splitter->chains.begin());
        }

        // Restore each chain
        juce::XmlElement* chainsElement = element->getChildByName(XML_CHAINS_STR);
        const int numChains {
            chainsElement == nullptr ? 0 : chainsElement->getNumChildElements()
        };

        for (int chainNumber {0}; chainNumber < numChains; chainNumber++) {
            juce::Logger::writeToLog("Restoring chain " + juce::String(chainNumber));

            const juce::String chainElementName = getChainXMLName(chainNumber);
            juce::XmlElement* thisChainElement = chainsElement->getChildByName(chainElementName);

            if (thisChainElement == nullptr) {
                juce::Logger::writeToLog("Failed to get element " + chainElementName);
            } else {
                bool isSoloed {false};
                if (thisChainElement->hasAttribute(XML_IS_CHAIN_SOLOED_STR)) {
                    isSoloed = thisChainElement->getBoolAttribute(XML_IS_CHAIN_SOLOED_STR);
                } else {
                    juce::Logger::writeToLog("Missing attribute " + juce::String(XML_IS_CHAIN_SOLOED_STR));
                }

                // Add the chain to the vector
                splitter->chains.emplace_back(std::make_shared<PluginChain>(getModulationValueCallback), false);
                PluginChainWrapper& thisChain = splitter->chains[splitter->chains.size() - 1];
                thisChain.chain = XmlReader::restoreChainFromXml(thisChainElement, configuration, pluginConfigurator, getModulationValueCallback, availableTypes, onErrorCallback);

                if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
                    // Since we deleted all chains at the start to make sure we have a
                    // clean starting point, that can mean the first few crossover bands could still exist
                    // and be pointing at chains that have been deleted. We handle this here.
                    if (multibandSplitter->chains.size() > CrossoverMutators::getNumBands(multibandSplitter->crossover)) {
                        // Need to add a new band and chain
                        CrossoverMutators::addBand(multibandSplitter->crossover);
                    } else {
                        // We already have the bands in the crossover
                    }

                    // Now assign the chain to the band
                    CrossoverMutators::setPluginChain(multibandSplitter->crossover, multibandSplitter->chains.size() - 1, thisChain.chain);
                }

                ChainProcessors::prepareToPlay(*thisChain.chain.get(), configuration);

                thisChain.chain->latencyListener.setSplitter(splitter.get());
                SplitterMutators::setChainSolo(splitter, chainNumber, isSoloed);
            }
        }

        if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
            // Restore the crossover frequencies
            juce::XmlElement* crossoversElement = element->getChildByName(XML_CROSSOVERS_STR);
            if (crossoversElement != nullptr) {
                for (int crossoverNumber {0}; crossoverNumber < multibandSplitter->chains.size() - 1; crossoverNumber++) {
                    const juce::String frequencyAttribute(getCrossoverXMLName(crossoverNumber));
                    if (crossoversElement->hasAttribute(frequencyAttribute)) {
                        SplitterMutators::setCrossoverFrequency(multibandSplitter, crossoverNumber, crossoversElement->getDoubleAttribute(frequencyAttribute));
                    } else {
                        juce::Logger::writeToLog("Missing attribute " + juce::String(frequencyAttribute));
                    }
                }
            } else {
                juce::Logger::writeToLog("Missing element " + juce::String(XML_CROSSOVERS_STR));
            }
        }

        splitter->onLatencyChange();

        return splitter;
    }

    std::unique_ptr<PluginChain> restoreChainFromXml(
            juce::XmlElement* element,
            HostConfiguration configuration,
            const PluginConfigurator& pluginConfigurator,
            std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
            juce::Array<juce::PluginDescription> availableTypes,
            std::function<void(juce::String)> onErrorCallback) {

        auto retVal = std::make_unique<PluginChain>(getModulationValueCallback);

        // Restore chain level bypass, mute, and name
        if (element->hasAttribute(XML_IS_CHAIN_BYPASSED_STR)) {
            retVal->isChainBypassed = element->getBoolAttribute(XML_IS_CHAIN_BYPASSED_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_IS_CHAIN_BYPASSED_STR));
        }

        if (element->hasAttribute(XML_IS_CHAIN_MUTED_STR)) {
            retVal->isChainMuted = element->getBoolAttribute(XML_IS_CHAIN_MUTED_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_IS_CHAIN_MUTED_STR));
        }

        if (element->hasAttribute(XML_CHAIN_CUSTOM_NAME_STR)) {
            retVal->customName = element->getStringAttribute(XML_CHAIN_CUSTOM_NAME_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_CHAIN_CUSTOM_NAME_STR));
        }

        // Load each plugin
        juce::XmlElement* pluginsElement = element->getChildByName(XML_PLUGINS_STR);
        if (pluginsElement == nullptr) {
            juce::Logger::writeToLog("Missing child element " + juce::String(XML_PLUGINS_STR));
        }

        const int numPlugins {
            pluginsElement == nullptr ? 0 : pluginsElement->getNumChildElements()
        };

        for (int pluginNumber {0}; pluginNumber < numPlugins; pluginNumber++) {
            juce::Logger::writeToLog("Restoring slot " + juce::String(pluginNumber));

            const juce::String pluginElementName = getSlotXMLName(pluginNumber);
            juce::XmlElement* thisPluginElement = pluginsElement->getChildByName(pluginElementName);

            if (thisPluginElement == nullptr) {
                juce::Logger::writeToLog("Failed to get element " + pluginElementName);
                continue;
            }

            if (XmlReader::XmlElementIsPlugin(thisPluginElement)) {
                auto loadPlugin = [&availableTypes](const juce::PluginDescription& description, const HostConfiguration& config) {
                    juce::AudioPluginFormatManager formatManager;
                    formatManager.addDefaultFormats();

                    // First try the exact match
                    juce::String errorMessage;
                    std::unique_ptr<juce::AudioPluginInstance> thisPlugin =
                        formatManager.createPluginInstance(
                            description, config.sampleRate, config.blockSize, errorMessage);

                    // Failing that, get all possible matches from the available types
                    if (thisPlugin == nullptr) {
                        juce::Logger::writeToLog("Failed to load plugin " + description.name + ": " + errorMessage);
                        juce::Logger::writeToLog("Looking for alternatives");

                        juce::Array<juce::PluginDescription> possibleTypes;
                        for (const juce::PluginDescription& availableType : availableTypes) {
                            if (description.manufacturerName == availableType.manufacturerName &&
                                description.name == availableType.name) {
                                possibleTypes.add(availableType);
                            }
                        }

                        // Sort by best match first
                        AvailableTypesSorter sorter(description);
                        possibleTypes.sort(sorter);

                        const juce::String possibleTypesString = pluginTypesToString(possibleTypes);
                        juce::Logger::writeToLog("Found alternatives: " + possibleTypesString);

                        for (const juce::PluginDescription& possibleType : possibleTypes) {
                            juce::String newErrorMessage;
                            thisPlugin = formatManager.createPluginInstance(
                                possibleType, config.sampleRate, config.blockSize, newErrorMessage);

                            if (thisPlugin != nullptr) {
                                juce::Logger::writeToLog("Loaded " + possibleType.pluginFormatName + " " + possibleType.version);
                                break;
                            }

                            errorMessage += " - " + newErrorMessage;
                        }
                    }

                    return std::make_tuple<std::unique_ptr<juce::AudioPluginInstance>, juce::String>(
                        std::move(thisPlugin), juce::String(errorMessage));
                };

                auto newPlugin = XmlReader::restoreChainSlotPlugin(
                    thisPluginElement, getModulationValueCallback, configuration, pluginConfigurator, loadPlugin, onErrorCallback);

                if (newPlugin != nullptr) {
                    newPlugin->plugin->addListener(&retVal->latencyListener);
                    retVal->chain.push_back(std::move(newPlugin));
                }
            } else if (XmlReader::XmlElementIsGainStage(thisPluginElement)) {
                auto newGainStage = XmlReader::restoreChainSlotGainStage(thisPluginElement, configuration.layout);

                if (newGainStage != nullptr) {
                    // Call prepareToPlay since some hosts won't call it after restoring
                    ChainProcessors::prepareToPlay(*newGainStage.get(), configuration);
                    retVal->chain.push_back(std::move(newGainStage));
                }
            } else {
                juce::Logger::writeToLog("Can't determine slot type");
            }
        }

        retVal->latencyListener.onPluginChainUpdate();

        return retVal;
    }

    bool XmlElementIsPlugin(juce::XmlElement* element) {
        if (element->hasAttribute(XML_SLOT_TYPE_STR)) {
            if (element->getStringAttribute(XML_SLOT_TYPE_STR) == XML_SLOT_TYPE_PLUGIN_STR) {
                return true;
            }
        }

        return false;
    }

    bool XmlElementIsGainStage(juce::XmlElement* element) {
        if (element->hasAttribute(XML_SLOT_TYPE_STR)) {
            if (element->getStringAttribute(XML_SLOT_TYPE_STR) == XML_SLOT_TYPE_GAIN_STAGE_STR) {
                return true;
            }
        }

        return false;
    }

    std::unique_ptr<ChainSlotGainStage> restoreChainSlotGainStage(
            juce::XmlElement* element, const juce::AudioProcessor::BusesLayout& busesLayout) {

        bool isSlotBypassed {false};
        if (element->hasAttribute(XML_SLOT_IS_BYPASSED_STR)) {
            isSlotBypassed = element->getBoolAttribute(XML_SLOT_IS_BYPASSED_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SLOT_IS_BYPASSED_STR));
        }

        float gain {1.0f};
        if (element->hasAttribute(XML_GAIN_STAGE_GAIN_STR)) {
            gain = element->getDoubleAttribute(XML_GAIN_STAGE_GAIN_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_GAIN_STAGE_GAIN_STR));
        }

        float pan {0.0f};
        if (element->hasAttribute(XML_GAIN_STAGE_PAN_STR)) {
            pan = element->getDoubleAttribute(XML_GAIN_STAGE_PAN_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_GAIN_STAGE_PAN_STR));
        }

        return std::make_unique<ChainSlotGainStage>(gain, pan, isSlotBypassed, busesLayout);
    }

    std::unique_ptr<ChainSlotPlugin> restoreChainSlotPlugin(
            juce::XmlElement* element,
            std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
            HostConfiguration configuration,
            const PluginConfigurator& pluginConfigurator,
            LoadPluginFunction loadPlugin,
            std::function<void(juce::String)> onErrorCallback) {

        // Restore the plugin level bypass
        bool isPluginBypassed {false};
        if (element->hasAttribute(XML_SLOT_IS_BYPASSED_STR)) {
            isPluginBypassed = element->getBoolAttribute(XML_SLOT_IS_BYPASSED_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SLOT_IS_BYPASSED_STR));
        }

        if (element->getNumChildElements() == 0) {
            juce::Logger::writeToLog("Plugin element missing description");
            return nullptr;
        }

        // Load the actual plugin
        juce::XmlElement* pluginDescriptionXml = element->getChildElement(0);
        juce::PluginDescription pluginDescription;

        if (!pluginDescription.loadFromXml(*pluginDescriptionXml)) {
            juce::Logger::writeToLog("Failed to parse plugin description");
            return nullptr;
        }

        auto [thisPlugin, errorMessage] = loadPlugin(pluginDescription, configuration);

        if (thisPlugin == nullptr) {
            juce::Logger::writeToLog("Failed to load plugin: " + errorMessage);
            onErrorCallback("Failed to restore plugin: " + errorMessage);
            return nullptr;
        }

        std::unique_ptr<ChainSlotPlugin> retVal;

        std::shared_ptr<juce::AudioPluginInstance> sharedPlugin = std::move(thisPlugin);

        if (pluginConfigurator.configure(sharedPlugin, configuration)) {
            retVal.reset(new ChainSlotPlugin(sharedPlugin, isPluginBypassed, getModulationValueCallback, configuration));

            // Restore the editor bounds
            if (element->hasAttribute(XML_PLUGIN_EDITOR_BOUNDS_STR)) {
                const juce::String boundsString = element->getStringAttribute(XML_PLUGIN_EDITOR_BOUNDS_STR);

                if (element->hasAttribute(XML_DISPLAY_AREA_STR)) {
                    const juce::String displayString = element->getStringAttribute(XML_DISPLAY_AREA_STR);

                    retVal->editorBounds.reset(new PluginEditorBounds());
                    *(retVal->editorBounds.get()) =  PluginEditorBoundsContainer(
                        juce::Rectangle<int>::fromString(boundsString),
                        juce::Rectangle<int>::fromString(displayString)
                    );
                } else {
                    juce::Logger::writeToLog("Missing attribute " + juce::String(XML_DISPLAY_AREA_STR));
                }

            } else {
                juce::Logger::writeToLog("Missing attribute " + juce::String(XML_PLUGIN_EDITOR_BOUNDS_STR));
            }

            // Restore the plugin's internal state
            if (element->hasAttribute(XML_PLUGIN_DATA_STR)) {
                const juce::String pluginDataString = element->getStringAttribute(XML_PLUGIN_DATA_STR);
                juce::MemoryBlock pluginData;
                pluginData.fromBase64Encoding(pluginDataString);

                sharedPlugin->setStateInformation(pluginData.getData(), pluginData.getSize());

                // Now that the plugin is restored, we can restore the modulation config
                juce::XmlElement* modulationConfigElement = element->getChildByName(XML_MODULATION_CONFIG_STR);
                if (modulationConfigElement != nullptr) {
                    retVal->modulationConfig = restorePluginModulationConfig(modulationConfigElement);
                } else {
                    juce::Logger::writeToLog("Missing element " + juce::String(XML_MODULATION_CONFIG_STR));
                }
            } else {
                juce::Logger::writeToLog("Missing attribute " + juce::String(XML_PLUGIN_DATA_STR));
            }
        } else {
            juce::Logger::writeToLog("Failed to configure plugin: " + sharedPlugin->getPluginDescription().name);
            onErrorCallback("Failed to restore " + sharedPlugin->getPluginDescription().name + " as it may be a mono only plugin being restored into a stereo instance of Syndicate or vice versa");
        }

        return retVal;
    }

    std::unique_ptr<PluginModulationConfig> restorePluginModulationConfig(juce::XmlElement* element) {
        auto retVal = std::make_unique<PluginModulationConfig>();

        if (element->hasAttribute(XML_MODULATION_IS_ACTIVE_STR)) {
            retVal->isActive = element->getBoolAttribute(XML_MODULATION_IS_ACTIVE_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_IS_ACTIVE_STR));
        }

        const int numParameterConfigs {element->getNumChildElements()};
        for (int index {0}; index < numParameterConfigs; index++) {
            juce::Logger::writeToLog("Restoring parameter modulation config " + juce::String(index));

            const juce::String parameterConfigElementName = getParameterModulationConfigXmlName(index);
            juce::XmlElement* thisParameterConfigElement = element->getChildByName(parameterConfigElementName);

            retVal->parameterConfigs.push_back(
                restorePluginParameterModulationConfig(thisParameterConfigElement));
        }

        return retVal;
    }

    std::unique_ptr<PluginParameterModulationConfig> restorePluginParameterModulationConfig(juce::XmlElement* element) {
        auto retVal = std::make_unique<PluginParameterModulationConfig>();

        if (element->hasAttribute(XML_MODULATION_TARGET_PARAMETER_NAME_STR)) {
            retVal->targetParameterName = element->getStringAttribute(XML_MODULATION_TARGET_PARAMETER_NAME_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_TARGET_PARAMETER_NAME_STR));
        }

        if (element->hasAttribute(XML_MODULATION_REST_VALUE_STR)) {
            retVal->restValue = element->getDoubleAttribute(XML_MODULATION_REST_VALUE_STR);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_REST_VALUE_STR));
        }

        const int numSources {element->getNumChildElements()};
        for (int index {0}; index < numSources; index++) {
            juce::Logger::writeToLog("Restoring parameter modulation sources " + juce::String(index));

            const juce::String sourceElementName = getParameterModulationSourceXmlName(index);
            juce::XmlElement* thisSourceElement = element->getChildByName(sourceElementName);

            std::shared_ptr<PluginParameterModulationSource> thisSource = restorePluginParameterModulationSource(thisSourceElement);
            retVal->sources.push_back(thisSource);
        }

        return retVal;
    }

    std::unique_ptr<PluginParameterModulationSource> restorePluginParameterModulationSource(juce::XmlElement* element) {
        // TODO tidy definition construction
        ModulationSourceDefinition definition(0, MODULATION_TYPE::MACRO);
        definition.restoreFromXml(element);

        float modulationAmount {0};
        if (element->hasAttribute(XML_MODULATION_SOURCE_AMOUNT)) {
            modulationAmount = element->getDoubleAttribute(XML_MODULATION_SOURCE_AMOUNT);
        } else {
            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_SOURCE_AMOUNT));
        }

        return std::make_unique<PluginParameterModulationSource>(definition, modulationAmount);
    }

    void restoreModulationSourcesFromXml(ModelInterface::ModulationSourcesState& state,
                                         juce::XmlElement* element,
                                         HostConfiguration configuration) {

        // Clear existing state
        state.lfos.clear();
        state.envelopes.clear();
        state.randomSources.clear();

        // LFOs
        juce::XmlElement* lfosElement = element->getChildByName(XML_LFOS_STR);
        if (lfosElement != nullptr) {
            const int numLfos {lfosElement->getNumChildElements()};

            for (int index {0}; index < numLfos; index++) {
                juce::Logger::writeToLog("Restoring LFO " + juce::String(index));

                const juce::String lfoElementName = getLfoXMLName(index);
                juce::XmlElement* thisLfoElement = lfosElement->getChildByName(lfoElementName);

                if (thisLfoElement == nullptr) {
                    juce::Logger::writeToLog("Failed to get element " + lfoElementName);
                    continue;
                }

                std::shared_ptr<ModelInterface::CloneableLFO> newLfo {new ModelInterface::CloneableLFO()};

                newLfo->setBypassSwitch(thisLfoElement->getBoolAttribute(XML_LFO_BYPASS_STR));
                newLfo->setPhaseSyncSwitch(thisLfoElement->getBoolAttribute(XML_LFO_PHASE_SYNC_STR));
                newLfo->setTempoSyncSwitch(thisLfoElement->getBoolAttribute(XML_LFO_TEMPO_SYNC_STR));
                newLfo->setInvertSwitch(thisLfoElement->getBoolAttribute(XML_LFO_INVERT_STR));
                newLfo->setOutputMode(thisLfoElement->getIntAttribute(XML_LFO_OUTPUT_MODE_STR, WECore::Richter::Parameters::OutputModeParameter::BIPOLAR)); // This paramter was added later - default to bipolar if not found
                newLfo->setWave(thisLfoElement->getIntAttribute(XML_LFO_WAVE_STR));
                newLfo->setTempoNumer(thisLfoElement->getIntAttribute(XML_LFO_TEMPO_NUMER_STR));
                newLfo->setTempoDenom(thisLfoElement->getIntAttribute(XML_LFO_TEMPO_DENOM_STR));
                newLfo->setFreq(thisLfoElement->getDoubleAttribute(XML_LFO_FREQ_STR));
                newLfo->setDepth(thisLfoElement->getDoubleAttribute(XML_LFO_DEPTH_STR));
                newLfo->setManualPhase(thisLfoElement->getDoubleAttribute(XML_LFO_MANUAL_PHASE_STR));
                newLfo->setSampleRate(configuration.sampleRate);

                juce::XmlElement* freqModElement = thisLfoElement->getChildByName(XML_LFO_FREQ_MODULATION_SOURCES_STR);
                if (freqModElement != nullptr) {
                    const int numFreqModSources {freqModElement->getNumChildElements()};

                    for (int sourceIndex {0}; sourceIndex < numFreqModSources; sourceIndex++) {
                        juce::Logger::writeToLog("Restoring LFO freq modulation source " + juce::String(sourceIndex));

                        const juce::String sourceElementName = getParameterModulationSourceXmlName(sourceIndex);
                        juce::XmlElement* thisSourceElement = freqModElement->getChildByName(sourceElementName);

                        if (thisSourceElement == nullptr) {
                            juce::Logger::writeToLog("Failed to get element " + sourceElementName);
                            continue;
                        }

                        // TODO error handling restoring definition
                        ModulationSourceDefinition definition(0, MODULATION_TYPE::MACRO);
                        definition.restoreFromXml(thisSourceElement);

                        auto newSource = std::make_shared<ModulationSourceProvider>(definition, state.getModulationValueCallback);
                        newLfo->addFreqModulationSource(newSource);

                        if (thisSourceElement->hasAttribute(XML_MODULATION_SOURCE_AMOUNT)) {
                            newLfo->setFreqModulationAmount(sourceIndex, thisSourceElement->getDoubleAttribute(XML_MODULATION_SOURCE_AMOUNT));
                        } else {
                            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_SOURCE_AMOUNT));
                        }
                    }
                }

                juce::XmlElement* depthModElement = thisLfoElement->getChildByName(XML_LFO_DEPTH_MODULATION_SOURCES_STR);
                if (depthModElement != nullptr) {
                    const int numDepthModSources {depthModElement->getNumChildElements()};

                    for (int sourceIndex {0}; sourceIndex < numDepthModSources; sourceIndex++) {
                        juce::Logger::writeToLog("Restoring LFO depth modulation source " + juce::String(sourceIndex));

                        const juce::String sourceElementName = getParameterModulationSourceXmlName(sourceIndex);
                        juce::XmlElement* thisSourceElement = depthModElement->getChildByName(sourceElementName);

                        if (thisSourceElement == nullptr) {
                            juce::Logger::writeToLog("Failed to get element " + sourceElementName);
                            continue;
                        }

                        // TODO error handling restoring definition
                        ModulationSourceDefinition definition(0, MODULATION_TYPE::MACRO);
                        definition.restoreFromXml(thisSourceElement);

                        auto newSource = std::make_shared<ModulationSourceProvider>(definition, state.getModulationValueCallback);
                        newLfo->addDepthModulationSource(newSource);

                        if (thisSourceElement->hasAttribute(XML_MODULATION_SOURCE_AMOUNT)) {
                            newLfo->setDepthModulationAmount(sourceIndex, thisSourceElement->getDoubleAttribute(XML_MODULATION_SOURCE_AMOUNT));
                        } else {
                            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_SOURCE_AMOUNT));
                        }
                    }
                }

                juce::XmlElement* phaseModElement = thisLfoElement->getChildByName(XML_LFO_PHASE_MODULATION_SOURCES_STR);
                if (phaseModElement != nullptr) {
                    const int numPhaseModSources {phaseModElement->getNumChildElements()};

                    for (int sourceIndex {0}; sourceIndex < numPhaseModSources; sourceIndex++) {
                        juce::Logger::writeToLog("Restoring LFO phase modulation source " + juce::String(sourceIndex));

                        const juce::String sourceElementName = getParameterModulationSourceXmlName(sourceIndex);
                        juce::XmlElement* thisSourceElement = phaseModElement->getChildByName(sourceElementName);

                        if (thisSourceElement == nullptr) {
                            juce::Logger::writeToLog("Failed to get element " + sourceElementName);
                            continue;
                        }

                        // TODO error handling restoring definition
                        ModulationSourceDefinition definition(0, MODULATION_TYPE::MACRO);
                        definition.restoreFromXml(thisSourceElement);

                        auto newSource = std::make_shared<ModulationSourceProvider>(definition, state.getModulationValueCallback);
                        newLfo->addPhaseModulationSource(newSource);

                        if (thisSourceElement->hasAttribute(XML_MODULATION_SOURCE_AMOUNT)) {
                            newLfo->setPhaseModulationAmount(sourceIndex, thisSourceElement->getDoubleAttribute(XML_MODULATION_SOURCE_AMOUNT));
                        } else {
                            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_SOURCE_AMOUNT));
                        }
                    }
                }

                state.lfos.push_back(newLfo);
            }
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_LFOS_STR));
        }

        // Envelopes
        juce::XmlElement* envelopesElement = element->getChildByName(XML_ENVELOPES_STR);
        if (envelopesElement != nullptr) {
            const int numEnvelopes {envelopesElement->getNumChildElements()};

            for (int index {0}; index < numEnvelopes; index++) {
                juce::Logger::writeToLog("Restoring envelope " + juce::String(index));

                const juce::String envelopeElementName = getEnvelopeXMLName(index);
                juce::XmlElement* thisEnvelopeElement = envelopesElement->getChildByName(envelopeElementName);

                if (thisEnvelopeElement == nullptr) {
                    juce::Logger::writeToLog("Failed to get element " + envelopeElementName);
                    continue;
                }

                std::shared_ptr<ModelInterface::EnvelopeWrapper> newEnv(new ModelInterface::EnvelopeWrapper());
                newEnv->envelope->setAttackTimeMs(thisEnvelopeElement->getDoubleAttribute(XML_ENV_ATTACK_TIME_STR));
                newEnv->envelope->setReleaseTimeMs(thisEnvelopeElement->getDoubleAttribute(XML_ENV_RELEASE_TIME_STR));
                newEnv->envelope->setFilterEnabled(thisEnvelopeElement->getBoolAttribute(XML_ENV_FILTER_ENABLED_STR));
                newEnv->envelope->setLowCutHz(thisEnvelopeElement->getDoubleAttribute(XML_ENV_LOW_CUT_STR));
                newEnv->envelope->setHighCutHz(thisEnvelopeElement->getDoubleAttribute(XML_ENV_HIGH_CUT_STR));
                newEnv->envelope->setSampleRate(configuration.sampleRate);

                newEnv->amount = thisEnvelopeElement->getDoubleAttribute(XML_ENV_AMOUNT_STR);
                newEnv->useSidechainInput = thisEnvelopeElement->getBoolAttribute(XML_ENV_USE_SIDECHAIN_INPUT_STR);

                state.envelopes.push_back(newEnv);
            }
        } else {
            juce::Logger::writeToLog("Missing element " + juce::String(XML_ENVELOPES_STR));
        }

        // Random
        juce::XmlElement* randomsElement = element->getChildByName(XML_RANDOMS_STR);
        if (randomsElement != nullptr) {
            const int numRandoms {randomsElement->getNumChildElements()};

            for (int index {0}; index < numRandoms; index++) {
                juce::Logger::writeToLog("Restoring randoms " + juce::String(index));

                const juce::String randomElementName = getRandomXMLName(index);
                juce::XmlElement* thisRandomElement = randomsElement->getChildByName(randomElementName);

                if (thisRandomElement == nullptr) {
                    juce::Logger::writeToLog("Failed to get element " + randomElementName);
                    continue;
                }

                std::shared_ptr<WECore::Perlin::PerlinSource> newRandom {new WECore::Perlin::PerlinSource()};

                newRandom->setOutputMode(thisRandomElement->getIntAttribute(XML_RANDOM_OUTPUT_MODE_STR));
                newRandom->setFreq(thisRandomElement->getDoubleAttribute(XML_RANDOM_FREQ_STR));
                newRandom->setDepth(thisRandomElement->getDoubleAttribute(XML_RANDOM_DEPTH_STR));
                newRandom->setSampleRate(configuration.sampleRate);

                juce::XmlElement* freqModElement = thisRandomElement->getChildByName(XML_RANDOM_FREQ_MODULATION_SOURCES_STR);
                if (freqModElement != nullptr) {
                    const int numFreqModSources {freqModElement->getNumChildElements()};

                    for (int sourceIndex {0}; sourceIndex < numFreqModSources; sourceIndex++) {
                        juce::Logger::writeToLog("Restoring random freq modulation source " + juce::String(sourceIndex));

                        const juce::String sourceElementName = getParameterModulationSourceXmlName(sourceIndex);
                        juce::XmlElement* thisSourceElement = freqModElement->getChildByName(sourceElementName);

                        if (thisSourceElement == nullptr) {
                            juce::Logger::writeToLog("Failed to get element " + sourceElementName);
                            continue;
                        }

                        // TODO error handling restoring definition
                        ModulationSourceDefinition definition(0, MODULATION_TYPE::MACRO);
                        definition.restoreFromXml(thisSourceElement);

                        auto newSource = std::make_shared<ModulationSourceProvider>(definition, state.getModulationValueCallback);
                        newRandom->addFreqModulationSource(newSource);

                        if (thisSourceElement->hasAttribute(XML_MODULATION_SOURCE_AMOUNT)) {
                            newRandom->setFreqModulationAmount(sourceIndex, thisSourceElement->getDoubleAttribute(XML_MODULATION_SOURCE_AMOUNT));
                        } else {
                            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_SOURCE_AMOUNT));
                        }
                    }
                }

                juce::XmlElement* depthModElement = thisRandomElement->getChildByName(XML_RANDOM_DEPTH_MODULATION_SOURCES_STR);
                if (depthModElement != nullptr) {
                    const int numDepthModSources {depthModElement->getNumChildElements()};

                    for (int sourceIndex {0}; sourceIndex < numDepthModSources; sourceIndex++) {
                        juce::Logger::writeToLog("Restoring random depth modulation source " + juce::String(sourceIndex));

                        const juce::String sourceElementName = getParameterModulationSourceXmlName(sourceIndex);
                        juce::XmlElement* thisSourceElement = depthModElement->getChildByName(sourceElementName);

                        if (thisSourceElement == nullptr) {
                            juce::Logger::writeToLog("Failed to get element " + sourceElementName);
                            continue;
                        }

                        // TODO error handling restoring definition
                        ModulationSourceDefinition definition(0, MODULATION_TYPE::MACRO);
                        definition.restoreFromXml(thisSourceElement);

                        auto newSource = std::make_shared<ModulationSourceProvider>(definition, state.getModulationValueCallback);
                        newRandom->addDepthModulationSource(newSource);

                        if (thisSourceElement->hasAttribute(XML_MODULATION_SOURCE_AMOUNT)) {
                            newRandom->setDepthModulationAmount(sourceIndex, thisSourceElement->getDoubleAttribute(XML_MODULATION_SOURCE_AMOUNT));
                        } else {
                            juce::Logger::writeToLog("Missing attribute " + juce::String(XML_MODULATION_SOURCE_AMOUNT));
                        }
                    }
                }

                state.randomSources.push_back(newRandom);
            }
        }
    }
}
