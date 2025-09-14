#pragma once

#include <JuceHeader.h>
#include "General/ParameterDefinition.h"
#include "General/CoreMath.h"

constexpr int NUM_MACROS {4};

const ParameterDefinition::RangedParameter<double> MACRO(0, 1, 0),
                                                   ENVELOPE_AMOUNT(-5, 5, 0),
                                                   OUTPUTGAIN(-60, 20, 0),
                                                   OUTPUTPAN(-1, 1, 0);

const juce::Array<juce::String> MACRO_STRS {"Macro1", "Macro2", "Macro3", "Macro4"};

const juce::String SPLIT_TYPE_STR("SplitType"),
                   GRAPH_STATE_STR("GraphStateParam"),
                   OUTPUTGAIN_STR("OutputGain"),
                   OUTPUTPAN_STR("OutputPan");
