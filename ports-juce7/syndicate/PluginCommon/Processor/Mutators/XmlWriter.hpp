#pragma once

#include <JuceHeader.h>
#include "ChainSlots.hpp"
#include "PluginChain.hpp"
#include "PluginSplitter.hpp"
#include "DataModelInterface.hpp"

// TODO lock on entry so UI can't make changes

namespace XmlWriter {
    void write(std::shared_ptr<PluginSplitter> splitter, juce::XmlElement* element);

    void write(std::shared_ptr<PluginChain> chain, juce::XmlElement* element);

    void write(std::shared_ptr<ChainSlotGainStage> gainStage, juce::XmlElement* element);

    void write(std::shared_ptr<ChainSlotPlugin> chainSlot, juce::XmlElement* element);
    void write(std::shared_ptr<PluginModulationConfig> config, juce::XmlElement* element);
    void write(std::shared_ptr<PluginParameterModulationConfig> config, juce::XmlElement* element);
    void write(std::shared_ptr<PluginParameterModulationSource> source, juce::XmlElement* element);

    void write(ModelInterface::ModulationSourcesState& state, juce::XmlElement* element);
}
