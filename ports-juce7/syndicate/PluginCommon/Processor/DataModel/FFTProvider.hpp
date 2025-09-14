#pragma once

#include <JuceHeader.h>

#include "WEFilters/EffectsProcessor.h"
#include "WEFilters/AREnvelopeFollowerSquareLaw.h"
#include "General/AudioSpinMutex.h"

/**
 * Performs an FFT on the signal which can be provided to the UI to drive the visualiser.
 */
class FFTProvider {
public:
    static constexpr int FFT_ORDER {9};
    static constexpr int FFT_SIZE {(1 << FFT_ORDER) * 2};
    static constexpr int NUM_OUTPUTS { FFT_SIZE / 4 };

    FFTProvider();
    ~FFTProvider();

    void setSampleRate(double sampleRate);

    void reset();

    void processBlock(juce::AudioBuffer<float>& buffer);

    const float* getOutputs() { return _outputs; }

    void setIsStereo(bool val) { _isStereo = val; }

    float getBinWidth() const { return _binWidth; }

private:
    float* _inputBuffer;
    float* _fftBuffer;
    float* _outputs;
    juce::dsp::FFT _fft;
    std::array<WECore::AREnv::AREnvelopeFollowerSquareLaw, NUM_OUTPUTS> _envs;
    WECore::AudioSpinMutex _fftMutex;
    bool _isStereo;
    float _binWidth;
};
