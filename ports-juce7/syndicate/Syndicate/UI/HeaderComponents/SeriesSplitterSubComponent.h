#pragma once

#include <JuceHeader.h>
#include "ChainButtonsComponent.h"
#include "SplitterHeaderComponent.h"

class SeriesSplitterSubComponent : public SplitterHeaderComponent {
public:
    SeriesSplitterSubComponent(SyndicateAudioProcessor& processor,
                               UIUtils::LinkedScrollView* graphView);
    ~SeriesSplitterSubComponent() = default;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeriesSplitterSubComponent)
};
