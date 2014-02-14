/*
  ==============================================================================

   Build options for juce based plugins

  ==============================================================================
*/

#ifndef JUCE_PLUGIN_APPCONFIG_H_INCLUDED
#define JUCE_PLUGIN_APPCONFIG_H_INCLUDED

#include "../build-juce/AppConfig.h"

#if USE_DROWAUDIO
 #include "../build-drowaudio/AppConfig.h"
#endif

#if USE_JUCED
 #include "../build-juced/AppConfig.h"
#endif

#endif // JUCE_PLUGIN_APPCONFIG_H_INCLUDED
