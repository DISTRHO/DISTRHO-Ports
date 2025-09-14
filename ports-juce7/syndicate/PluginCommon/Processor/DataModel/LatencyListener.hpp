#pragma once

#include <JuceHeader.h>

class PluginChain;
class PluginSplitter;

/**
 * Listens to updates to reported latency for plugins in the chain and calculates the new latency.
 */
class PluginChainLatencyListener : public juce::AsyncUpdater,
                                   public juce::AudioProcessorListener {
public:
    int calculatedTotalPluginLatency;

    PluginChainLatencyListener(PluginChain* chain);

    /**
     * Call when there is a change to the plugin chain's layout (ie. a plugin is added or removed).
     */
    void onPluginChainUpdate();

    /**
     * Called on the message thread after a latency change.
     */
    void handleAsyncUpdate() override;

    void audioProcessorParameterChanged(juce::AudioProcessor* processor,
                                        int parameterIndex,
                                        float newValue) override;

    /**
     * Called on the audio thread by a processor after a latency change.
     */
    void audioProcessorChanged(juce::AudioProcessor* processor,
                               const ChangeDetails& details) override;

    void setSplitter(PluginSplitter* splitter) { _splitter = splitter; }

    void removeSplitter() { _splitter = nullptr; }

private:
    PluginChain* _chain;
    PluginSplitter* _splitter;

    int _calculateTotalPluginLatency() const;
};
