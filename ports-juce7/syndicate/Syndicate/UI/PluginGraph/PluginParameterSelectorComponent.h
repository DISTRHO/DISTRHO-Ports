#pragma once

#include <JuceHeader.h>
#include "PluginParameterSelectorList.h"
#include "PluginParameterSelectorListParameters.h"
#include "UIUtils.h"

class PluginParameterSelectorComponent : public juce::Component,
                                         public juce::TextEditor::Listener {
public:
    PluginParameterSelectorComponent(PluginParameterSelectorListParameters selectorListParameters, std::function<void()> onCloseCallback);
    ~PluginParameterSelectorComponent();

    void textEditorTextChanged(juce::TextEditor& textEditor) override;

    void resized() override;
    void paint(juce::Graphics& g) override;
    bool keyPressed(const juce::KeyPress& key) override;

    /**
     * Restores the scroll position from the stored state. This must be done only after the
     * component bounds have been restored, otherwise it'll scroll to the wrong place.
     */
    void restoreScrollPosition();

private:
    PluginParameterSelectorState& _state;
    std::function<void()> _onCloseCallback;

    UIUtils::SearchBarLookAndFeel _searchBarLookAndFeel;
    UIUtils::TableHeaderLookAndFeel _tableHeaderLookAndFeel;

    std::unique_ptr<juce::TextEditor> _searchTextEditor;
    std::unique_ptr<PluginParameterSelectorTableListBox> _parameterTableListBox;
    std::unique_ptr<juce::Label> _hintLabel;
};
