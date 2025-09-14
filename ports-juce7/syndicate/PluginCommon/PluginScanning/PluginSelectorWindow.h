#pragma once

#include <JuceHeader.h>
#include "PluginSelectorComponent.h"
#include "PluginSelectorListParameters.h"

class PluginSelectorWindow : public juce::DocumentWindow {
public:
    PluginSelectorWindow(std::function<void()> onCloseCallback,
                         PluginSelectorListParameters selectorListParameters,
                         std::unique_ptr<SelectorComponentStyle> style,
                         juce::String title);
    virtual ~PluginSelectorWindow();

    virtual void closeButtonPressed() override;

    void takeFocus();

private:
    std::function<void()> _onCloseCallback;
    PluginSelectorComponent* _content;
    std::unique_ptr<SelectorComponentStyle> _style;

    // We need to keep a reference to state to update the bounds on resize
    PluginSelectorState& _state;
};
