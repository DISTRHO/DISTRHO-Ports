#pragma once

#include <JuceHeader.h>
#include "PluginSelectorList.h"
#include "PluginScanStatusBar.h"
#include "SelectorComponentStyle.h"

class PluginSelectorComponent  : public juce::Component,
                                 public juce::TextEditor::Listener,
                                 public juce::Button::Listener {
public:
    PluginSelectorComponent(PluginSelectorListParameters selectorListParameters,
                            std::function<void()> onCloseCallback,
                            const SelectorComponentStyle& style);
    ~PluginSelectorComponent() override;

    void textEditorTextChanged(juce::TextEditor& textEditor) override;

    void resized() override;
    void paint(juce::Graphics& g) override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;
    bool keyPressed(const juce::KeyPress& key) override;

    /**
     * Restores the scroll position from the stored state. This must be done only after the
     * component bounds have been restored, otherwise it'll scroll to the wrong place.
     */
    void restoreScrollPosition();

private:
    PluginSelectorState& _state;
    std::function<void()> _onCloseCallback;
    const juce::Colour _backgroundColour;

    std::unique_ptr<juce::TextEditor> searchEdt;
    std::unique_ptr<juce::TextButton> vstBtn;
    std::unique_ptr<juce::TextButton> vst3Btn;
    std::unique_ptr<juce::TextButton> auBtn;
    std::unique_ptr<PluginSelectorTableListBox> pluginTableListBox;
    std::unique_ptr<PluginScanStatusBar> statusBar;

    void _setupHeaderRow(const SelectorComponentStyle& style);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginSelectorComponent)
};
