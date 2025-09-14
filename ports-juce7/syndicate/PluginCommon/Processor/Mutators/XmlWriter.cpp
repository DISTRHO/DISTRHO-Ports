#include "XmlWriter.hpp"
#include "XmlConsts.hpp"

#include "SplitterMutators.hpp"
#include "ModulationMutators.hpp"

namespace XmlWriter {
    void write(std::shared_ptr<PluginSplitter> splitter, juce::XmlElement* element) {
        juce::Logger::writeToLog("Storing splitter state");

        const char* splitTypeString = XML_SPLIT_TYPE_SERIES_STR;
        if (auto seriesSplitter = std::dynamic_pointer_cast<PluginSplitterSeries>(splitter)) {
            splitTypeString = XML_SPLIT_TYPE_SERIES_STR;
        } else if (auto parallelSplitter = std::dynamic_pointer_cast<PluginSplitterParallel>(splitter)) {
            splitTypeString = XML_SPLIT_TYPE_PARALLEL_STR;
        } else if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
            splitTypeString = XML_SPLIT_TYPE_MULTIBAND_STR;
        } else if (auto leftRightSplitter = std::dynamic_pointer_cast<PluginSplitterLeftRight>(splitter)) {
            splitTypeString = XML_SPLIT_TYPE_LEFTRIGHT_STR;
        } else if (auto midSideSplitter = std::dynamic_pointer_cast<PluginSplitterMidSide>(splitter)) {
            splitTypeString = XML_SPLIT_TYPE_MIDSIDE_STR;
        }

        element->setAttribute(XML_SPLIT_TYPE_STR, splitTypeString);

        juce::XmlElement* chainsElement = element->createNewChildElement(XML_CHAINS_STR);

        for (int chainNumber {0}; chainNumber < splitter->chains.size(); chainNumber++) {
            juce::Logger::writeToLog("Storing chain " + juce::String(chainNumber));

            juce::XmlElement* thisChainElement = chainsElement->createNewChildElement(getChainXMLName(chainNumber));
            PluginChainWrapper& thisChain = splitter->chains[chainNumber];

            thisChainElement->setAttribute(XML_IS_CHAIN_SOLOED_STR, thisChain.isSoloed);
            XmlWriter::write(thisChain.chain, thisChainElement);
        }

