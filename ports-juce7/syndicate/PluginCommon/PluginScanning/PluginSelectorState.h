#pragma once

#include <JuceHeader.h>
#include <optional>

struct PluginSelectorState {
    int sortColumnId;
    bool sortForwards;
    bool includeLV2;
    bool includeVST;
    bool includeVST3;
    bool includeAU;

    // Stores the value of the text box
    juce::String filterString;

    // Stores the last location of the window - empty if the window hasn't been opened yet
    std::optional<juce::Rectangle<int>> bounds;

    // Stores scroll position (value from 0 to 1)
    double scrollPosition;

    PluginSelectorState() : sortColumnId(0),
                            sortForwards(true),
                            includeLV2(true),
                            includeVST(true),
                            includeVST3(true),
                            includeAU(true),
                            filterString(""),
                            bounds(),
                            scrollPosition(0) { }

    void restoreFromXml(juce::XmlElement* element);
    void writeToXml(juce::XmlElement* element) const;
};
