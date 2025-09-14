#include "PluginParameterSelectorComponent.h"

PluginParameterSelectorComponent::PluginParameterSelectorComponent(
        PluginParameterSelectorListParameters selectorListParameters,
        std::function<void()> onCloseCallback)
            : _state(selectorListParameters.state),
              _onCloseCallback(onCloseCallback) {
    _searchTextEditor.reset(new juce::TextEditor("Search Text Editor"));
    addAndMakeVisible(_searchTextEditor.get());
    _searchTextEditor->setMultiLine(false);
    _searchTextEditor->setReturnKeyStartsNewLine(false);
    _searchTextEditor->setReadOnly(false);
    _searchTextEditor->setScrollbarsShown(true);
    _searchTextEditor->setCaretVisible(true);
    _searchTextEditor->setPopupMenuEnabled(true);
    _searchTextEditor->setText(juce::String());
    _searchTextEditor->setEscapeAndReturnKeysConsumed(false);
    _searchTextEditor->setSelectAllWhenFocused(true);
    _searchTextEditor->setWantsKeyboardFocus(true);
    _searchTextEditor->addListener(this);
    _searchTextEditor->setLookAndFeel(&_searchBarLookAndFeel);
    _searchTextEditor->setColour(juce::TextEditor::outlineColourId, UIUtils::highlightColour);
    _searchTextEditor->setColour(juce::TextEditor::backgroundColourId, UIUtils::backgroundColour);
    _searchTextEditor->setColour(juce::TextEditor::textColourId, UIUtils::highlightColour);
    _searchTextEditor->setColour(juce::TextEditor::highlightColourId, UIUtils::highlightColour);
    _searchTextEditor->setColour(juce::TextEditor::highlightedTextColourId, UIUtils::neutralColour);
    _searchTextEditor->setColour(juce::CaretComponent::caretColourId, UIUtils::highlightColour);

    _parameterTableListBox.reset(new PluginParameterSelectorTableListBox(selectorListParameters));
    addAndMakeVisible(_parameterTableListBox.get());
    _parameterTableListBox->setName("Plugin Parameter Table List Box");
    _parameterTableListBox->getHeader().setLookAndFeel(&_tableHeaderLookAndFeel);
    _parameterTableListBox->getHeader().setColour(juce::TableHeaderComponent::textColourId, UIUtils::highlightColour);
    _parameterTableListBox->getHeader().setColour(juce::TableHeaderComponent::outlineColourId, UIUtils::highlightColour);
    _parameterTableListBox->getHeader().setColour(juce::TableHeaderComponent::backgroundColourId, UIUtils::backgroundColour);
    _parameterTableListBox->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::backgroundColourId, juce::Colour(0x00000000));
    _parameterTableListBox->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::thumbColourId, UIUtils::neutralColour.withAlpha(0.5f));
    _parameterTableListBox->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::trackColourId, juce::Colour(0x00000000));
    _parameterTableListBox->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::backgroundColourId, juce::Colour(0x00000000));
    _parameterTableListBox->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::thumbColourId, UIUtils::neutralColour.withAlpha(0.5f));
    _parameterTableListBox->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::trackColourId, juce::Colour(0x00000000));

    _hintLabel.reset(new juce::Label("Hint Label", juce::String()));
    addAndMakeVisible(_hintLabel.get());
    _hintLabel->setFont(juce::Font (15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    _hintLabel->setJustificationType(juce::Justification::centred);
    _hintLabel->setEditable(false, false, false);
    _hintLabel->setColour(juce::Label::textColourId, UIUtils::tooltipColour);
    _hintLabel->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    _hintLabel->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));
    if (selectorListParameters.isReplacingParameter) {
        _hintLabel->setText("Double click a parameter to select it", juce::NotificationType::dontSendNotification);
    } else {
        _hintLabel->setText("Double click a parameter to select it\nHold " + UIUtils::getCmdKeyName() + " to keep the window open and select another", juce::NotificationType::dontSendNotification);
    }

    // Recall UI from state
    _searchTextEditor->setText(_state.filterString, false);
}

PluginParameterSelectorComponent::~PluginParameterSelectorComponent() {
    // Save the scroll position
    _state.scrollPosition = _parameterTableListBox->getVerticalPosition();

    _searchTextEditor->setLookAndFeel(nullptr);
    _parameterTableListBox->getHeader().setLookAndFeel(nullptr);

    _searchTextEditor = nullptr;
    _parameterTableListBox = nullptr;
    _hintLabel = nullptr;
}

void PluginParameterSelectorComponent::resized() {
    constexpr int MARGIN_SIZE {10};
    constexpr int ROW_HEIGHT {24};

    juce::Rectangle<int> area = getLocalBounds().reduced(MARGIN_SIZE);
    _searchTextEditor->setBounds(area.removeFromTop(ROW_HEIGHT));
    _hintLabel->setBounds(area.removeFromBottom(ROW_HEIGHT * 2));
    area.removeFromTop(MARGIN_SIZE).removeFromBottom(MARGIN_SIZE);
    _parameterTableListBox->setBounds(area);
}

void PluginParameterSelectorComponent::textEditorTextChanged(juce::TextEditor& textEditor) {
    _state.filterString = _searchTextEditor->getText();
    _parameterTableListBox->onFilterUpdate();
}

void PluginParameterSelectorComponent::paint(juce::Graphics& g) {
    g.fillAll(UIUtils::backgroundColour);
}

bool PluginParameterSelectorComponent::keyPressed(const juce::KeyPress& key) {
    if (key.isKeyCode(juce::KeyPress::escapeKey)) {
        // Close the window
        _onCloseCallback();
        return true;
    }

    return false;
}

void PluginParameterSelectorComponent::restoreScrollPosition() {
    _parameterTableListBox->setVerticalPosition(_state.scrollPosition);
}