        if (auto multibandSplitter = std::dynamic_pointer_cast<PluginSplitterMultiband>(splitter)) {
            // Store the crossover frequencies
            juce::XmlElement* crossoversElement = element->createNewChildElement(XML_CROSSOVERS_STR);
            for (int crossoverNumber {0}; crossoverNumber < multibandSplitter->chains.size() - 1; crossoverNumber++) {
                crossoversElement->setAttribute(getCrossoverXMLName(crossoverNumber), SplitterMutators::getCrossoverFrequency(multibandSplitter, crossoverNumber));
            }
        }
    }

    void write(std::shared_ptr<PluginChain> chain, juce::XmlElement* element) {
        // Store chain level bypass, mute, and name
        element->setAttribute(XML_IS_CHAIN_BYPASSED_STR, chain->isChainBypassed);
        element->setAttribute(XML_IS_CHAIN_MUTED_STR, chain->isChainMuted);
        element->setAttribute(XML_CHAIN_CUSTOM_NAME_STR, chain->customName);

        // Store each plugin
        juce::XmlElement* pluginsElement = element->createNewChildElement(XML_PLUGINS_STR);
        for (int pluginNumber {0}; pluginNumber < chain->chain.size(); pluginNumber++) {
            juce::Logger::writeToLog("Storing plugin " + juce::String(pluginNumber));

            juce::XmlElement* thisPluginElement = pluginsElement->createNewChildElement(getSlotXMLName(pluginNumber));

            if (auto gainStage = std::dynamic_pointer_cast<ChainSlotGainStage>(chain->chain[pluginNumber])) {
                XmlWriter::write(gainStage, thisPluginElement);
            } else if (auto pluginSlot = std::dynamic_pointer_cast<ChainSlotPlugin>(chain->chain[pluginNumber])) {
                XmlWriter::write(pluginSlot, thisPluginElement);
            }
        }
    }

    void write(std::shared_ptr<ChainSlotGainStage> gainStage, juce::XmlElement* element) {
        element->setAttribute(XML_SLOT_TYPE_STR, XML_SLOT_TYPE_GAIN_STAGE_STR);

        element->setAttribute(XML_SLOT_IS_BYPASSED_STR, gainStage->isBypassed);
        element->setAttribute(XML_GAIN_STAGE_GAIN_STR, gainStage->gain);
        element->setAttribute(XML_GAIN_STAGE_PAN_STR, gainStage->pan);
    }

    void write(std::shared_ptr<ChainSlotPlugin> chainSlot, juce::XmlElement* element) {
        element->setAttribute(XML_SLOT_TYPE_STR, XML_SLOT_TYPE_PLUGIN_STR);

        // Store the plugin level bypass
        element->setAttribute(XML_SLOT_IS_BYPASSED_STR, chainSlot->isBypassed);

        // Store the plugin description
        std::unique_ptr<juce::XmlElement> pluginDescriptionXml =
            chainSlot->plugin->getPluginDescription().createXml();
        element->addChildElement(pluginDescriptionXml.release());

        // Store the plugin's internal state
        juce::MemoryBlock pluginMemoryBlock;
        chainSlot->plugin->getStateInformation(pluginMemoryBlock);
        element->setAttribute(XML_PLUGIN_DATA_STR, pluginMemoryBlock.toBase64Encoding());

        // Store the modulation config
        juce::XmlElement* modulationConfigElement = element->createNewChildElement(XML_MODULATION_CONFIG_STR);
        write(chainSlot->modulationConfig, modulationConfigElement);

        // Store the editor bounds
        if (chainSlot->editorBounds->has_value()) {
            element->setAttribute(XML_PLUGIN_EDITOR_BOUNDS_STR, chainSlot->editorBounds->value().editorBounds.toString());
            element->setAttribute(XML_DISPLAY_AREA_STR, chainSlot->editorBounds->value().displayArea.toString());
        }
    }

    void write(std::shared_ptr<PluginModulationConfig> config, juce::XmlElement* element) {
        element->setAttribute(XML_MODULATION_IS_ACTIVE_STR, config->isActive);

        for (int index {0}; index < config->parameterConfigs.size(); index++) {
            juce::XmlElement* thisParameterConfigElement =
                element->createNewChildElement(getParameterModulationConfigXmlName(index));
            write(config->parameterConfigs[index], thisParameterConfigElement);
        }
    }

    void write(std::shared_ptr<PluginParameterModulationConfig> config, juce::XmlElement* element) {
        element->setAttribute(XML_MODULATION_TARGET_PARAMETER_NAME_STR, config->targetParameterName);
        element->setAttribute(XML_MODULATION_REST_VALUE_STR, config->restValue);

        for (int index {0}; index < config->sources.size(); index++ ) {
            juce::XmlElement* thisSourceElement =
                element->createNewChildElement(getParameterModulationSourceXmlName(index));
            write(config->sources[index], thisSourceElement);
        }
    }

    void write(std::shared_ptr<PluginParameterModulationSource> source, juce::XmlElement* element) {
        source->definition.writeToXml(element);
        element->setAttribute(XML_MODULATION_SOURCE_AMOUNT, source->modulationAmount);
    }

    void write(ModelInterface::ModulationSourcesState& state, juce::XmlElement* element) {
        // LFOs
        juce::XmlElement* lfosElement = element->createNewChildElement(XML_LFOS_STR);
        for (int index {0}; index < state.lfos.size(); index++) {
            juce::XmlElement* thisLfoElement = lfosElement->createNewChildElement(getLfoXMLName(index));
            std::shared_ptr<WECore::Richter::RichterLFO> thisLfo = state.lfos[index];

            thisLfoElement->setAttribute(XML_LFO_BYPASS_STR, thisLfo->getBypassSwitch());
            thisLfoElement->setAttribute(XML_LFO_PHASE_SYNC_STR, thisLfo->getPhaseSyncSwitch());
            thisLfoElement->setAttribute(XML_LFO_TEMPO_SYNC_STR, thisLfo->getTempoSyncSwitch());
            thisLfoElement->setAttribute(XML_LFO_INVERT_STR, thisLfo->getInvertSwitch());
            thisLfoElement->setAttribute(XML_LFO_OUTPUT_MODE_STR, thisLfo->getOutputMode());
            thisLfoElement->setAttribute(XML_LFO_WAVE_STR, thisLfo->getWave());
            thisLfoElement->setAttribute(XML_LFO_TEMPO_NUMER_STR, thisLfo->getTempoNumer());
            thisLfoElement->setAttribute(XML_LFO_TEMPO_DENOM_STR, thisLfo->getTempoDenom());
            thisLfoElement->setAttribute(XML_LFO_FREQ_STR, thisLfo->getFreq());
            thisLfoElement->setAttribute(XML_LFO_DEPTH_STR, thisLfo->getDepth());
            thisLfoElement->setAttribute(XML_LFO_MANUAL_PHASE_STR, thisLfo->getManualPhase());

            // Freq modulation sources
            juce::XmlElement* freqModSourcesElement = thisLfoElement->createNewChildElement(XML_LFO_FREQ_MODULATION_SOURCES_STR);
            const std::vector<WECore::ModulationSourceWrapper<double>> freqModSources = thisLfo->getFreqModulationSources();

            for (int sourceIndex {0}; sourceIndex < freqModSources.size(); sourceIndex++) {
                juce::XmlElement* thisSourceElement = freqModSourcesElement->createNewChildElement(getParameterModulationSourceXmlName(sourceIndex));

                auto thisSource = std::dynamic_pointer_cast<ModulationSourceProvider>(freqModSources[sourceIndex].source);
                if (thisSource != nullptr) {
                    thisSource->definition.writeToXml(thisSourceElement);
                    thisSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, freqModSources[sourceIndex].amount);
                }
            }

            // Depth modulation sources
            juce::XmlElement* depthModSourcesElement = thisLfoElement->createNewChildElement(XML_LFO_DEPTH_MODULATION_SOURCES_STR);
            const std::vector<WECore::ModulationSourceWrapper<double>> depthModSources = thisLfo->getDepthModulationSources();

            for (int sourceIndex {0}; sourceIndex < depthModSources.size(); sourceIndex++) {
                juce::XmlElement* thisSourceElement = depthModSourcesElement->createNewChildElement(getParameterModulationSourceXmlName(sourceIndex));

                auto thisSource = std::dynamic_pointer_cast<ModulationSourceProvider>(depthModSources[sourceIndex].source);
                if (thisSource != nullptr) {
                    thisSource->definition.writeToXml(thisSourceElement);
                    thisSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, depthModSources[sourceIndex].amount);
                }
            }

            // Phase modulation sources
            juce::XmlElement* phaseModSourcesElement = thisLfoElement->createNewChildElement(XML_LFO_PHASE_MODULATION_SOURCES_STR);
            const std::vector<WECore::ModulationSourceWrapper<double>> phaseModSources = thisLfo->getPhaseModulationSources();

            for (int sourceIndex {0}; sourceIndex < phaseModSources.size(); sourceIndex++) {
                juce::XmlElement* thisSourceElement = phaseModSourcesElement->createNewChildElement(getParameterModulationSourceXmlName(sourceIndex));

                auto thisSource = std::dynamic_pointer_cast<ModulationSourceProvider>(phaseModSources[sourceIndex].source);
                if (thisSource != nullptr) {
                    thisSource->definition.writeToXml(thisSourceElement);
                    thisSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, phaseModSources[sourceIndex].amount);
                }
            }
        }

        // Envelopes
        juce::XmlElement* envelopesElement = element->createNewChildElement(XML_ENVELOPES_STR);
        for (int index {0}; index < state.envelopes.size(); index++) {
            juce::XmlElement* thisEnvelopeElement = envelopesElement->createNewChildElement(getEnvelopeXMLName(index));
            std::shared_ptr<ModelInterface::EnvelopeWrapper> thisEnvelope = state.envelopes[index];

            thisEnvelopeElement->setAttribute(XML_ENV_ATTACK_TIME_STR, thisEnvelope->envelope->getAttackTimeMs());
            thisEnvelopeElement->setAttribute(XML_ENV_RELEASE_TIME_STR, thisEnvelope->envelope->getReleaseTimeMs());
            thisEnvelopeElement->setAttribute(XML_ENV_FILTER_ENABLED_STR, thisEnvelope->envelope->getFilterEnabled());
            thisEnvelopeElement->setAttribute(XML_ENV_LOW_CUT_STR, thisEnvelope->envelope->getLowCutHz());
            thisEnvelopeElement->setAttribute(XML_ENV_HIGH_CUT_STR, thisEnvelope->envelope->getHighCutHz());
            thisEnvelopeElement->setAttribute(XML_ENV_AMOUNT_STR, thisEnvelope->amount);
            thisEnvelopeElement->setAttribute(XML_ENV_USE_SIDECHAIN_INPUT_STR, thisEnvelope->useSidechainInput);
        }

        // Random
        juce::XmlElement* randomsElement = element->createNewChildElement(XML_RANDOMS_STR);
        for (int index {0}; index < state.randomSources.size(); index++) {
            juce::XmlElement* thisRandomElement = randomsElement->createNewChildElement(getRandomXMLName(index));
            std::shared_ptr<WECore::Perlin::PerlinSource> thisRandom = state.randomSources[index];

            thisRandomElement->setAttribute(XML_RANDOM_OUTPUT_MODE_STR, thisRandom->getOutputMode());
            thisRandomElement->setAttribute(XML_RANDOM_FREQ_STR, thisRandom->getFreq());
            thisRandomElement->setAttribute(XML_RANDOM_DEPTH_STR, thisRandom->getDepth());

            // Freq modulation sources
            juce::XmlElement* freqModSourcesElement = thisRandomElement->createNewChildElement(XML_RANDOM_FREQ_MODULATION_SOURCES_STR);
            const std::vector<WECore::ModulationSourceWrapper<double>> freqModSources = thisRandom->getFreqModulationSources();

            for (int sourceIndex {0}; sourceIndex < freqModSources.size(); sourceIndex++) {
                juce::XmlElement* thisSourceElement = freqModSourcesElement->createNewChildElement(getParameterModulationSourceXmlName(sourceIndex));

                auto thisSource = std::dynamic_pointer_cast<ModulationSourceProvider>(freqModSources[sourceIndex].source);
                if (thisSource != nullptr) {
                    thisSource->definition.writeToXml(thisSourceElement);
                    thisSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, freqModSources[sourceIndex].amount);
                }
            }

            // Depth modulation sources
            juce::XmlElement* depthModSourcesElement = thisRandomElement->createNewChildElement(XML_RANDOM_DEPTH_MODULATION_SOURCES_STR);
            const std::vector<WECore::ModulationSourceWrapper<double>> depthModSources = thisRandom->getDepthModulationSources();

            for (int sourceIndex {0}; sourceIndex < depthModSources.size(); sourceIndex++) {
                juce::XmlElement* thisSourceElement = depthModSourcesElement->createNewChildElement(getParameterModulationSourceXmlName(sourceIndex));

                auto thisSource = std::dynamic_pointer_cast<ModulationSourceProvider>(depthModSources[sourceIndex].source);
                if (thisSource != nullptr) {
                    thisSource->definition.writeToXml(thisSourceElement);
                    thisSourceElement->setAttribute(XML_MODULATION_SOURCE_AMOUNT, depthModSources[sourceIndex].amount);
                }
            }
        }
    }
}
