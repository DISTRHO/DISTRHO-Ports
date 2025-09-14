/*
 *  File:       RichterLFO.h
 *
 *  Created:    18/01/2015
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

#include "RichterParameters.h"
#include "RichterWavetables.h"
#include "WEFilters/ModulationSource.h"

namespace WECore::Richter {

    class RichterLFOPair;

    /**
     * Provides and LFO with: depth, rate, tempo sync, phase, wave shape, and phase sync
     * controls, plus additional functionality to allow the depth and rate controls to be
     * modulated by an external source, with internal controls of the for the depth of this
     * modulation.
     *
     * This LFO oscillates between -1 and 1.
     *
     * To use, you simply need to call reset, prepareForNextBuffer, and getNextOutput
     * as necessary (see their descriptions for details), and use the provided getter
     * and setter methods to manipulate parameters.
     */
    class RichterLFO : public ModulationSource<double> {

    public:

        /**
         * Generates the wave tables on initialsation, while running gain values
         * are simply looked up from these wave tables.
         *
         * Also initialises parameters (that are not part of RichterLFOBase) to default values.
         */
        inline RichterLFO();

        virtual ~RichterLFO() override = default;

        friend class RichterLFOPair;

        /** @name Getter Methods */
        /** @{ */
        bool getBypassSwitch() const { return _bypassSwitch; }
        bool getPhaseSyncSwitch() const { return _phaseSyncSwitch; }
        bool getTempoSyncSwitch() const { return _tempoSyncSwitch; }
        bool getInvertSwitch() const { return _invertSwitch; }
        int getWave() const { return _wave; }
        int getOutputMode() { return _outputMode; }
        double getTempoNumer() const { return _tempoNumer; }
        double getTempoDenom() const { return _tempoDenom; }
        double getFreq() { return _rawFreq; }
        double getModulatedFreqValue() const { return _modulatedFreqValue; }
        double getDepth() { return _rawDepth; }
        double getModulatedDepthValue() const { return _modulatedDepthValue; }
        double getManualPhase() const { return _manualPhase; }
        double getModulatedPhaseValue() const { return _modulatedPhaseValue; }
        /** @} */

        /** @name Setter Methods */
        /** @{ */
        void setBypassSwitch(bool val) { _bypassSwitch = val; }
        void setPhaseSyncSwitch(bool val) { _phaseSyncSwitch = val; }
        void setTempoSyncSwitch(bool val) { _tempoSyncSwitch = val; }
        void setInvertSwitch(bool val) { _invertSwitch = val; }
        inline void setWave(int val);
        void setOutputMode(int val) { _outputMode = Parameters::OUTPUTMODE.BoundsCheck(val); }
        void setTempoNumer(int val) { _tempoNumer = Parameters::TEMPONUMER.BoundsCheck(val); }
        void setTempoDenom (int val) { _tempoDenom = Parameters::TEMPODENOM.BoundsCheck(val); }
        void setFreq(double val) { _rawFreq = Parameters::FREQ.BoundsCheck(val); }
        void setDepth(double val) { _rawDepth = Parameters::DEPTH.BoundsCheck(val); }
        void setManualPhase(double val) { _manualPhase = Parameters::PHASE.BoundsCheck(val); }
        void setSampleRate(double val) { _sampleRate = val; }
        /** @} */

        inline bool addFreqModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool removeFreqModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool setFreqModulationAmount(size_t index, double amount);
        std::vector<ModulationSourceWrapper<double>> getFreqModulationSources() const { return _freqModulationSources; }

        inline bool addDepthModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool removeDepthModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool setDepthModulationAmount(size_t index, double amount);
        std::vector<ModulationSourceWrapper<double>> getDepthModulationSources() const { return _depthModulationSources; }

        inline bool addPhaseModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool removePhaseModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool setPhaseModulationAmount(size_t index, double amount);
        std::vector<ModulationSourceWrapper<double>> getPhaseModulationSources() const { return _phaseModulationSources; }

        /**
         * Prepares for processing the next buffer of samples. For example if using JUCE, you
         * would call this in your processBlock() method before doing any processing.
         *
         * @param   bpm             Current bpm of the host
         * @param   timeInSeconds   Position of the host DAW's playhead at the start of
         *                          playback.
         */
        inline void prepareForNextBuffer(double bpm, double timeInSeconds);

        RichterLFO operator= (RichterLFO& other) = delete;
        RichterLFO(RichterLFO&) = delete;

    protected:
        int     _wave,
                _indexOffset,
                _outputMode;

        bool    _bypassSwitch,
                _tempoSyncSwitch,
                _phaseSyncSwitch,
                _invertSwitch,
                _needsSeekOffsetCalc;

        double  _tempoNumer,
                _tempoDenom,
                _rawFreq,
                _modulatedFreqValue,
                _rawDepth,
                _modulatedDepthValue,
                _manualPhase,
                _modulatedPhaseValue,
                _sampleRate,
                _bpm,
                _wavetablePosition;

        const double* _waveArrayPointer;

        std::vector<ModulationSourceWrapper<double>> _freqModulationSources,
                                                     _depthModulationSources,
                                                     _phaseModulationSources;

        /**
         * Calculates the phase offset to be applied to the oscillator, including any
         * offset required by the phase sync and any offset applied by the user.
         *
         * @param   timeInSeconds   Position of the host DAW's playhead at the start of
         *                          playback.
         */
        inline void _calcPhaseOffset(double timeInSeconds);

        /**
         * Calculates the frequency of the oscillator. Will use either the frequency
         * or tempoNumer/tempoDenom depending on whether tempo sync is enabled.
         *
         * @param   modAmount     The gain output from the modulation oscillator.
         */
        inline double _calcFreq(double modAmount);

        /**
         * Calculates the current index of the oscillator in its wavetable and increments the number
         * of samples processed.
         *
         * @param   freq        The absolute frequency of the LFO, including tempo sync or modulation.
         *
         * @return  The value in the wavetable at the current index.
         *
         */
        inline double _calcLFOValue(double freq, double phaseModValue);

        /**
         * Returns the next output of the LFO.
         *
         * Note: Calling this method will advance the oscillators internal counters by one
         *       sample. Calling this method will return a different value each time.
         *
         * @return  The value of the LFO's output at this moment, a value between -1 and 1.
         */
        inline double _getNextOutputImpl(double inSample) override;

        /**
         * Resets internal counters including indexOffset and currentScale.
         */
        virtual inline void _resetImpl() override;
    };

    RichterLFO::RichterLFO() : _wave(Parameters::WAVE.defaultValue),
                               _indexOffset(0),
                               _outputMode(Parameters::OUTPUTMODE.defaultValue),
                               _bypassSwitch(Parameters::LFOSWITCH_DEFAULT),
                               _tempoSyncSwitch(Parameters::TEMPOSYNC_DEFAULT),
                               _phaseSyncSwitch(Parameters::PHASESYNC_DEFAULT),
                               _invertSwitch(Parameters::INVERT_DEFAULT),
                               _needsSeekOffsetCalc(true),
                               _tempoNumer(Parameters::TEMPONUMER.defaultValue),
                               _tempoDenom(Parameters::TEMPODENOM.defaultValue),
                               _rawFreq(Parameters::FREQ.defaultValue),
                               _modulatedFreqValue(0),
                               _rawDepth(Parameters::DEPTH.defaultValue),
                               _modulatedDepthValue(0),
                               _manualPhase(Parameters::PHASE.defaultValue),
                               _modulatedPhaseValue(0),
                               _sampleRate(44100),
                               _bpm(0),
                               _wavetablePosition(0),
                               _waveArrayPointer(Wavetables::getInstance()->getSine()) {
    }

    void RichterLFO::setWave(int val) {
        _wave = Parameters::WAVE.BoundsCheck(val);

        if (_wave == Parameters::WAVE.SINE) {
            _waveArrayPointer = Wavetables::getInstance()->getSine();
        } else if (_wave == Parameters::WAVE.SQUARE) {
            _waveArrayPointer = Wavetables::getInstance()->getSquare();
        } else if (_wave == Parameters::WAVE.SAW) {
            _waveArrayPointer = Wavetables::getInstance()->getSaw();
        } else if (_wave == Parameters::WAVE.SIDECHAIN) {
            _waveArrayPointer = Wavetables::getInstance()->getSidechain();
        }
    }

    bool RichterLFO::addFreqModulationSource(std::shared_ptr<ModulationSource> source) {
        // Check if the source is already in the list
        for (const ModulationSourceWrapper<double>& existingSource : _freqModulationSources) {
            if (existingSource.source == source) {
                return false;
            }
        }

        _freqModulationSources.push_back({source, 0});
        return true;
    }

    bool RichterLFO::removeFreqModulationSource(std::shared_ptr<ModulationSource> source) {
        for (auto it = _freqModulationSources.begin(); it != _freqModulationSources.end(); it++) {
            if ((*it).source == source) {
                _freqModulationSources.erase(it);
                return true;
            }
        }

        return false;
    }

    bool RichterLFO::setFreqModulationAmount(size_t index, double amount) {
        if (index >= _freqModulationSources.size()) {
            return false;
        }

        _freqModulationSources[index].amount = amount;
        return true;
    }

    bool RichterLFO::addDepthModulationSource(std::shared_ptr<ModulationSource> source) {
        // Check if the source is already in the list
        for (const ModulationSourceWrapper<double>& existingSource : _depthModulationSources) {
            if (existingSource.source == source) {
                return false;
            }
        }

        _depthModulationSources.push_back({source, 0});
        return true;
    }

    bool RichterLFO::removeDepthModulationSource(std::shared_ptr<ModulationSource> source) {
        for (auto it = _depthModulationSources.begin(); it != _depthModulationSources.end(); it++) {
            if ((*it).source == source) {
                _depthModulationSources.erase(it);
                return true;
            }
        }

        return false;
    }

    bool RichterLFO::setDepthModulationAmount(size_t index, double amount) {
        if (index >= _depthModulationSources.size()) {
            return false;
        }

        _depthModulationSources[index].amount = amount;
        return true;
    }

    bool RichterLFO::addPhaseModulationSource(std::shared_ptr<ModulationSource> source) {
        // Check if the source is already in the list
        for (const ModulationSourceWrapper<double>& existingSource : _phaseModulationSources) {
            if (existingSource.source == source) {
                return false;
            }
        }

        _phaseModulationSources.push_back({source, 0});
        return true;
    }

    bool RichterLFO::removePhaseModulationSource(std::shared_ptr<ModulationSource> source) {
        for (auto it = _phaseModulationSources.begin(); it != _phaseModulationSources.end(); it++) {
            if ((*it).source == source) {
                _phaseModulationSources.erase(it);
                return true;
            }
        }

        return false;
    }

    bool RichterLFO::setPhaseModulationAmount(size_t index, double amount) {
        if (index >= _phaseModulationSources.size()) {
            return false;
        }

        _phaseModulationSources[index].amount = amount;
        return true;
    }

    void RichterLFO::prepareForNextBuffer(double bpm,
                                          double timeInSeconds) {
        _bpm = bpm;
        _calcPhaseOffset(timeInSeconds);
    }

    void RichterLFO::_resetImpl() {
        _needsSeekOffsetCalc = true;
        _indexOffset = 0;
        _wavetablePosition = 0;
    }

    void RichterLFO::_calcPhaseOffset(double timeInSeconds) {

        // The phase offset applied by the playhead's start position needs to be calculated only
        // when playback initially starts, and not for any subsequent buffers until playback stops
        // and starts again
        static int seekIndexOffset {0};
        if (_needsSeekOffsetCalc) {
            const double waveLength {1 / _calcFreq(0)};
            const double waveTimePosition {std::fmod(timeInSeconds, waveLength)};

            seekIndexOffset = static_cast<int>((waveTimePosition / waveLength) * Wavetables::SIZE);
            _needsSeekOffsetCalc = false;
        }

        if (_phaseSyncSwitch) {
            _indexOffset = seekIndexOffset;
        } else {
            _indexOffset = 0;
        }
    }

    double RichterLFO::_calcFreq(double modAmount) {
        // Calculate the frequency based on whether tempo sync is active
        double freq {0};

        if (_tempoSyncSwitch) {
            freq = (_bpm / 60) * (_tempoDenom / _tempoNumer);
        } else {
            freq = _rawFreq + ((Parameters::FREQ.maxValue / 2) * modAmount);
        }

        freq = Parameters::FREQ.BoundsCheck(freq);
        _modulatedFreqValue = freq;

        return freq;
    }

    double RichterLFO::_calcLFOValue(double freq, double phaseModValue) {
        const double samplesPerTremoloCycle {_sampleRate / freq};
        const double scale {Wavetables::SIZE / samplesPerTremoloCycle};

        // Calculate the current position within the wave table
        _wavetablePosition = std::fmod(_wavetablePosition + scale, Wavetables::SIZE);

        _modulatedPhaseValue = _manualPhase + (phaseModValue * Parameters::PHASE.maxValue);

        const int phaseIndexOffset {
            static_cast<int>((_modulatedPhaseValue / Parameters::PHASE.maxValue) * Wavetables::SIZE)
        };

        const int index {static_cast<int>(_wavetablePosition)};

        const double wavetableValue {_waveArrayPointer[(index + _indexOffset + phaseIndexOffset) % Wavetables::SIZE]};

        return _outputMode == Parameters::OUTPUTMODE.BIPOLAR ? wavetableValue : (wavetableValue + 1);
    }

    double RichterLFO::_getNextOutputImpl(double /*inSample*/) {
        // Get the mod amount to use, divide by 2 to reduce range to -0.5:0.5
        const double freqModValue {calcModValue(_freqModulationSources) / 2};
        const double depthModValue {calcModValue(_depthModulationSources) / 2};
        const double phaseModValue {calcModValue(_phaseModulationSources) / 2};

        const double lfoValue {_calcLFOValue(_calcFreq(freqModValue), phaseModValue)};

        // Calculate the depth value after modulation
        const double depth {Parameters::DEPTH.BoundsCheck(
            _rawDepth + (Parameters::DEPTH.maxValue * depthModValue)
        )};
        _modulatedDepthValue = depth;

        if (_bypassSwitch) {
            // Produce a value in the range -1:1 (or 0:2), invert if needed
            return (lfoValue * depth) * (_invertSwitch ? -1 : 1);
        } else {
            return 0;
        }
    }
}
