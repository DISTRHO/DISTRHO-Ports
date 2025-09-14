#include "MultibandSplitterSubComponent.h"

MultibandSplitterSubComponent::MultibandSplitterSubComponent(SyndicateAudioProcessor& processor,
                                                             juce::Component* extensionComponent,
                                                             UIUtils::LinkedScrollView* graphView)
        : SplitterHeaderComponent(processor, graphView) {
    addBandBtn.reset(new juce::TextButton("Add Band Button"));
    extensionComponent->addAndMakeVisible(addBandBtn.get());
    addBandBtn->setButtonText(TRANS("Chain"));
    addBandBtn->setTooltip("Add another parallel chain");
    addBandBtn->setLookAndFeel(&_buttonLookAndFeel);
    addBandBtn->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    addBandBtn->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    addBandBtn->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    addBandBtn->addListener(this);

    crossoverComponent.reset(new CrossoverWrapperComponent(processor));
    addAndMakeVisible(crossoverComponent.get());
    crossoverComponent->setName("Crossover Component");
}

MultibandSplitterSubComponent::~MultibandSplitterSubComponent() {
    addBandBtn->setLookAndFeel(nullptr);
    addBandBtn = nullptr;

    crossoverComponent = nullptr;
}

void MultibandSplitterSubComponent::resized() {
    juce::Rectangle<int> headerArea = getLocalBounds();

    juce::Rectangle<int> extensionArea = addBandBtn->getParentComponent()->getLocalBounds();
    constexpr int MARGIN {4};
    extensionArea.reduce(MARGIN, MARGIN);
    extensionArea = extensionArea.withSizeKeepingCentre(extensionArea.getWidth(), extensionArea.getWidth());
    addBandBtn->setBounds(extensionArea);

    // Resize within the header component
    crossoverComponent->setBounds(headerArea.removeFromTop(getHeight() / 2));
    _viewPort->setBounds(getLocalBounds());
    SplitterHeaderComponent::resized();
}

void MultibandSplitterSubComponent::buttonClicked(juce::Button* buttonThatWasClicked){
    if (buttonThatWasClicked == addBandBtn.get()) {
        _processor.addCrossoverBand();
    }
}

void MultibandSplitterSubComponent::refreshChainButtons() {
    crossoverComponent->onParameterUpdate();
    SplitterHeaderComponent::refreshChainButtons();
}

void MultibandSplitterSubComponent::onParameterUpdate() {
    crossoverComponent->onParameterUpdate();

    if (_chainButtons.size() != ModelInterface::getNumChains(_processor.manager)) {
        _rebuildHeader();
    }

    SplitterHeaderComponent::onParameterUpdate();
}

void MultibandSplitterSubComponent::_rebuildHeader() {
    crossoverComponent->onNumChainsChanged();

    const size_t numChains {ModelInterface::getNumChains(_processor.manager)};

    _chainButtons.clear();

    for (size_t index {0}; index < numChains; index++) {
        if (numChains > 2) {
            _chainButtons.emplace_back(std::make_unique<ChainButtonsComponent>(
                _processor,
                index,
                juce::String(index + 1),
                [&, index]() { _processor.removeCrossoverBand(index); }
            ));
        } else {
            // Don't provide a callback if there's only two chains - they can't be removed
            _chainButtons.emplace_back(std::make_unique<ChainButtonsComponent>(_processor, index, juce::String(index + 1)));
        }
        _chainButtons[index]->chainLabel->setText("", juce::dontSendNotification);
        _viewPort->getViewedComponent()->addAndMakeVisible(_chainButtons[index].get());
    }

    // Layout the buttons
    SplitterHeaderComponent::resized();
}
