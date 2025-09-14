#pragma once

#include "MONSTRFilters/MONSTRParameters.h"
#include "UIUtils.h"

namespace UIUtils::Crossover {
    constexpr int SLIDER_THUMB_RADIUS {6};
    constexpr int SLIDER_THUMB_TARGET_WIDTH {SLIDER_THUMB_RADIUS * 4}; // * 4 as it must be minimum 2 * radius, then add a little extra

    constexpr int BAND_BUTTON_PADDING {4};

    // These are tuned experimentally to get the desired log curve and crossings close to 0,0 and 1,1.
    constexpr double LOG_SCALE {3.00043};
    constexpr double LOG_OFFSET_1 {0.001};
    constexpr double LOG_OFFSET_2 {3};

    /**
     * Maps a linear scaled value in the range 0:1 to a log scaled value in the same range.
     */
    inline double sliderValueToInternalLog(double sliderValue) {
        return std::pow(10, LOG_SCALE * sliderValue - LOG_OFFSET_2) - LOG_OFFSET_1;
    }

    /**
     * Maps a log scaled value in the range 0:1 to a linear scaled value in the same range.
     */
    inline double internalLogToSliderValue(double internalValue) {
        return (std::log10(internalValue + LOG_OFFSET_1) + LOG_OFFSET_2) / LOG_SCALE;
    }

    /**
     * Converts a crossover frequency value between in the internal parameter range to an x coordinate.
     */
    inline double sliderValueToXPos(double sliderValue, int componentWidth) {
        const double MARGIN_PX {componentWidth * 0.05};
        const double realRange {componentWidth - 2 * MARGIN_PX};

        const double normalisedSliderValue {WECore::MONSTR::Parameters::CROSSOVER_FREQUENCY.InternalToNormalised(sliderValue)};

        return (internalLogToSliderValue(normalisedSliderValue) * realRange) + MARGIN_PX;
    }

    /**
     * Converts an x coordinate to a crossover frequency parameter value in the internal parameter range.
     */
    inline double XPosToSliderValue(int XPos, int componentWidth) {
        const double MARGIN_PX {componentWidth * 0.05};
        const double realRange {componentWidth - 2 * MARGIN_PX};

        return WECore::MONSTR::Parameters::CROSSOVER_FREQUENCY.NormalisedToInternal(sliderValueToInternalLog(std::max(XPos - MARGIN_PX, 0.0) / realRange));
    }
}
