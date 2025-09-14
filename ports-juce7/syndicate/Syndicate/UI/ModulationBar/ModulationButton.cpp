#include "ModulationButton.h"

#include "UIUtils.h"

namespace {
    juce::String definitionToButtonName(const ModulationSourceDefinition& definition) {
        juce::String name;

        if (definition.type == MODULATION_TYPE::LFO) {
            name = "LFO ";
        } else if (definition.type == MODULATION_TYPE::ENVELOPE) {
            name = "ENV ";
        } else if (definition.type == MODULATION_TYPE::RANDOM) {
            name = "RND ";
        }

        return name + juce::String(definition.id);
    }
}

ModulationButton::ModulationButton(ModulationSourceDefinition newDefinition,
                                   std::function<void(ModulationButton*)> onSelectCallback,
                                   std::function<void()> onRemoveCallback,
                                   juce::DragAndDropContainer* dragContainer) :
        definition(newDefinition),
        _onSelectCallback(onSelectCallback),
        _dragContainer(dragContainer) {

    _buttonLookAndFeel.reset(new ButtonLookAndFeel());

    const juce::String buttonName = definitionToButtonName(definition);
    const juce::String tooltipString(
        buttonName + " - Drag the handle to a plugin modulation tray or right click to remove");

    // Set up the select button
    selectButton.reset(new ModulationSelectButton(onRemoveCallback));
    addAndMakeVisible(selectButton.get());
    selectButton->setButtonText(TRANS(buttonName));
    selectButton->addListener(this);
    selectButton->setLookAndFeel(_buttonLookAndFeel.get());
    selectButton->setColour(juce::TextButton::buttonColourId, UIUtils::getColourForModulationType(definition.type));
    selectButton->setColour(juce::TextButton::textColourOffId, UIUtils::getColourForModulationType(definition.type));
    selectButton->setColour(juce::TextButton::textColourOnId, UIUtils::slotBackgroundColour);
    selectButton->setTooltip(tooltipString);

    // Set up the drag handle
    dragHandle.reset(new UIUtils::DragHandle());
    addAndMakeVisible(dragHandle.get());
    dragHandle->setColour(UIUtils::DragHandle::handleColourId, UIUtils::getColourForModulationType(definition.type));
    dragHandle->setTooltip(tooltipString);
    dragHandle->addMouseListener(this, false);
}

ModulationButton::~ModulationButton() {
    selectButton->setLookAndFeel(nullptr);
    dragHandle->setLookAndFeel(nullptr);

    _buttonLookAndFeel = nullptr;
}

void ModulationButton::setIsSelected(bool isSelected) {
    selectButton->setToggleState(isSelected, juce::dontSendNotification);
}

bool ModulationButton::getIsSelected() const {
    return selectButton->getToggleState();
}

void ModulationButton::resized() {
    const int labelWidth {getHeight()};
    const int selectButtonWidth {getWidth() - labelWidth};

    juce::Rectangle<int> area = getLocalBounds();

    selectButton->setBounds(area.removeFromLeft(selectButtonWidth));
    dragHandle->setBounds(area);
}

void ModulationButton::paint(juce::Graphics& g) {
    // Draw the outline
    constexpr int MARGIN {1};
    juce::Rectangle<int> area = dragHandle->getBounds()
                                .withTrimmedTop(MARGIN).withTrimmedBottom(MARGIN).withTrimmedRight(MARGIN);

    juce::Path p;
    const int centreY {area.getHeight() / 2 + area.getY()};
    const int centreX {area.getHeight() / 2 + area.getX()};
    const int radius {area.getHeight() / 2};

    p.clear();
    p.startNewSubPath(area.getX(), area.getY());
    p.addCentredArc(centreX,
                    centreY,
                    radius,
                    radius,
                    0,
                    0,
                    WECore::CoreMath::DOUBLE_PI);
    p.lineTo(area.getX(), area.getHeight() + area.getY());

    g.setColour(UIUtils::slotBackgroundColour);
    g.fillPath(p);
}

void ModulationButton::buttonClicked(juce::Button* buttonThatWasClicked) {
    if (buttonThatWasClicked == selectButton.get()) {
        _onSelectCallback(this);
    }
}

void ModulationButton::mouseDrag(const juce::MouseEvent& /*event*/) {
    _dragContainer->startDragging(definition.toVariant(), this);
}

void ModulationButton::ButtonLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                                               juce::Button& button,
                                                               const juce::Colour& /*backgroundColour*/,
                                                               bool /*shouldDrawButtonAsHighlighted*/,
                                                               bool /*shouldDrawButtonAsDown*/) {

    constexpr int MARGIN {1};
    juce::Rectangle<int> area = juce::Rectangle<int>(button.getWidth(), button.getHeight())
                                .withTrimmedTop(MARGIN).withTrimmedBottom(MARGIN).withTrimmedLeft(MARGIN);

    juce::Path p;
    const int centreY {area.getHeight() / 2 + area.getY()};
    const int centreX {area.getHeight() / 2 + area.getX()};
    const int radius {area.getHeight() / 2};

    p.startNewSubPath(area.getWidth() + area.getX(), area.getY());
    p.lineTo(centreX, area.getY());
    p.addCentredArc(centreX,
                    centreY,
                    radius,
                    radius,
                    0,
                    0,
                    -WECore::CoreMath::DOUBLE_PI);
    p.lineTo(button.getWidth(), area.getHeight() + area.getY());

    if (button.getToggleState()) {
        g.setColour(button.findColour(juce::TextButton::buttonColourId));
    } else {
        g.setColour(button.findColour(juce::TextButton::textColourOnId));
    }

    p.closeSubPath();
    g.fillPath(p);

}

void ModulationButton::ModulationSelectButton::mouseDown(const juce::MouseEvent& event) {
    // If this is a right click, we need to remove the target on mouseUp
    if (event.mods.isRightButtonDown()) {
        _isRightClick = true;
    } else {
        _isRightClick = false;
    }

    juce::Button::mouseDown(event);
}

void ModulationButton::ModulationSelectButton::mouseUp(const juce::MouseEvent& event) {
    // If it's a right click make sure the mouse is still over the button
    if (_isRightClick && isDown() && isOver()) {
        // Don't send an event, just call the callback
        _onRemoveCallback();
    } else {
        juce::Button::mouseUp(event);
    }
}
