#include "UIUtils.h"

#include "ModulationBarLfo.h"
namespace {
    const juce::Colour& baseColour = UIUtils::getColourForModulationType(MODULATION_TYPE::LFO);
}

ModulationBarLfo::ModulationBarLfo(SyndicateAudioProcessor& processor, int lfoIndex) :
        _processor(processor), _lfoIndex(lfoIndex) {

    namespace RP = WECore::Richter::Parameters;
    constexpr double INTERVAL {0.01};

    depthSlider.reset(new ModulatableParameter(
        ModelInterface::getLFODepthModulationSources(_processor.manager, _lfoIndex),
        [&](ModulationSourceDefinition definition) {
            _processor.addSourceToLFODepth(_lfoIndex, definition);
        },
        [&](ModulationSourceDefinition definition) {
            _processor.removeSourceFromLFODepth(_lfoIndex, definition);
        },
        [&](int sourceIndex, float value) {
            _processor.setLFODepthModulationAmount(_lfoIndex, sourceIndex, value);
        },
        [&]() {
            return ModelInterface::getLFOModulatedDepthValue(_processor.manager, _lfoIndex);
        },
        "LFO Depth Slider",
        "LFO Depth Label",
        "Depth"));
    addAndMakeVisible(depthSlider.get());
    depthSlider->slider->setTooltip(TRANS("Depth of the LFO"));
    depthSlider->slider->setRange(RP::DEPTH.minValue, RP::DEPTH.maxValue, INTERVAL);
    depthSlider->slider->setDoubleClickReturnValue(true, WECore::Richter::Parameters::DEPTH.defaultValue);
    depthSlider->slider->setLookAndFeel(&_sliderLookAndFeel);
    depthSlider->slider->setColour(juce::Slider::rotarySliderFillColourId, baseColour);
    depthSlider->slider->onValueChange = [&]() {
        _processor.setLfoDepth(_lfoIndex, depthSlider->slider->getValue());
        _updateWaveView();
    };

    freqSlider.reset(new ModulatableParameter(
        ModelInterface::getLFOFreqModulationSources(_processor.manager, _lfoIndex),
        [&](ModulationSourceDefinition definition) {
            _processor.addSourceToLFOFreq(_lfoIndex, definition);
        },
        [&](ModulationSourceDefinition definition) {
            _processor.removeSourceFromLFOFreq(_lfoIndex, definition);
        },
        [&](int sourceIndex, float value) {
            _processor.setLFOFreqModulationAmount(_lfoIndex, sourceIndex, value);
        },
        [&]() {
            return ModelInterface::getLFOModulatedFreqValue(_processor.manager, _lfoIndex);
        },
        "LFO Freq Slider",
        "LFO Freq Label",
        "Rate"));
    addAndMakeVisible(freqSlider.get());
    freqSlider->slider->setTooltip(TRANS("Frequency of the LFO in Hz"));
    freqSlider->slider->setRange(RP::FREQ.minValue, RP::FREQ.maxValue, INTERVAL);
    freqSlider->slider->setDoubleClickReturnValue(true, WECore::Richter::Parameters::FREQ.defaultValue);
    freqSlider->slider->setLookAndFeel(&_sliderLookAndFeel);
    freqSlider->slider->setColour(juce::Slider::rotarySliderFillColourId, baseColour);
    freqSlider->slider->onValueChange = [&]() {
        _processor.setLfoFreq(_lfoIndex, freqSlider->slider->getValue());
    };

    waveComboBox.reset(new juce::ComboBox("LFO Wave"));
    addAndMakeVisible(waveComboBox.get());
    waveComboBox->setTooltip(TRANS("Wave shape for this LFO"));
    waveComboBox->setEditableText(false);
    waveComboBox->setJustificationType(juce::Justification::centredLeft);
    waveComboBox->setTextWhenNothingSelected(juce::String());
    waveComboBox->setTextWhenNoChoicesAvailable(TRANS("(no choices)"));
    waveComboBox->addItem(TRANS("Sine"), 1);
    waveComboBox->addItem(TRANS("Square"), 2);
    waveComboBox->addItem(TRANS("Saw"), 3);
    waveComboBox->addItem(TRANS("SC Comp"), 4);
    waveComboBox->setLookAndFeel(&_comboBoxLookAndFeel);
    waveComboBox->setColour(juce::ComboBox::textColourId, baseColour);
    waveComboBox->setColour(juce::ComboBox::arrowColourId, baseColour);
    waveComboBox->addListener(this);

    _comboBoxLookAndFeel.setColour(juce::PopupMenu::backgroundColourId, UIUtils::slotBackgroundColour);
    _comboBoxLookAndFeel.setColour(juce::PopupMenu::textColourId, baseColour);
    _comboBoxLookAndFeel.setColour(juce::PopupMenu::highlightedBackgroundColourId, baseColour);
    _comboBoxLookAndFeel.setColour(juce::PopupMenu::highlightedTextColourId, UIUtils::slotBackgroundColour);

    tempoSyncButton.reset(new juce::TextButton("LFO Tempo Sync Button"));
    addAndMakeVisible(tempoSyncButton.get());
    tempoSyncButton->setTooltip(TRANS("Sync LFO frequency to host tempo"));
    tempoSyncButton->setButtonText(TRANS("Tempo"));
    tempoSyncButton->setLookAndFeel(&_buttonLookAndFeel);
    tempoSyncButton->setColour(UIUtils::ToggleButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    tempoSyncButton->setColour(UIUtils::ToggleButtonLookAndFeel::highlightColour, baseColour);
    tempoSyncButton->setColour(UIUtils::ToggleButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    tempoSyncButton->addListener(this);

    tempoNumerSlider.reset(new juce::Slider("LFO Tempo Numer Slider"));
    addAndMakeVisible(tempoNumerSlider.get());
    tempoNumerSlider->setTooltip(TRANS("Numerator of the tempo sync value"));
    tempoNumerSlider->setRange(RP::TEMPONUMER.minValue, RP::TEMPONUMER.maxValue, 1);
    tempoNumerSlider->setSliderStyle(juce::Slider::IncDecButtons);
    tempoNumerSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 40, 20);
    tempoNumerSlider->setColour(juce::Slider::textBoxTextColourId, baseColour);
    tempoNumerSlider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0x00000000));
    tempoNumerSlider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0x00000000));
    tempoNumerSlider->setColour(juce::TextButton::textColourOnId, baseColour);
    tempoNumerSlider->addListener(this);
    tempoNumerSlider->setIncDecButtonsMode(juce::Slider::incDecButtonsDraggable_Vertical);
    tempoNumerSlider->setLookAndFeel(&_tempoSliderLookAndFeel);

    tempoDenomSlider.reset(new juce::Slider("LFO Tempo Denom Slider"));
    addAndMakeVisible(tempoDenomSlider.get());
    tempoDenomSlider->setTooltip(TRANS("Denominator of the tempo sync value"));
    tempoDenomSlider->setRange(RP::TEMPODENOM.minValue, RP::TEMPODENOM.maxValue, 1);
    tempoDenomSlider->setSliderStyle(juce::Slider::IncDecButtons);
    tempoDenomSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 40, 20);
    tempoDenomSlider->setColour(juce::Slider::textBoxTextColourId, baseColour);
    tempoDenomSlider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0x00000000));
    tempoDenomSlider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0x00000000));
    tempoDenomSlider->setColour(juce::TextButton::textColourOnId, baseColour);
    tempoDenomSlider->addListener(this);
    tempoDenomSlider->setIncDecButtonsMode(juce::Slider::incDecButtonsDraggable_Vertical);
    tempoDenomSlider->setLookAndFeel(&_tempoSliderLookAndFeel);

    phaseSlider.reset(new ModulatableParameter(
        ModelInterface::getLFOPhaseModulationSources(_processor.manager, _lfoIndex),
        [&](ModulationSourceDefinition definition) {
            _processor.addSourceToLFOPhase(_lfoIndex, definition);
        },
        [&](ModulationSourceDefinition definition) {
            _processor.removeSourceFromLFOPhase(_lfoIndex, definition);
        },
        [&](int sourceIndex, float value) {
            _processor.setLFOPhaseModulationAmount(_lfoIndex, sourceIndex, value);
        },
        [&]() {
            return ModelInterface::getLFOModulatedPhaseValue(_processor.manager, _lfoIndex);
        },
        "LFO Phase Slider",
        "LFO Phase Label",
        "Phase"));
    addAndMakeVisible(phaseSlider.get());
    phaseSlider->slider->setTooltip(TRANS("Phase shift the LFO by up to 360 degrees"));
    phaseSlider->slider->setRange(RP::PHASE.minValue, RP::PHASE.maxValue, INTERVAL);
    phaseSlider->slider->setDoubleClickReturnValue(true, WECore::Richter::Parameters::PHASE.defaultValue);
    phaseSlider->slider->setLookAndFeel(&_sliderLookAndFeel);
    phaseSlider->slider->setColour(juce::Slider::rotarySliderFillColourId, baseColour);
    phaseSlider->slider->onValueChange = [&]() {
        _processor.setLfoManualPhase(_lfoIndex, phaseSlider->slider->getValue());
        _updateWaveView();
    };

    waveView.reset(new WECore::Richter::WaveViewer());
    addAndMakeVisible(waveView.get());
    waveView->setName("LFO Wave View");
    waveView->setTooltip(TRANS("Wave shape that will be output by this LFO"));
    waveView->setColour(WECore::Richter::WaveViewer::highlightColourId, baseColour);

    invertButton.reset(new juce::TextButton("LFO Invert Button"));
    addAndMakeVisible(invertButton.get());
    invertButton->setTooltip(TRANS("Invert the LFO output"));
    invertButton->setButtonText(TRANS("Invert"));
    invertButton->setLookAndFeel(&_buttonLookAndFeel);
    invertButton->setColour(juce::TextButton::buttonOnColourId, baseColour);
    invertButton->setColour(juce::TextButton::textColourOnId, UIUtils::backgroundColour);
    invertButton->setColour(juce::TextButton::textColourOffId, baseColour);
    invertButton->setColour(UIUtils::ToggleButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    invertButton->setColour(UIUtils::ToggleButtonLookAndFeel::highlightColour, baseColour);
    invertButton->setColour(UIUtils::ToggleButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    invertButton->addListener(this);

    outputModeButtons.reset(new UIUtils::UniBiModeButtons(
        [&processor, lfoIndex]() { processor.setLfoOutputMode(lfoIndex, WECore::Richter::Parameters::OUTPUTMODE.UNIPOLAR); },
        [&processor, lfoIndex]() { processor.setLfoOutputMode(lfoIndex, WECore::Richter::Parameters::OUTPUTMODE.BIPOLAR); },
        [&processor, lfoIndex]() { return ModelInterface::getLfoOutputMode(processor.manager, lfoIndex) == 1 ? true : false; },
        [&processor, lfoIndex]() { return ModelInterface::getLfoOutputMode(processor.manager, lfoIndex) == 2 ? true : false; },
        baseColour));
    addAndMakeVisible(outputModeButtons.get());

    // Load UI state
    tempoSyncButton->setToggleState(ModelInterface::getLfoTempoSyncSwitch(_processor.manager, _lfoIndex), juce::dontSendNotification);
    invertButton->setToggleState(ModelInterface::getLfoInvertSwitch(_processor.manager, _lfoIndex), juce::dontSendNotification);
    waveComboBox->setSelectedId(ModelInterface::getLfoWave(_processor.manager, _lfoIndex), juce::dontSendNotification);
    depthSlider->slider->setValue(ModelInterface::getLfoDepth(_processor.manager, _lfoIndex), juce::dontSendNotification);
    freqSlider->slider->setValue(ModelInterface::getLfoFreq(_processor.manager, _lfoIndex), juce::dontSendNotification);
    phaseSlider->slider->setValue(ModelInterface::getLfoManualPhase(_processor.manager, _lfoIndex), juce::dontSendNotification);
    tempoNumerSlider->setValue(ModelInterface::getLfoTempoNumer(_processor.manager, _lfoIndex), juce::dontSendNotification);
    tempoDenomSlider->setValue(ModelInterface::getLfoTempoDenom(_processor.manager, _lfoIndex), juce::dontSendNotification);

    _tempoSliderLookAndFeel.setColour(juce::TextButton::textColourOnId, baseColour);

    _updateTempoToggles();
    _updateWaveView();
}

ModulationBarLfo::~ModulationBarLfo() {
    freqSlider->slider->setLookAndFeel(nullptr);
    depthSlider->slider->setLookAndFeel(nullptr);
    phaseSlider->slider->setLookAndFeel(nullptr);
    invertButton->setLookAndFeel(nullptr);
    waveComboBox->setLookAndFeel(nullptr);
    tempoNumerSlider->setLookAndFeel(nullptr);
    tempoDenomSlider->setLookAndFeel(nullptr);

    freqSlider = nullptr;
    depthSlider = nullptr;
    waveComboBox = nullptr;
    tempoSyncButton = nullptr;
    tempoNumerSlider = nullptr;
    tempoDenomSlider = nullptr;
    phaseSlider = nullptr;
    waveView = nullptr;
    invertButton = nullptr;
}

void ModulationBarLfo::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();
    availableArea.reduce(4, 4);

    constexpr int BUTTON_HEIGHT {24};

    const int minParameterWidth {availableArea.getWidth() / 6};

    // Freq/tempo area
    juce::Rectangle<int> freqArea = availableArea.removeFromLeft(minParameterWidth);

    // Enforce the max width for this section (otherwise buttons look silly)
    constexpr int MAX_FREQ_AREA_WIDTH {100};
    const int excessWidth {std::max(freqArea.getWidth() - MAX_FREQ_AREA_WIDTH, 0)};
    freqArea = freqArea.reduced(excessWidth / 2.0, 0);

    const int freqSliderSize {freqArea.getHeight() / 2 - BUTTON_HEIGHT};
    tempoSyncButton->setBounds(freqArea.removeFromTop(BUTTON_HEIGHT));

    // Copy this now, we'll need it later
    juce::Rectangle<int> tempoSlidersArea = freqArea;

    freqSlider->setBounds(freqArea);

    // Now the tempo sliders
    juce::FlexBox tempoSlidersFlexBox;
    tempoSlidersFlexBox.flexDirection = juce::FlexBox::Direction::column;
    tempoSlidersFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    tempoSlidersFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    tempoSlidersFlexBox.alignContent = juce::FlexBox::AlignContent::center;

    tempoSlidersFlexBox.items.add(juce::FlexItem(*tempoNumerSlider.get()).withMinWidth(tempoSlidersArea.getWidth() * 0.75).withMinHeight(40));
    tempoSlidersFlexBox.items.add(juce::FlexItem(*tempoDenomSlider.get()).withMinWidth(tempoSlidersArea.getWidth() * 0.75).withMinHeight(40));

    tempoSlidersFlexBox.performLayout(tempoSlidersArea);

    depthSlider->setBounds(availableArea.removeFromLeft(minParameterWidth).withTrimmedTop(BUTTON_HEIGHT));
    phaseSlider->setBounds(availableArea.removeFromLeft(minParameterWidth).withTrimmedTop(BUTTON_HEIGHT));

    // Wave area
    constexpr int MAX_WAVE_AREA_WIDTH {450};
    const int excessWaveWidth {std::max(availableArea.getWidth() - MAX_WAVE_AREA_WIDTH, 0)};
    availableArea.removeFromLeft(excessWaveWidth / 2.0);
    availableArea.removeFromRight(excessWaveWidth / 2.0);

    juce::FlexBox waveFlexBox;
    waveFlexBox.flexDirection = juce::FlexBox::Direction::row;
    waveFlexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    waveFlexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    waveFlexBox.alignContent = juce::FlexBox::AlignContent::center;

    const int buttonWidth {static_cast<int>(availableArea.getWidth() * 0.3)};
    waveFlexBox.items.add(juce::FlexItem(*invertButton.get()).withMinWidth(buttonWidth).withMinHeight(BUTTON_HEIGHT));
    waveFlexBox.items.add(juce::FlexItem(*outputModeButtons.get()).withMinWidth(buttonWidth).withMinHeight(BUTTON_HEIGHT));
    waveFlexBox.items.add(juce::FlexItem(*waveComboBox.get()).withMinWidth(buttonWidth).withMinHeight(BUTTON_HEIGHT));

    waveFlexBox.performLayout(availableArea.removeFromBottom(BUTTON_HEIGHT).toFloat());

    waveView->setBounds(availableArea);
}

