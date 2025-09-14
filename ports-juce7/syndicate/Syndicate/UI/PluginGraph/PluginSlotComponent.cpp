#include "PluginSlotComponent.h"
#include "UIUtils.h"

namespace {
    constexpr int CIRCLE_MARGIN {5};
    constexpr int CIRCLE_DIAMETER {UIUtils::PLUGIN_SLOT_HEIGHT - CIRCLE_MARGIN * 2};
}

PluginSlotComponent::PluginSlotComponent(PluginSelectionInterface& pluginSelectionInterface,
                                         PluginModulationInterface& pluginModulationInterface,
                                         int chainNumber,
                                         int pluginNumber) :
        BaseSlotComponent(chainNumber, pluginNumber),
        _pluginSelectionInterface(pluginSelectionInterface),
        _pluginModulationInterface(pluginModulationInterface),
        _isHover(false) {

    _bypassButton.reset(new UIUtils::BypassButton("Bypass Button"));
    addAndMakeVisible(_bypassButton.get());
    _bypassButton->setTooltip(TRANS("Enable/disable this plugin slot"));
    _bypassButton->setColour(juce::TextButton::buttonOnColourId, UIUtils::highlightColour);
    _bypassButton->setColour(juce::TextButton::textColourOnId, UIUtils::slotBackgroundColour);
    _bypassButton->setColour(juce::TextButton::textColourOffId, UIUtils::highlightColour);
    _bypassButton->addMouseListener(this, false);
    _bypassButton->addListener(this);

    _openButton.reset(new UIUtils::PluginOpenButton("Plugin Open Button"));
    addAndMakeVisible(_openButton.get());
    _openButton->setTooltip(TRANS("Open the editor window for this plugin slot"));
    _openButton->setColour(juce::TextButton::buttonOnColourId, UIUtils::highlightColour);
    _openButton->addMouseListener(this, false);
    _openButton->addListener(this);

    _replaceButton.reset(new UIUtils::PluginReplaceButton("Plugin Replace Button"));
    addAndMakeVisible(_replaceButton.get());
    _replaceButton->setTooltip(TRANS("Replace the plugin in this slot"));
    _replaceButton->setColour(juce::TextButton::buttonOnColourId, UIUtils::highlightColour);
    _replaceButton->addMouseListener(this, false);
    _replaceButton->addListener(this);

    _deleteButton.reset(new UIUtils::CrossButton("Plugin delete Button"));
    addAndMakeVisible(_deleteButton.get());
    _deleteButton->setTooltip(TRANS("Remove this plugin slot"));
    _deleteButton->setColour(UIUtils::CrossButton::enabledColour, UIUtils::highlightColour);
    _deleteButton->setColour(UIUtils::CrossButton::disabledColour, UIUtils::deactivatedColour);
    _deleteButton->addMouseListener(this, false);
    _deleteButton->addListener(this);

    _modulationButton.reset(new UIUtils::ModulationButton("Modulation Button"));
    addAndMakeVisible(_modulationButton.get());
    _modulationButton->setTooltip(TRANS("Open the modulation tray for this plugin slot"));
    _modulationButton->setColour(juce::TextButton::buttonOnColourId, UIUtils::PLUGIN_SLOT_MODULATION_ON_COLOUR);
    _modulationButton->setColour(juce::TextButton::textColourOnId, UIUtils::slotBackgroundColour);
    _modulationButton->setColour(juce::TextButton::textColourOffId, UIUtils::PLUGIN_SLOT_MODULATION_ON_COLOUR);
    _modulationButton->addMouseListener(this, false);
    _modulationButton->addListener(this);

    _descriptionLabel.reset(new juce::Label("Description Label", _pluginSelectionInterface.getPluginName(chainNumber, pluginNumber)));
    addAndMakeVisible(_descriptionLabel.get());
    _descriptionLabel->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    _descriptionLabel->setJustificationType(juce::Justification::centred);
    _descriptionLabel->setEditable(false, false, false);
    _descriptionLabel->setColour(juce::Label::textColourId, UIUtils::highlightColour);
    _descriptionLabel->addMouseListener(this, false);

    PluginModulationConfig modulationConfig =
        _pluginModulationInterface.getPluginModulationConfig(_chainNumber, _slotNumber);

    if (modulationConfig.isActive) {
        _modulationTray.reset(new PluginSlotModulationTray(_pluginModulationInterface, _chainNumber, _slotNumber));
        addAndMakeVisible(_modulationTray.get());
    }

    // Set UI state
    _bypassButton->setToggleState(!_pluginSelectionInterface.getPluginBypass(_chainNumber, _slotNumber), juce::dontSendNotification);
    _modulationButton->setToggleState(modulationConfig.isActive, juce::dontSendNotification);

    _openButton->setVisible(false);
    _replaceButton->setVisible(false);
    _deleteButton->setVisible(false);
    _descriptionLabel->setVisible(true);
}

