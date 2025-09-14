/*
  ==============================================================================

    PluginSelectorListParameters.h
    Created: 28 May 2021 11:53:23pm
    Author:  Jack Devlin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "PluginScanClient.h"
#include "PluginSelectorState.h"

struct PluginSelectorListParameters {
    PluginScanClient& scanner;
    PluginSelectorState& state;
    juce::AudioPluginFormatManager& formatManager;
    std::function<void(std::unique_ptr<juce::AudioPluginInstance>, const juce::String&, bool)> pluginCreationCallback;
    std::function<double()> getSampleRate;
    std::function<int()> getBlockSize;
    bool isReplacingPlugin;
};
