#include "PluginParameterSelectorState.h"

namespace {
    const char* XML_SORT_FILTER_STRING_STR {"filterString"};
    const char* XML_SORT_BOUNDS_STR {"bounds"};
    const char* XML_SORT_SCROLL_POSITION_STR {"scrollPosition"};
}

void PluginParameterSelectorState::restoreFromXml(juce::XmlElement* element) {
    if (element->hasAttribute(XML_SORT_FILTER_STRING_STR)) {
        filterString = element->getStringAttribute(XML_SORT_FILTER_STRING_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SORT_FILTER_STRING_STR));
    }

    if (element->hasAttribute(XML_SORT_BOUNDS_STR)) {
        juce::String boundsStr = element->getStringAttribute(XML_SORT_BOUNDS_STR);
        bounds = juce::Rectangle<int>::fromString(boundsStr);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SORT_BOUNDS_STR));
    }

    if (element->hasAttribute(XML_SORT_SCROLL_POSITION_STR)) {
        scrollPosition = element->getDoubleAttribute(XML_SORT_SCROLL_POSITION_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SORT_SCROLL_POSITION_STR));
    }
}

void PluginParameterSelectorState::writeToXml(juce::XmlElement* element) const {
    element->setAttribute(XML_SORT_FILTER_STRING_STR, filterString);

    if (bounds.has_value()) {
        element->setAttribute(XML_SORT_BOUNDS_STR, bounds->toString());
    }

    element->setAttribute(XML_SORT_SCROLL_POSITION_STR, scrollPosition);
}
