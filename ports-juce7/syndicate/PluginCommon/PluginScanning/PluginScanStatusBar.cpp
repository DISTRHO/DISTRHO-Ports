#include "PluginScanStatusMessage.h"

#include "PluginScanStatusBar.h"

PluginScanStatusBar::PluginScanStatusBar(PluginScanClient& pluginScanClient,
                                         const SelectorComponentStyle& style) :
        _pluginScanClient(pluginScanClient) {

    statusLbl.reset(new juce::Label("Status Label", juce::String()));
    addAndMakeVisible(statusLbl.get());
    statusLbl->setFont(juce::Font (15.00f, juce::Font::plain).withTypefaceStyle ("Regular"));
    statusLbl->setJustificationType(juce::Justification::centredLeft);
    statusLbl->setEditable(false, false, false);
    statusLbl->setColour(juce::Label::textColourId, style.highlightColour);

    auto styleButton = [&style](std::unique_ptr<juce::TextButton>& button) {
        button->setLookAndFeel(style.scanButtonLookAndFeel.get());
        button->setColour(UIUtils::ToggleButtonLookAndFeel::backgroundColour, style.buttonBackgroundColour);
        button->setColour(UIUtils::ToggleButtonLookAndFeel::highlightColour, style.highlightColour);
        button->setColour(UIUtils::ToggleButtonLookAndFeel::disabledColour, style.disabledColour);
    };

    startScanBtn.reset(new juce::TextButton("Start Scan Button"));
    addAndMakeVisible(startScanBtn.get());
    startScanBtn->setButtonText(TRANS("Start Scan"));
    startScanBtn->addListener(this);
    styleButton(startScanBtn);

    stopScanBtn.reset(new juce::TextButton("Stop Scan Button"));
    addAndMakeVisible(stopScanBtn.get());
    stopScanBtn->setButtonText(TRANS("Stop Scan"));
    stopScanBtn->addListener(this);
    styleButton(stopScanBtn);

    rescanAllBtn.reset(new juce::TextButton("Rescan All Button"));
    addAndMakeVisible(rescanAllBtn.get());
    rescanAllBtn->setButtonText(TRANS("Rescan All Plugins"));
    rescanAllBtn->addListener(this);
    styleButton(rescanAllBtn);

    rescanCrashedBtn.reset(new juce::TextButton("Rescan Crashed Button"));
    addAndMakeVisible(rescanCrashedBtn.get());
    rescanCrashedBtn->setButtonText(TRANS("Rescan Crashed Plugins"));
    rescanCrashedBtn->addListener(this);
    styleButton(rescanCrashedBtn);

    viewCrashedBtn.reset(new juce::TextButton("View Crashed Button"));
    addAndMakeVisible(viewCrashedBtn.get());
    viewCrashedBtn->setButtonText(TRANS("View Crashed Plugins"));
    viewCrashedBtn->addListener(this);
    styleButton(viewCrashedBtn);

    configureBtn.reset(new juce::TextButton("Configure Button"));
    addAndMakeVisible(configureBtn.get());
    configureBtn->setButtonText(TRANS("Configure"));
    configureBtn->addListener(this);
    styleButton(configureBtn);

    _pluginScanClient.addListener(this);
}

PluginScanStatusBar::~PluginScanStatusBar() {
    _pluginScanClient.stopScan();
    _pluginScanClient.removeListener(this);

    statusLbl = nullptr;
    startScanBtn = nullptr;
    stopScanBtn = nullptr;
    rescanAllBtn = nullptr;
    rescanCrashedBtn = nullptr;
    viewCrashedBtn = nullptr;
    configureBtn = nullptr;
    crashedPluginsPopover = nullptr;
}

void PluginScanStatusBar::resized() {
    const int buttonsTotalWidth {
        getWidth() < MAX_BUTTONS_WIDTH + MIN_STATUS_WIDTH ? getWidth() - MIN_STATUS_WIDTH : MAX_BUTTONS_WIDTH
    };

    if (crashedPluginsPopover != nullptr) {
        crashedPluginsPopover->setBounds(getParentComponent()->getLocalBounds());
    }

    if (configurePopover != nullptr) {
        configurePopover->setBounds(getParentComponent()->getLocalBounds());
    }

    juce::Rectangle<int> availableArea = getLocalBounds();

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::row;
    flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    flexBox.alignContent = juce::FlexBox::AlignContent::center;

    const juce::FlexItem::Margin margin(0, SPACER_WIDTH, 0, 0);
    flexBox.items.add(juce::FlexItem(*startScanBtn.get()).withMinWidth(NARROW_BUTTON_WIDTH).withMinHeight(ROW_HEIGHT).withMargin(margin));
    flexBox.items.add(juce::FlexItem(*stopScanBtn.get()).withMinWidth(NARROW_BUTTON_WIDTH).withMinHeight(ROW_HEIGHT).withMargin(margin));
    flexBox.items.add(juce::FlexItem(*rescanAllBtn.get()).withMinWidth(WIDE_BUTTON_WIDTH).withMinHeight(ROW_HEIGHT).withMargin(margin));
    flexBox.items.add(juce::FlexItem(*rescanCrashedBtn.get()).withMinWidth(WIDE_BUTTON_WIDTH).withMinHeight(ROW_HEIGHT).withMargin(margin));
    flexBox.items.add(juce::FlexItem(*viewCrashedBtn.get()).withMinWidth(WIDE_BUTTON_WIDTH).withMinHeight(ROW_HEIGHT).withMargin(margin));
    flexBox.items.add(juce::FlexItem(*configureBtn.get()).withMinWidth(NARROW_BUTTON_WIDTH).withMinHeight(ROW_HEIGHT));

    flexBox.performLayout(availableArea.removeFromRight(buttonsTotalWidth).toFloat());

    availableArea.removeFromRight(SPACER_WIDTH);
    statusLbl->setBounds(availableArea);
}

