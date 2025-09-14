#pragma once

#include <JuceHeader.h>
#include "SplitterHeaderComponent.h"
#include "ChainButtonsComponent.h"

class MidsideSplitterSubComponent : public SplitterHeaderComponent {
public:
    MidsideSplitterSubComponent(SyndicateAudioProcessor& processor,
                                UIUtils::LinkedScrollView* graphView);
    ~MidsideSplitterSubComponent() = default;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidsideSplitterSubComponent)
};
