#include "SeriesSplitterSubComponent.h"

#include "UIUtils.h"

SeriesSplitterSubComponent::SeriesSplitterSubComponent(SyndicateAudioProcessor& processor,
                                                       UIUtils::LinkedScrollView* graphView)
        : SplitterHeaderComponent(processor, graphView) {
    auto chainButtons = std::make_unique<ChainButtonsComponent>(_processor, 0, "");
    _viewPort->getViewedComponent()->addAndMakeVisible(chainButtons.get());
    _chainButtons.push_back(std::move(chainButtons));
}
