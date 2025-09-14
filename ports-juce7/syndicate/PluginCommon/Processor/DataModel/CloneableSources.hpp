#pragma once

#include "RichterLFO/RichterLFO.h"
#include "WEFilters/AREnvelopeFollowerSquareLaw.h"

namespace ModelInterface {
    class CloneableLFO : public WECore::Richter::RichterLFO {
    public:
        CloneableLFO() : WECore::Richter::RichterLFO() {}

        CloneableLFO* clone() const {
            return new CloneableLFO(*this);
        }

         void setFreqModulationSources(std::vector<WECore::ModulationSourceWrapper<double>> sources) {
            _freqModulationSources = sources;
        }

        void setDepthModulationSources(std::vector<WECore::ModulationSourceWrapper<double>> sources) {
            _depthModulationSources = sources;
        }

        void setPhaseModulationSources(std::vector<WECore::ModulationSourceWrapper<double>> sources) {
            _phaseModulationSources = sources;
        }

    private:
        CloneableLFO(const CloneableLFO& other) {
            _wave = other._wave;
            _outputMode = other._outputMode;
            _indexOffset = other._indexOffset;

            _bypassSwitch = other._bypassSwitch;
            _tempoSyncSwitch = other._tempoSyncSwitch;
            _phaseSyncSwitch = other._phaseSyncSwitch;
            _invertSwitch = other._invertSwitch;
            _needsSeekOffsetCalc = other._needsSeekOffsetCalc;

            _tempoNumer = other._tempoNumer;
            _tempoDenom = other._tempoDenom;
            _rawFreq = other._rawFreq;
            _rawDepth = other._rawDepth;
            _manualPhase = other._manualPhase;
            _sampleRate = other._sampleRate;
            _bpm = other._bpm;
            _wavetablePosition = other._wavetablePosition;

            _waveArrayPointer = other._waveArrayPointer;

            _cachedOutput = other._cachedOutput;

            _freqModulationSources = other._freqModulationSources;
            _depthModulationSources = other._depthModulationSources;
            _phaseModulationSources = other._phaseModulationSources;
        }
    };

    class CloneableEnvelopeFollower : public WECore::AREnv::AREnvelopeFollowerSquareLaw {
    public:
        CloneableEnvelopeFollower() : WECore::AREnv::AREnvelopeFollowerSquareLaw() {}

        CloneableEnvelopeFollower* clone() const {
            return new CloneableEnvelopeFollower(*this);
        }

    private:
        CloneableEnvelopeFollower(const CloneableEnvelopeFollower& other) {
            _envVal = other._envVal;
            _attackTimeMs = other._attackTimeMs;
            _releaseTimeMs = other._releaseTimeMs;
            _attackCoef = other._attackCoef;
            _releaseCoef = other._releaseCoef;

            _filterEnabled = other._filterEnabled;
            _lowCutFilter = other._lowCutFilter.clone();
            _highCutFilter = other._highCutFilter.clone();

            _sampleRate = other._sampleRate;

            _cachedOutput = other._cachedOutput;
        }
    };
}
