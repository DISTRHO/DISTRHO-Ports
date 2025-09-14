/*
 *	File:		ButtonV2.h
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
     * -# ** TextButton::buttonOnColourId **
     * -# ** TextButton::buttonColourId **
     * -# ** TextButton::textColourOnId **
     * -# ** TextButton::textColourOffId **
     */
    template <typename BASE>
    class ButtonV2 : public BASE {

    public:
        ButtonV2() = default;
        virtual ~ButtonV2() = default;

        /** @{ LookAndFeel overrides */
        inline virtual void drawButtonBackground(juce::Graphics& g,
                                                 juce::Button& button,
                                                 const juce::Colour& backgroundColour,
                                                 bool isMouseOverButton,
                                                 bool isButtonDown) override;

        inline virtual void drawButtonText(juce::Graphics& g,
                                           juce::TextButton& textButton,
                                           bool isMouseOverButton,
                                           bool isButtonDown) override;
        /** @} */

    private:
        static constexpr float _disabledDarker {0.7f};
    };

    template <typename BASE>
    void ButtonV2<BASE>::drawButtonBackground(juce::Graphics& g,
                                              juce::Button& button,
                                              const juce::Colour& /*backgroundColour*/,
                                              bool /*isMouseOverButton*/,
                                              bool /*isButtonDown*/) {
        const int width {button.getWidth()};
        const int height {button.getHeight()};

        constexpr float indent {2.0f};
        const int cornerSize {juce::jmin(juce::roundToInt(width * 0.4f),
                                         juce::roundToInt(height * 0.4f))};

        juce::Path p;
        juce::PathStrokeType pStroke(1);

        if (button.isEnabled()) {
            if (button.getToggleState()) {
                g.setColour(button.findColour(juce::TextButton::buttonOnColourId));
            } else {
                g.setColour(button.findColour(juce::TextButton::buttonColourId));
            }
        } else {
            g.setColour(button.findColour(juce::TextButton::buttonColourId).darker(_disabledDarker));
        }

        p.addRoundedRectangle(indent,
                              indent,
                              width - 2 * indent,
                              height - 2 * indent,
                              static_cast<float>(cornerSize));

        g.strokePath(p, pStroke);
    }

    template <typename BASE>
    void ButtonV2<BASE>::drawButtonText(juce::Graphics& g,
                                        juce::TextButton& textButton,
                                        bool /*isMouseOverButton*/,
                                        bool /*isButtonDown*/) {
        if (textButton.isEnabled()) {
            if (textButton.getToggleState()) {
                g.setColour(textButton.findColour(juce::TextButton::textColourOnId));
            } else {
                g.setColour(textButton.findColour(juce::TextButton::textColourOffId));
            }
        } else {
            g.setColour(textButton.findColour(juce::TextButton::textColourOffId).darker(_disabledDarker));
        }

        constexpr int MARGIN {0};

        juce::Font font;
        font.setTypefaceName(BASE::getTypefaceForFont(font)->getName());
        g.setFont(font);

        g.drawFittedText(textButton.getButtonText(),
                        MARGIN,
                        0,
                        textButton.getWidth() - 2 * MARGIN,
                        textButton.getHeight(),
                        juce::Justification::centred,
                        0);
    }
}
