/*
  ==============================================================================

   Build options for juced static library

  ==============================================================================
*/

#ifndef BUILD_JUCED_APPCONFIG_H_INCLUDED
#define BUILD_JUCED_APPCONFIG_H_INCLUDED

#include "../build-juce/AppConfig.h"

//=============================================================================
/** Config: JUCE_PLUGINHOST_LADSPA
        Enabling this builds support for LADSPA audio plugins.
        Not enabled by default.
*/
#undef JUCE_PLUGINHOST_LADSPA
#define JUCE_PLUGINHOST_LADSPA 0

#endif // BUILD_JUCED_APPCONFIG_H_INCLUDED
