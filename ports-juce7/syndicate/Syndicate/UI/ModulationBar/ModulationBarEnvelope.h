#pragma once

#include <JuceHeader.h>
#include "CoreJUCEPlugin/LabelReadoutSlider.h"
#include "UIUtils.h"
#include "DataModelInterface.hpp"
#include "PluginProcessor.h"

class ModulationBarEnvelope  : public juce::Component,
                               public juce::Slider::Listener,
                               public juce::Button::Listener {
public:
    ModulationBarEnvelope(SyndicateAudioProcessor& processor, int envIndex);
    ~ModulationBarEnvelope() override;

    void resized() override;
    void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    class FilterSliderLookAndFeel : public WECore::JUCEPlugin::CoreLookAndFeel {
    public:
        void drawLinearSliderThumb(juce::Graphics& g,
                                   int x, int y, int width, int height,
                                   float sliderPos,
                                   float minSliderPos,
                                   float maxSliderPos,
                                   const juce::Slider::SliderStyle,
                                   juce::Slider& slider) override;

        int getSliderThumbRadius(juce::Slider& slider) override;
    };

    class FilterSlider : public WECore::JUCEPlugin::LabelReadoutSlider<double> {
    public:
        FilterSlider();

    private:
        virtual void _updateLabel(const juce::MouseEvent& event) override;
    };

    SyndicateAudioProcessor& _processor;
    int _envIndex;
    UIUtils::StandardSliderLookAndFeel _sliderLookAndFeel;
    UIUtils::MidAnchoredSliderLookAndFeel _midAnchorSliderLookAndFeel;
    UIUtils::ToggleButtonLookAndFeel _buttonLookAndFeel;
    FilterSliderLookAndFeel _filterSliderLookAndFeel;

    std::unique_ptr<WECore::JUCEPlugin::LabelReadoutSlider<double>> attackSlider;
    std::unique_ptr<WECore::JUCEPlugin::LabelReadoutSlider<double>> releaseSlider;
    std::unique_ptr<WECore::JUCEPlugin::LabelReadoutSlider<double>> amountSlider;
    std::unique_ptr<juce::Label> attackLabel;
    std::unique_ptr<juce::Label> releaseLabel;
    std::unique_ptr<juce::Label> amountLabel;
    std::unique_ptr<juce::TextButton> filterButton;
    std::unique_ptr<FilterSlider> filterSlider;
    std::unique_ptr<juce::TextButton> scInButton;
    std::unique_ptr<UIUtils::WaveStylusViewer> _envView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationBarEnvelope)
};
