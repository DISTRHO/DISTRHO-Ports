#include "ModulationTargetSlider.hpp"

#include "General/CoreMath.h"


ModulationTargetSlider::ModulationTargetSlider(const juce::String& componentName,
                                               std::function<float()> getModulatedValue) :
        WECore::JUCEPlugin::LabelReadoutSlider<double>(componentName),
        _getModulatedValue(getModulatedValue),
        _modulatedValue(0) {
    startTimerHz(15);
}

void ModulationTargetSlider::paint(juce::Graphics& g) {
    // Draw the normal slider using LookAndFeel
    juce::Slider::paint(g);

    // Draw the inner modulation ring
    g.setColour(juce::Colours::grey);

    constexpr double arcGap {WECore::CoreMath::DOUBLE_TAU / 4};
    constexpr double rangeOfMotion {WECore::CoreMath::DOUBLE_TAU - arcGap};

    const double sliderNormalisedValue {(getValue() - getMinimum()) /
                                        (getMaximum() - getMinimum())};
    const double arcStartPoint {sliderNormalisedValue * rangeOfMotion + arcGap / 2};
    const double arcEndPoint {_modulatedValue * rangeOfMotion + arcGap / 2};

    constexpr double margin {1.5};
    const double diameter {static_cast<float>(getHeight() - margin * 2)};

    constexpr int arcSpacing {6};
    juce::Path p;
    p.addCentredArc(getWidth() / 2,
                    getHeight() / 2,
                    diameter / 2 - arcSpacing,
                    diameter / 2 - arcSpacing,
                    WECore::CoreMath::DOUBLE_PI,
                    arcStartPoint,
                    arcEndPoint,
                    true);

    g.strokePath(p, juce::PathStrokeType(2.0f));
}

void ModulationTargetSlider::timerCallback() {
    // _modulatedValue should be between 0 and 1
    _modulatedValue = _getModulatedValue() / getRange().getLength();
    repaint();
}
