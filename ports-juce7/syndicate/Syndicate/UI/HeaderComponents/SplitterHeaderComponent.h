#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ChainButtonsComponent.h"

class SplitterHeaderComponent : public juce::Component,
                                public juce::DragAndDropTarget {
public:
    explicit SplitterHeaderComponent(SyndicateAudioProcessor& processor, UIUtils::LinkedScrollView* graphView);
    virtual ~SplitterHeaderComponent();

    virtual void refreshChainButtons();

    virtual void resized() override;

    void paint(juce::Graphics& g) override;

    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragMove(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

    virtual void onParameterUpdate();

protected:
    std::vector<std::unique_ptr<ChainButtonsComponent>> _chainButtons;
    std::unique_ptr<UIUtils::LinkedScrollView> _viewPort;
    SyndicateAudioProcessor& _processor;
    UIUtils::LinkedScrollView* _graphView;

private:
    bool _shouldDrawDragHint {false};
    int _dragHintChainNumber {0};

    int _dragCursorPositionToChainNumber(juce::Point<int> cursorPosition);
};
