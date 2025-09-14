/*
 *	File:		TooltipLabelUpdater.h
 *
 *	Version:	1.0.0
 *
 *	Created:	06/06/2021
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

namespace WECore::JUCEPlugin {

    /**
     * Acts as a MouseListener for multiple components, setting the given Label to display their
     * tooltip.
     */
    class TooltipLabelUpdater : public juce::MouseListener {
    public:
        inline TooltipLabelUpdater();
        ~TooltipLabelUpdater() = default;

        /**
         * Starts updating the label as necessary displaying an empty string when not showing a
         * tooltip.
         */
        inline void start(juce::Label* targetLabel);

        /**
         * Starts updating the label as necessary, displaying build information when not showing a
         * tooltip.
         */
        inline void start(juce::Label* targetLabel, juce::AudioProcessor::WrapperType pluginFormat, bool isDemo = false);

        /**
         * Must be called before the given label is destructed.
         */
        void stop() { _targetLabel = nullptr; }

        inline virtual void mouseEnter(const juce::MouseEvent& event) override;
        inline virtual void mouseExit(const juce::MouseEvent& event) override;

        inline void refreshTooltip(juce::Component* component);

    private:
        juce::Label* _targetLabel;
        juce::String _defaultString;
    };

    TooltipLabelUpdater::TooltipLabelUpdater() : _targetLabel(nullptr) {
    }

    void TooltipLabelUpdater::start(juce::Label* targetLabel) {
        _targetLabel = targetLabel;
        _defaultString = "";
    }

    void TooltipLabelUpdater::start(juce::Label* targetLabel, juce::AudioProcessor::WrapperType pluginFormat, bool isDemo) {
        _targetLabel = targetLabel;

        _defaultString = JucePlugin_Name;
        _defaultString += " ";
        _defaultString += JucePlugin_VersionString;

        // Format
        _defaultString += " ";
        _defaultString += juce::AudioProcessor::getWrapperTypeDescription(pluginFormat);

        // OS
        _defaultString += " ";
#if _WIN32
        _defaultString += "Win";
#elif __APPLE__
    #if TARGET_OS_IPHONE
        _defaultString += "iOS";
    #else
        _defaultString += "macOS";
    #endif
#elif __linux__
        _defaultString += "Linux";
#else
    #error "Unknown OS"
#endif

        // Arch
        _defaultString += " ";
#if defined(__x86_64__) || defined(_M_AMD64)
        _defaultString += "x86_64";
#elif defined(__aarch64__) || defined(_M_ARM64)
        _defaultString += "arm64";
#else
    #error "Unknown arch"
#endif

        // Demo
        if (isDemo) {
            _defaultString += " (DEMO)";
        }

        _targetLabel->setText(_defaultString, juce::dontSendNotification);
    }

    void TooltipLabelUpdater::mouseEnter(const juce::MouseEvent& event) {
        if (_targetLabel != nullptr) {
            juce::TooltipClient* tooltipClient = dynamic_cast<juce::TooltipClient*>(event.eventComponent);

            if (tooltipClient != nullptr) {
                const juce::String displayString = tooltipClient->getTooltip().isEmpty() ? _defaultString : tooltipClient->getTooltip();
                _targetLabel->setText(displayString, juce::dontSendNotification);
            }
        }
    }

    void TooltipLabelUpdater::mouseExit(const juce::MouseEvent& /*event*/) {
        if (_targetLabel != nullptr) {
            _targetLabel->setText(_defaultString, juce::dontSendNotification);
        }
    }

    void TooltipLabelUpdater::refreshTooltip(juce::Component* component) {
        if (_targetLabel != nullptr) {
            juce::TooltipClient* tooltipClient = dynamic_cast<juce::TooltipClient*>(component);

            if (tooltipClient != nullptr) {
                const juce::String displayString = tooltipClient->getTooltip().isEmpty() ? _defaultString : tooltipClient->getTooltip();
                _targetLabel->setText(displayString, juce::dontSendNotification);
            }
        }
    }
}
