/*
 *  File:       ParameterDefinition.h
 *
 *  Created:    22/09/2016
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

#include <unordered_map>
#include <string>

/**
 * Contains classes that are used for defining parameters. Note that these are not
 * intended to define individual parameters (and as such they will not store the
 * current value of a parameter), but are intended to define some characteristics
 * of a given type of parameter, such as the values that are valid for it and
 * provide some methods for performing calculations relating to those characteristics.
 */
namespace ParameterDefinition {
    /**
     * If the given value is between the minimum and maximum values for this parameter,
     * then the value is returned unchanged. If the given value is outside the minimum
     * and maximum values for this parameter, the given value is clipped to this range
     * and then returned.
     *
     * @param   val    Value to clip to minumum and maximum values
     *
     * @return  Clipped value
     */
    template <class T>
    T BoundsCheck(T val, T min, T max) {
        if (val < min) val = min;
        if (val > max) val = max;

        return val;
    }

    class BooleanParameter {
    public:
        BooleanParameter(bool newDefaultValue) :  defaultValue(newDefaultValue) {}

        bool defaultValue;
    };

    /**
     * Provides basic functionality that may be useful for building other parameters from.
     */
    template <class T>
    class BaseParameter {
    public:

        const T minValue;
        const T maxValue;
        const T defaultValue;

        BaseParameter() = delete;
        virtual ~BaseParameter() = default;

        BaseParameter(T newMinValue,
                      T newMaxValue,
                      T newDefaultValue) : minValue(newMinValue),
                                           maxValue(newMaxValue),
                                           defaultValue(newDefaultValue) {}

        /**
         * If the given value is between the minimum and maximum values for this parameter,
         * then the value is returned unchanged. If the given value is outside the minimum
         * and maximum values for this parameter, the given value is clipped to this range
         * and then returned.
         *
         * @param   val    Value to clip to minumum and maximum values
         *
         * @return  Clipped value
         */
        T BoundsCheck(T val) const {
            return ParameterDefinition::BoundsCheck(val, minValue, maxValue);
        }
    };

    /**
     * Provides storage for minimum, maximum and default values for a  parameter
     * which can contain a continuous value (such as a slider), as well as methods to convert
     * between the normalised and internal ranges, and clip a value to the appropriate range.
     */
    template <class T>
    class RangedParameter: public BaseParameter<T> {
    public:

        // Inherit the constructor
        using BaseParameter<T>::BaseParameter;

        /**
         * Translates parameter values from the normalised (0 to 1) range as required
         * by VSTs to the range used internally for that parameter
         *
         * @param   val    Normalised value of the parameter
         *
         * @return  The value of the parameter in the internal range for that parameter
         */
        T NormalisedToInternal(T val) const {
            return val * (this->maxValue - this->minValue) + this->minValue;
        }

        /**
         * Translates parameter values from the range used internally for that
         * parameter, to the normalised range (0 to 1) as required by VSTs.
         *
         * @param   val    Value of the parameter in the internal range
         *
         * @return  The normalised value of the parameter
         */
        T InternalToNormalised(T val) const {
            return (val - this->minValue) / (this->maxValue - this->minValue);
        }
    };
}