void PluginScanStatusBar::buttonClicked(juce::Button* buttonThatWasClicked) {
    if (buttonThatWasClicked == startScanBtn.get()) {
        _pluginScanClient.startScan();
    } else if (buttonThatWasClicked == stopScanBtn.get()) {
        _pluginScanClient.stopScan();
    } else if (buttonThatWasClicked == rescanAllBtn.get()) {
        _pluginScanClient.rescanAllPlugins();
    } else if (buttonThatWasClicked == rescanCrashedBtn.get()) {
        _pluginScanClient.rescanCrashedPlugins();
    } else if (buttonThatWasClicked == viewCrashedBtn.get()) {
        _createCrashedPluginsDialogue();
    } else if (buttonThatWasClicked == configureBtn.get()) {
        _createConfigureDialogue();
    }
}

void PluginScanStatusBar::handleMessage(const juce::Message& message) {
    const PluginScanStatusMessage* statusMessage {
            dynamic_cast<const PluginScanStatusMessage*>(&message)
    };

    if (statusMessage != nullptr) {

        // Set the status text
        juce::String statusString;
        const juce::String numPlugins(statusMessage->numPluginsScanned);

        if (statusMessage->errorText != "") {
            statusString = statusMessage->errorText;
        } else if (statusMessage->isScanRunning) {
            // Scan currently running
            statusString = "Found " + numPlugins + " plugins, scanning...";
        } else if (statusMessage->numPluginsScanned == 0) {
            // Couldn't restore any plugins, no scan currently running
            statusString = "No plugins available - click start scan to begin";
        } else {
            // Successfully restored plugins, no scan currently running
            statusString = "Found " + numPlugins + " plugins";
        }

        _updateButtonState(statusMessage->isScanRunning);
        statusLbl->setText(statusString, juce::dontSendNotification);
    }
}

void PluginScanStatusBar::_updateButtonState(bool isScanRunning) {
    if (isScanRunning) {
        startScanBtn->setEnabled(false);
        stopScanBtn->setEnabled(true);
        rescanAllBtn->setEnabled(false);
        rescanCrashedBtn->setEnabled(false);
        viewCrashedBtn->setEnabled(false);
        configureBtn->setEnabled(false);
    } else {
        startScanBtn->setEnabled(true);
        stopScanBtn->setEnabled(false);
        rescanAllBtn->setEnabled(true);
        rescanCrashedBtn->setEnabled(true);
        viewCrashedBtn->setEnabled(true);
        configureBtn->setEnabled(true);
    }
}

void PluginScanStatusBar::_createCrashedPluginsDialogue() {
    // Read the plugins from the crashed file
    juce::String bodyText;

    juce::File crashedPluginsFile = Utils::DataDirectory.getChildFile(Utils::CRASHED_PLUGINS_FILE_NAME);
    juce::String crashedPluginsStr;
    if (crashedPluginsFile.existsAsFile()) {
        crashedPluginsStr = crashedPluginsFile.loadFileAsString();
    }

    if (!crashedPluginsStr.isEmpty()) {
        bodyText += "The following plugins crashed during validation:\n\n";

        if (!crashedPluginsStr.isEmpty()) {
            bodyText += crashedPluginsStr;
        }
    } else {
        bodyText += "No plugins failed validation";
    }

    crashedPluginsPopover.reset(new UIUtils::PopoverComponent("Crashed plugins", bodyText, [&]() {crashedPluginsPopover.reset(); }));
    getParentComponent()->addAndMakeVisible(crashedPluginsPopover.get());
    crashedPluginsPopover->setBounds(getParentComponent()->getLocalBounds());
}

void PluginScanStatusBar::_createConfigureDialogue() {
    // Restore before opening the component
    _pluginScanClient.config.restoreFromXml();

    configurePopover.reset(new ConfigurePopover(_pluginScanClient.config, [&]() {
        _pluginScanClient.config.writeToXml();
        configurePopover.reset();
    }));

    getParentComponent()->addAndMakeVisible(configurePopover.get());
    configurePopover->setBounds(getParentComponent()->getLocalBounds());
}
