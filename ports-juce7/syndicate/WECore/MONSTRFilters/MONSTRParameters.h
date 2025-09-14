/*
 *  File:       MONSTRParameters.h
 *
 *  Created:    08/11/2016
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

namespace WECore::MONSTR::Parameters {
    constexpr bool BANDSWITCH_OFF {false},
                   BANDSWITCH_ON {true},
                   BANDSWITCH_DEFAULT {BANDSWITCH_ON},

                   BANDMUTED_OFF {false},
                   BANDMUTED_ON {true},
                   BANDMUTED_DEFAULT {BANDMUTED_OFF},

                   BANDSOLO_OFF {false},
                   BANDSOLO_ON {true},
                   BANDSOLO_DEFAULT {BANDSOLO_OFF};

    // constexpr as it initialises some internal memebers
    constexpr int _MAX_NUM_BANDS {6};
    constexpr int _DEFAULT_NUM_BANDS {2};

    //@{
    /**
     * A parameter which can take any float value between the ranges defined.
     * The values passed on construction are in the following order:
     *      minimum value,
     *      maximum value,
     *      default value
     */
    const ParameterDefinition::RangedParameter<double> CROSSOVER_FREQUENCY(40, 19500, 1000);
    const ParameterDefinition::RangedParameter<int> NUM_BANDS(2, _MAX_NUM_BANDS, _DEFAULT_NUM_BANDS);
    //@}
}