void ModulationBarLfo::sliderValueChanged(juce::Slider* sliderThatWasMoved) {
    if (sliderThatWasMoved == tempoNumerSlider.get()) {
        _processor.setLfoTempoNumer(_lfoIndex, tempoNumerSlider->getValue());
    } else if (sliderThatWasMoved == tempoDenomSlider.get()) {
        _processor.setLfoTempoDenom(_lfoIndex, tempoDenomSlider->getValue());
    }

    _updateWaveView();
}

void ModulationBarLfo::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
    if (comboBoxThatHasChanged == waveComboBox.get()) {
        _processor.setLfoWave(_lfoIndex, waveComboBox->getSelectedId());
    }

    _updateWaveView();
}

void ModulationBarLfo::buttonClicked(juce::Button* buttonThatWasClicked) {
    if (buttonThatWasClicked == tempoSyncButton.get()) {
        tempoSyncButton->setToggleState(!tempoSyncButton->getToggleState(), juce::dontSendNotification);
        _processor.setLfoTempoSyncSwitch(_lfoIndex, tempoSyncButton->getToggleState());
    } else if (buttonThatWasClicked == invertButton.get()) {
        invertButton->setToggleState(!invertButton->getToggleState(), juce::dontSendNotification);
        _processor.setLfoInvertSwitch(_lfoIndex, invertButton->getToggleState());
    }

    _updateTempoToggles();
    _updateWaveView();
}


