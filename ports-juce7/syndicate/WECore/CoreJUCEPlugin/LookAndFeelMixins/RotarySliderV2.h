/*
 *	File:		RotarySliderV2.h
 *
 *	Version:	1.0.0
 *
 *	Created:	19/03/2019
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

namespace WECore::LookAndFeelMixins {

    /**
     * V2 (December 2018) style lookandfeel button mixin.
     *
     * Uses the following colours:
     * -# ** Slider::rotarySliderFillColourId **
     * -# ** Slider::rotarySliderOutlineColourId **
     */
    template <typename BASE>
    class RotarySliderV2 : public BASE {

    public:
        RotarySliderV2() = default;
        virtual ~RotarySliderV2() = default;

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
    void RotarySliderV2<BASE>::drawRotarySlider(juce::Graphics& g,
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

        const double sliderNormalisedValue {slider.valueToProportionOfLength(slider.getValue())};
        const double arcEndPoint {sliderNormalisedValue * rangeOfMotion + arcGap / 2};

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
                        arcGap / 2,
                        arcEndPoint,
                        true);
        g.strokePath(p, juce::PathStrokeType(3.0f));
    }
}
