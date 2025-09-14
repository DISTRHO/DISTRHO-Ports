#pragma once

#include <JuceHeader.h>
#include <optional>

struct PluginParameterSelectorState {
    // Stores the value of the text box
    juce::String filterString;

    // Stores the last location of the window - empty if the window hasn't been opened yet
    std::optional<juce::Rectangle<int>> bounds;

    // Stores scroll position (value from 0 to 1)
    double scrollPosition;

    PluginParameterSelectorState() : scrollPosition(0) { }

    void restoreFromXml(juce::XmlElement* element);
    void writeToXml(juce::XmlElement* element) const;
};
