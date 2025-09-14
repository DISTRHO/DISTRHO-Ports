#pragma once

#include <JuceHeader.h>
#include "PluginChain.hpp"

namespace ChainMutators {
    /**
     * Inserts a plugin at the given position, or at the end if that position doesn't exist.
     */
    void insertPlugin(std::shared_ptr<PluginChain> chain,
                      std::shared_ptr<juce::AudioPluginInstance> plugin,
                      int position,
                      HostConfiguration config);

    /**
     * Replaces an existing plugin in the chain.
     */
    void replacePlugin(std::shared_ptr<PluginChain> chain,
                       std::shared_ptr<juce::AudioPluginInstance> plugin,
                       int position,
                       HostConfiguration config);

    /**
     * Removes the plugin or gain stage at the given position in the chain.
     */
    bool removeSlot(std::shared_ptr<PluginChain> chain, int position);

    /**
     * Inserts a gain stage at the given position, or at the end if that position doesn't exist.
     */
    void insertGainStage(std::shared_ptr<PluginChain> chain, int position, HostConfiguration config);

    /**
     * Returns a pointer to the plugin at the given position.
     */
    std::shared_ptr<juce::AudioPluginInstance> getPlugin(std::shared_ptr<PluginChain> chain,
                                                         int position);

    /**
     * Set the modulation config for the given plugin to the one provided.
     */
    bool setPluginModulationConfig(std::shared_ptr<PluginChain> chain,
                                   PluginModulationConfig config,
                                   int position);

    /**
     * Returns the modulation config for the given plugin.
     */
    PluginModulationConfig getPluginModulationConfig(std::shared_ptr<PluginChain> chain,
                                                     int position);

    /**
     * Returns the number of plugins and gain stages in this chain.
     */
    inline size_t getNumSlots(std::shared_ptr<PluginChain> chain) { return chain->chain.size(); }

    /**
     * Bypasses or enables the slot at the given position.
     */
    bool setSlotBypass(std::shared_ptr<PluginChain> chain, int position, bool isBypassed);

    /**
     * Returns true if the slot is bypassed.
     */
    bool getSlotBypass(std::shared_ptr<PluginChain> chain, int position);

    /**
     * Bypasses the entire chain if set to true.
     */
    void setChainBypass(std::shared_ptr<PluginChain> chain, bool val);

    /**
     * Mutes the entire chain if set to true.
     */
    void setChainMute(std::shared_ptr<PluginChain> chain, bool val);

    /**
     * Sets the gain for the gain stage at the given position.
     */
    bool setGainLinear(std::shared_ptr<PluginChain> chain, int position, float gain);

    /**
     * Returns the gain for the gain stage at the given position.
     */
    float getGainLinear(std::shared_ptr<PluginChain> chain, int position);

    float getGainStageOutputAmplitude(std::shared_ptr<PluginChain> chain, int position, int channelNumber);

    /**
     * Sets the pan/balance for the gain stage at the given position.
     */
    bool setPan(std::shared_ptr<PluginChain> chain,  int position, float pan);

    /**
     * Returns the pan/balance for the gain stage at the given position.
     */
    float getPan(std::shared_ptr<PluginChain> chain,  int position);

    /**
     * Sets the total amount of latency this chain should aim for to keep it inline with other
     * chains.
     *
     * The chain can't reduce its latency below the total of the plugins it hosts, but it can
     * increase its latency to match slower chains.
     */
    void setRequiredLatency(std::shared_ptr<PluginChain> chain, int numSamples, HostConfiguration config);

    /**
     * Returns a pointer to the bounds for this plugin's editor. Will pointer to an empty optional
     * if there isn't a plugin at the given position.
     */
    std::shared_ptr<PluginEditorBounds> getPluginEditorBounds(std::shared_ptr<PluginChain> chain,  int position);
}
