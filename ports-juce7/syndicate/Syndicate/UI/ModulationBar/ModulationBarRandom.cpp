#include "UIUtils.h"

#include "ModulationBarRandom.hpp"

namespace {
    const juce::Colour& baseColour = UIUtils::getColourForModulationType(MODULATION_TYPE::RANDOM);
}

ModulationBarRandom::ModulationBarRandom(SyndicateAudioProcessor& processor, int randomIndex) :
        _processor(processor), _randomIndex(randomIndex) {

    namespace PP = WECore::Perlin::Parameters;
    constexpr double INTERVAL {0.01};

    freqSlider.reset(new ModulatableParameter(
        ModelInterface::getRandomFreqModulationSources(_processor.manager, _randomIndex),
        [&](ModulationSourceDefinition definition) {
            _processor.addSourceToRandomFreq(_randomIndex, definition);
        },
        [&](ModulationSourceDefinition definition) {
            _processor.removeSourceFromRandomFreq(_randomIndex, definition);
        },
        [&](int sourceIndex, float value) {
            _processor.setRandomFreqModulationAmount(_randomIndex, sourceIndex, value);
        },
        [&]() {
            return ModelInterface::getRandomModulatedFreqValue(_processor.manager, _randomIndex);
        },
        "Random Freq Slider",
        "Random Freq Label",
        "Rate"));
    addAndMakeVisible(freqSlider.get());
    freqSlider->slider->setTooltip(TRANS("Frequency of the random source in Hz"));
    freqSlider->slider->setRange(PP::FREQ.minValue, PP::FREQ.maxValue, INTERVAL);
    freqSlider->slider->setDoubleClickReturnValue(true, PP::FREQ.defaultValue);
    freqSlider->slider->setLookAndFeel(&_sliderLookAndFeel);
    freqSlider->slider->setColour(juce::Slider::rotarySliderFillColourId, baseColour);
    freqSlider->slider->onValueChange = [&]() {
        _processor.setRandomFreq(_randomIndex, freqSlider->slider->getValue());
    };

    depthSlider.reset(new ModulatableParameter(
        ModelInterface::getRandomDepthModulationSources(_processor.manager, _randomIndex),
        [&](ModulationSourceDefinition definition) {
            _processor.addSourceToRandomDepth(_randomIndex, definition);
        },
        [&](ModulationSourceDefinition definition) {
            _processor.removeSourceFromRandomDepth(_randomIndex, definition);
        },
        [&](int sourceIndex, float value) {
            _processor.setRandomDepthModulationAmount(_randomIndex, sourceIndex, value);
        },
        [&]() {
            return ModelInterface::getRandomModulatedDepthValue(_processor.manager, _randomIndex);
        },
        "Random Depth Slider",
        "Random Depth Label",
        "Depth"));
    addAndMakeVisible(depthSlider.get());
    depthSlider->slider->setTooltip(TRANS("Depth of the random source"));
    depthSlider->slider->setRange(PP::DEPTH.minValue, PP::DEPTH.maxValue, INTERVAL);
    depthSlider->slider->setDoubleClickReturnValue(true, PP::DEPTH.defaultValue);
    depthSlider->slider->setLookAndFeel(&_sliderLookAndFeel);
    depthSlider->slider->setColour(juce::Slider::rotarySliderFillColourId, baseColour);
    depthSlider->slider->onValueChange = [&]() {
        _processor.setRandomDepth(_randomIndex, depthSlider->slider->getValue());
    };

    _sourceView.reset(new UIUtils::WaveStylusViewer([&processor, randomIndex]() {
        return ModelInterface::getRandomLastOutput(processor.manager, randomIndex);
    }));
    addAndMakeVisible(_sourceView.get());
    _sourceView->setTooltip(TRANS("Output of this random source"));
    _sourceView->setColour(UIUtils::WaveStylusViewer::lineColourId, UIUtils::getColourForModulationType(MODULATION_TYPE::RANDOM));

    outputModeButtons.reset(new UIUtils::UniBiModeButtons(
        [&processor, randomIndex]() { processor.setRandomOutputMode(randomIndex, WECore::Perlin::Parameters::OUTPUTMODE.UNIPOLAR); },
        [&processor, randomIndex]() { processor.setRandomOutputMode(randomIndex, WECore::Perlin::Parameters::OUTPUTMODE.BIPOLAR); },
        [&processor, randomIndex]() { return ModelInterface::getRandomOutputMode(processor.manager, randomIndex) == 1 ? true : false; },
        [&processor, randomIndex]() { return ModelInterface::getRandomOutputMode(processor.manager, randomIndex) == 2 ? true : false; },
        baseColour));
    addAndMakeVisible(outputModeButtons.get());

    // Load UI state
    freqSlider->slider->setValue(ModelInterface::getRandomFreq(_processor.manager, _randomIndex), juce::dontSendNotification);
    depthSlider->slider->setValue(ModelInterface::getRandomDepth(_processor.manager, _randomIndex), juce::dontSendNotification);
}

ModulationBarRandom::~ModulationBarRandom() {
    freqSlider->slider->setLookAndFeel(nullptr);
    depthSlider->slider->setLookAndFeel(nullptr);

    freqSlider = nullptr;
    depthSlider = nullptr;
}

void ModulationBarRandom::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();
    availableArea.reduce(4, 4);

    const int minParameterWidth {availableArea.getWidth() / 4};

    // Sliders
    freqSlider->setBounds(availableArea.removeFromLeft(minParameterWidth));
    depthSlider->setBounds(availableArea.removeFromLeft(minParameterWidth));

    // Buttons
    constexpr int BUTTON_HEIGHT {24};

    juce::FlexBox waveFlexBox;
    waveFlexBox.flexDirection = juce::FlexBox::Direction::row;
    waveFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    waveFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    waveFlexBox.alignContent = juce::FlexBox::AlignContent::center;

    const int buttonWidth {static_cast<int>(availableArea.getWidth() * 0.3)};
    waveFlexBox.items.add(juce::FlexItem(*outputModeButtons.get()).withMinWidth(buttonWidth).withMinHeight(BUTTON_HEIGHT));

    waveFlexBox.performLayout(availableArea.removeFromBottom(BUTTON_HEIGHT).toFloat());

    // Wave area
    _sourceView->setBounds(availableArea);
}
