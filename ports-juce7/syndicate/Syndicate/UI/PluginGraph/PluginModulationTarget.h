#pragma once

#include <JuceHeader.h>
#include "PluginModulationInterface.h"
#include "ModulationSourceDefinition.hpp"
#include "UIUtils.h"
#include "ModulationTargetSlider.hpp"
#include "ModulationTargetSourceSlider.hpp"

class PluginModulationTargetButton : public juce::TextButton {
public:
    PluginModulationTargetButton(std::function<void()> onRemoveCallback) : _onRemoveCallback(onRemoveCallback), _isRightClick(false) {}

    void mouseDown(const juce::MouseEvent& event) override;

    void mouseUp(const juce::MouseEvent& event) override;

private:
    std::function<void()> _onRemoveCallback;
    bool _isRightClick;
};

/**
 * Contains all the UI components needed for a particular modulation target, eg. the target slider,
 * target select button, and modulation slots.
 */
class PluginModulationTarget : public juce::Component,
                               public juce::Button::Listener,
                               public juce::DragAndDropTarget {
public:
    PluginModulationTarget(PluginModulationInterface& pluginModulationInterface,
                           int chainNumber,
                           int pluginNumber,
                           int targetNumber);

    ~PluginModulationTarget();

    void resized() override;

    void buttonClicked(juce::Button* buttonThatWasClicked) override;

    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

private:
    PluginModulationInterface& _pluginModulationInterface;
    int _chainNumber;
    int _pluginNumber;
    int _targetNumber;
    std::unique_ptr<ModulationTargetSlider> _targetSlider;
    std::unique_ptr<PluginModulationTargetButton> _targetSelectButton;
    std::unique_ptr<juce::Button> _targetAddButton;
    std::unique_ptr<ModulationTargetSourceSliders> _sourceSliders;
    UIUtils::StandardSliderLookAndFeel _sliderLookAndFeel;
    UIUtils::StaticButtonLookAndFeel _buttonLookAndFeel;
    UIUtils::AddButtonLookAndFeel _addButtonLookAndFeel;

    void _removeTargetSlot(ModulationSourceDefinition definition);
    void _reloadState();
};