void ModulationBarLfo::TempoSliderLookAndFeel::drawButtonBackground(
        juce::Graphics& /*g*/,
        juce::Button& /*button*/,
        const juce::Colour& /*backgroundColour*/,
        bool /*isMouseOverButton*/,
        bool /*isButtonDown*/) {
    // do nothing
}

void ModulationBarLfo::TempoSliderLookAndFeel::drawButtonText(juce::Graphics& g,
                                                              juce::TextButton& textButton,
                                                              bool /*isMouseOverButton*/,
                                                              bool /*isButtonDown*/) {
    g.setColour(findColour(juce::TextButton::textColourOnId));

    juce::Rectangle<int> area = textButton.getLocalBounds().withPosition(0, 0).reduced(8);

    constexpr int MAX_CARAT_WIDTH {10};
    const int excessWidth {std::max(area.getWidth() - MAX_CARAT_WIDTH, 0)};
    area = area.reduced(excessWidth / 2.0, 0);

    const int horizontalMid {area.getWidth() / 2 + area.getX()};
    const int centreY {area.getY() + (textButton.getButtonText() == "+" ? 0 : area.getHeight())};
    const int endY {area.getY() + (textButton.getButtonText() == "+" ? area.getHeight() : 0)};
    juce::Path p;

    p.startNewSubPath(area.getX(), endY);
    p.lineTo(horizontalMid, centreY);
    p.lineTo(area.getX() + area.getWidth(), endY);

    g.strokePath(p, juce::PathStrokeType(1));
}

