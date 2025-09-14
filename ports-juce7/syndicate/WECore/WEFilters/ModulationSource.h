/*
 *  File:       ModulationSource.h
 *
 *  Created:    22/11/2020
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
     * Provides a standard interface for modulation sources such as LFOs and envelope followers to
     * follow.
     *
     * This interface only defines how to get output from the modulation source and reset it, and
     * say nothing about setting its parameters, sample rate, etc as these will vary.
     */
    template <typename SampleType>
    class ModulationSource {
        static_assert(std::is_floating_point<SampleType>::value,
                      "Must be provided with a floating point template type");

    public:
        ModulationSource() : _cachedOutput(0) {}
        virtual ~ModulationSource() = default;

        /**
         * Given the provided audio sample, calculates the next output value and advances the
         * internal state (if applicable).
         */
        inline SampleType getNextOutput(SampleType inSample);

        /**
         * Returns the most recent output of getNextOutput without advancing the internal state.
         */
        virtual SampleType getLastOutput() const { return _cachedOutput; }

        /**
         * Resets the internal state of the modulation source.
         */
        inline void reset();

    protected:
        SampleType _cachedOutput;

        /**
         * Must be overriden by the inheriting class to provide the specific implementation of this
         * modulation source.
         *
         * The implementation may or may not need to use the provided audio sample.
         */
        virtual SampleType _getNextOutputImpl(SampleType inSample) = 0;

        /**
         * Must be overriden by the inheriting class to reset the internat state as required.
         */
        virtual void _resetImpl() = 0;
    };

    template <typename SampleType>
    SampleType ModulationSource<SampleType>::getNextOutput(SampleType inSample) {
        _cachedOutput = _getNextOutputImpl(inSample);
        return _cachedOutput;
    }

    template <typename SampleType>
    void ModulationSource<SampleType>::reset() {
        _resetImpl();
        _cachedOutput = 0;
    }

    template <typename SampleType>
    struct ModulationSourceWrapper {
        std::shared_ptr<ModulationSource<SampleType>> source;
        double amount;
    };

    template <typename SampleType>
    SampleType calcModValue(const std::vector<ModulationSourceWrapper<SampleType>>& sources) {
        SampleType retVal {0};

        for (const ModulationSourceWrapper<SampleType>& source : sources) {
            retVal += source.source->getLastOutput() * source.amount;
        }

        return retVal;
    }
}
