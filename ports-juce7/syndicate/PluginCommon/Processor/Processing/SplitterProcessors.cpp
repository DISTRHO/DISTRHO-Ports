#include "SplitterProcessors.hpp"
#include "ChainProcessors.hpp"

namespace {
    void copyBuffer(juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& destination) {
        if (source.getNumSamples() == destination.getNumSamples()) {
            const int channelsToCopy {std::min(source.getNumChannels(), destination.getNumChannels())};

            // For each channel, copy each sample
            for (int channelIndex {0}; channelIndex < channelsToCopy; channelIndex++) {
                const float* readPointer = source.getReadPointer(channelIndex);
                float* writePointer = destination.getWritePointer(channelIndex);

                juce::FloatVectorOperations::copy(writePointer, readPointer, source.getNumSamples());
            }
        }
    }

    void addBuffers(juce::AudioBuffer<float>& source, juce::AudioBuffer<float>& destination) {
        if (source.getNumSamples() == destination.getNumSamples()) {
            const int channelsToCopy {std::min(source.getNumChannels(), destination.getNumChannels())};

            // For each channel, add each sample
            for (int channelIndex {0}; channelIndex < channelsToCopy; channelIndex++) {
                const float* readPointer = source.getReadPointer(channelIndex);
                float* writePointer = destination.getWritePointer(channelIndex);

                juce::FloatVectorOperations::add(writePointer, readPointer, source.getNumSamples());
            }
        }
    }

    void processBlockSeries(PluginSplitterSeries& splitter, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, juce::AudioPlayHead* newPlayHead) {
        ChainProcessors::processBlock(*(splitter.chains[0].chain.get()), buffer, midiMessages, newPlayHead);
    }

    void processBlockParallel(PluginSplitterParallel& splitter, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, juce::AudioPlayHead* newPlayHead) {
        splitter.outputBuffer->clear();

        // Crop the internal buffers in case the DAW has provided a buffer smaller than the specified block size in prepareToPlay
        juce::AudioBuffer<float> inputBufferCropped(splitter.inputBuffer->getArrayOfWritePointers(), splitter.inputBuffer->getNumChannels(), buffer.getNumSamples());
        juce::AudioBuffer<float> outputBufferCropped(splitter.outputBuffer->getArrayOfWritePointers(), splitter.outputBuffer->getNumChannels(), buffer.getNumSamples());

        for (PluginChainWrapper& chain : splitter.chains) {
            // Only process if no bands are soloed or this one is soloed
            if (splitter.numChainsSoloed == 0 || chain.isSoloed) {
                // Make a copy of the input buffer for us to process, preserving the original for the other
                // chains
                // TODO: do the same for midi
                copyBuffer(buffer, inputBufferCropped);

                // Process the newly copied buffer
                ChainProcessors::processBlock(*(chain.chain.get()), inputBufferCropped, midiMessages, newPlayHead);

                // Add the output of this chain to the output buffer
                addBuffers(inputBufferCropped, outputBufferCropped);
            }
        }

        // Overwrite the original buffer with our own output
        copyBuffer(outputBufferCropped, buffer);
    }

    void processBlockMultiband(PluginSplitterMultiband& splitter, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, juce::AudioPlayHead* newPlayHead) {
        splitter.fftProvider.processBlock(buffer);
        CrossoverProcessors::processBlock(*splitter.crossover.get(), buffer, midiMessages, newPlayHead);
    }

