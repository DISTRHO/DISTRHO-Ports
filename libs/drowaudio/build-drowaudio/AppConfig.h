/*
  ==============================================================================

   Build options for drowaudio static library

  ==============================================================================
*/

#ifndef BUILD_DROWAUDIO_APPCONFIG_H_INCLUDED
#define BUILD_DROWAUDIO_APPCONFIG_H_INCLUDED

#include "../build-juce/AppConfig.h"

/** Config: DROWAUDIO_USE_FFTREAL
    Enables the FFTReal library. By default this is enabled except on the Mac
    where the Accelerate framework is preferred. However, if you do explicity 
    enable this setting fftreal can be used for testing purposes.
 */
#if JUCE_MAC
 #define DROWAUDIO_USE_FFTREAL 0
#else
 #define DROWAUDIO_USE_FFTREAL 1
#endif

/** Config: DROWAUDIO_USE_SOUNDTOUCH
    Enables the SoundTouch library and the associated SoundTouch classes for
    independant pitch and tempo scaling. By default this is enabled.
 */
#define DROWAUDIO_USE_SOUNDTOUCH 1

/** Config: DROWAUDIO_USE_CURL
    Enables the cURL library and the associated network classes. By default
    this is enabled.
 */
#define DROWAUDIO_USE_CURL 0

#endif // BUILD_DROWAUDIO_APPCONFIG_H_INCLUDED
