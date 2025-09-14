#pragma once

#include <JuceHeader.h>

#include "CoreJUCEPlugin/LabelReadoutSlider.h"

class ModulationTargetSlider : public WECore::JUCEPlugin::LabelReadoutSlider<double>,
                               public juce::Timer {
public:
    ModulationTargetSlider(const juce::String& componentName, std::function<float()> getModulatedValue);
    ~ModulationTargetSlider() = default;

    void paint(juce::Graphics& g) override;

    void timerCallback() override;

private:
    std::function<float()> _getModulatedValue;
    float _modulatedValue;
};