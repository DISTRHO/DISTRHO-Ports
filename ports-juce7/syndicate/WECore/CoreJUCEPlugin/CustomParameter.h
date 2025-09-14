/*
 *	File:		CustomParameter.h
 *
 *	Version:	1.0.0
 *
 *	Created:	18/08/2021
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
     * Used to store state that shouldn't be exposed to the host, but would benefit from using the
     * save/restore and update mechanism used by conventional parameters.
     *
     * Derive from this class and add your own setter/getter methods as needed, just call
     * _updateListener() to trigger an update.
     *
     * Provide an implementation for restoreFromXml and writeToXml to enable saving and restoring
     * parameter state.
     */
    class CustomParameter {
    public:
        inline CustomParameter();
        virtual ~CustomParameter() = default;

        inline void setListener(juce::AudioProcessorParameter::Listener* listener);
        inline void removeListener();

        virtual void restoreFromXml(juce::XmlElement* element) = 0;
        virtual void writeToXml(juce::XmlElement* element) = 0;

    protected:
        inline void _updateListener();

    private:
        std::mutex _listenerMutex;
        juce::AudioProcessorParameter::Listener* _listener;
    };

    CustomParameter::CustomParameter() : _listener(nullptr) {
    }

    void CustomParameter::setListener(juce::AudioProcessorParameter::Listener* listener) {
        std::scoped_lock lock(_listenerMutex);
        _listener = listener;
    }

    void CustomParameter::removeListener() {
        std::scoped_lock lock(_listenerMutex);
        _listener = nullptr;
    }

    void CustomParameter::_updateListener() {
        std::scoped_lock lock(_listenerMutex);
        if (_listener != nullptr) {
            _listener->parameterValueChanged(0, 0);
        }
    }
}