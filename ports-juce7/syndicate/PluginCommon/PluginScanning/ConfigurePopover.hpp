#pragma once

#include <JuceHeader.h>
#include "UIUtils.h"
#include "ScanConfiguration.hpp"

class CustomPathsList : public juce::Component {
public:
    CustomPathsList(juce::FileSearchPath& customPaths);

    ~CustomPathsList();

    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;

    int getRequiredHeight() const;

    void rebuild();

private:
    juce::FileSearchPath& _customPaths;
    std::vector<std::unique_ptr<juce::Label>> _customPathsLabels;
};

class FormatConfigureComponent : public juce::Component {
public:
    FormatConfigureComponent(juce::AudioPluginFormat* format,
                             bool& defaultPathsEnabled,
                             juce::FileSearchPath& customPaths);

    ~FormatConfigureComponent();

    void resized() override;

private:
    UIUtils::ToggleButtonLookAndFeel _toggleButtonLookAndFeel;
    UIUtils::StaticButtonLookAndFeel _staticButtonLookAndFeel;

    std::unique_ptr<juce::TextButton> _defaultPathsButton;
    std::unique_ptr<juce::Label> _defaultPathsListLabel;
    std::unique_ptr<juce::TextButton> _customPathsButton;
    std::unique_ptr<CustomPathsList> _customPathsListComponent;

    std::unique_ptr<juce::FileChooser> _fileChooser;
};

class ConfigurePopover : public juce::Component {
public:
    ConfigurePopover(ScanConfiguration& config, std::function<void()> onCloseCallback);
    ~ConfigurePopover();

    void resized() override;

    void paint(juce::Graphics& g) override;

    void mouseEnter(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;

private:
    std::function<void()> _onCloseCallback;

    UIUtils::StaticButtonLookAndFeel _buttonLookAndFeel;

    std::unique_ptr<FormatConfigureComponent> _vstConfigure;
    std::unique_ptr<FormatConfigureComponent> _vst3Configure;

    std::unique_ptr<juce::TextButton> _okButton;
    std::unique_ptr<juce::Label> _tooltipLabel;
};
