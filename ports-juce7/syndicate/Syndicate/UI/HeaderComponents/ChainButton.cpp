#include "ChainButton.h"
#include "UIUtils.h"

void ChainButtonLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                                  juce::Button& button,
                                                  const juce::Colour& /*backgroundColour*/,
                                                  bool /*shouldDrawButtonAsHighlighted*/,
                                                  bool /*shouldDrawButtonAsDown*/) {
    const juce::Rectangle<float> area = button.getLocalBounds().reduced(1, 1).toFloat();

    if (button.getToggleState()) {
        g.setColour(button.findColour(highlightColour));
    } else {
        g.setColour(button.findColour(backgroundColour));
    }
    g.fillEllipse(area);
}

void ChainButtonLookAndFeel::drawButtonText(juce::Graphics& g,
                                            juce::TextButton& button,
                                            bool /*shouldDrawButtonAsHighlighted*/,
                                            bool /*shouldDrawButtonAsDown*/) {

    if (!button.isEnabled()) {
        g.setColour(button.findColour(disabledColour));
    } else if (button.getToggleState()) {
        g.setColour(button.findColour(backgroundColour));
    } else {
        g.setColour(button.findColour(highlightColour));
    }

    g.drawText(button.getButtonText(), button.getLocalBounds().reduced(2), juce::Justification::centred, false);
}

ChainButton::ChainButton(CHAIN_BUTTON_TYPE type) {
    setLookAndFeel(&_lookAndFeel);

    setColour(ChainButtonLookAndFeel::backgroundColour, UIUtils::slotBackgroundColour);
    setColour(ChainButtonLookAndFeel::highlightColour, UIUtils::highlightColour);
    setColour(ChainButtonLookAndFeel::disabledColour, UIUtils::deactivatedColour);

    switch (type) {
        case CHAIN_BUTTON_TYPE::BYPASS:
            setButtonText("B");
            break;
        case CHAIN_BUTTON_TYPE::MUTE:
            setButtonText("M");
            break;
        case CHAIN_BUTTON_TYPE::SOLO:
            setButtonText("S");
            break;
    }
}

ChainButton::~ChainButton() {
    setLookAndFeel(nullptr);
}
