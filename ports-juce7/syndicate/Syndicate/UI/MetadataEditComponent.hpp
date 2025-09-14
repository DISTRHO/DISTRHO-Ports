#pragma once

#include <JuceHeader.h>

#include "PresetMetadata.hpp"
#include "UIUtils.h"

class MetadataEditComponent : public juce::Component {
public:
    MetadataEditComponent(
        const PresetMetadata& metadata,
        std::function<void(const PresetMetadata& newMetadata)> onUpdateCallback,
        std::function<void()> onCloseCallback);

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    std::function<void(const PresetMetadata& newMetadata)> _onUpdateCallback;
    std::function<void()> _onCloseCallback;

    UIUtils::StaticButtonLookAndFeel _buttonLookAndFeel;
    UIUtils::SearchBarLookAndFeel _searchBarLookAndFeel;

    std::unique_ptr<juce::Label> _nameLabel;
    std::unique_ptr<juce::Label> _nameValueLabel;
    std::unique_ptr<juce::Label> _fullPathLabel;
    std::unique_ptr<juce::Label> _fullPathValueLabel;
    std::unique_ptr<juce::Label> _authorLabel;
    std::unique_ptr<juce::TextEditor> _authorEditor;
    std::unique_ptr<juce::Label> _descriptionLabel;
    std::unique_ptr<juce::TextEditor> _descriptionEditor;
    std::unique_ptr<juce::TextButton> _closeButton;
};
