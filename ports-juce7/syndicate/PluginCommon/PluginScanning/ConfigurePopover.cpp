#include "ConfigurePopover.hpp"

namespace {
    constexpr int TEXT_HEIGHT {16};

    juce::Colour stateToLabelColour(bool state) {
        if (state) {
            return UIUtils::highlightColour;
        }

        return UIUtils::highlightColour.withAlpha(0.5f);
    }

    int getHeightForLabelText(juce::String text) {
        const int numLines = juce::StringArray::fromTokens(text, "\n").size();
        return TEXT_HEIGHT * numLines;
    }

    void removePathFromSearchPaths(juce::FileSearchPath& searchPaths, juce::String path) {
        for (int pathNumber {0}; pathNumber < searchPaths.getNumPaths(); pathNumber++) {
            if (searchPaths[pathNumber].getFullPathName() == path) {
                searchPaths.remove(pathNumber);
                return;
            }
        }
    }
}

CustomPathsList::CustomPathsList(juce::FileSearchPath& customPaths) : _customPaths(customPaths) {
    rebuild();
}

CustomPathsList::~CustomPathsList() {
}

void CustomPathsList::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();

    for (auto& label : _customPathsLabels) {
        label->setBounds(availableArea.removeFromTop(TEXT_HEIGHT));
    }
}

void CustomPathsList::mouseDown(const juce::MouseEvent& event) {
    if (!event.mods.isRightButtonDown()) {
        return;
    }

    if (auto label = dynamic_cast<juce::Label*>(event.eventComponent); label != nullptr) {
        removePathFromSearchPaths(_customPaths, label->getText());
        rebuild();
        resized();
    }
}

int CustomPathsList::getRequiredHeight() const {
    return TEXT_HEIGHT * _customPathsLabels.size();
}

void CustomPathsList::rebuild() {
    _customPathsLabels.clear();

    for (int pathNumber {0}; pathNumber < _customPaths.getNumPaths(); pathNumber++) {
        auto thisLabel = std::make_unique<juce::Label>(
            "Path " + juce::String(pathNumber) + " Label",
            _customPaths[pathNumber].getFullPathName()
        );

        addAndMakeVisible(thisLabel.get());
        thisLabel->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
        thisLabel->setJustificationType(juce::Justification::verticallyCentred | juce::Justification::horizontallyCentred);
        thisLabel->setEditable(false, false, false);
        thisLabel->setColour(juce::Label::textColourId, UIUtils::highlightColour);
        thisLabel->addMouseListener(this, false);
        thisLabel->setTooltip("Right click to remove");

        _customPathsLabels.push_back(std::move(thisLabel));
    }
}

