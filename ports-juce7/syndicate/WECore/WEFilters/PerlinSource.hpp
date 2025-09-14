/*
 *  File:       PerlinSource.hpp
 *
 *  Created:    30/12/2024
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

#include <algorithm>
#include <random>
#include <vector>
#include "ModulationSource.h"
#include "General/ParameterDefinition.h"

namespace WECore::Perlin {
    inline float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    inline float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    inline float grad(int hash, float x) {
        // Keep only the last 4 bits
        const int h = hash & 15;

        // Randomly return a negtive if the top bit is 1
        if (h & 8) {
            return -x;
        } else {
            return x;
        }
    }

    inline float perlin1D(float x, const std::vector<int>& permutation) {
        // Get the indexes that our x is between
        const int x0 = static_cast<int>(std::floor(x)) & 255;
        const int x1 = (x0 + 1) & 255;

        // Look up the indexes
        const int g0 = permutation[x0];
        const int g1 = permutation[x1];

        // Get the distance of x from the lower index
        const float xf = x - floor(x);

        // Get the gradients at our position x for each index
        const float dot0 = grad(g0, xf);
        const float dot1 = grad(g1, xf - 1.0f);

        // Interpolate between gradient contributions
        const float u = fade(xf);
        return lerp(dot0, dot1, u);
    }

    namespace Parameters {
        const ParameterDefinition::RangedParameter<double> FREQ(0, 20, 1),
                                                           DEPTH(0, 1, 0.5);

        class OutputModeParameter : public ParameterDefinition::BaseParameter<int> {
        public:
            OutputModeParameter() : ParameterDefinition::BaseParameter<int>(UNIPOLAR, BIPOLAR, BIPOLAR) {}

            static constexpr int UNIPOLAR {1},
                                 BIPOLAR {2};
        };

        const OutputModeParameter OUTPUTMODE;
    }

    class PerlinSource : public ModulationSource<double> {
    public:
        PerlinSource() : _outputMode(Parameters::OUTPUTMODE.defaultValue),
                         _rawFreq(Parameters::FREQ.defaultValue),
                         _modulatedFreqValue(0),
                         _rawDepth(Parameters::DEPTH.defaultValue),
                         _modulatedDepthValue(0),
                         _sampleRate(44100),
                         _wavePosition(0) {
            // Initialize the permutation vector with the default permutation
            _permutation = {
                151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
                140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
                247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
                 57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
                 74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
                 60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
                 65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
                200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
                 52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
                207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
                119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
                129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
                218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
                 81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
                184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
                222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180
            };
        }

        virtual ~PerlinSource() override = default;

        void setFreq(double freq) { _rawFreq = Parameters::FREQ.BoundsCheck(freq); }
        void setDepth(double depth) { _rawDepth = Parameters::DEPTH.BoundsCheck(depth); }
        void setOutputMode(int val) { _outputMode = Parameters::OUTPUTMODE.BoundsCheck(val); }
        void setSampleRate(double val) { _sampleRate = val; }

        double getFreq() const { return _rawFreq; }
        double getModulatedFreqValue() const { return _modulatedFreqValue; }
        double getDepth() const { return _rawDepth; }
        double getModulatedDepthValue() const { return _modulatedDepthValue; }
        int getOutputMode() { return _outputMode; }

        inline bool addFreqModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool removeFreqModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool setFreqModulationAmount(size_t index, double amount);
        void setFreqModulationSources(std::vector<WECore::ModulationSourceWrapper<double>> sources) { _freqModulationSources = sources; }
        std::vector<ModulationSourceWrapper<double>> getFreqModulationSources() const { return _freqModulationSources; }

        inline bool addDepthModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool removeDepthModulationSource(std::shared_ptr<ModulationSource> source);
        inline bool setDepthModulationAmount(size_t index, double amount);
        void setDepthModulationSources(std::vector<WECore::ModulationSourceWrapper<double>> sources) { _depthModulationSources = sources; }
        std::vector<ModulationSourceWrapper<double>> getDepthModulationSources() const { return _depthModulationSources; }

        /**
         * Returns a pointer to a deep copy of this instance, including all state. The caller is
         * responsible for managing the lifetime of the returned object.
         */
        PerlinSource* clone() const {
            return new PerlinSource(*this);
        }

    protected:
        int _outputMode;

        double _rawFreq,
               _modulatedFreqValue,
               _rawDepth,
               _modulatedDepthValue,
               _sampleRate,
               _wavePosition;

        std::vector<ModulationSourceWrapper<double>> _freqModulationSources,
                                                     _depthModulationSources;

        std::vector<int> _permutation;

        inline double _getNextOutputImpl(double inSample) override;

        void _resetImpl() override { _wavePosition = 0; }

    private:
        // Used when cloning
        PerlinSource(const PerlinSource& other) {
            _outputMode = other._outputMode;

            _rawFreq = other._rawFreq;
            _rawDepth = other._rawDepth;
            _sampleRate = other._sampleRate;
            _wavePosition = other._wavePosition;

            _freqModulationSources = other._freqModulationSources;
            _depthModulationSources = other._depthModulationSources;

            _permutation = other._permutation;
        }
    };

    bool PerlinSource::addFreqModulationSource(std::shared_ptr<ModulationSource> source) {
        // Check if the source is already in the list
        for (const ModulationSourceWrapper<double>& existingSource : _freqModulationSources) {
            if (existingSource.source == source) {
                return false;
            }
        }

        _freqModulationSources.push_back({source, 0});
        return true;
    }

    bool PerlinSource::removeFreqModulationSource(std::shared_ptr<ModulationSource> source) {
        for (auto it = _freqModulationSources.begin(); it != _freqModulationSources.end(); it++) {
            if ((*it).source == source) {
                _freqModulationSources.erase(it);
                return true;
            }
        }

        return false;
    }

    bool PerlinSource::setFreqModulationAmount(size_t index, double amount) {
        if (index >= _freqModulationSources.size()) {
            return false;
        }

        _freqModulationSources[index].amount = amount;
        return true;
    }

    bool PerlinSource::addDepthModulationSource(std::shared_ptr<ModulationSource> source) {
        // Check if the source is already in the list
        for (const ModulationSourceWrapper<double>& existingSource : _depthModulationSources) {
            if (existingSource.source == source) {
                return false;
            }
        }

        _depthModulationSources.push_back({source, 0});
        return true;
    }

    bool PerlinSource::removeDepthModulationSource(std::shared_ptr<ModulationSource> source) {
        for (auto it = _depthModulationSources.begin(); it != _depthModulationSources.end(); it++) {
            if ((*it).source == source) {
                _depthModulationSources.erase(it);
                return true;
            }
        }

        return false;
    }

    bool PerlinSource::setDepthModulationAmount(size_t index, double amount) {
        if (index >= _depthModulationSources.size()) {
            return false;
        }

        _depthModulationSources[index].amount = amount;
        return true;
    }

    double PerlinSource::_getNextOutputImpl(double /*inSample*/) {
        // Get the mod amount to use, divide by 2 to reduce range to -0.5:0.5
        const double freqModValue {calcModValue(_freqModulationSources) / 2};
        const double depthModValue {calcModValue(_depthModulationSources) / 2};

        // Calculate the frequency value after modulation
        const double freq {Parameters::FREQ.BoundsCheck(
            _rawFreq + (Parameters::FREQ.maxValue * freqModValue)
        )};
        _modulatedFreqValue = freq;

        // Calculate the depth value after modulation
        const double depth {Parameters::DEPTH.BoundsCheck(
            _rawDepth + (Parameters::DEPTH.maxValue * depthModValue)
        )};
        _modulatedDepthValue = depth;

        // Calculate the Perlin noise value
        double noiseValue {perlin1D(_wavePosition, _permutation)};
        _wavePosition += (_modulatedFreqValue / 2) / _sampleRate;

        noiseValue = _outputMode == Parameters::OUTPUTMODE.BIPOLAR ? noiseValue : (noiseValue + 0.5);

        return noiseValue * _modulatedDepthValue;
    }
}
