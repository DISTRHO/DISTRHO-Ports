#include "ChainButtonsComponent.h"
#include "ModelInterface.hpp"

ChainButtonsComponent::ChainButtonsComponent(SyndicateAudioProcessor& processor,
                                             int chainNumber,
                                             const juce::String& defaultName) :
        _processor(processor), _chainNumber(chainNumber), _defaultName(defaultName) {
    chainLabel.reset(new juce::Label("Chain Label", TRANS("")));
    addAndMakeVisible(chainLabel.get());
    chainLabel->setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    chainLabel->setJustificationType(juce::Justification::centred);
    chainLabel->setEditable(true, true, false);
    chainLabel->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    chainLabel->setColour(juce::Label::textColourId, UIUtils::highlightColour);
    chainLabel->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    chainLabel->setColour(juce::Label::backgroundWhenEditingColourId, UIUtils::backgroundColour);
    chainLabel->setColour(juce::Label::textWhenEditingColourId, UIUtils::highlightColour);
    chainLabel->setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
    chainLabel->setColour(juce::TextEditor::highlightColourId, UIUtils::highlightColour);
    chainLabel->setColour(juce::TextEditor::highlightedTextColourId, UIUtils::neutralColour);
    chainLabel->setColour(juce::CaretComponent::caretColourId, UIUtils::highlightColour);
    chainLabel->onTextChange = [this] () {
        _processor.setChainCustomName(_chainNumber, chainLabel->getText());
    };

    secondaryLabel.reset(new juce::Label("Chain Label", TRANS("")));
    secondaryLabel->setFont(juce::Font(12.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    secondaryLabel->setJustificationType(juce::Justification::centred);
    secondaryLabel->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    secondaryLabel->setColour(juce::Label::textColourId, UIUtils::highlightColour.withBrightness(0.7));
    secondaryLabel->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(secondaryLabel.get());
    secondaryLabel->toBehind(chainLabel.get());

    _setLabelsText();

    dragHandle.reset(new UIUtils::DragHandle());
    addAndMakeVisible(dragHandle.get());
    dragHandle->setColour(UIUtils::DragHandle::handleColourId, UIUtils::highlightColour);
    dragHandle->setTooltip(TRANS("Drag to move this chain to another position - hold " + UIUtils::getCopyKeyName() + " key to copy"));
    dragHandle->addMouseListener(this, false);

    bypassBtn.reset(new ChainButton(CHAIN_BUTTON_TYPE::BYPASS));
    addAndMakeVisible(bypassBtn.get());
    bypassBtn->setTooltip("Bypass this chain");
    bypassBtn->addMouseListener(this, false);
    bypassBtn->onClick = [&processor, chainNumber] () {
        processor.setChainBypass(chainNumber, !ModelInterface::getChainBypass(processor.manager, chainNumber));
    };

    muteBtn.reset(new ChainButton(CHAIN_BUTTON_TYPE::MUTE));
    addAndMakeVisible(muteBtn.get());
    muteBtn->setTooltip("Mute this chain");
    muteBtn->addMouseListener(this, false);
    muteBtn->onClick = [&processor, chainNumber] () {
        processor.setChainMute(chainNumber, !ModelInterface::getChainMute(processor.manager, chainNumber));
    };

    soloBtn.reset(new ChainButton(CHAIN_BUTTON_TYPE::SOLO));
    addAndMakeVisible(soloBtn.get());
    soloBtn->setTooltip("Solo this chain");
    soloBtn->addMouseListener(this, false);
    soloBtn->onClick = [&processor, chainNumber] () {
        processor.setChainSolo(chainNumber, !ModelInterface::getChainSolo(processor.manager, chainNumber));
    };

    removeButton.reset(new UIUtils::CrossButton("Remove Button"));
    addAndMakeVisible(removeButton.get());
    removeButton->setTooltip("Remove this chain");
    removeButton->setColour(UIUtils::CrossButton::enabledColour, UIUtils::highlightColour);
    removeButton->setColour(UIUtils::CrossButton::disabledColour, UIUtils::deactivatedColour);
    removeButton->addMouseListener(this, false);
    removeButton->setEnabled(false);
    removeButton->onClick = [this] () {
        _removeChainCallback();
    };
}

ChainButtonsComponent::ChainButtonsComponent(SyndicateAudioProcessor& processor,
                                             int chainNumber,
                                             const juce::String& defaultName,
                                             std::function<void()> removeChainCallback) :
        ChainButtonsComponent(processor, chainNumber, defaultName) {
    _removeChainCallback = removeChainCallback;
    removeButton->setEnabled(true);
}

ChainButtonsComponent::~ChainButtonsComponent() {
    chainLabel = nullptr;
    secondaryLabel = nullptr;
    dragHandle = nullptr;
    bypassBtn = nullptr;
    muteBtn = nullptr;
    soloBtn = nullptr;
    removeButton = nullptr;
}

void ChainButtonsComponent::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();

    const juce::Rectangle<int> topArea = availableArea.removeFromTop(availableArea.getHeight() / 2);
    chainLabel->setBounds(topArea);
    secondaryLabel->setBounds(topArea.withHeight(12));

    constexpr int BUTTON_ROW_MARGIN {5};
    availableArea = availableArea.reduced(BUTTON_ROW_MARGIN, 0);

    constexpr int DRAG_HANDLE_WIDTH {22};
    constexpr int BAND_BUTTON_WIDTH {21};

    juce::FlexBox flexBox;
    flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    flexBox.alignContent = juce::FlexBox::AlignContent::center;
    flexBox.items.add(juce::FlexItem(*dragHandle.get()).withMinWidth(DRAG_HANDLE_WIDTH).withMinHeight(DRAG_HANDLE_WIDTH));
    flexBox.items.add(juce::FlexItem(*bypassBtn.get()).withMinWidth(BAND_BUTTON_WIDTH).withMinHeight(BAND_BUTTON_WIDTH));
    flexBox.items.add(juce::FlexItem(*muteBtn.get()).withMinWidth(BAND_BUTTON_WIDTH).withMinHeight(BAND_BUTTON_WIDTH));
    flexBox.items.add(juce::FlexItem(*soloBtn.get()).withMinWidth(BAND_BUTTON_WIDTH).withMinHeight(BAND_BUTTON_WIDTH));
    flexBox.items.add(juce::FlexItem(*removeButton.get()).withMinWidth(BAND_BUTTON_WIDTH).withMinHeight(BAND_BUTTON_WIDTH));
    flexBox.performLayout(availableArea.toFloat());
}

void ChainButtonsComponent::refresh() {
    _setLabelsText();
    bypassBtn->setToggleState(ModelInterface::getChainBypass(_processor.manager, _chainNumber), juce::dontSendNotification);
    muteBtn->setToggleState(ModelInterface::getChainMute(_processor.manager, _chainNumber), juce::dontSendNotification);
    soloBtn->setToggleState(ModelInterface::getChainSolo(_processor.manager, _chainNumber), juce::dontSendNotification);
}

void ChainButtonsComponent::mouseDrag(const juce::MouseEvent& e) {
    if (dragHandle != nullptr && e.originalComponent == dragHandle.get()) {
        juce::DragAndDropContainer* container = juce::DragAndDropContainer::findParentDragContainerFor(this);

        if (container != nullptr) {
            juce::var details;
            details.append(_chainNumber);

            // This is a copy if alt is down, otherwise move
            details.append(juce::ModifierKeys::currentModifiers.isAltDown());

            container->startDragging(details, this);
        }
    }
}

void ChainButtonsComponent::_setLabelsText() {
    juce::String customName = ModelInterface::getChainCustomName(_processor.manager, _chainNumber);

    if (customName.isEmpty()) {
        chainLabel->setText(_defaultName, juce::dontSendNotification);
        secondaryLabel->setVisible(false);
    } else {
        chainLabel->setText(customName, juce::dontSendNotification);
        secondaryLabel->setText(_defaultName, juce::dontSendNotification);
        secondaryLabel->setVisible(true);
    }
}
