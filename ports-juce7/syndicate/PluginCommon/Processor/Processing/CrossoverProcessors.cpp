#include "CrossoverProcessors.hpp"
#include "ChainProcessors.hpp"

namespace CrossoverProcessors {
    void prepareToPlay(CrossoverState& state, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout) {
        // We don't filter sidechain channels but do copy them to each buffer - so filters may need less channels than the buffers do
        const int numFilterChannels {canDoStereoSplitTypes(layout) ? 2 : 1};
        for (std::shared_ptr<CloneableLRFilter<float>>& filter : state.lowpassFilters) {
            filter->prepare({sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(numFilterChannels)});
        }

        for (std::shared_ptr<CloneableLRFilter<float>>& filter : state.highpassFilters) {
            filter->prepare({sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(numFilterChannels)});
        }

        for (std::shared_ptr<CloneableLRFilter<float>>& filter : state.allpassFilters) {
            filter->prepare({sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(numFilterChannels)});
        }

        for (juce::AudioBuffer<float>& buffer : state.buffers) {
            buffer.setSize(getTotalNumInputChannels(layout), samplesPerBlock);
        }

        state.config.sampleRate = sampleRate;
        state.config.blockSize = samplesPerBlock;
        state.config.layout = layout;
    }

    void reset(CrossoverState& state) {
        for (std::shared_ptr<CloneableLRFilter<float>>& filter : state.lowpassFilters) {
            filter->reset();
        }

        for (std::shared_ptr<CloneableLRFilter<float>>& filter : state.highpassFilters) {
            filter->reset();
        }

        for (std::shared_ptr<CloneableLRFilter<float>>& filter : state.allpassFilters) {
            filter->reset();
        }

        for (juce::AudioBuffer<float>& buffer : state.buffers) {
            buffer.clear();
        }
    }

    void processBlock(CrossoverState& state, juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, juce::AudioPlayHead* newPlayHead) {
        const int numFilterChannels {canDoStereoSplitTypes(state.config.layout) ? 2 : 1};
        const size_t numCrossovers {state.bands.size() - 1};

        // First split everything into bands and apply the processing
        for (int crossoverNumber {0}; crossoverNumber < numCrossovers; crossoverNumber++) {
            // We need to make a copy of the input buffer before processing
            // lowBuffer will be lowpassed, highBuffer will be highpassed
            juce::AudioBuffer<float>& lowBuffer = crossoverNumber == 0 ? buffer : state.buffers[crossoverNumber - 1];
            juce::AudioBuffer<float>& highBuffer = state.buffers[crossoverNumber];

            highBuffer.makeCopyOf(lowBuffer);

            {
                juce::dsp::AudioBlock<float> block(juce::dsp::AudioBlock<float>(lowBuffer).getSubsetChannelBlock(0, numFilterChannels));
                juce::dsp::ProcessContextReplacing context(block);
                state.lowpassFilters[crossoverNumber]->process(context);

                // Crop the internal buffer in case the DAW has provided a buffer smaller than the specified block size in prepareToPlay
                juce::AudioBuffer<float> lowBufferCropped(lowBuffer.getArrayOfWritePointers(), lowBuffer.getNumChannels(), buffer.getNumSamples());
                ChainProcessors::processBlock(*state.bands[crossoverNumber].chain.get(), lowBufferCropped, midiMessages, newPlayHead);
            }

            {
                juce::dsp::AudioBlock<float> block(juce::dsp::AudioBlock<float>(highBuffer).getSubsetChannelBlock(0, numFilterChannels));
                juce::dsp::ProcessContextReplacing context(block);
                state.highpassFilters[crossoverNumber]->process(context);

                // If this is the last band we need to apply the processing
                if (crossoverNumber + 1 == numCrossovers) {
                    // Crop the internal buffer in case the DAW has provided a buffer smaller than the specified block size in prepareToPlay
                    juce::AudioBuffer<float> highBufferCropped(highBuffer.getArrayOfWritePointers(), highBuffer.getNumChannels(), buffer.getNumSamples());
                    ChainProcessors::processBlock(*state.bands[crossoverNumber + 1].chain.get(), highBufferCropped, midiMessages, newPlayHead);
                }
            }
        }


        // Finally add the bands back together
        if (state.numBandsSoloed > 0 && !state.bands[0].isSoloed) {
            buffer.clear();
        }

        for (int crossoverNumber {0}; crossoverNumber < numCrossovers; crossoverNumber++) {
            // If there is another crossover after this one, we need to use an allpass to rotate the phase of the lower bands
            if (crossoverNumber + 1 < numCrossovers) {
                juce::dsp::AudioBlock<float> block(juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(0, numFilterChannels));
                juce::dsp::ProcessContextReplacing context(block);
                state.allpassFilters[crossoverNumber]->process(context);

            }

            if (state.numBandsSoloed == 0 || state.bands[crossoverNumber + 1].isSoloed) {
                for (int channelNumber {0}; channelNumber < numFilterChannels; channelNumber++) {
                    buffer.addFrom(channelNumber, 0, state.buffers[crossoverNumber], channelNumber, 0, buffer.getNumSamples());
                }
            }
        }
    }
}
