#pragma once

#include <JuceHeader.h>
#include "BaseSlotComponent.h"
#include "PluginSelectionInterface.h"
#include "UIUtils.h"

class EmptyPluginSlotComponent : public BaseSlotComponent,
                                 public juce::Button::Listener {
public:
    EmptyPluginSlotComponent(PluginSelectionInterface& pluginSelectionInterface,
                             int chainNumber,
                             int pluginNumber);
    ~EmptyPluginSlotComponent() override;

    void resized() override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    PluginSelectionInterface& _pluginSelectionInterface;
    int _chainNumber;
    int _pluginNumber;

    std::unique_ptr<UIUtils::TextOnlyButtonLookAndFeel> _buttonLookAndFeel;
    std::unique_ptr<juce::TextButton> _addPluginButton;
    std::unique_ptr<juce::TextButton> _addGainStageButton;
};
