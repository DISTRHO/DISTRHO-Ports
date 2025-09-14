#include "EmptyPluginSlotComponent.h"

EmptyPluginSlotComponent::EmptyPluginSlotComponent(PluginSelectionInterface& pluginSelectionInterface,
                                                   int chainNumber,
                                                   int pluginNumber) :
        _pluginSelectionInterface(pluginSelectionInterface),
        _chainNumber(chainNumber),
        _pluginNumber(pluginNumber) {
            _buttonLookAndFeel.reset(new UIUtils::TextOnlyButtonLookAndFeel());

    _addPluginButton.reset(new juce::TextButton("Add Plugin Button"));
    addAndMakeVisible(_addPluginButton.get());
    _addPluginButton->setButtonText(TRANS("+ Plugin"));
    _addPluginButton->setTooltip(TRANS("Add a new plugin to the chain"));
    _addPluginButton->setLookAndFeel(_buttonLookAndFeel.get());
    _addPluginButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _addPluginButton->addListener(this);

    _addGainStageButton.reset(new juce::TextButton("Add Gain Stage Button"));
    addAndMakeVisible(_addGainStageButton.get());
    _addGainStageButton->setButtonText(TRANS("+ Gain Stage"));
    _addGainStageButton->setTooltip(TRANS("Add a new gain stage to the chain"));
    _addGainStageButton->setLookAndFeel(_buttonLookAndFeel.get());
    _addGainStageButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _addGainStageButton->addListener(this);
}

EmptyPluginSlotComponent::~EmptyPluginSlotComponent() {
    _addPluginButton->setLookAndFeel(nullptr);
    _addGainStageButton->setLookAndFeel(nullptr);

    _addPluginButton = nullptr;
    _addGainStageButton = nullptr;
}

void EmptyPluginSlotComponent::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();
    _addPluginButton->setBounds(availableArea.removeFromLeft(availableArea.getWidth() / 2));
    _addGainStageButton->setBounds(availableArea);
}


void EmptyPluginSlotComponent::buttonClicked(juce::Button* buttonThatWasClicked) {
    if (buttonThatWasClicked == _addPluginButton.get()) {
        _pluginSelectionInterface.selectNewPlugin(_chainNumber, _pluginNumber);
    } else if (buttonThatWasClicked == _addGainStageButton.get()) {
        _pluginSelectionInterface.insertGainStage(_chainNumber, _pluginNumber);
    }
}
