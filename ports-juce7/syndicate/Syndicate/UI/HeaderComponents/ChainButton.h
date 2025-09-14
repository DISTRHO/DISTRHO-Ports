#pragma once

#include <JuceHeader.h>
#include "CoreJUCEPlugin/LookAndFeelMixins/LookAndFeelMixins.h"

enum class CHAIN_BUTTON_TYPE {
    BYPASS,
    MUTE,
    SOLO
};

class ChainButtonLookAndFeel : public WECore::LookAndFeelMixins::WEV2LookAndFeel {
public:
    enum ColourIds {
        backgroundColour,
        highlightColour,
        disabledColour
    };

    ChainButtonLookAndFeel() = default;
    virtual ~ChainButtonLookAndFeel() = default;

    virtual void drawButtonBackground(juce::Graphics& g,
                                      juce::Button& button,
                                      const juce::Colour& backgroundColour,
                                      bool shouldDrawButtonAsHighlighted,
                                      bool shouldDrawButtonAsDown) override;

    virtual void drawButtonText(juce::Graphics& g,
                                juce::TextButton& button,
                                bool shouldDrawButtonAsHighlighted,
                                bool shouldDrawButtonAsDown) override;
};

class ChainButton : public juce::TextButton {
public:
    ChainButton(CHAIN_BUTTON_TYPE type);
    virtual ~ChainButton();

private:
    ChainButtonLookAndFeel _lookAndFeel;
};
