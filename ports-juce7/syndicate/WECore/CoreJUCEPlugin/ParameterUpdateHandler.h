/*
 *	File:		CoreProcessorEditor.h
 *
 *	Created:	24/03/2021
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
#include "CoreAudioProcessor.h"

namespace WECore::JUCEPlugin {

    class ParameterUpdateHandler {

    public:
        ParameterUpdateHandler() : _parameterListener(this) { }
        virtual ~ParameterUpdateHandler() = default;

    protected:
        /**
         * Is notified when a parameter has changed and calls _onParameterUpdate.
         */
        class ParameterUpdateListener : public juce::ChangeListener {
        public:
            ParameterUpdateListener(ParameterUpdateHandler* parent) : _parent(parent) {};
            virtual ~ParameterUpdateListener() = default;

            virtual void changeListenerCallback(juce::ChangeBroadcaster* /*source*/) {
                _parent->_onParameterUpdate();
            }

        private:
            ParameterUpdateHandler* _parent;
        };

        ParameterUpdateListener _parameterListener;

        /**
         * This will be called whenever a parameter has been updated.
         */
        virtual void _onParameterUpdate() = 0;

    };
}
