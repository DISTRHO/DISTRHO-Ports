#include "ModulatableParameter.hpp"
#include "UIUtils.h"

ModulatableParameter::ModulatableParameter(
        std::vector<std::shared_ptr<PluginParameterModulationSource>> sources,
        std::function<void(ModulationSourceDefinition)> onSourceSlotAdded,
        std::function<void(ModulationSourceDefinition)> onSourceSlotRemoved,
        std::function<void(int, float)> onSliderMovedCallback,
        std::function<float()> getModulatedValue,
        juce::String sliderName,
        juce::String labelName,
        juce::String labelText) : _sources(sources), _onSourceSlotAdded(onSourceSlotAdded) {
    slider.reset(new ModulationTargetSlider(sliderName, getModulatedValue));
    addAndMakeVisible(slider.get());
    slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);

    label.reset(new juce::Label(labelName, TRANS(labelText)));
    addAndMakeVisible(label.get());
    UIUtils::setDefaultLabelStyle(label);
    slider->start(label.get(), label->getText());

    sourceSliders.reset(new ModulationTargetSourceSliders(
        sources,
        onSliderMovedCallback,
        onSourceSlotRemoved
    ));
    addAndMakeVisible(sourceSliders.get());
}

ModulatableParameter::~ModulatableParameter() {
    slider->stop();
    slider = nullptr;
    label = nullptr;
}

void ModulatableParameter::resized() {
    constexpr int SLIDER_HEIGHT {39};
    constexpr int LABEL_HEIGHT {25};
    constexpr int SOURCE_SLIDERS_HEIGHT {17};

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::column;
    flexBox.flexWrap = juce::FlexBox::Wrap::noWrap;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    flexBox.alignContent = juce::FlexBox::AlignContent::center;

    flexBox.items.add(juce::FlexItem(*slider.get()).withMinHeight(SLIDER_HEIGHT).withMinWidth(getWidth()));
    flexBox.items.add(juce::FlexItem(*label.get()).withMinHeight(LABEL_HEIGHT).withMinWidth(getWidth()));
    flexBox.items.add(juce::FlexItem(*sourceSliders.get()).withMinHeight(SOURCE_SLIDERS_HEIGHT).withMinWidth(getWidth()));

    juce::Rectangle<int> availableArea = getLocalBounds();
    flexBox.performLayout(availableArea.toFloat());
}

bool ModulatableParameter::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    const std::optional<ModulationSourceDefinition> draggedDefinition =
        ModulationSourceDefinition::fromVariant(dragSourceDetails.description);

    // Check dragged item contains a valid definition
    if (!draggedDefinition.has_value()) {
        return false;
    }

    // Check if the source is already in the list
    for (const auto source : _sources) {
        if (source->definition == draggedDefinition.value()) {
            return false;
        }
    }

    return true;

}

void ModulatableParameter::itemDragEnter(const SourceDetails& dragSourceDetails) {
    // Make the modulation slot visible
    const std::optional<ModulationSourceDefinition> draggedDefinition =
        ModulationSourceDefinition::fromVariant(dragSourceDetails.description);

    if (draggedDefinition.has_value()) {
        // Add a slot to the UI (but not on the backend)
        sourceSliders->addSource(draggedDefinition.value());
    }
}

void ModulatableParameter::itemDragExit(const SourceDetails& dragSourceDetails) {
    // Remove the modulation slot from the UI
    sourceSliders->removeLastSource();
}

void ModulatableParameter::itemDropped(const SourceDetails& dragSourceDetails) {
    const std::optional<ModulationSourceDefinition> draggedDefinition =
        ModulationSourceDefinition::fromVariant(dragSourceDetails.description);

    // // Add another source to the target
    if (draggedDefinition.has_value()) {
        _onSourceSlotAdded(draggedDefinition.value());
    }
}
