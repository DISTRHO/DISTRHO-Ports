#include "FFTProvider.hpp"

FFTProvider::FFTProvider() : _inputBuffer(new float[FFT_SIZE]),
                             _fftBuffer(new float[FFT_SIZE]),
                             _outputs(new float[NUM_OUTPUTS]),
                             _fft(FFT_ORDER),
                             _isStereo(false),
                             _binWidth(0) {
    juce::FloatVectorOperations::fill(_inputBuffer, 0, FFT_SIZE);
    juce::FloatVectorOperations::fill(_fftBuffer, 0, FFT_SIZE);
    juce::FloatVectorOperations::fill(_outputs, 0, NUM_OUTPUTS);

    for (auto& env : _envs) {
        env.setAttackTimeMs(0.1);
        env.setReleaseTimeMs(0.5);
        env.setFilterEnabled(false);
    }
}

FFTProvider::~FFTProvider() {
    WECore::AudioSpinLock lock(_fftMutex);
    delete[] _inputBuffer;
    delete[] _fftBuffer;
    delete[] _outputs;
}

void FFTProvider::setSampleRate(double sampleRate) {
    _binWidth = (sampleRate / 2) / NUM_OUTPUTS;

    for (auto& env : _envs) {
        env.setSampleRate(sampleRate);
    }
}

void FFTProvider::reset() {
    for (auto& env : _envs) {
        env.reset();
    }
}

void FFTProvider::processBlock(juce::AudioBuffer<float>& buffer) {
    WECore::AudioSpinTryLock lock(_fftMutex);
    if (lock.isLocked()) {
        const size_t numBuffersRequired {static_cast<size_t>(
            std::ceil(static_cast<double>(buffer.getNumSamples()) / FFT_SIZE)
        )};

        for (size_t bufferNumber {0}; bufferNumber < numBuffersRequired; bufferNumber++) {
            // Calculate how many samples need to be processed in this chunk
            const size_t numSamplesRemaining {buffer.getNumSamples() - (bufferNumber * FFT_SIZE)};
            const size_t numSamplesToCopy {
                std::min(numSamplesRemaining, static_cast<size_t>(FFT_SIZE))
            };

            // The input buffer size might be smaller than the FFT buffer size, so then we need to
            // append the the existing FFT buffer by shifting it back and adding the new samples to
            // the end
            juce::FloatVectorOperations::copy(_inputBuffer, _inputBuffer + numSamplesToCopy, FFT_SIZE - numSamplesToCopy);

            float* const fillStart {_inputBuffer + FFT_SIZE - numSamplesToCopy};
            if (_isStereo) {
                // Add the left and right buffers
                const float* const leftBufferInputStart {buffer.getReadPointer(0) + bufferNumber * FFT_SIZE};
                const float* const rightBufferInputStart {buffer.getReadPointer(1) + bufferNumber * FFT_SIZE};
                juce::FloatVectorOperations::add(fillStart, leftBufferInputStart, rightBufferInputStart, numSamplesToCopy);
                juce::FloatVectorOperations::multiply(fillStart, 0.5, numSamplesToCopy);
            } else {
                const float* const bufferInputStart {buffer.getReadPointer(0) + bufferNumber * FFT_SIZE};
                juce::FloatVectorOperations::copy(fillStart, bufferInputStart, numSamplesToCopy);
            }

            // Perform the FFT
            juce::FloatVectorOperations::copy(_fftBuffer, _inputBuffer, FFT_SIZE);
            _fft.performFrequencyOnlyForwardTransform(_fftBuffer);

            // Run each FFT output bin through an envelope follower so that it is smoothed when
            // displayed on the UI
            for (int index {0}; index < NUM_OUTPUTS; index++) {
                _outputs[index] = _envs[index].getNextOutput(_fftBuffer[index]);
            }
        }
    }
}