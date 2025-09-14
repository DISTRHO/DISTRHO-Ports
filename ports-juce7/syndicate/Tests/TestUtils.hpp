#pragma once

#include <JuceHeader.h>
#include "PluginConfigurator.hpp"

namespace TestUtils {
    // TODO remove
    inline juce::AudioProcessor::BusesLayout createLayoutWithInputChannels(juce::AudioChannelSet channelSet) {
        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add(channelSet);
        return layout;
    };

    inline juce::AudioProcessor::BusesLayout createLayoutWithChannels(juce::AudioChannelSet inputSet, juce::AudioChannelSet outputSet) {
        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add(inputSet);
        layout.outputBuses.add(outputSet);
        return layout;
    };

    // Default implementation that tests can build on top of
    class TestPluginInstance : public juce::AudioPluginInstance {
    public:
        TestPluginInstance() = default;
        virtual ~TestPluginInstance() = default;

        TestPluginInstance(BusesProperties supportedBuses) : juce::AudioPluginInstance(supportedBuses) {}

        virtual void fillInPluginDescription(juce::PluginDescription& desc) const { desc.name = "TestPlugin"; }
        virtual const juce::String getName() const { return "TestPlugin"; }
        virtual void prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {}
        virtual void releaseResources() {}
        virtual void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {}
        virtual double getTailLengthSeconds() const { return 0; }
        virtual bool acceptsMidi() const { return false; }
        virtual bool producesMidi() const { return false; }
        virtual juce::AudioProcessorEditor* createEditor() { return nullptr; }
        virtual bool hasEditor() const { return false; }
        virtual int getNumPrograms() { return 0; }
        virtual int getCurrentProgram() { return 0; }
        virtual void setCurrentProgram(int index) {}
        virtual const juce::String getProgramName(int index) { return ""; }
        virtual void changeProgramName(int index, const juce::String& newName) {}
        virtual void getStateInformation(juce::MemoryBlock& destData) { }
        virtual void setStateInformation(const void* data, int sizeInBytes) { }
    };
}
