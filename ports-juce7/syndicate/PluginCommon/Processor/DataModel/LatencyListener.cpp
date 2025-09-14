#include "LatencyListener.hpp"

#include "PluginChain.hpp"
#include "PluginSplitter.hpp"

PluginChainLatencyListener::PluginChainLatencyListener(PluginChain* chain) :
        calculatedTotalPluginLatency(0),
        _chain(chain),
        _splitter(nullptr) {
}

void PluginChainLatencyListener::onPluginChainUpdate() {
    handleAsyncUpdate();
}

void PluginChainLatencyListener::handleAsyncUpdate() {
    calculatedTotalPluginLatency = _calculateTotalPluginLatency();

    // Notify the splitter
    if (_splitter != nullptr) {
        _splitter->onLatencyChange();
    }
}

void PluginChainLatencyListener::audioProcessorParameterChanged(juce::AudioProcessor* /*processor*/,
                                                                int /*parameterIndex*/,
                                                                float /*newValue*/) {
    // Do nothing
}

void PluginChainLatencyListener::audioProcessorChanged(juce::AudioProcessor* /*processor*/,
                                                       const ChangeDetails& details) {
    if (details.latencyChanged) {
        // Trigger the update on another thread
        triggerAsyncUpdate();
    }
}

int PluginChainLatencyListener::_calculateTotalPluginLatency() const {
    // Iterate through each plugin and total the reported latency
    int totalLatency {0};

    // If the chain is bypassed the reported latency should be 0
    if (!_chain->isChainBypassed) {
        for (int index {0}; index < _chain->chain.size(); index++) {
            const auto pluginSlot = std::dynamic_pointer_cast<ChainSlotPlugin>(_chain->chain[index]);

            // If this slot is a plugin and it's not bypassed, add it to the total
            if (pluginSlot != nullptr && !pluginSlot->isBypassed) {
                totalLatency += pluginSlot->plugin->getLatencySamples();
            }
        }
    }

    return totalLatency;
}
