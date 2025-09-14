#include "PluginSelectorWindow.h"

namespace {
    const juce::Colour BACKGROUND_COLOUR(0, 0, 0);
    constexpr int TITLE_BAR_BUTTONS {
        juce::DocumentWindow::TitleBarButtons::minimiseButton |
        juce::DocumentWindow::TitleBarButtons::closeButton
    };
}

PluginSelectorWindow::PluginSelectorWindow(std::function<void()> onCloseCallback,
                                           PluginSelectorListParameters selectorListParameters,
                                           std::unique_ptr<SelectorComponentStyle> style,
                                           juce::String title) :
        juce::DocumentWindow(title, BACKGROUND_COLOUR, TITLE_BAR_BUTTONS),
        _onCloseCallback(onCloseCallback),
        _content(nullptr),
        _style(std::move(style)),
        _state(selectorListParameters.state) {

    if (_state.bounds.has_value()) {
        // Use the previous bounds if we have them
        setBounds(_state.bounds.value());
    } else {
        // Default to the centre
        constexpr int DEFAULT_WIDTH {900};
        constexpr int DEFAULT_HEIGHT {550};
        centreWithSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    }

    setVisible(true);
    setResizable(true, true);
    setAlwaysOnTop(true);
    _content = new PluginSelectorComponent(selectorListParameters, onCloseCallback, *(_style.get()));
    setContentOwned(_content, false);
    _content->setBounds(0, getTitleBarHeight(), getWidth(), getHeight() - getTitleBarHeight());

    _content->restoreScrollPosition();

    selectorListParameters.scanner.clearMissingPlugins();

    juce::Logger::writeToLog("Created PluginSelectorWindow");
}

PluginSelectorWindow::~PluginSelectorWindow() {
    juce::Logger::writeToLog("Closing PluginSelectorWindow");
    _state.bounds = getBounds();
    clearContentComponent();
}

void PluginSelectorWindow::closeButtonPressed() {
    _onCloseCallback();
}

void PluginSelectorWindow::takeFocus() {
    if (_content != nullptr) {
        _content->grabKeyboardFocus();
    }
}
