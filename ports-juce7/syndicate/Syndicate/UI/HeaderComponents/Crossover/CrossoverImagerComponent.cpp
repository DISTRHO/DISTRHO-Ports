#include "CrossoverImagerComponent.h"
#include "UIUtils.h"
#include "UIUtilsCrossover.h"
#include "ModelInterface.hpp"

namespace {
    int dBToYPos(float dBValue, int crossoverHeight) {
        constexpr float MIN_DB {-60};
        constexpr float MAX_DB {20};
        constexpr float range = MAX_DB - MIN_DB;

        return crossoverHeight - std::max(((std::min(dBValue, MAX_DB) - MIN_DB) / range) * crossoverHeight, 0.0f);
    }
}

CrossoverImagerComponent::CrossoverImagerComponent(SyndicateAudioProcessor& processor)
        : _processor(processor) {
    start();
}

void CrossoverImagerComponent::paint(juce::Graphics& g) {
    _stopEvent.reset();

    g.fillAll(UIUtils::backgroundColour);

    auto [fftBuffer, binWidth] = ModelInterface::getFFTOutputs(_processor.manager);

    // Draw a line to each point in the FFT
    juce::Path p;

    for (int index {0}; index < fftBuffer.size(); index++) {
        const float binCentreFreq {binWidth * index + binWidth / 2};
        const int XPos {static_cast<int>(UIUtils::Crossover::sliderValueToXPos(binCentreFreq, getWidth()))};

        const float leveldB {static_cast<float>(WECore::CoreMath::linearTodB(fftBuffer[index]))};
        const int YPos {dBToYPos(leveldB, getHeight())};

        if (index == 0) {
            p.startNewSubPath(XPos, YPos);
        } else {
            p.lineTo(XPos, YPos);
        }
    }

    g.setColour(UIUtils::highlightColour);
    g.strokePath(p, juce::PathStrokeType(0.5f));

    _stopEvent.signal();
}
