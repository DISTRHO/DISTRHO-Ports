#include "UIUtils.h"

#include "ParallelSplitterSubComponent.h"

ParallelSplitterSubComponent::ParallelSplitterSubComponent(SyndicateAudioProcessor& processor,
                                                           juce::Component* extensionComponent,
                                                           UIUtils::LinkedScrollView* graphView)
        : SplitterHeaderComponent(processor, graphView) {
    addChainBtn.reset(new juce::TextButton("Add Chain Button"));
    extensionComponent->addAndMakeVisible(addChainBtn.get());
    addChainBtn->setButtonText(TRANS("Chain"));
    addChainBtn->setTooltip("Add another parallel chain");
    addChainBtn->setLookAndFeel(&_buttonLookAndFeel);
    addChainBtn->setColour(UIUtils::StaticButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    addChainBtn->setColour(UIUtils::StaticButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    addChainBtn->setColour(UIUtils::StaticButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);
    addChainBtn->addListener(this);
}

ParallelSplitterSubComponent::~ParallelSplitterSubComponent() {
    addChainBtn->setLookAndFeel(nullptr);
    addChainBtn = nullptr;
}

void ParallelSplitterSubComponent::resized() {
    // Resize within the extension
    juce::Rectangle<int> extensionArea = addChainBtn->getParentComponent()->getLocalBounds();
    constexpr int MARGIN {4};
    extensionArea.reduce(MARGIN, MARGIN);
    extensionArea = extensionArea.withSizeKeepingCentre(extensionArea.getWidth(), extensionArea.getWidth());

    addChainBtn->setBounds(extensionArea);

    // Resize within the header component
    _viewPort->setBounds(getLocalBounds());
    SplitterHeaderComponent::resized();
}

void ParallelSplitterSubComponent::buttonClicked(juce::Button* buttonThatWasClicked) {
    if (buttonThatWasClicked == addChainBtn.get()) {
        _processor.addParallelChain();
    }
}

void ParallelSplitterSubComponent::onParameterUpdate() {
    if (_chainButtons.size() != ModelInterface::getNumChains(_processor.manager)) {
        _rebuildHeader();
    }

    SplitterHeaderComponent::onParameterUpdate();
}

void ParallelSplitterSubComponent::_rebuildHeader() {
    const size_t numChains {ModelInterface::getNumChains(_processor.manager)};

    _chainButtons.clear();

    for (size_t index {0}; index < numChains; index++) {
        if (numChains == 1) {
            // Don't provide a callback if there's only one chain - it can't be removed
            _chainButtons.emplace_back(std::make_unique<ChainButtonsComponent>(
                _processor,
                index,
                juce::String(index + 1)
            ));
        } else {
            _chainButtons.emplace_back(std::make_unique<ChainButtonsComponent>(
                _processor,
                index,
                juce::String(index + 1),
                [&, index]() { _processor.removeParallelChain(index); }
            ));
        }

        _viewPort->getViewedComponent()->addAndMakeVisible(_chainButtons[index].get());
    }

    // Layout the buttons
    SplitterHeaderComponent::resized();
}
