#pragma once

#include <JuceHeader.h>

#include "PluginProcessor.h"
#include "UIUtils.h"

/**
 * Draws the calculated width for each band behind the crossover controls.
 */
class CrossoverImagerComponent : public UIUtils::SafeAnimatedComponent {
public:
    CrossoverImagerComponent(SyndicateAudioProcessor& processor);

    void paint(juce::Graphics& g) override;

private:
    SyndicateAudioProcessor& _processor;
};