    void processBlockLeftRight(PluginSplitterLeftRight& splitter, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, juce::AudioPlayHead* newPlayHead) {
        // TODO: maybe this should be done using mono buffers? (depends if plugins can handle it reliably)

        // Make sure to clear the buffers each time, as on a previous call the plugins may have left
        // data in the unused channel of each buffer, and since it's not used it won't get implicitly
        // overwritten (but it'll still be copied to the output)
        splitter.leftBuffer->clear();
        splitter.rightBuffer->clear();

        // We only need to check the first two chains
        // (otherwise if the user is in parallel/multiband and soloes a chain above the first two
        // and then switches to left/right, the output will be muted)
        const bool isAnythingSoloed {splitter.chains[0].isSoloed || splitter.chains[1].isSoloed};
        const bool processLeftChain {!isAnythingSoloed || splitter.chains[0].isSoloed};
        const bool processRightChain {!isAnythingSoloed || splitter.chains[1].isSoloed};

        // Copy the left and right channels to separate buffers
        if (processLeftChain) {
            const float* leftRead {buffer.getReadPointer(0)};
            float* leftWrite {splitter.leftBuffer->getWritePointer(0)};
            juce::FloatVectorOperations::copy(leftWrite, leftRead, buffer.getNumSamples());
        }

        if (processRightChain) {
            const float* rightRead {buffer.getReadPointer(1)};
            float* rightWrite {splitter.rightBuffer->getWritePointer(1)};
            juce::FloatVectorOperations::copy(rightWrite, rightRead, buffer.getNumSamples());
        }

        // Now the input has been copied we can clear the original
        buffer.clear();

        // Process the left chain
        if (processLeftChain) {
            // Crop the internal buffer in case the DAW has provided a buffer smaller than the specified block size in prepareToPlay
            juce::AudioBuffer<float> leftBufferCropped(splitter.leftBuffer->getArrayOfWritePointers(), splitter.leftBuffer->getNumChannels(), buffer.getNumSamples());
            ChainProcessors::processBlock(*(splitter.chains[0].chain.get()), leftBufferCropped, midiMessages, newPlayHead);
            addBuffers(leftBufferCropped, buffer);
        }

        // Process the right chain
        if (processRightChain) {
            // Crop the internal buffer in case the DAW has provided a buffer smaller than the specified block size in prepareToPlay
            juce::AudioBuffer<float> rightBufferCropped(splitter.rightBuffer->getArrayOfWritePointers(), splitter.rightBuffer->getNumChannels(), buffer.getNumSamples());
            ChainProcessors::processBlock(*(splitter.chains[1].chain.get()), rightBufferCropped, midiMessages, newPlayHead);
            addBuffers(rightBufferCropped, buffer);
        }
    }

    void processBlockMidSide(PluginSplitterMidSide& splitter, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, juce::AudioPlayHead* newPlayHead) {
        // TODO: check if this can be done using mono buffers that refer to the original buffer rather
        // than copying to new ones (guest plugins don't seem to like mono buffers)

        // Make sure to clear the buffers each time, as on a previous call the plugins may have left
        // data in the unused channel of each buffer, and since it's not used it won't get implicitly
        // overwritten (but it'll still be copied to the output)
        splitter.midBuffer->clear();
        splitter.sideBuffer->clear();

        // Convert the left/right buffer to mid/side
        const float* leftRead {buffer.getReadPointer(0)};
        const float* rightRead {buffer.getReadPointer(1)};
        float* midWrite {splitter.midBuffer->getWritePointer(0)};
        float* sideWrite {splitter.sideBuffer->getWritePointer(0)};

        const int numSamples {buffer.getNumSamples()};

        // Add the right channel to get the mid, subtract it to get the side
        juce::FloatVectorOperations::add(midWrite, leftRead, rightRead, numSamples);
        juce::FloatVectorOperations::subtract(sideWrite, leftRead, rightRead, numSamples);

        // Multiply both by 0.5
        juce::FloatVectorOperations::multiply(midWrite, 0.5, numSamples);
        juce::FloatVectorOperations::multiply(sideWrite, 0.5, numSamples);

        // We only need to check the first two chains
        // (otherwise if the user is in parallel/multiband and soloes a chain above the first two
        // and then switches to mid/side, the output will be muted)
        const bool isAnythingSoloed {splitter.chains[0].isSoloed || splitter.chains[1].isSoloed};

        // Process the buffers
        if (!isAnythingSoloed || splitter.chains[0].isSoloed) {
            // Crop the internal buffer in case the DAW has provided a buffer smaller than the specified block size in prepareToPlay
            juce::AudioBuffer<float> midBufferCropped(splitter.midBuffer->getArrayOfWritePointers(), splitter.midBuffer->getNumChannels(), numSamples);
            ChainProcessors::processBlock(*(splitter.chains[0].chain.get()), midBufferCropped, midiMessages, newPlayHead);
        } else {
            // Mute the mid channel if only the other one is soloed
            juce::FloatVectorOperations::fill(midWrite, 0, numSamples);
        }

        if (!isAnythingSoloed || splitter.chains[1].isSoloed) {
            // Crop the internal buffer in case the DAW has provided a buffer smaller than the specified block size in prepareToPlay
            juce::AudioBuffer<float> sideBufferCropped(splitter.sideBuffer->getArrayOfWritePointers(), splitter.sideBuffer->getNumChannels(), numSamples);
            ChainProcessors::processBlock(*(splitter.chains[1].chain.get()), sideBufferCropped, midiMessages, newPlayHead);
        } else {
            // Mute the side channel if only the other one is soloed
            juce::FloatVectorOperations::fill(sideWrite, 0, numSamples);
        }

        // Convert from mid/side back to left/right, overwrite the original buffer with our own output
        float* leftWrite {buffer.getWritePointer(0)};
        float* rightWrite {buffer.getWritePointer(1)};
        const float* midRead {splitter.midBuffer->getReadPointer(0)};
        const float* sideRead {splitter.sideBuffer->getReadPointer(0)};

        // Add mid and side to get the left buffer, subtract them to get the right buffer
        juce::FloatVectorOperations::add(leftWrite, midRead, sideRead, numSamples);
        juce::FloatVectorOperations::subtract(rightWrite, midRead, sideRead, numSamples);
    }
}

