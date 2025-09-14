#pragma once

#include <JuceHeader.h>
#include "PluginScanClient.h"
#include "SelectorComponentStyle.h"
#include "UIUtils.h"
#include "ConfigurePopover.hpp"

class PluginScanStatusBar : public juce::Component,
                            public juce::MessageListener,
                            public juce::Button::Listener {
public:
    PluginScanStatusBar(PluginScanClient& pluginScanClient,
                        const SelectorComponentStyle& style);
    ~PluginScanStatusBar() override;

    void resized() override;

    void handleMessage(const juce::Message& message) override;

    void buttonClicked(juce::Button* buttonThatWasClicked) override;

    static constexpr int SPACER_WIDTH {10};
    static constexpr int NARROW_BUTTON_WIDTH {80};
    static constexpr int WIDE_BUTTON_WIDTH {130};
    static constexpr int ROW_HEIGHT {24};

    static constexpr int MAX_BUTTONS_WIDTH {
        NARROW_BUTTON_WIDTH + SPACER_WIDTH +
        NARROW_BUTTON_WIDTH + SPACER_WIDTH +
        WIDE_BUTTON_WIDTH + SPACER_WIDTH +
        WIDE_BUTTON_WIDTH + SPACER_WIDTH +
        WIDE_BUTTON_WIDTH + SPACER_WIDTH +
        NARROW_BUTTON_WIDTH + SPACER_WIDTH
    };

    static constexpr int MIN_STATUS_WIDTH {140};

private:
    std::unique_ptr<juce::Label> statusLbl;
    std::unique_ptr<juce::TextButton> startScanBtn;
    std::unique_ptr<juce::TextButton> stopScanBtn;
    std::unique_ptr<juce::TextButton> rescanAllBtn;
    std::unique_ptr<juce::TextButton> rescanCrashedBtn;
    std::unique_ptr<juce::TextButton> viewCrashedBtn;
    std::unique_ptr<juce::TextButton> configureBtn;
    std::unique_ptr<ConfigurePopover> configurePopover;
    std::unique_ptr<UIUtils::PopoverComponent> crashedPluginsPopover;
    PluginScanClient& _pluginScanClient;

    void _updateButtonState(bool isScanRunning);

    void _createCrashedPluginsDialogue();
    void _createConfigureDialogue();
};
