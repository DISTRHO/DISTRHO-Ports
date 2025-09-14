#include "UndoRedoComponent.h"

namespace {
    const juce::String undoTooltipPrefix = "Undo the previous change - ";
    const juce::String redoTooltipPrefix = "Redo the last change - ";
    const juce::String defaultUndoTooltip = undoTooltipPrefix + "no changes to undo";
    const juce::String defaultRedoTooltip = redoTooltipPrefix + "no changes to redo";
}

UndoRedoComponent::UndoRedoComponent(SyndicateAudioProcessor& processor)
        : _processor(processor) {
    _undoButton.reset(new juce::TextButton("Undo Button"));
    addAndMakeVisible(_undoButton.get());
    _undoButton->setButtonText(TRANS("Undo"));
    _undoButton->setTooltip(defaultUndoTooltip);
    _undoButton->setLookAndFeel(&_buttonLookAndFeel);
    _undoButton->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    _undoButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _undoButton->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    _undoButton->onClick = [&]() {
        _processor.undo();
    };

    _redoButton.reset(new juce::TextButton("Redo Button"));
    addAndMakeVisible(_redoButton.get());
    _redoButton->setButtonText(TRANS("Redo"));
    _redoButton->setTooltip(defaultRedoTooltip);
    _redoButton->setLookAndFeel(&_buttonLookAndFeel);
    _redoButton->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    _redoButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _redoButton->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    _redoButton->onClick = [&]() {
        processor.redo();
    };

    refresh();
}

UndoRedoComponent::~UndoRedoComponent() {
}

void UndoRedoComponent::resized() {
    constexpr int BUTTON_HEIGHT {24};
    juce::Rectangle<int> availableArea = getLocalBounds();
    availableArea.reduce(4, 0);
    availableArea.removeFromTop(8);

    // Undo/Redo
    _undoButton->setBounds(availableArea.removeFromTop(BUTTON_HEIGHT));
    availableArea.removeFromTop(4);
    _redoButton->setBounds(availableArea.removeFromTop(BUTTON_HEIGHT));
}

void UndoRedoComponent::refresh() {
    const std::optional<juce::String> undoOperation = ModelInterface::getUndoOperation(_processor.manager);
    if (undoOperation.has_value()) {
        _undoButton->setTooltip(undoTooltipPrefix + undoOperation.value());
        _undoButton->setEnabled(true);
    } else {
        _undoButton->setTooltip(defaultUndoTooltip);
        _undoButton->setEnabled(false);
    }

    const std::optional<juce::String> redoOperation = ModelInterface::getRedoOperation(_processor.manager);
    if (redoOperation.has_value()) {
        _redoButton->setTooltip(redoTooltipPrefix + redoOperation.value());
        _redoButton->setEnabled(true);
    } else {
        _redoButton->setTooltip(defaultRedoTooltip);
        _redoButton->setEnabled(false);
    }
}
