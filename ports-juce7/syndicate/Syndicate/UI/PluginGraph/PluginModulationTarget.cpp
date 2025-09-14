#include "PluginModulationTarget.h"
#include "UIUtils.h"

void PluginModulationTargetButton::mouseDown(const juce::MouseEvent& event) {
    // If this is a right click, we need to remove the target on mouseUp
    if (event.mods.isRightButtonDown()) {
        _isRightClick = true;
    } else {
        _isRightClick = false;
    }

    juce::Button::mouseDown(event);
}

void PluginModulationTargetButton::mouseUp(const juce::MouseEvent& event) {
    // If it's a right click make sure the mouse is still over the button
    if (_isRightClick && isDown() && isOver()) {
        // Don't send an event, just call the callback
        _onRemoveCallback();
    } else {
        juce::Button::mouseUp(event);
    }
}

PluginModulationTarget::PluginModulationTarget(PluginModulationInterface& pluginModulationInterface,
                                               int chainNumber,
                                               int pluginNumber,
                                               int targetNumber) :
        _pluginModulationInterface(pluginModulationInterface),
        _chainNumber(chainNumber),
        _pluginNumber(pluginNumber),
        _targetNumber(targetNumber) {

    juce::AudioProcessorParameter* param = _pluginModulationInterface.getPluginParameterForTarget(chainNumber, pluginNumber, targetNumber);
    _targetSlider.reset(new ModulationTargetSlider("Plugin parameter slider", [param]() { return param == nullptr ? 0.0f : param->getValue(); }));
    addAndMakeVisible(_targetSlider.get());
    _targetSlider->setRange(0, 1, 0);
    _targetSlider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    _targetSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    _targetSlider->setLookAndFeel(&_sliderLookAndFeel);
    _targetSlider->setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlightColour);
    _targetSlider->setTooltip("Control for the selected plugin parameter, drag a source here to modulate");
    _targetSlider->onValueChange = [&]() {
        _pluginModulationInterface.setModulationTargetValue(_chainNumber, _pluginNumber, _targetNumber, _targetSlider->getValue());
    };

    _targetSelectButton.reset(new PluginModulationTargetButton([&]() { _pluginModulationInterface.removeModulationTarget(_chainNumber, _pluginNumber, _targetNumber); }));
    addAndMakeVisible(_targetSelectButton.get());
    _targetSelectButton->addListener(this);
    _targetSelectButton->setLookAndFeel(&_buttonLookAndFeel);
    _targetSelectButton->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    _targetSelectButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _targetSelectButton->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);

    _targetAddButton.reset(new juce::TextButton("Param"));
    addAndMakeVisible(_targetAddButton.get());
    _targetAddButton->addListener(this);
    _targetAddButton->setLookAndFeel(&_addButtonLookAndFeel);
    _targetAddButton->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    _targetAddButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _targetAddButton->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    _targetAddButton->setTooltip("Add a plugin parameter to modulate");

    const PluginModulationConfig& modulationConfig =
        _pluginModulationInterface.getPluginModulationConfig(_chainNumber, _pluginNumber);

    std::vector<std::shared_ptr<PluginParameterModulationSource>> sources;
    if (modulationConfig.parameterConfigs.size() > _targetNumber) {
        sources = modulationConfig.parameterConfigs[_targetNumber]->sources;
    }

    _sourceSliders.reset(new ModulationTargetSourceSliders(
        sources,
        [&](int slotIndex, float value) { _pluginModulationInterface.setModulationSourceValue(_chainNumber, _pluginNumber, _targetNumber, slotIndex, value); },
        [&](ModulationSourceDefinition definition) { _removeTargetSlot(definition); }));
    addAndMakeVisible(_sourceSliders.get());

    _reloadState();
}

PluginModulationTarget::~PluginModulationTarget() {
    _targetSlider->setLookAndFeel(nullptr);
    _targetSelectButton->setLookAndFeel(nullptr);
    _targetAddButton->setLookAndFeel(nullptr);

    _targetSlider = nullptr;
    _targetSelectButton = nullptr;
    _targetAddButton = nullptr;
}

void PluginModulationTarget::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();

    _targetSlider->setBounds(availableArea.removeFromTop(availableArea.getHeight() / 2));
    _targetSelectButton->setBounds(availableArea.removeFromTop(availableArea.getHeight() / 2));
    _targetAddButton->setBounds(getLocalBounds().withTrimmedBottom(availableArea.getHeight()));
    _sourceSliders->setBounds(availableArea);
}

