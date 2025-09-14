#pragma once

#include <JuceHeader.h>

#include "PluginParameterSelectorState.h"

struct PluginParameterSelectorListParameters {
    PluginParameterSelectorState& state;
    const juce::Array<juce::AudioProcessorParameter*> fullParameterList;
    std::function<void(juce::AudioProcessorParameter*, bool)> parameterSelectedCallback;
    bool isReplacingParameter;
};
