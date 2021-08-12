/*
  ==============================================================================

   Extra stuff for juce plugins

  ==============================================================================
*/

#include "AppConfig.h"

#define JUCE_CORE_INCLUDE_NATIVE_HEADERS 1
#if ! JUCE_AUDIOPROCESSOR_NO_GUI
 #define JUCE_GUI_BASICS_INCLUDE_XHEADERS 1
#endif
#include "modules/juce_gui_basics/juce_gui_basics.h"
#undef None

#include "JucePluginMain.h"

#if JucePlugin_Build_AU || JucePlugin_Build_LV2 || JucePlugin_Build_RTAS || JucePlugin_Build_VST || JucePlugin_Build_VST3 || JucePlugin_Build_Standalone
 // all good here
#else
 #error Invalid configuration
#endif

#if ! JucePlugin_Build_Standalone
 #include "modules/juce_audio_plugin_client/utility/juce_PluginUtilities.cpp"
#endif

#if JUCE_MAC && (JucePlugin_Build_VST || JucePlugin_Build_VST3)
 #undef JUCE_CHECKSETTINGMACROS_H
 #include "modules/juce_audio_plugin_client/VST/juce_VST_Wrapper.mm"
#endif
