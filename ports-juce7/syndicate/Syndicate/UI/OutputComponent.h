#pragma once

#include <JuceHeader.h>
#include "CoreJUCEPlugin/LabelReadoutSlider.h"
#include "PluginProcessor.h"
#include "UIUtils.h"

/**
 * Displays the output amplitude of this gain stage.
 */
class OutputMeter : public juce::AnimatedAppComponent,
                    public juce::SettableTooltipClient {
public:
    OutputMeter(const SyndicateAudioProcessor& processor);
    ~OutputMeter() = default;

    void update() override {
        // Do nothing
    }

    void paint(juce::Graphics& g) override;

private:
    const SyndicateAudioProcessor& _processor;
};

class OutputComponent : public juce::Component, public juce::Slider::Listener {
public:
    OutputComponent(SyndicateAudioProcessor& processor);
    ~OutputComponent() override;

    void onParameterUpdate();

    void resized() override;
    void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;
    void sliderDragStarted(juce::Slider* slider) override;
    void sliderDragEnded(juce::Slider* slider) override;

private:
    SyndicateAudioProcessor& _processor;
    UIUtils::StandardSliderLookAndFeel _gainSliderLookAndFeel;
    UIUtils::MidAnchoredSliderLookAndFeel _panSliderLookAndFeel;

    std::unique_ptr<WECore::JUCEPlugin::LabelReadoutSlider<double>> panSlider;
    std::unique_ptr<juce::Label> panLabel;
    std::unique_ptr<OutputMeter> outputMeter;
    std::unique_ptr<WECore::JUCEPlugin::LabelReadoutSlider<double>> outputGainSlider;
    std::unique_ptr<juce::Label> outputGainLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputComponent)
};
