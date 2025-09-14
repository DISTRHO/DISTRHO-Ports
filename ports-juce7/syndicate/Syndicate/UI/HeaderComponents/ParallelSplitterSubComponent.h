#pragma once

#include <JuceHeader.h>
#include "SplitterHeaderComponent.h"
#include "UIUtils.h"

class ParallelSplitterSubComponent : public SplitterHeaderComponent,
                                     public juce::Button::Listener {
public:
    ParallelSplitterSubComponent(SyndicateAudioProcessor& processor,
                                 juce::Component* extensionComponent,
                                 UIUtils::LinkedScrollView* graphView);
    ~ParallelSplitterSubComponent() override;

    void onParameterUpdate() override;

    void resized() override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;

private:
    UIUtils::AddButtonLookAndFeel _buttonLookAndFeel;
    std::unique_ptr<juce::TextButton> addChainBtn;

    void _rebuildHeader();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParallelSplitterSubComponent)
};
