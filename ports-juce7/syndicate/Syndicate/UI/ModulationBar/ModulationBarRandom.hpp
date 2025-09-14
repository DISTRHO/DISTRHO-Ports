#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ModulatableParameter.hpp"
#include "UIUtils.h"

class ModulationBarRandom : public juce::Component {
public:
    ModulationBarRandom(SyndicateAudioProcessor& processor, int randomIndex);
    ~ModulationBarRandom() override;

    void resized() override;

private:
    SyndicateAudioProcessor& _processor;
    int _randomIndex;

    UIUtils::StandardSliderLookAndFeel _sliderLookAndFeel;

    std::unique_ptr<ModulatableParameter> freqSlider;
    std::unique_ptr<ModulatableParameter> depthSlider;
    std::unique_ptr<UIUtils::WaveStylusViewer> _sourceView;
    std::unique_ptr<UIUtils::UniBiModeButtons> outputModeButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationBarRandom)
};
