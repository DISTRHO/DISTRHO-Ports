#include "ImportExportComponent.h"

#include "PluginEditor.h"

namespace {
    const juce::String undoTooltipPrefix = "Undo the previous change - ";
    const juce::String redoTooltipPrefix = "Redo the last change - ";
    const juce::String defaultUndoTooltip = undoTooltipPrefix + "no changes to undo";
    const juce::String defaultRedoTooltip = redoTooltipPrefix + "no changes to redo";

    void redactXml(juce::XmlElement* element) {
        if (element == nullptr) {
            return;
        }

        // Remove the file attribute that plugins have
        if (element->hasAttribute("file")) {
            element->removeAttribute("file");
        }

        // Remove the metadata file path
        // TODO could optimise this - we don't need to check every element
        if (element->hasAttribute("MetadataFullPath")) {
            element->removeAttribute("MetadataFullPath");
        }

        redactXml(element->getFirstChildElement());
        redactXml(element->getNextElement());
    }
}

ImportExportComponent::ImportExportComponent(SyndicateAudioProcessor& processor, SyndicateAudioProcessorEditor& editor)
        : _processor(processor), _editor(editor) {

    auto styleButton = [this](juce::TextButton* button) {
        button->setLookAndFeel(&_buttonLookAndFeel);
        button->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
        button->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
        button->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    };

    _exportButton.reset(new juce::TextButton("Save Button"));
    addAndMakeVisible(_exportButton.get());
    _exportButton->setButtonText(TRANS("Save"));
    _exportButton->setTooltip("Save the current settings to a file");
    styleButton(_exportButton.get());
    _exportButton->onClick = [&]() {
        const int flags {juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting};
        _fileChooser.reset(new juce::FileChooser("Export Syndicate Preset", juce::File(), "*.syn"));
        _fileChooser->launchAsync(flags, [&](const juce::FileChooser& chooser) {
            _onExportToFile(chooser.getResult());
        });
    };

    _importButton.reset(new juce::TextButton("Load Button"));
    addAndMakeVisible(_importButton.get());
    _importButton->setButtonText(TRANS("Load"));
    _importButton->setTooltip("Load settings from a previously saved file");
    styleButton(_importButton.get());
    _importButton->onClick = [&]() {
        const int flags {juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode};
        _fileChooser.reset(new juce::FileChooser("Import Syndicate Preset", juce::File(), "*.syn"));
        _fileChooser->launchAsync(flags, [&](const juce::FileChooser& chooser) {
            _onImportFromFile(chooser.getResult());
        });
    };

    _metaButton.reset(new juce::TextButton("Meta Button"));
    addAndMakeVisible(_metaButton.get());
    _metaButton->setButtonText(TRANS("Info"));
    _metaButton->setTooltip("View and edit preset metadata");
    styleButton(_metaButton.get());
    _metaButton->onClick = [&]() {
        _metadataEditPopover.reset(new MetadataEditComponent(
            _processor.presetMetadata,
            [&](const PresetMetadata& newMetadata) { _processor.setPresetMetadata(newMetadata); },
            [&]() { _metadataEditPopover.reset(); }
        ));

        getParentComponent()->addAndMakeVisible(_metadataEditPopover.get());
        _metadataEditPopover->setBounds(getParentComponent()->getLocalBounds());
    };

    _nameLabel.reset(new juce::Label("Name Label", UIUtils::presetNameOrPlaceholder(_processor.presetMetadata.name)));
    addAndMakeVisible(_nameLabel.get());
    _nameLabel->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    _nameLabel->setJustificationType(juce::Justification::centred);
    _nameLabel->setEditable(false, false, false);
    _nameLabel->setColour(juce::Label::textColourId, UIUtils::tooltipColour);
    _nameLabel->toBack();

    _initButton.reset(new juce::TextButton("Init Button"));
    addAndMakeVisible(_initButton.get());
    _initButton->setButtonText(TRANS("Init"));
    _initButton->setTooltip("Reset all state and parameters to their default values");
    styleButton(_initButton.get());
    _initButton->onClick = [&]() {
        _processor.resetAllState();
    };
}

ImportExportComponent::~ImportExportComponent() {
}

void ImportExportComponent::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();
    availableArea.reduce(4, 2);

    constexpr int BUTTON_HEIGHT {24};
    constexpr int BUTTON_WIDTH {56}; // Match sidebar buttons
    constexpr int SPACER_WIDTH {8};

    _nameLabel->setBounds(availableArea);

    _exportButton->setBounds(availableArea.removeFromLeft(BUTTON_WIDTH));
    availableArea.removeFromLeft(SPACER_WIDTH);
    _importButton->setBounds(availableArea.removeFromLeft(BUTTON_WIDTH));
    availableArea.removeFromLeft(SPACER_WIDTH);
    _metaButton->setBounds(availableArea.removeFromLeft(BUTTON_WIDTH));
    availableArea.removeFromLeft(SPACER_WIDTH);
    _initButton->setBounds(availableArea.removeFromLeft(BUTTON_WIDTH));
}

void ImportExportComponent::paint(juce::Graphics& g) {
    g.fillAll(UIUtils::modulationTrayBackgroundColour);
}

void ImportExportComponent::refresh() {
    _nameLabel->setText(UIUtils::presetNameOrPlaceholder(_processor.presetMetadata.name), juce::dontSendNotification);
}

void ImportExportComponent::_onExportToFile(juce::File file) {
    PresetMetadata newMetadata = _processor.presetMetadata;
    newMetadata.name = file.getFileNameWithoutExtension();
    newMetadata.fullPath = file.getFullPathName();
    _processor.setPresetMetadata(newMetadata);

    std::unique_ptr<juce::XmlElement> element = _processor.writeToXml();
    redactXml(element.get());
    element->writeTo(file);
}

void ImportExportComponent::_onImportFromFile(juce::File file) {
    if (file.existsAsFile()) {
        std::unique_ptr<juce::XmlElement> element = juce::XmlDocument::parse(file);
        if (element != nullptr) {
            _processor.restoreFromXml(std::move(element));

            PresetMetadata newMetadata = _processor.presetMetadata;
            newMetadata.name = file.getFileNameWithoutExtension();
            newMetadata.fullPath = file.getFullPathName();
            _processor.setPresetMetadata(newMetadata);

            _editor.closeGuestPluginWindows();
            _editor.needsToRefreshAll();
        }
    }
}
