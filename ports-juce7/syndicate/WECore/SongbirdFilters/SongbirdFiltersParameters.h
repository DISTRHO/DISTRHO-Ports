/*
 *  File:       SongbirdFiltersParameters.h
 *
 *  Created:    02/10/2016
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

namespace WECore::Songbird::Parameters {

    class VowelParameter : public ParameterDefinition::BaseParameter<int> {
    public:
        VowelParameter() : ParameterDefinition::BaseParameter<int>(VOWEL_A, VOWEL_U, VOWEL_A) { }

        static constexpr int VOWEL_A {1},
                             VOWEL_E {2},
                             VOWEL_I {3},
                             VOWEL_O {4},
                             VOWEL_U {5};
    };
    const VowelParameter VOWEL;

    //@{
    /**
     * A parameter which can take any float value between the ranges defined.
     * The values passed on construction are in the following order:
     *      minimum value,
     *      maximum value,
     *      default value
     */
    const ParameterDefinition::RangedParameter<double>  FILTER_POSITION(0, 1, 0.5),
                                                        MIX(0, 1, 1),
                                                        AIR_GAIN(0, 1, 0.5),
                                                        OUTPUTGAIN(0, 2, 1);
    //@}

    constexpr bool MODMODE_BLEND = false,
                   MODMODE_FREQ = true,
                   MODMODE_DEFAULT = MODMODE_FREQ;

    constexpr int FILTER_ORDER {8};

}
