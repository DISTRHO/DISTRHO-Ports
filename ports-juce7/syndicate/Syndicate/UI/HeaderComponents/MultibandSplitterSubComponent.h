#pragma once

#include <JuceHeader.h>
#include "SplitterHeaderComponent.h"
#include "CrossoverWrapperComponent.h"
#include "ChainButtonsComponent.h"

class MultibandSplitterSubComponent : public SplitterHeaderComponent,
                                      public juce::Button::Listener {
public:
    MultibandSplitterSubComponent(SyndicateAudioProcessor& processor,
                                  juce::Component* extensionComponent,
                                  UIUtils::LinkedScrollView* graphView);
    ~MultibandSplitterSubComponent() override;

    void onParameterUpdate() override;

    void resized() override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;

    void refreshChainButtons() override;

private:
    UIUtils::AddButtonLookAndFeel _buttonLookAndFeel;

    std::unique_ptr<juce::TextButton> addBandBtn;
    std::unique_ptr<CrossoverWrapperComponent> crossoverComponent;

    void _rebuildHeader();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultibandSplitterSubComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

