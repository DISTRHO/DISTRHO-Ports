/*
 *	File:		LinearSliderV2.h
 *
 *	Version:	1.0.0
 *
 *	Created:	04/07/2021
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
     * -# ** Slider::backgroundColourId **
     * -# ** Slider::thumbColourId **
     * -# ** Slider::trackColourId **
     */
    template <typename BASE>
    class LinearSliderV2 : public BASE {
    public:
        LinearSliderV2() = default;
        virtual ~LinearSliderV2() = default;

        /** @{ LookAndFeel overrides */
       inline virtual void drawLinearSlider(juce::Graphics& g,
                                            int x,
                                            int y,
                                            int width,
                                            int height,
                                            float sliderPos,
                                            float minSliderPos,
                                            float maxSliderPos,
                                            const juce::Slider::SliderStyle style,
                                            juce::Slider& slider) override;

        inline virtual void drawLinearSliderThumb(juce::Graphics& g,
                                                  int x,
                                                  int y,
                                                  int width,
                                                  int height,
                                                  float sliderPos,
                                                  float minSliderPos,
                                                  float maxSliderPos,
                                                  const juce::Slider::SliderStyle style,
                                                  juce::Slider& slider) override;

        inline virtual void drawLinearSliderBackground(juce::Graphics& g,
                                                       int x,
                                                       int y,
                                                       int width,
                                                       int height,
                                                       float sliderPos,
                                                       float minSliderPos,
                                                       float maxSliderPos,
                                                       const juce::Slider::SliderStyle style,
                                                       juce::Slider& slider) override;
        /** @} */
    };

    template <typename BASE>
    void LinearSliderV2<BASE>::drawLinearSlider(juce::Graphics& g,
                                                int x,
                                                int y,
                                                int width,
                                                int height,
                                                float sliderPos,
                                                float minSliderPos,
                                                float maxSliderPos,
                                                const juce::Slider::SliderStyle style,
                                                juce::Slider& slider) {
        // Draw background first
        drawLinearSliderBackground(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        drawLinearSliderThumb(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }

    template <typename BASE>
    void LinearSliderV2<BASE>::drawLinearSliderThumb(juce::Graphics& g,
                                                     int x,
                                                     int y,
                                                     int width,
                                                     int height,
                                                     float sliderPos,
                                                     float minSliderPos,
                                                     float /*maxSliderPos*/,
                                                     const juce::Slider::SliderStyle style,
                                                     juce::Slider& slider) {

        constexpr float MARGIN {2};

        if (slider.isEnabled()) {
            g.setColour(slider.findColour(juce::Slider::thumbColourId));
        } else {
            g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        }

        if (style == juce::Slider::LinearHorizontal) {
            // Horizontal rectangle
            g.fillRect(juce::Rectangle<float>(minSliderPos,
                                              y + MARGIN,
                                              sliderPos - minSliderPos,
                                              height - 3 * MARGIN));
        } else {
            // Vertical rectangle
            g.fillRect(juce::Rectangle<float>(x + MARGIN,
                                              y + height,
                                              width - 3 * MARGIN,
                                              -(y + height - sliderPos)));
        }
    }

    template <typename BASE>
    void LinearSliderV2<BASE>::drawLinearSliderBackground(juce::Graphics& g,
                                                          int x,
                                                          int y,
                                                          int width,
                                                          int height,
                                                          float /*sliderPos*/,
                                                          float /*minSliderPos*/,
                                                          float /*maxSliderPos*/,
                                                          const juce::Slider::SliderStyle style,
                                                          juce::Slider& slider) {

        constexpr int MARGIN {2};
        constexpr int LINE_WIDTH {1};

        if (slider.isEnabled()) {
            g.setColour(slider.findColour(juce::Slider::trackColourId));
        } else {
            g.setColour(slider.findColour(juce::Slider::backgroundColourId));
        }

        if (style == juce::Slider::LinearHorizontal) {
            // Horizontal line
            g.fillRect(juce::Rectangle<float>(x,
                                              y + height - MARGIN - (LINE_WIDTH / 2),
                                              width,
                                              LINE_WIDTH));
        } else {
            // Vertical line
            g.fillRect(juce::Rectangle<float>(x + width - MARGIN - (LINE_WIDTH / 2),
                                              y,
                                              LINE_WIDTH,
                                              height));

        }
    }
}
