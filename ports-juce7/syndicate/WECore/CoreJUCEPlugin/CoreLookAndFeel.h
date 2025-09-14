/*
 *	File:		CoreLookAndFeel.h
 *
 *	Version:	2.0.0
 *
 *	Created:	17/09/2015
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

namespace WECore::JUCEPlugin {

    /**
     * A class which contains most of the basic design elements which the white elephant audio plugins
     * have in common.
     *
     * Not all drawing methods are defined, and so methods inherited from LookAndFeel_V2 may be used.
     *
     * By default the three colours which are used are dark grey, light grey, and neon blue. These can
     * be changed using the provided setter methods.
     *
     * The colour members (highlightColour, etc) are being phased out as it doesn't fit well with
     * how colours are managed in JUCE.
     */
    class CoreLookAndFeel : public juce::LookAndFeel_V2 {
    public:
        CoreLookAndFeel() {
            setHighlightColour(juce::Colour(34, 252, 255));
            setLightColour(juce::Colour(200, 200, 200));
            setDarkColour(juce::Colour(107, 107, 107));
        }

        virtual ~CoreLookAndFeel() = default;

        CoreLookAndFeel operator=(CoreLookAndFeel&) = delete;
        CoreLookAndFeel(CoreLookAndFeel&) = delete;

        virtual inline void drawLinearSliderThumb(juce::Graphics& g,
                                                  int x,
                                                  int y,
                                                  int width,
                                                  int height,
                                                  float sliderPos,
                                                  float /*minSliderPos*/,
                                                  float /*maxSliderPos*/,
                                                  const juce::Slider::SliderStyle style,
                                                  juce::Slider& slider) override;

        virtual inline void drawButtonBackground(juce::Graphics& g,
                                                 juce::Button& button,
                                                 const juce::Colour& /*backgroundColour*/,
                                                 bool /*isMouseOverButton*/,
                                                 bool /*isButtonDown*/) override;

        virtual inline void drawButtonText(juce::Graphics& g,
                                           juce::TextButton& textButton,
                                           bool /*isMouseOverButton*/,
                                           bool /*isButtonDown*/) override;

        virtual inline void drawComboBox(juce::Graphics& g,
                                         int /*width*/,
                                         int /*height*/,
                                         const bool /*isButtonDown*/,
                                         int buttonX,
                                         int buttonY,
                                         int buttonW,
                                         int buttonH,
                                         juce::ComboBox& box) override;

        virtual inline void drawLinearSlider(juce::Graphics& g,
                                             int x,
                                             int y,
                                             int width,
                                             int height,
                                             float sliderPos,
                                             float minSliderPos,
                                             float maxSliderPos,
                                             const juce::Slider::SliderStyle style,
                                             juce::Slider& slider) override;

        virtual inline void drawLinearSliderBackground(juce::Graphics& g,
                                                       int x,
                                                       int y,
                                                       int width,
                                                       int height,
                                                       float /*sliderPos*/,
                                                       float /*minSliderPos*/,
                                                       float /*maxSliderPos*/,
                                                       const juce::Slider::SliderStyle /*style*/,
                                                       juce::Slider& slider) override;

        virtual inline void drawTooltip(juce::Graphics& g,
                                        const juce::String& text,
                                        int width,
                                        int height) override;

        virtual void setHighlightColour(juce::Colour newColour) {
            setColour(juce::ComboBox::arrowColourId, newColour);
            setColour(juce::GroupComponent::textColourId, newColour);
            setColour(juce::Slider::rotarySliderFillColourId, newColour);
            setColour(juce::Slider::thumbColourId, newColour);
            setColour(juce::Slider::trackColourId, newColour);
            setColour(juce::TextButton::buttonOnColourId, newColour);
            setColour(juce::TextButton::textColourOnId, newColour);

            highlightColour = newColour;
        }

        virtual void setLightColour(juce::Colour newColour) {
            setColour(juce::PopupMenu::backgroundColourId, newColour);
            setColour(juce::Slider::backgroundColourId, newColour);
            setColour(juce::Slider::rotarySliderOutlineColourId, newColour);
            setColour(juce::TextButton::buttonColourId, newColour);
            setColour(juce::TextButton::textColourOffId, newColour);

            lightColour = newColour;
        }

        virtual void setDarkColour(juce::Colour newColour) {
            setColour(juce::PopupMenu::highlightedBackgroundColourId, newColour);

            darkColour = newColour;
        }

    protected:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoreLookAndFeel)

        juce::Colour lightColour,
                     darkColour,
                     highlightColour;
    };

    void CoreLookAndFeel::drawLinearSliderThumb(juce::Graphics& g,
                                                int x,
                                                int y,
                                                int width,
                                                int height,
                                                float sliderPos,
                                                float /*minSliderPos*/,
                                                float /*maxSliderPos*/,
                                                const juce::Slider::SliderStyle style,
                                                juce::Slider& slider) {

        const float sliderRadius = static_cast<float>(getSliderThumbRadius(slider) - 2);

        juce::Colour* ring;

        if (slider.isEnabled()) {
            ring = &highlightColour;
        } else {
            ring = &lightColour;
        }

        if (style == juce::Slider::LinearHorizontal || style == juce::Slider::LinearVertical)
        {
            float kx, ky;

            if (style == juce::Slider::LinearVertical)
            {
                kx = x + width * 0.5f;
                ky = sliderPos;
            }
            else
            {
                kx = sliderPos;
                ky = y + height * 0.5f;
            }

            juce::Path p;
            p.addEllipse(kx - sliderRadius, ky - sliderRadius, sliderRadius * 2, sliderRadius * 2);

            g.setColour(darkColour);
            g.fillPath(p);

            g.setColour(*ring);
            g.strokePath(p, juce::PathStrokeType(2.0f));
        }

    }

    void CoreLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                               juce::Button& button,
                                               const juce::Colour& /*backgroundColour*/,
                                               bool /*isMouseOverButton*/,
                                               bool /*isButtonDown*/) {
        const int width {button.getWidth()};
        const int height {button.getHeight()};

        const float indent {2.0f};
        const int cornerSize {juce::jmin(juce::roundToInt(width * 0.4f),
                                         juce::roundToInt(height * 0.4f))};

        juce::Path p;
        juce::PathStrokeType pStroke(1);
        juce::Colour* bc {nullptr};




        if (button.isEnabled()) {
            if (button.getToggleState()) {
                bc = &highlightColour;
            } else {
                bc = &lightColour;
            }
        } else {
            bc = &darkColour;
        }

        p.addRoundedRectangle(indent, indent, width - 2 * indent, height - 2 * indent, static_cast<float>(cornerSize));


        g.setColour(*bc);
        g.strokePath(p, pStroke);
    }

    void CoreLookAndFeel::drawButtonText(juce::Graphics& g,
                                         juce::TextButton& textButton,
                                         bool /*isMouseOverButton*/,
                                         bool /*isButtonDown*/) {

        juce::Colour* textColour {nullptr};

        if (textButton.isEnabled()) {
            if (textButton.getToggleState() || textButton.getWidth() < 24) {
                textColour = &highlightColour;
            } else {
                textColour = &lightColour;
            }
        } else {
            textColour = &darkColour;
        }

        g.setColour(*textColour);
        int margin {0};

        // differentiates between the small button on the tempo sync ratio and larger buttons
        if (textButton.getWidth() > 24) {
            margin = 5;
        }

        g.drawFittedText(textButton.getButtonText(), margin, 0, textButton.getWidth() - 2 * margin, textButton.getHeight(), juce::Justification::centred, 0);
    }

    void CoreLookAndFeel::drawComboBox(juce::Graphics& g,
                                       int /*width*/,
                                       int /*height*/,
                                       const bool /*isButtonDown*/,
                                       int buttonX,
                                       int buttonY,
                                       int buttonW,
                                       int buttonH,
                                       juce::ComboBox& box) {

        g.fillAll(lightColour);
        g.setColour(darkColour);
        g.fillRect(buttonX, buttonY, buttonW, buttonH);

        const float arrowX {0.2f};
        const float arrowH {0.3f};

        if (box.isEnabled()) {
            juce::Path p;
            p.addTriangle(buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
                          buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                          buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);

            p.addTriangle(buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.55f + arrowH),
                          buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
                          buttonX + buttonW * arrowX,          buttonY + buttonH * 0.55f);

            g.setColour(box.isPopupActive() ? highlightColour : lightColour);

            g.fillPath(p);
        }
    }

    void CoreLookAndFeel::drawLinearSlider(juce::Graphics& g,
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

    void CoreLookAndFeel::drawLinearSliderBackground(juce::Graphics& g,
                                                     int x,
                                                     int y,
                                                     int width,
                                                     int height,
                                                     float /*sliderPos*/,
                                                     float /*minSliderPos*/,
                                                     float /*maxSliderPos*/,
                                                     const juce::Slider::SliderStyle /*style*/,
                                                     juce::Slider& slider) {
        g.setColour(lightColour);

        if (slider.isHorizontal()) {
            g.fillRect(x, y + height / 2, width, 2);
        }
    }

    void CoreLookAndFeel::drawTooltip(juce::Graphics& g,
                                      const juce::String& text,
                                      int width,
                                      int height) {
        g.setColour(lightColour);
        g.fillRect(0, 0, width, height);

        g.setColour(darkColour);
        g.drawFittedText(text, 0, 0, width, height, juce::Justification::centred, 3);
    }
}
