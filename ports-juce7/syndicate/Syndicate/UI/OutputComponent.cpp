#include "ParameterData.h"

#include "OutputComponent.h"
#include "PluginConfigurator.hpp"

OutputMeter::OutputMeter(const SyndicateAudioProcessor& processor) :
            _processor(processor) {
    setFramesPerSecond(20);
}

void OutputMeter::paint(juce::Graphics& g) {
    g.fillAll(UIUtils::backgroundColour);

    constexpr float MIN_DB {-60};
    constexpr float MAX_DB {20};
    const float meterRange = MAX_DB - MIN_DB;

    auto dBToHeight = [&](float dBValue) {
        return std::max(((std::min(dBValue, MAX_DB) - MIN_DB) / meterRange) * getHeight(), 0.0f);
    };

    const int zeroLineHeight = static_cast<int>(dBToHeight(0));

    // Draw the meter for each channel
    const int numChannels {canDoStereoSplitTypes(_processor.getBusesLayout()) ? 2 : 1};
    constexpr int MARGIN {4};
    const int availableWidth {getWidth() - ((1 + numChannels) * MARGIN)};
    const int channelWidth {availableWidth / numChannels};
    juce::Rectangle<int> availableArea = getLocalBounds();


    for (int channel {0}; channel < numChannels; channel++) {
        const float gaindB = WECore::CoreMath::linearTodB(_processor.meterEnvelopes[channel].getLastOutput());
        const int meterHeight = static_cast<int>(dBToHeight(gaindB));

        availableArea.removeFromLeft(MARGIN);
        juce::Rectangle<int> meterArea = availableArea.removeFromLeft(channelWidth).withTrimmedTop(availableArea.getHeight() - meterHeight);

        // Draw the zero line
        g.setColour(juce::Colours::grey);
        g.drawLine(meterArea.getX(), getHeight() - zeroLineHeight, meterArea.getX() + meterArea.getWidth(), getHeight() - zeroLineHeight);

        // Draw the lower part of the meter
        g.setColour(UIUtils::highlightColour.withBrightness(0.5));
        g.fillRect(meterArea.removeFromBottom(std::min(meterArea.getHeight(), zeroLineHeight)));

        // Draw the area above 0dB
        if (meterHeight > zeroLineHeight) {
            g.setColour(UIUtils::highlightColour.withLightness(0.5).withBrightness(0.5));
            g.fillRect(meterArea);
        }
    }
}

OutputComponent::OutputComponent(SyndicateAudioProcessor& processor) : _processor(processor) {
    panSlider.reset(new WECore::JUCEPlugin::LabelReadoutSlider<double>("Balance Slider"));
    addAndMakeVisible(panSlider.get());
    panSlider->setRange(OUTPUTPAN.minValue, OUTPUTPAN.maxValue, 0.01);
    panSlider->setDoubleClickReturnValue(true, OUTPUTPAN.defaultValue);
    panSlider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    panSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    panSlider->addListener(this);
    panSlider->setLookAndFeel(&_panSliderLookAndFeel);
    panSlider->setColour(juce::Slider::rotarySliderFillColourId, UIUtils::highlightColour);
    panSlider->setColour(juce::Slider::rotarySliderOutlineColourId, UIUtils::deactivatedColour);
    panSlider->setTooltip("Balance applied to the output (if in stereo)");

    panLabel.reset(new juce::Label("Balance Label", TRANS("Balance")));
    addAndMakeVisible(panLabel.get());
    UIUtils::setDefaultLabelStyle(panLabel);

    outputMeter.reset(new OutputMeter(_processor));
    addAndMakeVisible(outputMeter.get());
    outputMeter->setTooltip("Output level");

    outputGainSlider.reset(new WECore::JUCEPlugin::LabelReadoutSlider<double>("Output Gain Slider"));
    addAndMakeVisible(outputGainSlider.get());
    outputGainSlider->setRange(OUTPUTGAIN.minValue, OUTPUTGAIN.maxValue, 0.1);
    outputGainSlider->setDoubleClickReturnValue(true, OUTPUTGAIN.defaultValue);
    outputGainSlider->setSliderStyle(juce::Slider::LinearVertical);
    outputGainSlider->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    outputGainSlider->addListener(this);
    outputGainSlider->setLookAndFeel(&_gainSliderLookAndFeel);
    outputGainSlider->setColour(juce::Slider::thumbColourId, UIUtils::highlightColour);
    outputGainSlider->setColour(juce::Slider::trackColourId, UIUtils::highlightColour);
    outputGainSlider->setTooltip("Gain applied to the output in dB");

    outputGainLabel.reset(new juce::Label("Output Gain Label", TRANS("Output")));
    addAndMakeVisible(outputGainLabel.get());
    UIUtils::setDefaultLabelStyle(outputGainLabel);

    panSlider->setEnabled(canDoStereoSplitTypes(_processor.getBusesLayout()));

    panSlider->start(panLabel.get(), panLabel->getText());
    outputGainSlider->setValueToString([](double value) { return juce::String(value, 1) + " dB";});
    outputGainSlider->start(outputGainLabel.get(), outputGainLabel->getText());
}

OutputComponent::~OutputComponent() {
    panSlider->setLookAndFeel(nullptr);
    outputGainSlider->setLookAndFeel(nullptr);

    panSlider->stop();
    outputGainSlider->stop();

    panSlider = nullptr;
    panLabel = nullptr;
    outputGainSlider = nullptr;
    outputGainLabel = nullptr;
}

void OutputComponent::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();
    availableArea.removeFromTop(10);
    availableArea.removeFromLeft(8);
    availableArea.removeFromRight(8);

    panSlider->setBounds(availableArea.removeFromTop(availableArea.getWidth()));
    panLabel->setBounds(availableArea.removeFromTop(24));
    availableArea.removeFromTop(20);

    availableArea.removeFromBottom(8);
    outputGainLabel->setBounds(availableArea.removeFromBottom(24));

    outputMeter->setBounds(availableArea.removeFromLeft(availableArea.getWidth() / 2));
    outputGainSlider->setBounds(availableArea);
}

void OutputComponent::sliderValueChanged(juce::Slider* sliderThatWasMoved) {
    if (sliderThatWasMoved == outputGainSlider.get()) {
        _processor.setParameterValueInternal(_processor.outputGainLog, outputGainSlider->getValue());
    } else if (sliderThatWasMoved == panSlider.get()) {
        _processor.setParameterValueInternal(_processor.outputPan, panSlider->getValue());
    }
}

void OutputComponent::sliderDragStarted(juce::Slider* slider) {
    if (slider == outputGainSlider.get()) {
        _processor.outputGainLog->beginChangeGesture();
    } else if (slider == panSlider.get()) {
        _processor.outputPan->beginChangeGesture();
    }
}

void OutputComponent::sliderDragEnded(juce::Slider* slider) {
    if (slider == outputGainSlider.get()) {
        _processor.outputGainLog->endChangeGesture();
    } else if (slider == panSlider.get()) {
        _processor.outputPan->endChangeGesture();
    }
}

void OutputComponent::onParameterUpdate() {
    outputGainSlider->setValue(_processor.outputGainLog->get(), juce::dontSendNotification);
    panSlider->setValue(_processor.outputPan->get(), juce::dontSendNotification);
}
