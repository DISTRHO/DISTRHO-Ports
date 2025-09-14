#include "MetadataEditComponent.hpp"

MetadataEditComponent::MetadataEditComponent(
        const PresetMetadata& metadata,
        std::function<void(const PresetMetadata& newMetadata)> onUpdateCallback,
        std::function<void()> onCloseCallback)
        : _onUpdateCallback(onUpdateCallback),
            _onCloseCallback(onCloseCallback) {
    auto setUpLabel = [](juce::Label* label) {
        label->setFont(juce::Font(20.00f, juce::Font::plain).withTypefaceStyle("Bold"));
        label->setJustificationType(juce::Justification::left);
        label->setEditable(false, false, false);
        label->setColour(juce::Label::textColourId, UIUtils::highlightColour);
    };

    auto setUpValueLabel = [](juce::Label* label) {
        label->setFont(juce::Font(20.00f, juce::Font::plain).withTypefaceStyle("Regular"));
        label->setJustificationType(juce::Justification::left);
        label->setEditable(false, false, false);
        label->setColour(juce::Label::textColourId, UIUtils::highlightColour);
    };

    auto setUpEditor = [](juce::TextEditor* editor, UIUtils::SearchBarLookAndFeel* lookAndFeel) {
        editor->setMultiLine(false);
        editor->setReturnKeyStartsNewLine(false);
        editor->setReadOnly(false);
        editor->setScrollbarsShown(true);
        editor->setCaretVisible(true);
        editor->setPopupMenuEnabled(true);
        editor->setText(juce::String());
        editor->setEscapeAndReturnKeysConsumed(false);
        editor->setSelectAllWhenFocused(true);
        editor->setLookAndFeel(lookAndFeel);
        editor->setColour(juce::TextEditor::outlineColourId, UIUtils::highlightColour);
        editor->setColour(juce::TextEditor::backgroundColourId, UIUtils::backgroundColour);
        editor->setColour(juce::TextEditor::textColourId, UIUtils::highlightColour);
        editor->setColour(juce::TextEditor::highlightColourId, UIUtils::highlightColour);
        editor->setColour(juce::TextEditor::highlightedTextColourId, UIUtils::neutralColour);
        editor->setColour(juce::CaretComponent::caretColourId, UIUtils::highlightColour);
    };

    _nameLabel.reset(new juce::Label("Name Label", "Name"));
    addAndMakeVisible(_nameLabel.get());
    setUpLabel(_nameLabel.get());

    _nameValueLabel.reset(new juce::Label("Name Value", UIUtils::presetNameOrPlaceholder(metadata.name)));
    addAndMakeVisible(_nameValueLabel.get());
    setUpValueLabel(_nameValueLabel.get());

    _fullPathLabel.reset(new juce::Label("Full Path Label", "File Path"));
    addAndMakeVisible(_fullPathLabel.get());
    setUpLabel(_fullPathLabel.get());

    _fullPathValueLabel.reset(new juce::Label("Full Path Value", UIUtils::presetNameOrPlaceholder(metadata.fullPath)));
    addAndMakeVisible(_fullPathValueLabel.get());
    setUpValueLabel(_fullPathValueLabel.get());

    _authorLabel.reset(new juce::Label("Author Label", "Author"));
    addAndMakeVisible(_authorLabel.get());
    setUpLabel(_authorLabel.get());

    _authorEditor.reset(new juce::TextEditor("Author Editor"));
    addAndMakeVisible(_authorEditor.get());
    setUpEditor(_authorEditor.get(), &_searchBarLookAndFeel);
    _authorEditor->setText(metadata.author);

    _descriptionLabel.reset(new juce::Label("Description Label", "Description"));
    addAndMakeVisible(_descriptionLabel.get());
    setUpLabel(_descriptionLabel.get());

    _descriptionEditor.reset(new juce::TextEditor("Description Editor"));
    addAndMakeVisible(_descriptionEditor.get());
    setUpEditor(_descriptionEditor.get(), &_searchBarLookAndFeel);
    _descriptionEditor->setMultiLine(true);
    _descriptionEditor->setReturnKeyStartsNewLine(true);
    _descriptionEditor->setText(metadata.description);

    _closeButton.reset(new juce::TextButton("OK button"));
    addAndMakeVisible(_closeButton.get());
    _closeButton->setButtonText(TRANS("OK"));
    _closeButton->setLookAndFeel(&_buttonLookAndFeel);
    _closeButton->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    _closeButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _closeButton->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    _closeButton->onClick = [&, metadata] {
        _onUpdateCallback(PresetMetadata{
                metadata.name,
                metadata.fullPath,
                _authorEditor->getText(),
                _descriptionEditor->getText()
        });
        _onCloseCallback();
    };
}

void MetadataEditComponent::resized() {
    constexpr int ROW_HEIGHT {24};

    juce::Rectangle<int> availableArea = getLocalBounds().reduced(20);

    // Name row
    _nameLabel->setBounds(availableArea.removeFromTop(ROW_HEIGHT));
    _nameValueLabel->setBounds(availableArea.removeFromTop(ROW_HEIGHT));

    availableArea.removeFromTop(ROW_HEIGHT);

    // Full path row
    _fullPathLabel->setBounds(availableArea.removeFromTop(ROW_HEIGHT));
    _fullPathValueLabel->setBounds(availableArea.removeFromTop(ROW_HEIGHT));

    availableArea.removeFromTop(ROW_HEIGHT);

    // Author row
    _authorLabel->setBounds(availableArea.removeFromTop(ROW_HEIGHT));
    _authorEditor->setBounds(availableArea.removeFromTop(ROW_HEIGHT));

    availableArea.removeFromTop(ROW_HEIGHT);

    // Description row
    _descriptionLabel->setBounds(availableArea.removeFromTop(ROW_HEIGHT));
    _descriptionEditor->setBounds(availableArea.removeFromTop(4 * ROW_HEIGHT));

    juce::Rectangle<int> closeButtonArea = availableArea.removeFromBottom(availableArea.getHeight() / 4);
    _closeButton->setBounds(closeButtonArea.withSizeKeepingCentre(60, 40));
}

void MetadataEditComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black.withAlpha(0.8f));
}
