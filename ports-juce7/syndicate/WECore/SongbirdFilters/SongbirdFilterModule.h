/*
 *  File:       SongbirdFilterModule.h
 *
 *  Created:    12/06/2016
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

#include <array>
#include <memory>

#include "SongbirdFormantFilter.h"
#include "SongbirdFiltersParameters.h"
#include "WEFilters/ModulationSource.h"

namespace WECore::Songbird {
    /**
     * The number of formants (bandpass filters) which are used in a single vowel.
     */
    constexpr int NUM_FORMANTS_PER_VOWEL {2};

    /**
     * The number of vowels supported.
     */
    constexpr int NUM_VOWELS {5};

    /**
     * The number of formants that are used for the "air" filters that add brightness.
     */
    constexpr int NUM_AIR_FORMANTS {2};

    /**
     * A type to make refering to a group of formants easier.
     */
    typedef std::array<Formant, NUM_FORMANTS_PER_VOWEL> Vowel;

    /**
     * A filter module providing five different vowel sounds, any two of which can be selected
     * simulaneously and blended between.
     *
     * Also provides an "air" function which is a pair of fixed frequency formant filters at higher
     * frequency that can be blended into the output.
     *
     * To use this class, simply call reset, and the process methods as necessary, using the provided
     * getter and setter methods to manipulate parameters.
     *
     * You must call setSampleRate before beginning any processing as the default sample rate might not
     * be the one you want.
     *
     * Internally relies on the parameters provided in SongbirdFiltersParameters.h
     *
     * @see SongbirdFormantFilter   - SongbirdFilterModule is composed of two pairs of
     *                                SongbirdFormantFilters (pairs to allow stereo processing), each
     *                                pair is assigned one of the five supported vowels at any time
     *
     * A SongbirdFilterModule can be created and used to process a buffer as follows:
     * @code
     * SongbirdFilterModule filter;
     * filter.setSampleRate(44100);
     * filter.setVowel1(VOWEL.VOWEL_O);
     * filter.setVowel2(VOWEL.VOWEL_I);
     * ...
     * set any other parameters you need
     * ...
     *
     * filter.Process2in2out(leftSamples, rightSamples, numSamples);
     * @endcode
     */
    template <typename T>
    class SongbirdFilterModule {
        static_assert(std::is_floating_point<T>::value,
                "Must be provided with a floating point template type");

    public:
        /**
         * Does some basic setup and defaulting of parameters, though do not rely on this
         * to sensibly default all parameters.
         */
        SongbirdFilterModule() : _vowel1(Parameters::VOWEL.VOWEL_A),
                                 _vowel2(Parameters::VOWEL.VOWEL_E),
                                 _filterPosition(Parameters::FILTER_POSITION.defaultValue),
                                 _sampleRate(44100),
                                 _mix(Parameters::MIX.defaultValue),
                                 _airGain(Parameters::AIR_GAIN.defaultValue),
                                 _outputGain(Parameters::OUTPUTGAIN.defaultValue),
                                 _modMode(Parameters::MODMODE_DEFAULT) {

            // initialise the filters to some default values
            setVowel1(_vowel1);
            setVowel2(_vowel2);

            // Set up the air filters
            const std::array<Formant, NUM_AIR_FORMANTS> airFormants {
                Formant(2700, -15), Formant(3500, -25)
            };

            _filterAirLeft.setFormants(airFormants);
            _filterAirRight.setFormants(airFormants);
        }

        virtual ~SongbirdFilterModule() {}

        /** @name Setter Methods */
        /** @{ */

        /**
         * Sets the vowel sound that should be created by filter 1 using one of
         * the built in Vowel objects stored in this class.
         *
         * @param[in]   val Value that should be used for Vowel 1
         *
         * @see         VowelParameter for valid values
         */
        inline void setVowel1(int val);

        /**
         * Sets the vowel sound that should be created by filter 2.
         *
         * @param[in]   val Value that should be used for Vowel 2
         *
         * @see         VowelParameter for valid values
         */
        inline void setVowel2(int val);

        /**
         * Sets the position between the two filters that have been selected.
         *
         * @param[in]   val Filter position to use.
         *
         * @see         FILTER_POSITION for valid values
         * @see         modMode which the effect of this parameter is dependent on

        */
        void setFilterPosition(double val) { _filterPosition = Parameters::FILTER_POSITION.BoundsCheck(val); }

        /**
         * Set the sample rate that the filters expect of the audio which will be processed.
         *
         * @param[in]   val The sample rate to set the filters to
         */
        inline void setSampleRate(double val);

        /**
         * Sets the dry/wet mix level.
         * Lowest value = completely dry, unprocessed signal, no filtering applied.
         * Highest value = completely wet signal, no unprocessed audio survives.
         *
         * @param[in]   val Mix value that should be used
         *
         * @see         MIX for valid values
         */
        void setMix(double val) { _mix = Parameters::MIX.BoundsCheck(val); }

        /**
         * Sets the output gain.
         *
         * @param[in]   val The output gain that should be used
         *
         * @see         OUTPUTGAIN for valid values
         */
        void setOutputGain(double val) { _outputGain = Parameters::OUTPUTGAIN.BoundsCheck(val); }

        /**
         * Sets the modulation mode to apply to the filters.
         *
         * @param[in]   val Chooses the modulation mode
         */
        void setModMode(bool val) { _modMode = val; }

        /**
         * Sets the level of the upper formant filter peaks.
         *
         * @param[in]   val Gain value that should be used
         *
         * @see         AIR_GAIN for valid values
         */
        void setAirGain(double val) { _airGain = Parameters::AIR_GAIN.BoundsCheck(val); }

        /**
         * Sets the modulation source that will be used to modulate the filter position.
         *
         * @param[in]   val The modulation source to use
         */
        void setModulationSource(std::shared_ptr<ModulationSource<double>> val) { _modulationSource = val; }

        /** @} */

        /**
         * Resets all filters.
         * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
         */
        inline void reset();

        /** @name Getter Methods */
        /** @{ */

        /**
         * @see setVowel1
         */
        int getVowel1() const { return _vowel1; }

        /**
         * @see setVowel2
         */
        int getVowel2() const { return _vowel2; }

        /**
         * Return a vowel object describing one of the built in vowels.
         *
         * @see     VowelParameter for valid values
         */
        inline Vowel getVowelDescription(int val) const;

        /**
         * @see setFilterPosition
         */
        double getFilterPosition() const { return _filterPosition; }

        /**
         * @see setMix
         */
        double getMix() const { return _mix; }

        /**
         * @see modMode
         */
        bool getModMode() const { return _modMode; }

        /**
         * @see setAirGain
         */
        double getAirGain() const { return _airGain; }

        /**
         * @see setOutputGain
         */
        double getOutputGain() const { return _outputGain; }

        /** @} */

        /**
         * Applies the filtering to a mono buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         *
         * @param   inSamples      Pointer to the first sample of the buffer
         * @param   numSamples     Number of samples in the buffer. The left and right buffers
         *                         must be the same size.
         */
        inline void Process1in1out(T* inSamples, size_t numSamples);

        /**
         * Applies the filtering to a stereo buffer of samples.
         * Expect seg faults or other memory issues if arguements passed are incorrect.
         *
         * @param   leftSamples    Pointer to the first sample of the left channel's buffer
         * @param   rightSamples   Pointer to the first sample of the right channel's buffer
         * @param   numSamples     Number of samples in the buffer. The left and right buffers
         *                         must be the same size.
         */
        inline void Process2in2out(T* leftSamples, T* rightSamples, size_t numSamples);

        SongbirdFilterModule operator=(SongbirdFilterModule& other) = delete;
        SongbirdFilterModule(SongbirdFilterModule& other) = delete;

    private:
        int _vowel1,
            _vowel2;

        double  _filterPosition,
                _sampleRate,
                _mix,
                _airGain,
                _outputGain;

        bool _modMode;

        std::shared_ptr<ModulationSource<double>> _modulationSource;

        SongbirdFormantFilter<T, NUM_FORMANTS_PER_VOWEL> _filter1Left;
        SongbirdFormantFilter<T, NUM_FORMANTS_PER_VOWEL> _filter1Right;

        SongbirdFormantFilter<T, NUM_FORMANTS_PER_VOWEL> _filter2Left;
        SongbirdFormantFilter<T, NUM_FORMANTS_PER_VOWEL> _filter2Right;

        SongbirdFormantFilter<T, NUM_AIR_FORMANTS> _filterAirLeft;
        SongbirdFormantFilter<T, NUM_AIR_FORMANTS> _filterAirRight;

        /**
         * Sets the vowel sound that should be created by filter 1 using a Vowel object provided by
         * the caller rather than one of the built in Vowel objects stored in this class.
         *
         * @param   val Value that should be used for Vowel 1
         */
        inline void _setVowel1(const Vowel& val);

        /**
         * Uses the filterPosition parameter and the modulation source to calculate the vowel that
         * should be used when in MODMODE_FREQ, as this vowel will sit somewhere between the two
         * vowels that have been selected by the user.
         *
         * @param   modAmount Modulation amount to be applied
         */
        inline Vowel _calcVowelForFreqMode(double modAmount);

        /**
         * An array which defines all the formants that will be needed.
         */
        // (TODO: could be made static again)
        const Vowel _allFormants[NUM_VOWELS] {
            {Formant(800, 0), Formant(1150, -4)},
            {Formant(400, 0), Formant(2100, -24)},
            {Formant(350, 0), Formant(2400, -20)},
            {Formant(450, 0), Formant(800, -9)},
            {Formant(325, 0), Formant(700, -12)},
        };
    };

    template <typename T>
    void SongbirdFilterModule<T>::setVowel1(int val) {
        // perform a bounds check, then apply the appropriate formants
        _vowel1 = Parameters::VOWEL.BoundsCheck(val);

        _filter1Left.setFormants(_allFormants[_vowel1 - 1]);
        _filter1Right.setFormants(_allFormants[_vowel1 - 1]);
    }

    template <typename T>
    void SongbirdFilterModule<T>::setVowel2(int val) {
        // perform a bounds check, then apply the appropriate formants
        _vowel2 = Parameters::VOWEL.BoundsCheck(val);

        _filter2Left.setFormants(_allFormants[_vowel2 - 1]);
        _filter2Right.setFormants(_allFormants[_vowel2 - 1]);
    }

    template <typename T>
    void SongbirdFilterModule<T>::setSampleRate(double val) {
        _sampleRate = val;

        _filter1Left.setSampleRate(val);
        _filter1Right.setSampleRate(val);
        _filter2Left.setSampleRate(val);
        _filter2Right.setSampleRate(val);
        _filterAirLeft.setSampleRate(val);
        _filterAirRight.setSampleRate(val);
    }

    template <typename T>
    void SongbirdFilterModule<T>::reset() {
        _filter1Left.reset();
        _filter1Right.reset();
        _filter2Left.reset();
        _filter2Right.reset();
        _filterAirLeft.reset();
        _filterAirRight.reset();
    }

    template <typename T>
    Vowel SongbirdFilterModule<T>::getVowelDescription(int val) const {
        Vowel tempVowel;

        std::copy(&_allFormants[val - 1][0],
                  &_allFormants[val - 1][NUM_FORMANTS_PER_VOWEL],
                  std::begin(tempVowel));

        return tempVowel;
    }

    template <typename T>
    void SongbirdFilterModule<T>::Process1in1out(T* inSamples, size_t numSamples) {

        for (size_t index {0}; index < numSamples; index++) {

            // Figure out the modulation here. We have two ways to modulation between
            // two formant filters.
            // For MODMODE_BLEND we modulation the filter position to blend between the two filters.
            // For MODMOD_FREQ we set the filter position to 0 so that we're only using filter 1,
            // and then modulate the freqency of filter 1 between the two vowels
            double blendFilterPosition {0};
            const double modAmount {_modulationSource != nullptr ? _modulationSource->getNextOutput(inSamples[index]) : 0};
            if (_modMode == Parameters::MODMODE_BLEND) {
                blendFilterPosition = _filterPosition + modAmount;
            } else {
                blendFilterPosition = 0;
                _setVowel1(_calcVowelForFreqMode(modAmount));
            }

            // Do the processing for each filter
            const T originalInput {inSamples[index]};
            const T filter1Out = _filter1Left.process(originalInput);
            const T filter2Out = _filter2Left.process(originalInput);
            const T filterAirOut = _filterAirLeft.process(originalInput);

            // Write to output, applying filter position and mix level
            inSamples[index] = (
                                 originalInput * (1 - _mix)
                                 + filter1Out * (1 - blendFilterPosition) * _mix
                                 + filter2Out * blendFilterPosition * _mix
                                 + filterAirOut * _airGain * _mix
                             )
                             * _outputGain;
        }
    }

    template <typename T>
    void SongbirdFilterModule<T>::Process2in2out(T* leftSamples,
                                                 T* rightSamples,
                                                 size_t numSamples) {

        for (size_t index {0}; index < numSamples; index++) {

            // Figure out the modulation here. We have two ways to modulation between
            // two formant filters.
            // For MODMODE_BLEND we modulation the filter position to blend between the two filters.
            // For MODMOD_FREQ we set the filter position to 0 so that we're only using filter 1,
            // and then modulate the freqency of filter 1 between the two vowels
            double blendFilterPosition {0};
            const double modAmount {
                _modulationSource != nullptr ? _modulationSource->getNextOutput((leftSamples[index] + rightSamples[index]) / 2) : 0
            };

            if (_modMode == Parameters::MODMODE_BLEND) {
                blendFilterPosition = _filterPosition + modAmount;
            } else {
                blendFilterPosition = 0;
                _setVowel1(_calcVowelForFreqMode(modAmount));
            }

            // Do the processing for each filter
            const T originalLeftIn {leftSamples[index]};
            const T originalRightIn {rightSamples[index]};

            const T filter1LeftOut = _filter1Left.process(originalLeftIn);
            const T filter1RightOut = _filter1Right.process(originalRightIn);

            const T filter2LeftOut = _filter2Left.process(originalLeftIn);
            const T filter2RightOut = _filter2Right.process(originalRightIn);

            const T filterAirLeftOut = _filterAirLeft.process(originalLeftIn);
            const T filterAirRightOut = _filterAirRight.process(originalRightIn);


            // Write to output, applying filter position and mix level
            leftSamples[index] = (
                                    originalLeftIn * (1 - _mix)
                                    + filter1LeftOut * (1 - blendFilterPosition) * _mix
                                    + filter2LeftOut * blendFilterPosition * _mix
                                    + filterAirLeftOut * _airGain * _mix
                                )
                                * _outputGain;

            rightSamples[index] = (
                                    originalRightIn * (1 - _mix)
                                    + filter1RightOut * (1 - blendFilterPosition) * _mix
                                    + filter2RightOut * blendFilterPosition * _mix
                                    + filterAirRightOut * _airGain * _mix
                                )
                                * _outputGain;
        }
    }

    template <typename T>
    void SongbirdFilterModule<T>::_setVowel1(const Vowel& val) {
        _filter1Left.setFormants(val);
        _filter1Right.setFormants(val);
    }

    template <typename T>
    Vowel SongbirdFilterModule<T>::_calcVowelForFreqMode(double modAmount) {
        // get the first and second vowels
        Vowel tempVowel1(getVowelDescription(getVowel1()));
        Vowel tempVowel2(getVowelDescription(getVowel2()));

        Vowel retVal(tempVowel1);

        for (size_t iii {0}; iii < NUM_FORMANTS_PER_VOWEL; iii++) {
            // Calculate frequency modulation
            const double freqDelta {tempVowel2[iii].frequency - tempVowel1[iii].frequency};
            retVal[iii].frequency = tempVowel1[iii].frequency + freqDelta * (_filterPosition + modAmount);

            // Calculate gain modulation
            const double gainDelta {tempVowel2[iii].gaindB - tempVowel1[iii].gaindB};
            retVal[iii].gaindB = tempVowel1[iii].gaindB + gainDelta * (_filterPosition + modAmount);
        }

        return retVal;
    }
}
