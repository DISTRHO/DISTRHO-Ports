#pragma once

#include <memory>
#include <deque>
#include "PluginSplitter.hpp"
#include "General/AudioSpinMutex.h"
#include "SplitterProcessors.hpp"
#include "CloneableSources.hpp"
#include "WEFilters/PerlinSource.hpp"

namespace ModelInterface {
    struct SplitterState {
        // Internal representation of the data model
        std::shared_ptr<PluginSplitter> splitter;

        // We store the crossover frequencies so they can be restored if the user switches from a
        // multiband split to another type and back again
        std::optional<std::vector<float>> cachedcrossoverFrequencies;

        SplitterState(HostConfiguration config,
                 std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                 std::function<void(int)> latencyChangeCallback) {
            splitter.reset(
                new PluginSplitterSeries(config, getModulationValueCallback, latencyChangeCallback)
            );

            SplitterProcessors::prepareToPlay(*splitter.get(), config.sampleRate, config.blockSize, config.layout);
        }

        SplitterState* clone() const {
            return new SplitterState(*this);
        }

    private:
        SplitterState(const SplitterState& other) : splitter(other.splitter->clone()) {
            if (other.cachedcrossoverFrequencies.has_value()) {
                cachedcrossoverFrequencies = other.cachedcrossoverFrequencies;
            }
        }
    };

    struct EnvelopeWrapper {
        std::shared_ptr<CloneableEnvelopeFollower> envelope;
        float amount;
        bool useSidechainInput;

        EnvelopeWrapper() : envelope(new CloneableEnvelopeFollower()), amount(0), useSidechainInput(false) { }

        EnvelopeWrapper* clone() const {
            return new EnvelopeWrapper(*this);
        }

    private:
        EnvelopeWrapper(const EnvelopeWrapper& other) : envelope(other.envelope->clone()), amount(other.amount), useSidechainInput(other.useSidechainInput) {
        }
    };

    struct ModulationSourcesState {
        std::vector<std::shared_ptr<CloneableLFO>> lfos;
        std::vector<std::shared_ptr<EnvelopeWrapper>> envelopes;
        std::vector<std::shared_ptr<WECore::Perlin::PerlinSource>> randomSources;

        // Needed for the envelope followers to figure out which buffers to read from
        HostConfiguration hostConfig;

        std::function<float(int, MODULATION_TYPE)> getModulationValueCallback;

        ModulationSourcesState(std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback) :
            getModulationValueCallback(newGetModulationValueCallback) { }

        ModulationSourcesState* clone() const {
            return new ModulationSourcesState(*this);
        }

    private:
        ModulationSourcesState(const ModulationSourcesState& other) : hostConfig(other.hostConfig), getModulationValueCallback(other.getModulationValueCallback) {
            for (std::shared_ptr<CloneableLFO> lfo : other.lfos) {
                lfos.emplace_back(lfo->clone());
            }

            for (std::shared_ptr<EnvelopeWrapper> env : other.envelopes) {
                envelopes.emplace_back(env->clone());
            }

            for (std::shared_ptr<WECore::Perlin::PerlinSource> source : other.randomSources) {
                randomSources.emplace_back(source->clone());
            }
        }
    };

    struct StateWrapper {
        std::shared_ptr<SplitterState> splitterState;
        std::shared_ptr<ModulationSourcesState> modulationSourcesState;

        // String representation of the operation that was performed to get to this state
        juce::String operation;

        StateWrapper(HostConfiguration config,
                     std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                     std::function<void(int)> latencyChangeCallback) :
                splitterState(new SplitterState(config, getModulationValueCallback, latencyChangeCallback)),
                modulationSourcesState(new ModulationSourcesState(getModulationValueCallback)),
                operation("") { }

        StateWrapper(std::shared_ptr<SplitterState> newSplitterState,
                     std::shared_ptr<ModulationSourcesState> newModulationSourcesState,
                     juce::String newOperation) : splitterState(newSplitterState),
                                                  modulationSourcesState(newModulationSourcesState),
                                                  operation(newOperation) { }
    };

    struct StateManager {
        std::deque<std::shared_ptr<StateWrapper>> undoHistory;
        std::deque<std::shared_ptr<StateWrapper>> redoHistory;

        // This mutex must be locked by all mutators before attempting to read or write to or from
        // data model. Its purpose is to stop a mutator being called on one thread from changing the
        // data model in a way that would crash a mutator being called on another thread.
        //
        // Recursive because the application code may call something via the mutator forEach methods
        // that tries to take the lock on the same thread as the forEach method itself
        std::recursive_mutex mutatorsMutex;

        // This mutex must be locked by mutators which change the structure of the data model, and
        // also by the processors. Its purpose is to stop a mutator being called on one thread from
        // changing the data model in a way that would crash a processor being called on another
        // thread.
        //
        // Mutators reading from the data model or writing only primitive values don't need to lock
        // this
        WECore::AudioSpinMutex sharedMutex;


        StateManager(HostConfiguration config,
                     std::function<float(int, MODULATION_TYPE)> getModulationValueCallback,
                     std::function<void(int)> latencyChangeCallback) {
            undoHistory.push_back(std::make_shared<StateWrapper>(config, getModulationValueCallback, latencyChangeCallback));
        }

        // TODO remove this once all functions are migrated
        SplitterState& getSplitterStateUnsafe() { return *(undoHistory.back()->splitterState); }
        std::shared_ptr<ModulationSourcesState> getSourcesStateUnsafe() { return undoHistory.back()->modulationSourcesState; }
    };
}
