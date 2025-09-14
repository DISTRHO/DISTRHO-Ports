#pragma once

#include <JuceHeader.h>
#include "ChainSlots.hpp"
#include "PluginChain.hpp"
#include "PluginSplitter.hpp"
#include "DataModelInterface.hpp"

// TODO lock on entry so UI can't make changes

namespace XmlReader {
    std::shared_ptr<PluginSplitter> restoreSplitterFromXml(
        juce::XmlElement* element,
        std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
        std::function<void(int)> latencyChangeCallback,
        HostConfiguration configuration,
        const PluginConfigurator& pluginConfigurator,
        juce::Array<juce::PluginDescription> availableTypes,
        std::function<void(juce::String)> onErrorCallback);

    std::unique_ptr<PluginChain> restoreChainFromXml(
        juce::XmlElement* element,
        HostConfiguration configuration,
        const PluginConfigurator& pluginConfigurator,
        std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
        juce::Array<juce::PluginDescription> availableTypes,
        std::function<void(juce::String)> onErrorCallback);

    bool XmlElementIsPlugin(juce::XmlElement* element);
    bool XmlElementIsGainStage(juce::XmlElement* element);

    std::unique_ptr<ChainSlotGainStage> restoreChainSlotGainStage(
        juce::XmlElement* element, const juce::AudioProcessor::BusesLayout& busesLayout);

    typedef std::function<
        std::tuple<std::unique_ptr<juce::AudioPluginInstance>, juce::String>(
            const juce::PluginDescription&, const HostConfiguration&)> LoadPluginFunction;

    std::unique_ptr<ChainSlotPlugin> restoreChainSlotPlugin(
            juce::XmlElement* element,
            std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
            HostConfiguration configuration,
            const PluginConfigurator& pluginConfigurator,
            LoadPluginFunction loadPlugin,
            std::function<void(juce::String)> onErrorCallback);
    std::unique_ptr<PluginModulationConfig> restorePluginModulationConfig(juce::XmlElement* element);
    std::unique_ptr<PluginParameterModulationConfig> restorePluginParameterModulationConfig(juce::XmlElement* element);
    std::unique_ptr<PluginParameterModulationSource> restorePluginParameterModulationSource(juce::XmlElement* element);

    void restoreModulationSourcesFromXml(
        ModelInterface::ModulationSourcesState& state,
        juce::XmlElement* element,
        HostConfiguration configuration);
}