juce::Slider::SliderLayout ModulationBarLfo::TempoSliderLookAndFeel::getSliderLayout(
            juce::Slider& slider) {
    juce::Rectangle<int> area = slider.getLocalBounds();

    juce::Slider::SliderLayout retVal;
    retVal.sliderBounds = area.removeFromRight(area.getWidth() / 2);
    retVal.textBoxBounds = area;

    return retVal;
}

juce::Label* ModulationBarLfo::TempoSliderLookAndFeel::createSliderTextBox(juce::Slider& slider) {
    juce::Label* label = UIUtils::StandardSliderLookAndFeel::createSliderTextBox(slider);

    // Use a bigger font
    label->setFont(juce::Font(20.00f, juce::Font::plain).withTypefaceStyle("Regular"));

    return label;
}

void ModulationBarLfo::_updateWaveView() {
    const double* wave {nullptr};

    const int waveValue {ModelInterface::getLfoWave(_processor.manager, _lfoIndex)};
    if (waveValue == 1) {
        wave = WECore::Richter::Wavetables::getInstance()->getSine();
    } else if (waveValue == 2) {
        wave = WECore::Richter::Wavetables::getInstance()->getSquare();
    } else if (waveValue == 3) {
        wave = WECore::Richter::Wavetables::getInstance()->getSaw();
    } else {
        wave = WECore::Richter::Wavetables::getInstance()->getSidechain();
    }

    waveView->setWave(wave,
                      ModelInterface::getLfoDepth(_processor.manager, _lfoIndex),
                      ModelInterface::getLfoManualPhase(_processor.manager, _lfoIndex),
                      ModelInterface::getLfoInvertSwitch(_processor.manager, _lfoIndex));
    waveView->repaint();
}

void ModulationBarLfo::_updateTempoToggles() {
    if (tempoSyncButton->getToggleState()) {
        freqSlider->setVisible(false);
        tempoNumerSlider->setVisible(true);
        tempoDenomSlider->setVisible(true);
    } else {
        freqSlider->setVisible(true);
        tempoNumerSlider->setVisible(false);
        tempoDenomSlider->setVisible(false);
    }
}
