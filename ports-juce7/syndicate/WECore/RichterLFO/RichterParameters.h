/*
 *  File:       RichterParameters.h
 *
 *  Created:    25/09/2016
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
 */

#pragma once

#include "General/ParameterDefinition.h"
#include "RichterWavetables.h"

namespace WECore::Richter::Parameters {

    class WaveParameter : public ParameterDefinition::BaseParameter<int> {
    public:
        WaveParameter() : ParameterDefinition::BaseParameter<int>(SINE, SIDECHAIN, SINE) {}

        static constexpr int SINE {1},
                             SQUARE {2},
                             SAW {3},
                             SIDECHAIN {4};
    };

    const WaveParameter WAVE;

    class OutputModeParameter : public ParameterDefinition::BaseParameter<int> {
    public:
        OutputModeParameter() : ParameterDefinition::BaseParameter<int>(UNIPOLAR, BIPOLAR, BIPOLAR) {}

        static constexpr int UNIPOLAR {1},
                             BIPOLAR {2};
    };

    const OutputModeParameter OUTPUTMODE;

    const ParameterDefinition::RangedParameter<int> TEMPONUMER(1, 32, 1),
                                                    TEMPODENOM(1, 32, 1);

    const ParameterDefinition::RangedParameter<double>  DEPTH(0, 1, 0.5),
                                                        DEPTHMOD(0, 1, 0),
                                                        FREQ(0, 20, 2),
                                                        FREQMOD(0, 1, 0),
                                                        PHASE(0, 360, 0);

    constexpr bool LFOSWITCH_OFF = false,
                   LFOSWITCH_ON = true,
                   LFOSWITCH_DEFAULT = LFOSWITCH_OFF,

                   TEMPOSYNC_OFF = false,
                   TEMPOSYNC_ON = true,
                   TEMPOSYNC_DEFAULT = TEMPOSYNC_OFF,

                   PHASESYNC_OFF = false,
                   PHASESYNC_ON = true,
                   PHASESYNC_DEFAULT = PHASESYNC_ON,

                   INVERT_OFF = false,
                   INVERT_ON = true,
                   INVERT_DEFAULT = INVERT_OFF;

}
