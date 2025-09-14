/*
 *  File:       StereoWidthProcessor.h
 *
 *  Created:    03/12/2020
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

#include <vector>

#include "EffectsProcessor.h"
#include "StereoWidthProcessorParameters.h"

namespace WECore::StereoWidth {

    /**
     * Processor which can apply stereo widening or narrowing.
     */
    template <typename SampleType>
    class StereoWidthProcessor : public EffectsProcessor2in2out<SampleType> {
    public:
        StereoWidthProcessor() : _width(Parameters::WIDTH.defaultValue) {}
        virtual ~StereoWidthProcessor() override = default;

        /**
         * Sets the stereo width.
         * 0 = mono
         * 1 = neutral, no processing
         * 2 = highest width
         *
         * @param   val   The width to be applied.
         *
         * @see     WIDTH for valid values
         */
        void setWidth(double val) { _width = Parameters::WIDTH.BoundsCheck(val); }

        /**
         * @see setWidth
         */
        double getWidth() const { return _width; }


        inline virtual void process2in2out(SampleType* inSamplesLeft,
                                           SampleType* inSamplesRight,
                                           size_t numSamples) override;

    private:
        double _width;
    };

    template <typename SampleType>
    void StereoWidthProcessor<SampleType>::process2in2out(SampleType* inSamplesLeft,
                                                          SampleType* inSamplesRight,
                                                          size_t numSamples) {

        for (size_t index {0}; index < numSamples; index++) {

            double mid {(inSamplesLeft[index] + inSamplesRight[index]) * 0.5};
            double side {(inSamplesRight[index] - inSamplesLeft[index]) * (_width / 2)};

            inSamplesLeft[index] = mid - side;
            inSamplesRight[index] = mid + side;
        }
    }
}
