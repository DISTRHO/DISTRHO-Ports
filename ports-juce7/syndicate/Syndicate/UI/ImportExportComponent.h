#pragma once

#include <JuceHeader.h>
#include "UIUtils.h"
#include "PluginProcessor.h"
#include "MetadataEditComponent.hpp"

class SyndicateAudioProcessorEditor;

class ImportExportComponent : public juce::Component {
public:
    ImportExportComponent(SyndicateAudioProcessor& processor, SyndicateAudioProcessorEditor& editor);
    ~ImportExportComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void refresh();

private:
    UIUtils::StaticButtonLookAndFeel _buttonLookAndFeel;

    std::unique_ptr<juce::TextButton> _exportButton;
    std::unique_ptr<juce::TextButton> _importButton;
    std::unique_ptr<juce::FileChooser> _fileChooser;

    std::unique_ptr<juce::TextButton> _metaButton;
    std::unique_ptr<MetadataEditComponent> _metadataEditPopover;

    std::unique_ptr<juce::Label> _nameLabel;

    std::unique_ptr<juce::TextButton> _initButton;

    SyndicateAudioProcessor& _processor;
    SyndicateAudioProcessorEditor& _editor;

    void _onExportToFile(juce::File file);
    void _onImportFromFile(juce::File file);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImportExportComponent)
};
