#include "PluginSelectorState.h"

namespace {
    const char* XML_SORT_COLUMN_ID_STR {"sortColumnId"};
    const char* XML_SORT_FORWARDS_STR {"sortForwards"};
    const char* XML_SORT_INCLUDE_VST_STR {"includeVST"};
    const char* XML_SORT_INCLUDE_VST3_STR {"includeVST3"};
    const char* XML_SORT_INCLUDE_AU_STR {"includeAU"};
    const char* XML_SORT_FILTER_STRING_STR {"filterString"};
    const char* XML_SORT_BOUNDS_STR {"bounds"};
    const char* XML_SORT_SCROLL_POSITION_STR {"scrollPosition"};
}

void PluginSelectorState::restoreFromXml(juce::XmlElement* element) {
    if (element->hasAttribute(XML_SORT_COLUMN_ID_STR)) {
        sortColumnId = element->getIntAttribute(XML_SORT_COLUMN_ID_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SORT_COLUMN_ID_STR));
    }

    if (element->hasAttribute(XML_SORT_FORWARDS_STR)) {
        sortForwards = element->getBoolAttribute(XML_SORT_FORWARDS_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SORT_FORWARDS_STR));
    }

    if (element->hasAttribute(XML_SORT_INCLUDE_VST_STR)) {
        includeVST = element->getBoolAttribute(XML_SORT_INCLUDE_VST_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SORT_INCLUDE_VST_STR));
    }

    if (element->hasAttribute(XML_SORT_INCLUDE_VST3_STR)) {
        includeVST3 = element->getBoolAttribute(XML_SORT_INCLUDE_VST3_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SORT_INCLUDE_VST3_STR));
    }

    if (element->hasAttribute(XML_SORT_INCLUDE_AU_STR)) {
        includeAU = element->getBoolAttribute(XML_SORT_INCLUDE_AU_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SORT_INCLUDE_AU_STR));
    }

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

void PluginSelectorState::writeToXml(juce::XmlElement* element) const {
    element->setAttribute(XML_SORT_COLUMN_ID_STR, sortColumnId);
    element->setAttribute(XML_SORT_FORWARDS_STR, sortForwards);
    element->setAttribute(XML_SORT_INCLUDE_VST_STR, includeVST);
    element->setAttribute(XML_SORT_INCLUDE_VST3_STR, includeVST3);
    element->setAttribute(XML_SORT_INCLUDE_AU_STR, includeAU);
    element->setAttribute(XML_SORT_FILTER_STRING_STR, filterString);

    if (bounds.has_value()) {
        element->setAttribute(XML_SORT_BOUNDS_STR, bounds->toString());
    }

    element->setAttribute(XML_SORT_SCROLL_POSITION_STR, scrollPosition);
}
