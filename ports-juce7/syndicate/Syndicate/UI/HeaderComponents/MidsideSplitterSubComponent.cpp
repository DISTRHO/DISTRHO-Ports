#include "UIUtils.h"

#include "MidsideSplitterSubComponent.h"

MidsideSplitterSubComponent::MidsideSplitterSubComponent(SyndicateAudioProcessor& processor,
                                                         UIUtils::LinkedScrollView* graphView)
        : SplitterHeaderComponent(processor, graphView) {
    auto midChainbuttons = std::make_unique<ChainButtonsComponent>(_processor, 0, "Mid");
    _viewPort->getViewedComponent()->addAndMakeVisible(midChainbuttons.get());
    _chainButtons.push_back(std::move(midChainbuttons));

    auto sideChainbuttons = std::make_unique<ChainButtonsComponent>(_processor, 1, "Side");
    _viewPort->getViewedComponent()->addAndMakeVisible(sideChainbuttons.get());
    _chainButtons.push_back(std::move(sideChainbuttons));
}
