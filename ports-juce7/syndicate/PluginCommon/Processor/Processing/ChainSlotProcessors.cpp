#include "ChainSlotProcessors.hpp"

#include <assert.h>
#include "PluginUtils.h"

namespace {
    void applyModulationForParamter(ChainSlotPlugin& slot,
                                    juce::AudioProcessorParameter* targetParameter,
                                    const std::shared_ptr<PluginParameterModulationConfig> parameterConfig) {
        // Start from the rest value
        float paramValue {parameterConfig->restValue};

        for (const auto source : parameterConfig->sources) {
            // Add the modulation from each source
            const float valueForThisSource {
                slot.getModulationValueCallback(source->definition.id, source->definition.type)
                    * source->modulationAmount
            };

            paramValue += valueForThisSource;
        }

        // Assign the value to the parameter
        targetParameter->setValue(paramValue);
    }
}

namespace ChainProcessors {
    void prepareToPlay(ChainSlotGainStage& gainStage, HostConfiguration config) {
        gainStage.numMainChannels = config.layout.getMainInputChannels();
        assert(gainStage.numMainChannels <= 2);

        for (auto& env : gainStage.meterEnvelopes) {
            env.setSampleRate(config.sampleRate);
        }
    }

    void releaseResources(ChainSlotGainStage& /*gainStage*/) {
        // Do nothing
    }

    void reset(ChainSlotGainStage& gainStage) {
        for (auto& env : gainStage.meterEnvelopes) {
            env.reset();
        }
    }

    void processBlock(ChainSlotGainStage& gainStage, juce::AudioBuffer<float>& buffer) {
        if (!gainStage.isBypassed) {
            // Apply gain
            for (int channel {0}; channel < gainStage.numMainChannels; channel++) {
                juce::FloatVectorOperations::multiply(buffer.getWritePointer(channel),
                                                      gainStage.gain,
                                                      buffer.getNumSamples());
            }

            if (gainStage.numMainChannels == 2) {
                // Stereo input - apply balance
                Utils::processBalance(gainStage.pan, buffer);
            }
        }

        // Update the envelope follower
        for (int sampleIndex {0}; sampleIndex < buffer.getNumSamples(); sampleIndex++) {
            for (int channel {0}; channel < gainStage.numMainChannels; channel++) {
                gainStage.meterEnvelopes[channel].getNextOutput(buffer.getReadPointer(channel)[sampleIndex]);
            }
        }
    }

    void prepareToPlay(ChainSlotPlugin& slot, HostConfiguration config) {
        slot.plugin->setRateAndBufferSizeDetails(config.sampleRate, config.blockSize);
        slot.plugin->prepareToPlay(config.sampleRate, config.blockSize);
        slot.spareSCBuffer.reset(new juce::AudioBuffer<float>(config.layout.getMainInputChannels() * 2, config.blockSize));
    }

    void releaseResources(ChainSlotPlugin& slot) {
        slot.plugin->releaseResources();
    }

    void reset(ChainSlotPlugin& slot) {
        slot.plugin->reset();
        slot.spareSCBuffer->clear();
    }

    void processBlock(ChainSlotPlugin& slot,
                      juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages,
                      juce::AudioPlayHead* newPlayHead) {
        if (newPlayHead != nullptr) {
            slot.plugin->setPlayHead(newPlayHead);
        }

        if (!slot.isBypassed) {
            // Apply parameter modulation
            if (slot.modulationConfig->isActive) {
                // TODO we need a faster way of retrieving parameters
                // Try again to get PluginParameterModulationConfig to hold a pointer to the parameter directly
                const juce::Array<juce::AudioProcessorParameter*>& parameters = slot.plugin->getParameters();
                for (const auto parameterConfig : slot.modulationConfig->parameterConfigs) {
                    for (juce::AudioProcessorParameter* targetParameter : parameters) {
                        if (targetParameter->getName(PluginParameterModulationConfig::PLUGIN_PARAMETER_NAME_LENGTH_LIMIT) == parameterConfig->targetParameterName) {
                            applyModulationForParamter(slot, targetParameter, parameterConfig);
                        }
                    }
                }
            }

            const int numPluginInputs {getTotalNumInputChannels(slot.plugin->getBusesLayout())};

            const bool useSpareSidechainBuffer = {
                buffer.getNumChannels() < numPluginInputs && slot.spareSCBuffer->getNumChannels() == numPluginInputs
            };

            if (useSpareSidechainBuffer) {
                // Copy from the real input buffer into our buffer with the correct number of channels
                for (int channelIndex {0}; channelIndex < buffer.getNumChannels(); channelIndex++) {
                    slot.spareSCBuffer->copyFrom(channelIndex, 0, buffer, channelIndex, 0, buffer.getNumSamples());
                }

                // Do processing
                slot.plugin->processBlock(*slot.spareSCBuffer, midiMessages);

                // Copy back to the real buffer
                for (int channelIndex {0}; channelIndex < buffer.getNumChannels(); channelIndex++) {
                    buffer.copyFrom(channelIndex, 0, *slot.spareSCBuffer, channelIndex, 0, buffer.getNumSamples());
                }
            } else {
                // Do processing
                slot.plugin->processBlock(buffer, midiMessages);
            }
        }
    }
}
