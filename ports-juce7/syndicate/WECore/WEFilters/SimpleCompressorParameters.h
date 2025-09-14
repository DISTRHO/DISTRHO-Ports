/*
 *  File:       SimpleCompressorParameters.h
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

namespace WECore::SimpleCompressor::Parameters {
    //@{
    /**
     * A parameter which can take any float value between the ranges defined.
     * The values passed on construction are in the following order:
     *      minimum value,
     *      maximum value,
     *      default value
     */
    const ParameterDefinition::RangedParameter<double> ATTACK_MS(0.1, 500, 10),
                                                       RELEASE_MS(1, 5000, 100),
                                                       THRESHOLD(-60, 0, 0),
                                                       RATIO(1, 30, 2),
                                                       KNEE_WIDTH(1, 10, 2);

    //@}
}
