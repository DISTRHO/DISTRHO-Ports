#pragma once

#include <JuceHeader.h>
#include "BaseSlotComponent.h"
#include "PluginSelectionInterface.h"
#include "UIUtils.h"
#include "General/CoreMath.h"
#include "CoreJUCEPlugin/LabelReadoutSlider.h"

/**
 * Displays the output amplitude of this gain stage.
 */
class GainStageMeter : public UIUtils::SafeAnimatedComponent {
public:
    GainStageMeter(const PluginSelectionInterface& pluginSelectionInterface, int chainNumber, int slotNumber);
    ~GainStageMeter() = default;

    void paint(juce::Graphics& g) override;

private:
    const PluginSelectionInterface& _pluginSelectionInterface;
    const int _chainNumber;
    const int _slotNumber;
};

/**
 * Represents a gain stage in a chain.
 */
class GainStageSlotComponent : public BaseSlotComponent,
                               public juce::Slider::Listener,
                               public juce::Button::Listener {
public:
    GainStageSlotComponent(PluginSelectionInterface& pluginSelectionInterface,
                           int chainNumber,
                           int slotNumber);
    ~GainStageSlotComponent() override;

    void resized() override;
    void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    UIUtils::StandardSliderLookAndFeel _gainSliderLookAndFeel;
    UIUtils::MidAnchoredSliderLookAndFeel _panSliderLookAndFeel;

    PluginSelectionInterface& _pluginSelectionInterface;

    std::unique_ptr<UIUtils::TextOnlyButtonLookAndFeel> _buttonLookAndFeel;
    std::unique_ptr<GainStageMeter> levelMeter;
    std::unique_ptr<juce::Label> valueLabel;
    std::unique_ptr<WECore::JUCEPlugin::LabelReadoutSlider<double>> gainSld;
    std::unique_ptr<WECore::JUCEPlugin::LabelReadoutSlider<double>> panSld;
    std::unique_ptr<UIUtils::CrossButton> removeBtn;
};
