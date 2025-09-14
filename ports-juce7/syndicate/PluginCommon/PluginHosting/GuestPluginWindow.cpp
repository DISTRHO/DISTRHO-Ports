#include "GuestPluginWindow.h"

namespace {
    const juce::Colour BACKGROUND_COLOUR(0, 0, 0);
    constexpr int TITLE_BAR_BUTTONS {
        juce::DocumentWindow::TitleBarButtons::minimiseButton |
        juce::DocumentWindow::TitleBarButtons::closeButton
    };
}

GuestPluginWindow::GuestPluginWindow(std::function<void()> onCloseCallback,
                                     std::shared_ptr<juce::AudioPluginInstance> newPlugin,
                                     std::shared_ptr<PluginEditorBounds> editorBounds)
       : DocumentWindow(newPlugin->getPluginDescription().name, BACKGROUND_COLOUR, TITLE_BAR_BUTTONS),
         plugin(newPlugin),
         _onCloseCallback(onCloseCallback),
         _editorBounds(editorBounds) {

    juce::AudioProcessorEditor* editor = plugin->createEditorIfNeeded();

    if (editor != nullptr) {
        setContentOwned(editor, true);
        setResizable(editor->isResizable(), true);
    } else {
        juce::Logger::writeToLog("GuestPluginWindow failed to create editor");
    }

    // Attempt to restore the previous editor bounds
    if (_editorBounds != nullptr && _editorBounds->has_value()) {
        // The user may have unplugged the display they were using or made some other change to
        // their displays since last opening the plugin - we need to make sure the editor bounds
        // are still within one of the displays so that the UI doesn't appear off screen
        // (which is really annoying)

        // Find the display the editor should be on
        const juce::Rectangle<int> nearestDisplayArea =
            juce::Desktop::getInstance().getDisplays().getDisplayForRect(_editorBounds->value().editorBounds)->userArea;

        // If this is different to the one used last time just set the top left corner to 0, 0
        if (nearestDisplayArea != _editorBounds->value().displayArea) {
            _editorBounds->value().editorBounds.setPosition(0, 0);
        }

        setBounds(_editorBounds->value().editorBounds);
    }

    // Can't use setUsingNativeTitleBar(true) as it prevents some plugin (ie. NI) UIs from loading
    // for some reason

    setVisible(true);

#if defined(__APPLE__) || defined(_WIN32)
    setAlwaysOnTop(true);
#elif __linux__
    // TODO find a way to make this work on Linux
    setAlwaysOnTop(false);
#else
    #error Unsupported OS
#endif

    juce::Logger::writeToLog("Created GuestPluginWindow");
}

GuestPluginWindow::~GuestPluginWindow() {
    juce::Logger::writeToLog("Closing GuestPluginWindow");
    clearContentComponent();
}

void GuestPluginWindow::closeButtonPressed() {
    PluginEditorBoundsContainer newBounds(
        getBounds(),
        juce::Desktop::getInstance().getDisplays().getDisplayForRect(getBounds())->userArea
    );

    *_editorBounds.get() = newBounds;
    _onCloseCallback();
}
