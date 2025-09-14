#pragma once

#include <JuceHeader.h>
#include "ChainSlots.hpp"
#include "LatencyListener.hpp"
#include "General/AudioSpinMutex.h"
#include "CloneableDelayLine.hpp"

typedef CloneableDelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> CloneableDelayLineType;

class PluginChain {
public:
    std::vector<std::shared_ptr<ChainSlotBase>> chain;

    bool isChainBypassed;
    bool isChainMuted;

    std::function<float(int, MODULATION_TYPE)> getModulationValueCallback;

    std::unique_ptr<CloneableDelayLineType> latencyCompLine;
    WECore::AudioSpinMutex latencyCompLineMutex;

    PluginChainLatencyListener latencyListener;

    juce::String customName;

    PluginChain(std::function<float(int, MODULATION_TYPE)> getModulationValueCallback) :
            isChainBypassed(false),
            isChainMuted(false),
            getModulationValueCallback(getModulationValueCallback),
            latencyListener(this) {
        latencyCompLine.reset(new CloneableDelayLineType(0));
        latencyCompLine->setDelay(0);
    }

    virtual ~PluginChain() {
        // The plugins might outlive this chain if they're also referenced by another copy in the
        // history, so remove the listener
        for (auto& slot : chain) {
            if (std::shared_ptr<ChainSlotPlugin> plugin = std::dynamic_pointer_cast<ChainSlotPlugin>(slot)) {
                plugin->plugin->removeListener(&latencyListener);
            }
        }
    }

    PluginChain* clone() const {
        return new PluginChain(
            chain,
            isChainBypassed,
            isChainMuted,
            getModulationValueCallback,
            std::unique_ptr<CloneableDelayLineType>(latencyCompLine->clone()),
            customName
        );
    }

private:
    PluginChain(
        std::vector<std::shared_ptr<ChainSlotBase>> newChain,
        bool newIsChainBypassed,
        bool newIsChainMuted,
        std::function<float(int, MODULATION_TYPE)> newGetModulationValueCallback,
        std::unique_ptr<CloneableDelayLineType> newLatencyCompLine,
        const juce::String& newCustomName) :
            isChainBypassed(newIsChainBypassed),
            isChainMuted(newIsChainMuted),
            getModulationValueCallback(newGetModulationValueCallback),
            latencyCompLine(std::move(newLatencyCompLine)),
            latencyListener(this),
            customName(newCustomName) {
        for (auto& slot : newChain) {
            chain.push_back(std::shared_ptr<ChainSlotBase>(slot->clone()));

            if (auto plugin = std::dynamic_pointer_cast<ChainSlotPlugin>(slot)) {
                plugin->plugin->addListener(&latencyListener);
            }
        }

        latencyListener.onPluginChainUpdate();
    }
};
