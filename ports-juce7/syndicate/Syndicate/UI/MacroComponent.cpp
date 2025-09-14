#include "ParameterData.h"

#include "MacroComponent.h"

MacroComponent::MacroComponent(int macroNumber,
                               juce::DragAndDropContainer* dragContainer,
                               juce::AudioParameterFloat* macroParam,
                               juce::String& macroName)
    : _dragContainer(dragContainer),
      _modulationSourceDefinition(macroNumber, MODULATION_TYPE::MACRO),
      _macroParam(macroParam),
      _macroName(macroName) {

    const juce::String tooltipString("Macro " + juce::String(macroNumber) + " - Drag handle to a plugin modulation tray to assign");
    setTooltip(tooltipString);

    macroSld.reset(new juce::Slider("Macro Slider"));
    addAndMakeVisible(macroSld.get());
    macroSld->setRange(0, 1, 0);
    macroSld->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    macroSld->setTextBoxStyle(juce::Slider::NoTextBox, false, 80, 20);
    macroSld->addListener(this);
    macroSld->setTooltip(tooltipString);

    nameLbl.reset(new juce::Label("Name Label", TRANS("Macro")));
    addAndMakeVisible(nameLbl.get());
    nameLbl->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    nameLbl->setJustificationType(juce::Justification::centred);
    nameLbl->setEditable(true, true, false);
    nameLbl->setColour(juce::Label::textColourId, UIUtils::neutralColour);
    nameLbl->setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
    nameLbl->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    nameLbl->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));
    nameLbl->addListener(this);
    nameLbl->setTooltip(tooltipString);

    dragHandle.reset(new UIUtils::DragHandle());
    addAndMakeVisible(dragHandle.get());
    dragHandle->setColour(UIUtils::DragHandle::handleColourId, UIUtils::neutralColour);
    dragHandle->setTooltip(tooltipString);

    const juce::Colour& baseColour = UIUtils::getColourForModulationType(MODULATION_TYPE::MACRO);

    macroSld->setDoubleClickReturnValue(true, MACRO.defaultValue);
    macroSld->setLookAndFeel(&_sliderLookAndFeel);
    macroSld->setColour(juce::Slider::rotarySliderFillColourId, baseColour);

    nameLbl->setText(_macroName, juce::dontSendNotification);
    dragHandle->addMouseListener(this, false);
}

MacroComponent::~MacroComponent() {
    macroSld->setLookAndFeel(nullptr);

    macroSld = nullptr;
    nameLbl = nullptr;
    dragHandle = nullptr;
}

void MacroComponent::resized() {
    constexpr int Y_SPACING {4};

    juce::Rectangle<int> availableArea = getLocalBounds();
    macroSld->setBounds(availableArea.removeFromTop(getWidth()));
    availableArea.removeFromTop(Y_SPACING);
    nameLbl->setBounds(availableArea.removeFromTop(24));
    availableArea.removeFromTop(Y_SPACING);
    dragHandle->setBounds(availableArea.removeFromTop(24));
}

void MacroComponent::sliderValueChanged(juce::Slider* sliderThatWasMoved) {
    if (sliderThatWasMoved == macroSld.get()) {
        // TODO call ourProcessor->setParameterValueInternal here instead
        _macroParam->setValueNotifyingHost(macroSld->getValue());
    }
}

void MacroComponent::sliderDragStarted(juce::Slider* slider) {
    if (slider == macroSld.get()) {
        _macroParam->beginChangeGesture();
    }
}

void MacroComponent::sliderDragEnded(juce::Slider* slider) {
    if (slider == macroSld.get()) {
        _macroParam->endChangeGesture();
    }
}

void MacroComponent::labelTextChanged(juce::Label* labelThatHasChanged) {
    if (labelThatHasChanged == nameLbl.get()) {
        _macroName = nameLbl->getText();
    }
}

void MacroComponent::mouseDrag(const juce::MouseEvent& e) {
    _dragContainer->startDragging(_modulationSourceDefinition.toVariant(), dragHandle.get());
}

void MacroComponent::onParameterUpdate() {
    macroSld->setValue(_macroParam->get(), juce::dontSendNotification);
}

void MacroComponent::updateName(juce::String name) {
    nameLbl->setText(name, juce::dontSendNotification);
}
