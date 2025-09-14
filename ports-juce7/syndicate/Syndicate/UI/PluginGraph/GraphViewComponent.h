#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginSlotComponent.h"
#include "ChainViewComponent.h"
#include "UIUtils.h"

class GraphViewComponent  : public juce::Component {
public:
    GraphViewComponent(SyndicateAudioProcessor& processor);
    ~GraphViewComponent();

    void resized() override;

    void onParameterUpdate();

    void closeGuestPluginWindows();

    UIUtils::LinkedScrollView* getViewport() { return _viewPort.get(); }

private:
    SyndicateAudioProcessor& _processor;
    std::vector<std::unique_ptr<ChainViewComponent>> _chainViews;
    PluginSelectionInterface _pluginSelectionInterface;
    PluginModulationInterface _pluginModulationInterface;
    std::unique_ptr<UIUtils::LinkedScrollView> _viewPort;
    bool _hasRestoredScroll;
    std::recursive_mutex _graphMutex;
};
