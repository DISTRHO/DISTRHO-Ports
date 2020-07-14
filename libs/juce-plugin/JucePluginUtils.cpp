/*
  ==============================================================================

   Extra stuff for juce plugins

  ==============================================================================
*/

#include "AppConfig.h"
#include "JucePluginMain.h"

#if ! JucePlugin_Build_Standalone
 #include "modules/juce_audio_plugin_client/utility/juce_PluginUtilities.cpp"
#endif
