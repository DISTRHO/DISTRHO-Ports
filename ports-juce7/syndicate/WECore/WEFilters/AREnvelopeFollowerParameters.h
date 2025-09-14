/*
 *  File:       AREnveloperFollowerParameters.h
 *
 *  Created:    07/05/2017
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
#include "TPTSVFilterParameters.h"

namespace WECore::AREnv::Parameters {
    //@{
    /**
     * A parameter which can take any float value between the ranges defined.
     * The values passed on construction are in the following order:
     *      minimum value,
     *      maximum value,
     *      default value
     */
    const ParameterDefinition::RangedParameter<double>  ATTACK_MS(0.1, 10000, 20),
                                                        RELEASE_MS(0.1, 10000, 200),
                                                        LOW_CUT(TPTSVF::Parameters::CUTOFF.minValue, TPTSVF::Parameters::CUTOFF.maxValue, 0),
                                                        HIGH_CUT(TPTSVF::Parameters::CUTOFF.minValue, TPTSVF::Parameters::CUTOFF.maxValue, 20000);
    //@}

    const ParameterDefinition::BooleanParameter FILTER_ENABLED(false);
}
