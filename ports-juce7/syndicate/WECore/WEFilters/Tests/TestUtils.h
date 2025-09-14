/*
 *  File:       TestUtils.h
 *
 *  Created:    05/12/2020
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

#include <vector>

#include "General/CoreMath.h"

namespace WECore::TestUtils {
    static void generateSine(std::vector<double>& buffer, double sampleRate, double frequency) {

        const double samplesPerCycle {sampleRate / frequency};

        std::generate(buffer.begin(),
                      buffer.end(),
                      [index = 0, samplesPerCycle]() mutable {return std::sin(CoreMath::LONG_TAU * (index++ / samplesPerCycle));} );
    }
}