void PluginModulationTarget::buttonClicked(juce::Button* buttonThatWasClicked) {
    if (buttonThatWasClicked == _targetSelectButton.get()) {
        if (juce::ModifierKeys::currentModifiers.isRightButtonDown()) {
            // Remove this target
            _pluginModulationInterface.removeModulationTarget(_chainNumber, _pluginNumber, _targetNumber);
        } else {
            // Select a target
            _pluginModulationInterface.selectModulationTarget(_chainNumber, _pluginNumber, _targetNumber);
        }
    } else if (buttonThatWasClicked == _targetAddButton.get()) {
        // Select a target
        _pluginModulationInterface.selectModulationTarget(_chainNumber, _pluginNumber, _targetNumber);
    }
}

bool PluginModulationTarget::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    const std::optional<ModulationSourceDefinition> draggedDefinition =
        ModulationSourceDefinition::fromVariant(dragSourceDetails.description);

    // Check dragged item contains a valid definition
    if (!draggedDefinition.has_value()) {
        return false;
    }

    // Check if we already have this source assigned to this target
    PluginModulationConfig modulationConfig =
                    _pluginModulationInterface.getPluginModulationConfig(_chainNumber, _pluginNumber);

    if (modulationConfig.parameterConfigs.size() > _targetNumber) {
        std::vector<std::shared_ptr<PluginParameterModulationSource>> sources = modulationConfig.parameterConfigs[_targetNumber]->sources;

        for (const auto source : sources) {
            if (source->definition == draggedDefinition.value()) {
                return false;
            }
        }
    } else {
        // This is an empty target
        return false;
    }

    return true;
}

void PluginModulationTarget::itemDragEnter(const SourceDetails& dragSourceDetails) {
    // Make the modulation slot visible
    const std::optional<ModulationSourceDefinition> draggedDefinition =
        ModulationSourceDefinition::fromVariant(dragSourceDetails.description);

    if (draggedDefinition.has_value()) {
        // Add a slot to the UI (but not on the backend)
        _sourceSliders->addSource(draggedDefinition.value());
    }
}

void PluginModulationTarget::itemDragExit(const SourceDetails& /*dragSourceDetails*/) {
    // Remove the modulation slot from the UI
    _sourceSliders->removeLastSource();
}

void PluginModulationTarget::itemDropped(const SourceDetails& dragSourceDetails) {
    const std::optional<ModulationSourceDefinition> draggedDefinition =
        ModulationSourceDefinition::fromVariant(dragSourceDetails.description);

    // Add another source to the target
    if (draggedDefinition.has_value()) {
        _pluginModulationInterface.addSourceToTarget(_chainNumber, _pluginNumber, _targetNumber, draggedDefinition.value());
    }
}

void PluginModulationTarget::_removeTargetSlot(ModulationSourceDefinition definition) {
    _pluginModulationInterface.removeSourceFromTarget(_chainNumber, _pluginNumber, _targetNumber, definition);
}

void PluginModulationTarget::_reloadState() {
    // Try to restore UI state from the modulation config stored in the processor
    const PluginModulationConfig& modulationConfig =
        _pluginModulationInterface.getPluginModulationConfig(_chainNumber, _pluginNumber);

    if (modulationConfig.parameterConfigs.size() > _targetNumber) {
        _targetAddButton->setEnabled(false);
        _targetAddButton->setVisible(false);

        const std::shared_ptr<PluginParameterModulationConfig> thisParameterConfig = modulationConfig.parameterConfigs[_targetNumber];

        // Restoring slider position
        _targetSlider->setEnabled(true);
        _targetSlider->setVisible(true);
        _targetSlider->setValue(thisParameterConfig->restValue,
                                juce::NotificationType::dontSendNotification);

        // Restoring button text
        _targetSelectButton->setButtonText(thisParameterConfig->targetParameterName);
        _targetSelectButton->setTooltip(thisParameterConfig->targetParameterName + " - Click to select a different plugin parameter, right click to remove");
    } else {
        _targetSlider->setEnabled(false);
        _targetSlider->setVisible(false);
        _targetSelectButton->setEnabled(false);
        _targetSelectButton->setVisible(false);
        _sourceSliders->setVisible(false);
    }
}