FormatConfigureComponent::FormatConfigureComponent(
        juce::AudioPluginFormat* format,
        bool& defaultPathsEnabled,
        juce::FileSearchPath& customPaths) {

    const juce::String formatName = format->getName();

    _defaultPathsListLabel.reset(new juce::Label(formatName + " Default Paths List Label", format->getDefaultLocationsToSearch().toStringWithSeparator("\n")));
    addAndMakeVisible(_defaultPathsListLabel.get());
    _defaultPathsListLabel->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    _defaultPathsListLabel->setJustificationType(juce::Justification::verticallyCentred | juce::Justification::horizontallyCentred);
    _defaultPathsListLabel->setEditable(false, false, false);
    _defaultPathsListLabel->setColour(juce::Label::textColourId, stateToLabelColour(defaultPathsEnabled));

    _defaultPathsButton.reset(new juce::TextButton(formatName + " Default Paths Button"));
    addAndMakeVisible(_defaultPathsButton.get());
    _defaultPathsButton->setButtonText(TRANS("Include " + formatName + " Default Paths"));
    _defaultPathsButton->setLookAndFeel(&_toggleButtonLookAndFeel);
    _defaultPathsButton->setColour(UIUtils::ToggleButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    _defaultPathsButton->setColour(UIUtils::ToggleButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _defaultPathsButton->setColour(UIUtils::ToggleButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    _defaultPathsButton->setToggleState(defaultPathsEnabled, juce::NotificationType::dontSendNotification);
    _defaultPathsButton->onClick = [&]() {
        defaultPathsEnabled = !defaultPathsEnabled;
        _defaultPathsButton->setToggleState(defaultPathsEnabled, juce::NotificationType::dontSendNotification);
        _defaultPathsListLabel->setColour(juce::Label::textColourId, stateToLabelColour(defaultPathsEnabled));
    };

    _customPathsListComponent.reset(new CustomPathsList(customPaths));
    addAndMakeVisible(_customPathsListComponent.get());

    _customPathsButton.reset(new juce::TextButton(formatName + " Custom Paths Button"));
    addAndMakeVisible(_customPathsButton.get());
    _customPathsButton->setButtonText(TRANS("Add Custom " + formatName + " Paths"));
    _customPathsButton->setLookAndFeel(&_staticButtonLookAndFeel);
    _customPathsButton->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    _customPathsButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _customPathsButton->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    _customPathsButton->onClick = [&, formatName]() {
        const int flags {juce::FileBrowserComponent::canSelectDirectories | juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems};
        _fileChooser.reset(new juce::FileChooser("Add " + formatName + " Directories"));

#if _WIN32
        // Workaround on windows to stop the file chooser from appearing behind the plugin selector window
        getTopLevelComponent()->setAlwaysOnTop(false);
#endif

        _fileChooser->launchAsync(flags, [&](const juce::FileChooser& chooser) {
            for (juce::File selectedDirectory : chooser.getResults()) {
                customPaths.addIfNotAlreadyThere(selectedDirectory);
            }

            customPaths.removeNonExistentPaths();
            _customPathsListComponent->rebuild();
            resized();

#if _WIN32
            // Other half of the workaround
            // Restore the always on top state of the plugin selector window after the file chooser has closed
            getTopLevelComponent()->setAlwaysOnTop(true);
#endif
        });
    };
}

FormatConfigureComponent::~FormatConfigureComponent() {
    _defaultPathsButton->setLookAndFeel(nullptr);
    _defaultPathsListLabel->setLookAndFeel(nullptr);
    _customPathsButton->setLookAndFeel(nullptr);
}

void FormatConfigureComponent::resized() {
    constexpr int BUTTON_WIDTH {180};
    constexpr int BUTTON_HEIGHT {24};
    constexpr int MARGIN {5};

    juce::Rectangle<int> availableArea = getLocalBounds();

    juce::Rectangle<int> defaultArea = availableArea.removeFromLeft(availableArea.getWidth() / 2).reduced(MARGIN);
    _defaultPathsButton->setBounds(defaultArea.removeFromTop(BUTTON_HEIGHT).withSizeKeepingCentre(BUTTON_WIDTH, BUTTON_HEIGHT));

    const int defaultPathsHeight {getHeightForLabelText(_defaultPathsListLabel->getText())};
    defaultArea.removeFromTop(TEXT_HEIGHT);
    _defaultPathsListLabel->setBounds(defaultArea.removeFromTop(defaultPathsHeight));

    availableArea.reduced(MARGIN);
    _customPathsButton->setBounds(availableArea.removeFromTop(BUTTON_HEIGHT).withSizeKeepingCentre(BUTTON_WIDTH, BUTTON_HEIGHT));

    const int customPathsHeight {_customPathsListComponent->getRequiredHeight()};
    availableArea.removeFromTop(TEXT_HEIGHT);
    _customPathsListComponent->setBounds(availableArea.removeFromTop(customPathsHeight));
}

ConfigurePopover::ConfigurePopover(ScanConfiguration& config, std::function<void()> onCloseCallback) :
        _onCloseCallback(onCloseCallback) {

    _vstConfigure.reset(new FormatConfigureComponent(
        &config.vstFormat,
        config.VSTDefaultPaths,
        config.customVSTPaths));
    addAndMakeVisible(_vstConfigure.get());

    _vst3Configure.reset(new FormatConfigureComponent(
        &config.vst3Format,
        config.VST3DefaultPaths,
        config.customVST3Paths));
    addAndMakeVisible(_vst3Configure.get());

    _okButton.reset(new juce::TextButton("OK button"));
    addAndMakeVisible(_okButton.get());
    _okButton->setButtonText(TRANS("OK"));
    _okButton->setLookAndFeel(&_buttonLookAndFeel);
    _okButton->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    _okButton->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    _okButton->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    _okButton->onClick = [&]() { _onCloseCallback(); };

    _tooltipLabel.reset(new juce::Label("Tooltip Label", ""));
    addAndMakeVisible(_tooltipLabel.get());
    _tooltipLabel->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    _tooltipLabel->setJustificationType(juce::Justification::centred);
    _tooltipLabel->setEditable(false, false, false);
    _tooltipLabel->setColour(juce::Label::textColourId, UIUtils::tooltipColour);

    // Start tooltip label
    _vstConfigure->addMouseListener(this, true);
    _vst3Configure->addMouseListener(this, true);
}

ConfigurePopover::~ConfigurePopover() {
    _okButton->setLookAndFeel(nullptr);
}

void ConfigurePopover::resized() {
    constexpr int WINDOW_PADDING {20};
    constexpr int AREA_PADDING {5};

    juce::Rectangle<int> availableArea = getLocalBounds().reduced(WINDOW_PADDING);

    _tooltipLabel->setBounds(availableArea.removeFromBottom(16));

    juce::Rectangle<int> okButtonArea = availableArea.removeFromBottom(availableArea.getHeight() / 4);
    _okButton->setBounds(okButtonArea.withSizeKeepingCentre(60, 40));

    _vstConfigure->setBounds(availableArea.removeFromTop(availableArea.getHeight() / 2).reduced(AREA_PADDING));
    _vst3Configure->setBounds(availableArea.reduced(AREA_PADDING));
}

void ConfigurePopover::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black.withAlpha(0.8f));
}

void ConfigurePopover::mouseEnter(const juce::MouseEvent& event) {
    juce::TooltipClient* tooltipClient = dynamic_cast<juce::TooltipClient*>(event.eventComponent);

    if (tooltipClient != nullptr) {
        const juce::String displayString = tooltipClient->getTooltip();
        _tooltipLabel->setText(displayString, juce::dontSendNotification);
    }
}

void ConfigurePopover::mouseExit(const juce::MouseEvent& /*event*/) {
    if (_tooltipLabel != nullptr) {
        _tooltipLabel->setText("", juce::dontSendNotification);
    }
}
