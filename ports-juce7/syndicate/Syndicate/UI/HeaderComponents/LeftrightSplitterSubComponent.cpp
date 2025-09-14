#include "UIUtils.h"

#include "LeftrightSplitterSubComponent.h"

LeftrightSplitterSubComponent::LeftrightSplitterSubComponent(SyndicateAudioProcessor& processor,
                                                             UIUtils::LinkedScrollView* graphView)
        : SplitterHeaderComponent(processor, graphView) {
    auto leftChainbuttons = std::make_unique<ChainButtonsComponent>(_processor, 0, "Left");
    _viewPort->getViewedComponent()->addAndMakeVisible(leftChainbuttons.get());
    _chainButtons.push_back(std::move(leftChainbuttons));

    auto rightChainbuttons = std::make_unique<ChainButtonsComponent>(_processor, 1, "Right");
    _viewPort->getViewedComponent()->addAndMakeVisible(rightChainbuttons.get());
    _chainButtons.push_back(std::move(rightChainbuttons));
}
