#pragma once

#include <JuceHeader.h>
#include "PluginModulationTarget.h"
#include "PluginModulationInterface.h"

class PluginSlotModulationTray : public juce::Component {
public:
    PluginSlotModulationTray(PluginModulationInterface& pluginModulationInterface,
                             int chainNumber,
                             int pluginNumber);

    ~PluginSlotModulationTray() = default;

    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    std::vector<std::unique_ptr<PluginModulationTarget>> _modulationTargets;
    std::unique_ptr<juce::Viewport> _targetsView;
};
