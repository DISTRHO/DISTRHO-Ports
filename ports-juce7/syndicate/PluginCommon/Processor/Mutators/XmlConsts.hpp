#pragma once

#include "SplitTypes.hpp"

inline const char* XML_SLOT_TYPE_STR {"SlotType"};
inline const char* XML_SLOT_TYPE_PLUGIN_STR {"Plugin"};
inline const char* XML_SLOT_TYPE_GAIN_STAGE_STR {"GainStage"};
inline const char* XML_SLOT_IS_BYPASSED_STR {"isSlotBypassed"};

inline const char* XML_GAIN_STAGE_GAIN_STR {"Gain"};
inline const char* XML_GAIN_STAGE_PAN_STR {"Pan"};

inline const char* XML_PLUGIN_DATA_STR {"PluginData"};
inline const char* XML_MODULATION_CONFIG_STR {"ModulationConfig"};
inline const char* XML_MODULATION_IS_ACTIVE_STR {"ModulationIsActive"};
inline const char* XML_MODULATION_TARGET_PARAMETER_NAME_STR {"TargetParameterName"};
inline const char* XML_MODULATION_REST_VALUE_STR {"RestValue"};
inline const char* XML_MODULATION_SOURCE_AMOUNT {"SourceAmount"};
inline const char* XML_PLUGIN_EDITOR_BOUNDS_STR {"PluginEditorBounds"};
inline const char* XML_DISPLAY_AREA_STR {"DisplayArea"};

inline const char* XML_MODULATION_SOURCE_ID {"SourceId"};
inline const char* XML_MODULATION_SOURCE_TYPE {"SourceType"};

inline const char* XML_IS_CHAIN_BYPASSED_STR {"isChainBypassed"};
inline const char* XML_IS_CHAIN_MUTED_STR {"isChainMuted"};
inline const char* XML_CHAIN_CUSTOM_NAME_STR {"ChainCustomName"};
inline const char* XML_PLUGINS_STR {"Plugins"};

inline const char* XML_CHAINS_STR {"Chains"};
inline const char* XML_IS_CHAIN_SOLOED_STR {"isSoloed"};

inline const char* XML_SPLITTER_STR {"Splitter"};
inline const char* XML_SPLIT_TYPE_STR {"SplitType"};
inline const char* XML_SPLIT_TYPE_SERIES_STR {"series"};
inline const char* XML_SPLIT_TYPE_PARALLEL_STR {"parallel"};
inline const char* XML_SPLIT_TYPE_MULTIBAND_STR {"multiband"};
inline const char* XML_SPLIT_TYPE_LEFTRIGHT_STR {"leftright"};
inline const char* XML_SPLIT_TYPE_MIDSIDE_STR {"midside"};

inline const char* XML_CROSSOVERS_STR {"Crossovers"};
inline const char* XML_CACHED_CROSSOVER_FREQUENCIES_STR {"CrossoverFrequencies"};

inline const char* XML_LFOS_STR {"LFOs"};
inline const char* XML_LFO_BYPASS_STR {"lfoBypass"};
inline const char* XML_LFO_PHASE_SYNC_STR {"lfoPhaseSync"};
inline const char* XML_LFO_TEMPO_SYNC_STR {"lfoTempoSync"};
inline const char* XML_LFO_INVERT_STR {"lfoInvert"};
inline const char* XML_LFO_OUTPUT_MODE_STR {"lfoOutputMode"};
inline const char* XML_LFO_WAVE_STR {"lfoWave"};
inline const char* XML_LFO_TEMPO_NUMER_STR {"lfoTempoNumer"};
inline const char* XML_LFO_TEMPO_DENOM_STR {"lfoTempoDenom"};
inline const char* XML_LFO_FREQ_STR {"lfoFreq"};
inline const char* XML_LFO_DEPTH_STR {"lfoDepth"};
inline const char* XML_LFO_MANUAL_PHASE_STR {"lfoManualPhase"};
inline const char* XML_LFO_FREQ_MODULATION_SOURCES_STR {"lfoFreqModulationSources"};
inline const char* XML_LFO_DEPTH_MODULATION_SOURCES_STR {"lfoDepthModulationSources"};
inline const char* XML_LFO_PHASE_MODULATION_SOURCES_STR {"lfoPhaseModulationSources"};

