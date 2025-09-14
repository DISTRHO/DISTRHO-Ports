#include "ProcessingInterface.hpp"

#include "SplitterProcessors.hpp"
#include "ModulationProcessors.hpp"

namespace ModelInterface {
    void prepareToPlay(StateManager& manager, double sampleRate, int samplesPerBlock, juce::AudioProcessor::BusesLayout layout) {
        WECore::AudioSpinLock lock(manager.sharedMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();
        ModulationSourcesState& sources = *manager.getSourcesStateUnsafe();

        ModulationProcessors::prepareToPlay(sources, sampleRate, samplesPerBlock, layout);

        if (splitter.splitter != nullptr) {
            SplitterProcessors::prepareToPlay(*splitter.splitter, sampleRate, samplesPerBlock, layout);
        }
    }

    void releaseResources(StateManager& manager) {
        WECore::AudioSpinLock lock(manager.sharedMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();

        if (splitter.splitter != nullptr) {
            SplitterProcessors::releaseResources(*splitter.splitter.get());
        }
    }

    void reset(StateManager& manager) {
        WECore::AudioSpinLock lock(manager.sharedMutex);
        SplitterState& splitter = manager.getSplitterStateUnsafe();
        ModulationSourcesState& sources = *manager.getSourcesStateUnsafe();

        ModulationProcessors::reset(sources);

        if (splitter.splitter != nullptr) {
            SplitterProcessors::reset(*splitter.splitter.get());
        }
    }

    void processBlock(StateManager& manager,
                      juce::AudioBuffer<float>& buffer,
                      juce::MidiBuffer& midiMessages,
                      juce::AudioPlayHead* newPlayHead,
                      juce::AudioPlayHead::CurrentPositionInfo tempoInfo) {
        // Use the try lock on the audio thread
        WECore::AudioSpinTryLock lock(manager.sharedMutex);

        if (lock.isLocked()) {
            SplitterState& splitter = manager.getSplitterStateUnsafe();
            ModulationSourcesState& sources = *manager.getSourcesStateUnsafe();

            // Advance the modulation sources
            // (the envelopes need to be done now before we overwrite the buffer)
            ModulationProcessors::processBlock(sources, buffer, tempoInfo);

            // TODO LFOs should still be advanced even if the lock is not acquired to stop them
            // drifting out of sync

            if (splitter.splitter != nullptr) {
                SplitterProcessors::processBlock(*splitter.splitter, buffer, midiMessages, newPlayHead);
            }
        }
    }

    double getLfoModulationValue(StateManager& manager, int lfoNumber) {
        // No locks here - they're called from ChainSlotProcessors::prepareToPlay so will already be locked
        return ModulationProcessors::getLfoModulationValue(*manager.getSourcesStateUnsafe(), lfoNumber);
    }

    double getEnvelopeModulationValue(StateManager& manager, int envelopeNumber) {
        // No locks here - they're called from ChainSlotProcessors::prepareToPlay so will already be locked
        return ModulationProcessors::getEnvelopeModulationValue(*manager.getSourcesStateUnsafe(), envelopeNumber);
    }

    double getRandomModulationValue(StateManager& manager, int randomNumber) {
        // No locks here - they're called from ChainSlotProcessors::prepareToPlay so will already be locked
        return ModulationProcessors::getRandomModulationValue(*manager.getSourcesStateUnsafe(), randomNumber);
    }
}
