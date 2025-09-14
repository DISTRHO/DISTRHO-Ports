#include "PluginParameterSelectorWindow.h"
#include "UIUtils.h"

namespace {
    const juce::Colour BACKGROUND_COLOUR(0, 0, 0);
    constexpr int TITLE_BAR_BUTTONS {
        juce::DocumentWindow::TitleBarButtons::minimiseButton |
        juce::DocumentWindow::TitleBarButtons::closeButton
    };
}

PluginParameterSelectorWindow::PluginParameterSelectorWindow(
        std::function<void()> onCloseCallback,
        PluginParameterSelectorListParameters selectorListParameters,
        juce::String title) :
            juce::DocumentWindow(title,
                             BACKGROUND_COLOUR,
                             TITLE_BAR_BUTTONS,
                             true),
            _onCloseCallback(onCloseCallback),
            _content(nullptr),
            _state(selectorListParameters.state) {

    if (_state.bounds.has_value()) {
        // Use the previous bounds if we have them
        setBounds(_state.bounds.value());
    } else {
        // Default to the centre
        constexpr int DEFAULT_WIDTH {300};
        constexpr int DEFAULT_HEIGHT {500};
        centreWithSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    }

    setVisible(true);
    setResizable(true, true);
    setAlwaysOnTop(true);
    _content = new PluginParameterSelectorComponent(selectorListParameters, onCloseCallback);
    setContentOwned(_content, false);
    _content->restoreScrollPosition();

    juce::Logger::writeToLog("Created PluginParameterSelectorWindow");
}

PluginParameterSelectorWindow::~PluginParameterSelectorWindow() {
    juce::Logger::writeToLog("Closing PluginParameterSelectorWindow");
    _state.bounds = getBounds();
    clearContentComponent();
}

void PluginParameterSelectorWindow::closeButtonPressed() {
    _onCloseCallback();
}

void PluginParameterSelectorWindow::takeFocus() {
    if (_content != nullptr) {
        _content->grabKeyboardFocus();
    }
}
