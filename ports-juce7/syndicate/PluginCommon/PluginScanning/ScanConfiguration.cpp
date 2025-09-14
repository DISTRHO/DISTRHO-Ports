#include "ScanConfiguration.hpp"
#include "AllUtils.h"

namespace {
    const char* XML_SCAN_VST_DEFAULT_PATHS_STR {"vstDefaultPaths"};
    const char* XML_CUSTOM_VST_PATHS_STR {"vstCustomPaths"};
    const char* XML_SCAN_VST3_DEFAULT_PATHS_STR {"vst3DefaultPaths"};
    const char* XML_CUSTOM_VST3_PATHS_STR {"vst3CustomPaths"};
}

ScanConfiguration::ScanConfiguration() : VSTDefaultPaths(true), VST3DefaultPaths(true) {
}

juce::FileSearchPath ScanConfiguration::getAUPaths() {
    // Audiounit scanning is managed by macOS, so no custom paths
#ifdef __APPLE__
    return auFormat.getDefaultLocationsToSearch();
#else
    return juce::FileSearchPath();
#endif
}

juce::FileSearchPath ScanConfiguration::getVSTPaths() {
    juce::FileSearchPath paths = customVSTPaths;
    if (VSTDefaultPaths) {
        paths.addPath(vstFormat.getDefaultLocationsToSearch());
    }

    return paths;
}

juce::FileSearchPath ScanConfiguration::getVST3Paths() {
    juce::FileSearchPath paths = customVST3Paths;
    if (VST3DefaultPaths) {
        paths.addPath(vst3Format.getDefaultLocationsToSearch());
    }

    return paths;
}

void ScanConfiguration::restoreFromXml() {
    juce::File configFile = Utils::DataDirectory.getChildFile(Utils::SCAN_CONFIGURATION_FILE_NAME);
    if (!configFile.existsAsFile()) {
        juce::Logger::writeToLog("Scan configuration file doesn't exist");
        return;
    }

    std::unique_ptr<juce::XmlElement> element = juce::XmlDocument::parse(configFile);
    if (element == nullptr) {
        juce::Logger::writeToLog("Failed to parse XML from scan configuration file");
        return;
    }

    if (element->hasAttribute(XML_SCAN_VST_DEFAULT_PATHS_STR)) {
        VSTDefaultPaths = element->getBoolAttribute(XML_SCAN_VST_DEFAULT_PATHS_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SCAN_VST_DEFAULT_PATHS_STR));
    }

    if (element->hasAttribute(XML_CUSTOM_VST_PATHS_STR)) {
        // Clear existing paths
        while (customVSTPaths.getNumPaths() > 0) {
            customVSTPaths.remove(0);
        }

        auto paths = juce::StringArray::fromTokens(element->getStringAttribute(XML_CUSTOM_VST_PATHS_STR), ";", "");
        for (juce::String path : paths) {
            customVSTPaths.addIfNotAlreadyThere(juce::File(path));
        }
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_CUSTOM_VST_PATHS_STR));
    }

    if (element->hasAttribute(XML_SCAN_VST3_DEFAULT_PATHS_STR)) {
        VST3DefaultPaths = element->getBoolAttribute(XML_SCAN_VST3_DEFAULT_PATHS_STR);
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_SCAN_VST3_DEFAULT_PATHS_STR));
    }

    if (element->hasAttribute(XML_CUSTOM_VST3_PATHS_STR)) {
        // Clear existing paths
        while (customVST3Paths.getNumPaths() > 0) {
            customVST3Paths.remove(0);
        }

        auto paths = juce::StringArray::fromTokens(element->getStringAttribute(XML_CUSTOM_VST3_PATHS_STR), ";", "");
        for (juce::String path : paths) {
            customVST3Paths.addIfNotAlreadyThere(juce::File(path));
        }
    } else {
        juce::Logger::writeToLog("Missing attribute " + juce::String(XML_CUSTOM_VST3_PATHS_STR));
    }
}

void ScanConfiguration::writeToXml() const {
    juce::File configFile = Utils::DataDirectory.getChildFile(Utils::SCAN_CONFIGURATION_FILE_NAME);
    configFile.deleteFile();

    auto element = std::make_unique<juce::XmlElement>("scanConfiguration");

    element->setAttribute(XML_SCAN_VST_DEFAULT_PATHS_STR, VSTDefaultPaths);
    element->setAttribute(XML_CUSTOM_VST_PATHS_STR, customVSTPaths.toString());
    element->setAttribute(XML_SCAN_VST3_DEFAULT_PATHS_STR, VST3DefaultPaths);
    element->setAttribute(XML_CUSTOM_VST3_PATHS_STR, customVST3Paths.toString());

    element->writeTo(configFile);
}
