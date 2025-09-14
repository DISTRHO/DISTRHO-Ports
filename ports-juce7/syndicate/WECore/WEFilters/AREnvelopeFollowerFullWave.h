/*
 *  File:       AREnveloperFollowerFullWave.h
 *
 *  Created:    17/11/2020
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

#include "General/CoreMath.h"
#include "AREnvelopeFollowerParameters.h"
#include "AREnvelopeFollowerBase.h"

namespace WECore::AREnv {
    /**
     * Implements a full wave rectification envelope follower.
     */
    class AREnvelopeFollowerFullWave : public AREnvelopeFollowerBase {
    public:

        AREnvelopeFollowerFullWave() = default;
        virtual ~AREnvelopeFollowerFullWave() = default;

        /**
         * Updates the envelope with the current sample and returns the updated envelope value. Must
         * be called for every sample.
         *
         * @param[in]   inSample    Sample used to update the envelope state
         *
         * @return  The updated envelope value
         */
        virtual double _envGetNextOutputImpl(double inSample) override {
            const double tmp = std::abs(inSample);

            if (tmp > _envVal)
            {
                _envVal = _attackCoef * (_envVal - tmp) + tmp;
            }
            else
            {
                _envVal = _releaseCoef * (_envVal - tmp) + tmp;
            }

            return _envVal;
        }
    };
}