namespace SplitterProcessors {
    void prepareToPlay(PluginSplitter& splitter, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout) {
        splitter.config.sampleRate = sampleRate;
        splitter.config.blockSize = samplesPerBlock;
        splitter.config.layout = layout;

        if (auto parallelSplitter = dynamic_cast<PluginSplitterParallel*>(&splitter)) {
            parallelSplitter->inputBuffer.reset(new juce::AudioBuffer<float>(getTotalNumInputChannels(layout), samplesPerBlock));
            parallelSplitter->outputBuffer.reset(new juce::AudioBuffer<float>(2, samplesPerBlock)); // stereo main
        } else if (auto multibandSplitter = dynamic_cast<PluginSplitterMultiband*>(&splitter)) {
            CrossoverProcessors::prepareToPlay(*multibandSplitter->crossover.get(), sampleRate, samplesPerBlock, layout);
            CrossoverProcessors::reset(*multibandSplitter->crossover.get());
            multibandSplitter->fftProvider.reset();
            multibandSplitter->fftProvider.setSampleRate(sampleRate);
            multibandSplitter->fftProvider.setIsStereo(canDoStereoSplitTypes(layout));
        } else if (auto leftRightSplitter = dynamic_cast<PluginSplitterLeftRight*>(&splitter)) {
            leftRightSplitter->leftBuffer.reset(new juce::AudioBuffer<float>(getTotalNumInputChannels(layout), samplesPerBlock));
            leftRightSplitter->rightBuffer.reset(new juce::AudioBuffer<float>(getTotalNumInputChannels(layout), samplesPerBlock));
        } else if (auto midSideSplitter = dynamic_cast<PluginSplitterMidSide*>(&splitter)) {
            midSideSplitter->midBuffer.reset(new juce::AudioBuffer<float>(getTotalNumInputChannels(layout), samplesPerBlock));
            midSideSplitter->sideBuffer.reset(new juce::AudioBuffer<float>(getTotalNumInputChannels(layout), samplesPerBlock));
        }

        for (PluginChainWrapper& chainWrapper : splitter.chains) {
            ChainProcessors::prepareToPlay(*chainWrapper.chain.get(), splitter.config);
        }
    }
    void releaseResources(PluginSplitter& splitter) {
        for (PluginChainWrapper& chainWrapper : splitter.chains) {
            ChainProcessors::releaseResources(*chainWrapper.chain.get());
        }
    }

    void reset(PluginSplitter& splitter) {
        for (PluginChainWrapper& chainWrapper : splitter.chains) {
            ChainProcessors::reset(*chainWrapper.chain.get());
        }
    }

    void processBlock(PluginSplitter& splitter,
                      juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages,
                      juce::AudioPlayHead* newPlayHead) {
        if (auto seriesSplitter = dynamic_cast<PluginSplitterSeries*>(&splitter)) {
            processBlockSeries(*seriesSplitter, buffer, midiMessages, newPlayHead);
        } else if (auto parallelSplitter = dynamic_cast<PluginSplitterParallel*>(&splitter)) {
            processBlockParallel(*parallelSplitter, buffer, midiMessages, newPlayHead);
        } else if (auto multibandSplitter = dynamic_cast<PluginSplitterMultiband*>(&splitter)) {
            processBlockMultiband(*multibandSplitter, buffer, midiMessages, newPlayHead);
        } else if (auto leftRightSplitter = dynamic_cast<PluginSplitterLeftRight*>(&splitter)) {
            processBlockLeftRight(*leftRightSplitter, buffer, midiMessages, newPlayHead);
        } else if (auto midSideSplitter = dynamic_cast<PluginSplitterMidSide*>(&splitter)) {
            processBlockMidSide(*midSideSplitter, buffer, midiMessages, newPlayHead);
        }
    }
}
