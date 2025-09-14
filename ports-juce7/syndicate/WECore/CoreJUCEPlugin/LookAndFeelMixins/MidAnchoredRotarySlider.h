/*
 *	File:		MidAnchoredRotarySlider.h
 *
 *	Version:	1.0.0
 *
 *	Created:	21/11/2020
 *
 *	This file is part of WECore.
 *
 *  WECore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WECore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WECore.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "JuceHeader.h"
#include "General/CoreMath.h"

namespace WECore::LookAndFeelMixins {

    /**
     * Based on RotarySliderV2, but fills the right side of the slider when above half of the
     * slider's value, and fills the left half when below.
     *
     * Uses the following colours:
     * -# ** Slider::rotarySliderFillColourId **
     * -# ** Slider::rotarySliderOutlineColourId **
     */
    template <typename BASE>
    class MidAnchoredRotarySlider : public BASE {

    public:
        MidAnchoredRotarySlider() = default;
        virtual ~MidAnchoredRotarySlider() = default;

        /** @{ LookAndFeel overrides */
        inline virtual void drawRotarySlider(juce::Graphics& g,
                                             int x,
                                             int y,
                                             int width,
                                             int height,
                                             float sliderPosProportional,
                                             float rotaryStartAngle,
                                             float rotaryEndAngle,
                                             juce::Slider &slider) override;
        /** @} */
    };

    template <typename BASE>
    void MidAnchoredRotarySlider<BASE>::drawRotarySlider(juce::Graphics& g,
                                                         int /*x*/,
                                                         int /*y*/,
                                                         int width,
                                                         int height,
                                                         float /*sliderPosProportional*/,
                                                         float /*rotaryStartAngle*/,
                                                         float /*rotaryEndAngle*/,
                                                         juce::Slider &slider) {

        // Calculate useful constants
        constexpr double arcGap {CoreMath::DOUBLE_TAU / 4};
        constexpr double rangeOfMotion {CoreMath::DOUBLE_TAU - arcGap};

        const double sliderNormalisedValue {(slider.getValue() - slider.getMinimum()) /
                                            (slider.getMaximum() - slider.getMinimum())};

        double arcStartPoint {0};
        double arcEndPoint {0};
        if (sliderNormalisedValue > 0.5) {
            arcStartPoint = CoreMath::DOUBLE_PI;
            arcEndPoint = CoreMath::DOUBLE_PI + (sliderNormalisedValue - 0.5) * rangeOfMotion;
        } else {
            arcStartPoint = CoreMath::DOUBLE_PI + (sliderNormalisedValue - 0.5) * rangeOfMotion;
            arcEndPoint = CoreMath::DOUBLE_PI;
        }

        constexpr int margin {2};
        juce::Rectangle<int> area = slider.getBounds();
        area.reduce(margin, margin);
        const int diameter {std::min(area.getWidth(), area.getHeight())};

        if (slider.isEnabled()) {
            g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
        } else {
            g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        }

        juce::Path p;

        // Draw inner ring
        constexpr int arcSpacing {3};
        p.addCentredArc(width / 2,
                        height / 2,
                        diameter / 2 - arcSpacing,
                        diameter / 2 - arcSpacing,
                        CoreMath::DOUBLE_PI,
                        arcGap / 2,
                        CoreMath::DOUBLE_TAU - (arcGap / 2),
                        true);

        g.strokePath(p, juce::PathStrokeType(0.7f));

        // Draw outer ring
        p.clear();
        p.addCentredArc(width / 2,
                        height / 2,
                        diameter / 2,
                        diameter / 2,
                        CoreMath::DOUBLE_PI,
                        arcStartPoint,
                        arcEndPoint,
                        true);
        g.strokePath(p, juce::PathStrokeType(3.0f));
    }
}
