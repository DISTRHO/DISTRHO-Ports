#pragma once

#include <JuceHeader.h>
#include "ChainSlots.hpp"

class GuestPluginWindow  : public juce::DocumentWindow
{
public:
    const std::shared_ptr<juce::AudioPluginInstance> plugin;

    GuestPluginWindow(std::function<void()> onCloseCallback,
                      std::shared_ptr<juce::AudioPluginInstance> newPlugin,
                      std::shared_ptr<PluginEditorBounds> editorBounds);
    ~GuestPluginWindow();

    void closeButtonPressed() override;

private:
    std::function<void()> _onCloseCallback;
    std::shared_ptr<PluginEditorBounds> _editorBounds;
};
