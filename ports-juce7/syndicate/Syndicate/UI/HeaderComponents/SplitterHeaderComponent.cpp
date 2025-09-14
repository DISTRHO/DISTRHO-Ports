#include "SplitterHeaderComponent.h"
#include "UIUtils.h"

namespace {
    std::tuple<bool, int, bool> chainDetailsFromVariant(juce::var variant) {
        bool isValid {false};
        int chainNumber {0};
        bool isCopy {false};

        if (variant.isArray() && variant.size() == 2 && variant[0].isInt() && variant[1].isBool()) {
            isValid = true;
            chainNumber = variant[0];
            isCopy = variant[1];
        }

        return std::make_tuple(isValid, chainNumber, isCopy);
    }
}

SplitterHeaderComponent::SplitterHeaderComponent(SyndicateAudioProcessor& processor,
                                                 UIUtils::LinkedScrollView* graphView)
        : _processor(processor), _graphView(graphView) {
    _viewPort.reset(new UIUtils::LinkedScrollView());
    _viewPort->setViewedComponent(new juce::Component());
    _viewPort->setScrollBarsShown(false, false, false, true);
    _viewPort->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::backgroundColourId, juce::Colour(0x00000000));
    _viewPort->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::thumbColourId, UIUtils::neutralColour.withAlpha(0.5f));
    _viewPort->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::trackColourId, juce::Colour(0x00000000));
    addAndMakeVisible(_viewPort.get());
    _viewPort->setBounds(getLocalBounds());
    _viewPort->setOtherView(_graphView);
    _graphView->setOtherView(_viewPort.get());
}

SplitterHeaderComponent::~SplitterHeaderComponent() {
    _graphView->removeOtherView(_viewPort.get());
}

void SplitterHeaderComponent::refreshChainButtons() {
    for (auto& buttons : _chainButtons) {
        buttons->refresh();
    }
}

void SplitterHeaderComponent::resized() {
    _viewPort->setBounds(getLocalBounds());

    const size_t numChains {_chainButtons.size()};
    const int scrollPosition {_viewPort->getViewPositionX()};
    const int scrollableWidth {std::max(getWidth(), static_cast<int>(UIUtils::CHAIN_WIDTH * numChains))};
    const int scrollableHeight {getHeight()};
    _viewPort->getViewedComponent()->setBounds(juce::Rectangle<int>(scrollableWidth, scrollableHeight));

    // Preserve scroll position during resize
    _viewPort->setViewPosition(scrollPosition, 0);

    juce::Rectangle<int> scrollableArea = _viewPort->getViewedComponent()->getLocalBounds();
    if (scrollableWidth == getWidth()) {
        // If the scrollable area is the same as the width we need to remove from the left to
        // make the chains centred properly (otherwise we just left align them since the scrolling
        // will make it appear correct)
        scrollableArea.removeFromLeft(UIUtils::getChainXPos(0, numChains, getWidth()));
    }

    // Actually set the bounds of each set of buttons
    for (std::unique_ptr<ChainButtonsComponent>& chainButton : _chainButtons) {
        chainButton->setBounds(scrollableArea.removeFromLeft(UIUtils::CHAIN_WIDTH));
    }
}

void SplitterHeaderComponent::paint(juce::Graphics& g) {
    if (_shouldDrawDragHint) {
        g.setColour(UIUtils::neutralColour);
        const int hintXPos {
            _dragHintChainNumber < _chainButtons.size() ?
                _chainButtons[_dragHintChainNumber]->getX() - _viewPort->getViewPositionX() :
                _chainButtons[_chainButtons.size() - 1]->getX() - _viewPort->getViewPositionX() + UIUtils::CHAIN_WIDTH
        };
        g.drawLine(hintXPos, 0, hintXPos, getHeight(), 2.0f);
    }
}

bool SplitterHeaderComponent::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    auto [isValid, fromChainNumber, isCopy] = chainDetailsFromVariant(dragSourceDetails.description);

    // TODO only interested if chain has actually moved or isCopy

    return isValid;
}

void SplitterHeaderComponent::itemDragEnter(const SourceDetails& dragSourceDetails) {
    auto [isValid, fromChainNumber, isCopy] = chainDetailsFromVariant(dragSourceDetails.description);

    if (isValid) {
        _shouldDrawDragHint = true;
        _dragHintChainNumber = _dragCursorPositionToChainNumber(dragSourceDetails.localPosition);
        repaint();
    }
}

void SplitterHeaderComponent::itemDragMove(const SourceDetails& dragSourceDetails) {
auto [isValid, fromChainNumber, isCopy] = chainDetailsFromVariant(dragSourceDetails.description);

    if (isValid) {
        _dragHintChainNumber = _dragCursorPositionToChainNumber(dragSourceDetails.localPosition);
        repaint();
    }
}

void SplitterHeaderComponent::itemDragExit(const SourceDetails& dragSourceDetails) {
    _shouldDrawDragHint = false;
    repaint();
}

void SplitterHeaderComponent::itemDropped(const SourceDetails& dragSourceDetails) {
    _shouldDrawDragHint = false;
    repaint();

    // Actually move the slot
    auto [isValid, fromChainNumber, isCopy] = chainDetailsFromVariant(dragSourceDetails.description);

    if (isValid) {
        if (isCopy) {
            _processor.copyChain(fromChainNumber, _dragHintChainNumber);
        } else {
            _processor.moveChain(fromChainNumber, _dragHintChainNumber);
        }
    }
}

void SplitterHeaderComponent::onParameterUpdate() {
    for (auto& buttons : _chainButtons) {
        buttons->refresh();
    }
}

int SplitterHeaderComponent::_dragCursorPositionToChainNumber(juce::Point<int> cursorPosition) {
    const int numChains {static_cast<int>(_chainButtons.size())};

    // Check which component the drag is over
    const int scrollPosition {_viewPort->getViewPositionX()};
    for (size_t chainNumber {0}; chainNumber < numChains; chainNumber++) {
        if (cursorPosition.getX() < _chainButtons[chainNumber]->getRight() - scrollPosition) {
            return chainNumber;
        }
    }

    return numChains;
}

