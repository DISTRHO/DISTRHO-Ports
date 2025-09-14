#pragma once

#include <JuceHeader.h>
#include "ModulationTargetSlider.hpp"
#include "ModulationTargetSourceSlider.hpp"
#include "PluginProcessor.h"

class ModulatableParameter : public juce::Component,
                             public juce::DragAndDropTarget {
public:
    std::unique_ptr<ModulationTargetSlider> slider;
    std::unique_ptr<juce::Label> label;
    std::unique_ptr<ModulationTargetSourceSliders> sourceSliders;

    ModulatableParameter(
        std::vector<std::shared_ptr<PluginParameterModulationSource>> sources,
        std::function<void(ModulationSourceDefinition)> onSourceSlotAdded,
        std::function<void(ModulationSourceDefinition)> onSourceSlotRemoved,
        std::function<void(int, float)> onSliderMovedCallback,
        std::function<float()> getModulatedValue,
        juce::String sliderName,
        juce::String labelName,
        juce::String labelText);

    virtual ~ModulatableParameter() override;

    void resized() override;

    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
    const std::vector<std::shared_ptr<PluginParameterModulationSource>> _sources;
    std::function<void(ModulationSourceDefinition)> _onSourceSlotAdded;
};
