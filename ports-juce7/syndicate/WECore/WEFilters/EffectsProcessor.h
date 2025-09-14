/*
 *  File:       EffectsProcessor.h
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

namespace WECore {

    /**
     * Base class for EffectsProcessors with different channel configurations to inherit from.
     */
    template <typename SampleType>
    class EffectsProcessorBase {
        static_assert(std::is_floating_point<SampleType>::value,
                      "Must be provided with a floating point template type");

    public:
        EffectsProcessorBase() = default;
        virtual ~EffectsProcessorBase() = default;

        /**
         * Resets the internal state of the processor.
         *
         * Override this to reset everything as necessary.
         */
        inline virtual void reset() {}
    };

    /**
     * Provides a standard interface for effects which process a mono buffer of samples.
     */
    template <typename SampleType>
    class EffectsProcessor1in1out : public EffectsProcessorBase<SampleType> {
    public:
        EffectsProcessor1in1out() = default;
        virtual ~EffectsProcessor1in1out() override = default;

        virtual void process1in1out(SampleType* inSamples, size_t numSamples) = 0;
    };

    /**
     * Provides a standard interface for effects which process a mono to stereo buffer of samples.
     */
    template <typename SampleType>
    class EffectsProcessor1in2out : public EffectsProcessorBase<SampleType> {
    public:
        EffectsProcessor1in2out() = default;
        virtual ~EffectsProcessor1in2out() override = default;

        virtual void process1in2out(SampleType* inSamplesLeft, SampleType* inSamplesRight, size_t numSamples) = 0;
    };

    /**
     * Provides a standard interface for effects which process stereo buffers of samples.
     */
    template <typename SampleType>
    class EffectsProcessor2in2out : public EffectsProcessorBase<SampleType> {
    public:
        EffectsProcessor2in2out() = default;
        virtual ~EffectsProcessor2in2out() override = default;

        virtual void process2in2out(SampleType* inSamplesLeft, SampleType* inSamplesRight, size_t numSamples) = 0;
    };
}