inline const char* XML_ENVELOPES_STR {"Envelopes"};
inline const char* XML_ENV_ATTACK_TIME_STR {"envelopeAttack"};
inline const char* XML_ENV_RELEASE_TIME_STR {"envelopeRelease"};
inline const char* XML_ENV_FILTER_ENABLED_STR {"envelopeFilterEnabled"};
inline const char* XML_ENV_LOW_CUT_STR {"envelopeLowCut"};
inline const char* XML_ENV_HIGH_CUT_STR {"envelopeHighCut"};
inline const char* XML_ENV_AMOUNT_STR {"envelopeAmount"};
inline const char* XML_ENV_USE_SIDECHAIN_INPUT_STR {"envelopeUseSidechainInput"};

inline const char* XML_RANDOMS_STR {"Randoms"};
inline const char* XML_RANDOM_OUTPUT_MODE_STR {"randomOutputMode"};
inline const char* XML_RANDOM_FREQ_STR {"randomFreq"};
inline const char* XML_RANDOM_DEPTH_STR {"randomDepth"};
inline const char* XML_RANDOM_FREQ_MODULATION_SOURCES_STR {"randomFreqModulationSources"};
inline const char* XML_RANDOM_DEPTH_MODULATION_SOURCES_STR {"randomDepthModulationSources"};

inline std::string getParameterModulationConfigXmlName(int configNumber) {
    std::string retVal("ParamConfig_");
    retVal += std::to_string(configNumber);
    return retVal;
}

inline std::string getParameterModulationSourceXmlName(int sourceNumber) {
    std::string retVal("Source_");
    retVal += std::to_string(sourceNumber);
    return retVal;
}

inline std::string getSlotXMLName(int pluginNumber) {
    std::string retVal("Slot_");
    retVal += std::to_string(pluginNumber);
    return retVal;
}

inline std::string getChainXMLName(int chainNumber) {
    std::string retVal("Chain_");
    retVal += std::to_string(chainNumber);
    return retVal;
}

inline const char* splitTypeToString(SPLIT_TYPE splitType) {
    switch (splitType) {
        case SPLIT_TYPE::SERIES:
            return XML_SPLIT_TYPE_SERIES_STR;
        case SPLIT_TYPE::PARALLEL:
            return XML_SPLIT_TYPE_PARALLEL_STR;
        case SPLIT_TYPE::MULTIBAND:
            return XML_SPLIT_TYPE_MULTIBAND_STR;
        case SPLIT_TYPE::LEFTRIGHT:
            return XML_SPLIT_TYPE_LEFTRIGHT_STR;
        case SPLIT_TYPE::MIDSIDE:
            return XML_SPLIT_TYPE_MIDSIDE_STR;
    }
}

inline SPLIT_TYPE stringToSplitType(juce::String splitTypeString) {
    SPLIT_TYPE retVal {SPLIT_TYPE::SERIES};

    if (splitTypeString == XML_SPLIT_TYPE_PARALLEL_STR) {
        retVal = SPLIT_TYPE::PARALLEL;
    } else if (splitTypeString == XML_SPLIT_TYPE_MULTIBAND_STR) {
        retVal = SPLIT_TYPE::MULTIBAND;
    } else if (splitTypeString == XML_SPLIT_TYPE_LEFTRIGHT_STR) {
        retVal = SPLIT_TYPE::LEFTRIGHT;
    } else if (splitTypeString == XML_SPLIT_TYPE_MIDSIDE_STR) {
        retVal = SPLIT_TYPE::MIDSIDE;
    }

    return retVal;
}

inline juce::String getCrossoverXMLName(int crossoverNumber) {
    juce::String retVal("Crossover_");
    retVal += juce::String(crossoverNumber);
    return retVal;
}

inline juce::String getCachedCrossoverFreqXMLName(int crossoverNumber) {
    juce::String retVal("Crossover_");
    retVal += juce::String(crossoverNumber);
    return retVal;
}

inline std::string getLfoXMLName(int lfoNumber) {
    std::string retVal("LFO_");
    retVal += std::to_string(lfoNumber);
    return retVal;
}

inline std::string getEnvelopeXMLName(int envelopeNumber) {
    std::string retVal("Envelope_");
    retVal += std::to_string(envelopeNumber);
    return retVal;
}

inline std::string getRandomXMLName(int randomNumber) {
    std::string retVal("Random_");
    retVal += std::to_string(randomNumber);
    return retVal;
}
