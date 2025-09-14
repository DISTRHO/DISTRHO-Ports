#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginParameterSelectorWindow.h"

class GraphViewComponent;

/**
 * The interface between the processor and parts of the UI that control modulation.
 */
class PluginModulationInterface {
public:
    PluginModulationInterface(SyndicateAudioProcessor& processor);
    ~PluginModulationInterface() = default;

    PluginModulationConfig getPluginModulationConfig(int chainNumber, int pluginNumber);
    void togglePluginModulationActive(int chainNumber, int pluginNumber);
    void addSourceToTarget(int chainNumber, int pluginNumber, int targetNumber, ModulationSourceDefinition source);
    void removeSourceFromTarget(int chainNumber, int pluginNumber, int targetNumber, ModulationSourceDefinition source);
    void setModulationTargetValue(int chainNumber, int pluginNumber, int targetNumber, float val);
    void setModulationSourceValue(int chainNumber, int pluginNumber, int targetNumber, int sourceNumber, float val);
    void selectModulationTarget(int chainNumber, int pluginNumber, int targetNumber);
    void removeModulationTarget(int chainNumber, int pluginNumber, int targetNumber);
    juce::AudioProcessorParameter* getPluginParameterForTarget(int chainNumber, int pluginNumber, int targetNumber);

private:
    SyndicateAudioProcessor& _processor;
    std::unique_ptr<PluginParameterSelectorWindow> _parameterSelectorWindow;

    void _onPluginParameterSelected(juce::AudioProcessorParameter* parameter, int chainNumber, int pluginNumber, int targetNumber, bool shouldClose);
};
