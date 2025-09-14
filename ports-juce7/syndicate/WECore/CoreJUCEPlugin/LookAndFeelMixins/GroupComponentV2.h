/*
 *	File:		GroupComponentV2.h
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
     * V2 (December 2018) style lookandfeel group mixin.
     *
     * Uses the following colours:
     * -# ** GroupComponent::textColourId **
     */
    template <typename BASE>
    class GroupComponentV2 : public BASE {

    public:
        GroupComponentV2() : _groupFontName("Courier New") {}
        virtual ~GroupComponentV2() = default;

        /** @{ LookAndFeel overrides */
        inline virtual void drawGroupComponentOutline(juce::Graphics& g,
                                                      int width,
                                                      int height,
                                                      const juce::String& text,
                                                      const juce::Justification& justification,
                                                      juce::GroupComponent& group) override;

        /** @} */

        void setGroupFontName(const char* fontName) { _groupFontName = fontName; }

    private:
        const char* _groupFontName;
    };

    template <typename BASE>
    void GroupComponentV2<BASE>::drawGroupComponentOutline(juce::Graphics& g,
                                                           int width,
                                                           int height,
                                                           const juce::String& text,
                                                           const juce::Justification& /*justification*/,
                                                           juce::GroupComponent& group) {

        constexpr int MARGIN {2};

        g.setColour(group.findColour(juce::GroupComponent::textColourId));

        juce::Font font;
        font.setTypefaceName(_groupFontName);
        g.setFont(font);

        g.drawText(text,
                MARGIN,
                MARGIN,
                width,
                height,
                juce::Justification::topLeft,
                true);
    }

}
