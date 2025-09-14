#include "CrossoverMouseListener.h"

#include <algorithm>

#include "UIUtilsCrossover.h"
#include "WEFilters/StereoWidthProcessorParameters.h"

namespace {
    std::function<void(const juce::MouseEvent&)> createDragCallback(SyndicateAudioProcessor& processor, int crossoverNumber) {
        return [&processor, crossoverNumber](const juce::MouseEvent& event) {
            const int currentXPos {event.getPosition().getX()};
            const int componentWidth {event.eventComponent->getWidth()};

            processor.setCrossoverFrequency(crossoverNumber, UIUtils::Crossover::XPosToSliderValue(currentXPos, componentWidth));

            // Check if this crossover handle is getting too close to another and move it if
            // needed
            const size_t numCrossovers {ModelInterface::getNumChains(processor.manager) - 1};
            for (int otherCrossoverNumber {0}; otherCrossoverNumber < numCrossovers; otherCrossoverNumber++) {
                const double otherXPos {
                    UIUtils::Crossover::sliderValueToXPos(ModelInterface::getCrossoverFrequency(processor.manager, otherCrossoverNumber), componentWidth)
                };

                constexpr double MIN_SPACING {4 * UIUtils::Crossover::SLIDER_THUMB_RADIUS};
                const double requiredGap {MIN_SPACING * std::abs(crossoverNumber - otherCrossoverNumber)};
                const double actualGap {std::abs(currentXPos - otherXPos)};

                if (otherCrossoverNumber < crossoverNumber && actualGap < requiredGap) {
                    processor.setCrossoverFrequency(otherCrossoverNumber, UIUtils::Crossover::XPosToSliderValue(currentXPos - requiredGap, componentWidth));
                } else if (otherCrossoverNumber > crossoverNumber && actualGap < requiredGap) {
                    processor.setCrossoverFrequency(otherCrossoverNumber, UIUtils::Crossover::XPosToSliderValue(currentXPos + requiredGap, componentWidth));
                }
            }
        };
    }
}

CrossoverMouseListener::CrossoverMouseListener(SyndicateAudioProcessor& processor)
        : _processor(processor) {
}

CrossoverMouseListener::~CrossoverMouseListener() {
}

void CrossoverMouseListener::mouseDown(const juce::MouseEvent& event) {
    _resolveParameterInteraction(event);
}

void CrossoverMouseListener::mouseDrag(const juce::MouseEvent& event) {
    if (_dragCallback.has_value()) {
        _dragCallback.value()(event);
    }
}

void CrossoverMouseListener::mouseUp(const juce::MouseEvent& /*event*/) {
    if (_dragCallback.has_value()) {
        _dragCallback.reset();
    }
}

void CrossoverMouseListener::_resolveParameterInteraction(const juce::MouseEvent& event) {
    const int mouseDownX {event.getMouseDownX()};

    // For each available band, check if the cursor landed on a crossover frequency handle or on
    // the gaps in between
    const size_t numBands {ModelInterface::getNumChains(_processor.manager)};

    for (size_t bandIndex {0}; bandIndex < numBands; bandIndex++) {
        const double crossoverXPos {bandIndex < numBands - 1 ?
            UIUtils::Crossover::sliderValueToXPos(ModelInterface::getCrossoverFrequency(_processor.manager, bandIndex), event.eventComponent->getWidth()) :
            event.eventComponent->getWidth()
        };

        if (mouseDownX < crossoverXPos - UIUtils::Crossover::SLIDER_THUMB_TARGET_WIDTH) {
            // Click landed below a crossover handle
            break;

        } else if (mouseDownX < crossoverXPos + UIUtils::Crossover::SLIDER_THUMB_TARGET_WIDTH) {
            // Click landed on a crossover handle
            _dragCallback = createDragCallback(_processor, bandIndex);
            break;
        }
    }
}