PluginSlotComponent::~PluginSlotComponent() {
    _bypassButton = nullptr;
    _openButton = nullptr;
    _replaceButton = nullptr;
    _deleteButton = nullptr;
    _modulationButton = nullptr;
    _modulationTray = nullptr;
}

void PluginSlotComponent::resized() {
    BaseSlotComponent::resized();

    if (_modulationTray != nullptr) {
        _modulationTray->setBounds(0, UIUtils::PLUGIN_SLOT_HEIGHT, getWidth(), UIUtils::PLUGIN_SLOT_MOD_TRAY_HEIGHT);
    }

    // Figure out the areas for each button
    juce::Rectangle<int> availableArea = getAvailableSlotArea();
    availableArea.removeFromLeft(CIRCLE_MARGIN);
    _bypassButton->setBounds(availableArea.removeFromLeft(CIRCLE_DIAMETER).withTrimmedTop(CIRCLE_MARGIN).withTrimmedBottom(CIRCLE_MARGIN));

    availableArea.removeFromLeft(CIRCLE_MARGIN);
    availableArea.removeFromRight(CIRCLE_MARGIN);

    _modulationButton->setBounds(availableArea.removeFromRight(CIRCLE_DIAMETER).withTrimmedTop(CIRCLE_MARGIN).withTrimmedBottom(CIRCLE_MARGIN));
    availableArea.removeFromRight(CIRCLE_MARGIN);

    _descriptionLabel->setBounds(availableArea);

    _openButton->setBounds(availableArea.removeFromLeft(availableArea.getWidth() / 3));
    _replaceButton->setBounds(availableArea.removeFromLeft(availableArea.getWidth() / 2));
    _deleteButton->setBounds(availableArea);
}

void PluginSlotComponent::mouseMove(const juce::MouseEvent& event) {

    const juce::Point<int> mousePoint {event.getEventRelativeTo(this).position.toInt()};

    // Show the buttons if the mouse is over anywhere other than the drag handle
    if (getAvailableSlotArea().contains(mousePoint)) {
        _openButton->setVisible(true);
        _replaceButton->setVisible(true);
        _deleteButton->setVisible(true);
        _descriptionLabel->setVisible(false);

        _isHover = true;
    } else if (_isHover) {
        _openButton->setVisible(false);
        _replaceButton->setVisible(false);
        _deleteButton->setVisible(false);
        _descriptionLabel->setVisible(true);

        _isHover = false;
    }
}

void PluginSlotComponent::mouseExit(const juce::MouseEvent& /*event*/) {
    _openButton->setVisible(false);
    _replaceButton->setVisible(false);
    _deleteButton->setVisible(false);
    _descriptionLabel->setVisible(true);

    _isHover = false;
}

void PluginSlotComponent::buttonClicked(juce::Button* buttonThatWasClicked) {
    if (buttonThatWasClicked == _bypassButton.get()) {
        _pluginSelectionInterface.togglePluginBypass(_chainNumber, _slotNumber);
        _bypassButton->setToggleState(!_pluginSelectionInterface.getPluginBypass(_chainNumber, _slotNumber), juce::dontSendNotification);
    } else if (buttonThatWasClicked == _openButton.get()) {
        _pluginSelectionInterface.openPluginEditor(_chainNumber, _slotNumber);
    } else if (buttonThatWasClicked == _replaceButton.get()) {
        _pluginSelectionInterface.selectNewPlugin(_chainNumber, _slotNumber);
    } else if (buttonThatWasClicked == _deleteButton.get()) {
        _pluginSelectionInterface.removePlugin(_chainNumber, _slotNumber);
    } else if (buttonThatWasClicked == _modulationButton.get()) {
        // Don't update the button - togglePluginModulationActive() will cause the graph to redraw
        _pluginModulationInterface.togglePluginModulationActive(_chainNumber, _slotNumber);
    }
}

void PluginSlotComponent::paint(juce::Graphics& g) {
    // Fill the space between the plugin slot and the modulation tray
    if (_modulationTray != nullptr) {
        g.setColour(UIUtils::modulationTrayBackgroundColour);
        g.fillRect(MARGIN, UIUtils::PLUGIN_SLOT_HEIGHT / 2, getWidth() - MARGIN * 2, UIUtils::PLUGIN_SLOT_HEIGHT / 2);
    }

    // Draw the slot background
    BaseSlotComponent::paint(g);
}
