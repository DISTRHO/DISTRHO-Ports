#pragma once

#include <JuceHeader.h>
#include "WEFilters/AREnvelopeFollowerSquareLaw.h"
#include "ModulationSourceDefinition.hpp"
#include "PluginConfigurator.hpp"

struct ChainSlotBase {
    bool isBypassed;

    ChainSlotBase(bool newIsBypassed) : isBypassed(newIsBypassed) {}
    virtual ~ChainSlotBase() = default;

    virtual ChainSlotBase* clone() const = 0;
};

/**
 * Represents a gain stage in a slot in a processing chain.
 */
struct ChainSlotGainStage : ChainSlotBase {
    // Linear 0 to 1 (or a little more) values
    float gain;

    // -1 to 1 values
    float pan;

    int numMainChannels;
    std::array<WECore::AREnv::AREnvelopeFollowerSquareLaw, 2> meterEnvelopes;

    ChainSlotGainStage(float newGain, float newPan, bool newIsBypassed, const juce::AudioProcessor::BusesLayout& busesLayout)
        : ChainSlotBase(newIsBypassed), gain(newGain), pan(newPan), numMainChannels(busesLayout.getMainInputChannels()) {
        _setUpEnvelopes();
    }

    ~ChainSlotGainStage() = default;

    ChainSlotGainStage* clone() const override {
        return new ChainSlotGainStage(gain, pan, isBypassed, numMainChannels);
    }

private:
    void _setUpEnvelopes() {
        for (auto& env : meterEnvelopes) {
            env.setAttackTimeMs(1);
            env.setReleaseTimeMs(50);
            env.setFilterEnabled(false);
        }
    }

    ChainSlotGainStage(
        float newGain,
        float newPan,
        bool newIsBypassed,
        int newNumMainChannels)
            : ChainSlotBase(newIsBypassed), gain(newGain), pan(newPan), numMainChannels(newNumMainChannels) {
        _setUpEnvelopes();
    }
};

struct PluginParameterModulationSource {
    // Definition of the modulation source
    ModulationSourceDefinition definition;

    // Amount of modulation to be applied (-1 : 1)
    float modulationAmount;

    PluginParameterModulationSource() : definition(0, MODULATION_TYPE::MACRO), modulationAmount(0) { }

    PluginParameterModulationSource(ModulationSourceDefinition newDefinition,
                                    float newModulationAmount) :
            definition(newDefinition),
            modulationAmount(newModulationAmount) { }
};

struct PluginParameterModulationConfig {
    // Name of the parameter being modulated
    juce::String targetParameterName;

    // Parameter value without modulation applied (0 : 1)
    float restValue;

    // All the sources being provided for this parameter
    std::vector<std::shared_ptr<PluginParameterModulationSource>> sources;

    // Used when retrieving the parameter name from a juce::AudioProcessorParameter
    static constexpr int PLUGIN_PARAMETER_NAME_LENGTH_LIMIT {30};

    PluginParameterModulationConfig() : restValue(0) {}

    PluginParameterModulationConfig* clone() const {
        auto newConfig = new PluginParameterModulationConfig();
        newConfig->targetParameterName = targetParameterName;
        newConfig->restValue = restValue;

        for (auto& source : sources) {
            newConfig->sources.push_back(std::make_shared<PluginParameterModulationSource>(source->definition, source->modulationAmount));
        }

        return newConfig;
    }
};

struct PluginModulationConfig {
    bool isActive;
    std::vector<std::shared_ptr<PluginParameterModulationConfig>> parameterConfigs;

    PluginModulationConfig() : isActive(false) {}

    PluginModulationConfig& operator=(const PluginModulationConfig& other) {
        isActive = other.isActive;
        parameterConfigs = other.parameterConfigs;

        return *this;
    }

    PluginModulationConfig* clone() const {
        auto newConfig = new PluginModulationConfig();
        newConfig->isActive = isActive;

        for (auto& parameterConfig : parameterConfigs) {
            newConfig->parameterConfigs.push_back(std::shared_ptr<PluginParameterModulationConfig>(parameterConfig->clone()));
        }

        return newConfig;
    }
};

struct PluginEditorBoundsContainer {
    juce::Rectangle<int> editorBounds;
    juce::Rectangle<int> displayArea;

    PluginEditorBoundsContainer(
        juce::Rectangle<int> newEditorBounds,
        juce::Rectangle<int> newDisplayAreaBounds) : editorBounds(newEditorBounds),
                                                     displayArea(newDisplayAreaBounds) { }
};

typedef std::optional<PluginEditorBoundsContainer> PluginEditorBounds;

/**
 * Represents a plugin in a slot in a processing chain.
 */
struct ChainSlotPlugin : ChainSlotBase {
    std::shared_ptr<juce::AudioPluginInstance> plugin;
    std::shared_ptr<PluginModulationConfig> modulationConfig;
    std::function<float(int, MODULATION_TYPE)> getModulationValueCallback;
    std::shared_ptr<PluginEditorBounds> editorBounds;

    // AUs always require a sidechain input, but if Syndicate is loaded as a VST3 it might not have
    // one, as VST3s don't require a sidechain input. So we keep this empty buffer to provide as a
    // sidechain for AUs if needed.
    std::unique_ptr<juce::AudioBuffer<float>> spareSCBuffer;

    ChainSlotPlugin(std::shared_ptr<juce::AudioPluginInstance> newPlugin,
                    bool newIsBypassed,
                    std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
                    HostConfiguration config)
        : ChainSlotBase(newIsBypassed),
          plugin(newPlugin),
          modulationConfig(std::make_shared<PluginModulationConfig>()),
          getModulationValueCallback(newGetModulationValueCallback),
          editorBounds(new PluginEditorBounds()),
          spareSCBuffer(new juce::AudioBuffer<float>(config.layout.getMainInputChannels() * 2, config.blockSize)) {}

    ~ChainSlotPlugin() = default;

    ChainSlotPlugin* clone() const override {
        auto newSpareSCBuffer = std::make_unique<juce::AudioBuffer<float>>(spareSCBuffer->getNumChannels(), spareSCBuffer->getNumSamples());
        return new ChainSlotPlugin(plugin, isBypassed, modulationConfig, getModulationValueCallback, editorBounds, std::move(newSpareSCBuffer));
    }

private:
    ChainSlotPlugin(
        std::shared_ptr<juce::AudioPluginInstance> newPlugin,
        bool newIsBypassed,
        std::shared_ptr<PluginModulationConfig> newModulationConfig,
        std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
        std::shared_ptr<PluginEditorBounds> newEditorBounds,
        std::unique_ptr<juce::AudioBuffer<float>> newSpareSCBuffer)
            : ChainSlotBase(newIsBypassed),
              plugin(newPlugin),
              modulationConfig(std::shared_ptr<PluginModulationConfig>(newModulationConfig->clone())),
              getModulationValueCallback(newGetModulationValueCallback),
              editorBounds(newEditorBounds),
              spareSCBuffer(std::move(newSpareSCBuffer)) {}
};
