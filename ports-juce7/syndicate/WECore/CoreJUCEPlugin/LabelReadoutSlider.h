/*
 *	File:		LabelReadoutSlider.h
 *
 *	Version:	1.0.0
 *
 *	Created:	01/07/2020
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
#include "General/ParameterDefinition.h"

namespace WECore::JUCEPlugin {

    /**
     * Handles mouse events that may indicate that the Slider value has changed.
     */
    class SliderLabelUpdater : public juce::Slider {
    public:
    explicit SliderLabelUpdater(const juce::String& componentName) : Slider(componentName) {}
        virtual ~SliderLabelUpdater() = default;

        /** @name Mouse event handlers */
        /** @{ */
        virtual void mouseEnter(const juce::MouseEvent& event) override {
            Slider::mouseEnter(event);
            _updateLabel(event);
        }

        virtual void mouseMove(const juce::MouseEvent& event) override {
            Slider::mouseMove(event);
            _updateLabel(event);
        }

        virtual void mouseExit(const juce::MouseEvent& event) override {
            Slider::mouseExit(event);
            _resetLabel();
        }

        virtual void mouseDoubleClick(const juce::MouseEvent& event) override {
            Slider::mouseDoubleClick(event);
            _updateLabel(event);
        }

        virtual void mouseDrag(const juce::MouseEvent& event) override {
            Slider::mouseDrag(event);
            _updateLabel(event);
        }

        virtual void mouseWheelMove(const juce::MouseEvent& event,
                                    const juce::MouseWheelDetails& wheel) override {
            Slider::mouseWheelMove(event, wheel);
            _updateLabel(event);
        }
        /** @} */

    private:
        /**
         * Called when the Slider value may have changed and the Label(s) should be updated.
         */
        virtual void _updateLabel(const juce::MouseEvent& event) = 0;

        /**
         * Called when the mouse is no longer over the Slider, so the Label(s) can be reset.
         */
        virtual void _resetLabel() = 0;
    };

    /**
     * Outputs the value of the Slider to a label while hovering over the slider.
     */
    template <class T>
    class LabelReadoutSlider : public SliderLabelUpdater {
    public:
        explicit LabelReadoutSlider(const juce::String& componentName) : SliderLabelUpdater(componentName),
                                                                         _isRunning(false),
                                                                         _valueToString([](T value) { return juce::String(value, 2); }) {}

        virtual ~LabelReadoutSlider() = default;

        /**
         * Tells the slider to start writing to the given component on mouse enter events.
         *
         * Doesn't take ownership of the component.
         */
        /** @{ */
        inline void start(juce::Label* targetLabel, juce::String defaultText);
        inline void start(juce::TextButton* targetButton, juce::String defaultText);
        /** @} */

        /**
         * Tells the slider to stop writing to the label.
         *
         * Call this in your destructor if the Label or RangedParameter might which we depend on
         * might be deallocated before LabelReadoutSlider.
         */
        inline void stop();

        void setValueToString(std::function<juce::String(T)> valueToString) { _valueToString = valueToString; }

    protected:
        std::function<void(const juce::String&)> _targetCallback;
        bool _isRunning;
        std::function<juce::String(T)> _valueToString;

    private:
        juce::String _defaultText;

        static std::function<void(const juce::String&)> _labelToCallback(juce::Label* label) {
            return [label](const juce::String& text) { label->setText(text, juce::dontSendNotification); };
        }

        static std::function<void(const juce::String&)> _textButtonToCallback(juce::TextButton* button) {
            return [button](const juce::String& text) { button->setButtonText(text); };
        }

        inline virtual void _updateLabel(const juce::MouseEvent& event) override;

        inline virtual void _resetLabel() override;
    };

    template <class T>
    void LabelReadoutSlider<T>::start(juce::Label* targetLabel, juce::String defaultText) {
        _targetCallback = _labelToCallback(targetLabel);
        _defaultText = defaultText;
        _isRunning = true;
    }

    template <class T>
    void LabelReadoutSlider<T>::start(juce::TextButton* targetButton, juce::String defaultText) {
        _targetCallback = _textButtonToCallback(targetButton);
        _defaultText = defaultText;
        _isRunning = true;
    }

    template <class T>
    void LabelReadoutSlider<T>::stop() {
        _isRunning = false;
    }

    template <class T>
    void LabelReadoutSlider<T>::_updateLabel(const juce::MouseEvent& /*event*/) {
        if (_isRunning) {
            _targetCallback(_valueToString(getValue()));
        }
    }

    template <class T>
    void LabelReadoutSlider<T>::_resetLabel() {
        if (_isRunning) {
            _targetCallback(_defaultText);
        }
    }
}
