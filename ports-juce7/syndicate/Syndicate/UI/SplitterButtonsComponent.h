#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UIUtils.h"

class SplitterButtonsComponent  : public juce::Component, public juce::Button::Listener {
public:
    SplitterButtonsComponent(SyndicateAudioProcessor& processor);
    ~SplitterButtonsComponent() override;

    void resized() override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;

    void onParameterUpdate();

private:
    SyndicateAudioProcessor& _processor;
    std::unique_ptr<UIUtils::ToggleButtonLookAndFeel> _buttonLookAndFeel;

    std::unique_ptr<juce::TextButton> seriesBtn;
    std::unique_ptr<juce::TextButton> parallelBtn;
    std::unique_ptr<juce::TextButton> multibandBtn;
    std::unique_ptr<juce::TextButton> leftrightBtn;
    std::unique_ptr<juce::TextButton> midsideBtn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SplitterButtonsComponent)
};
