#pragma once

#include "JucePluginMain.h"

// #include "BinaryData.h"

#if ! DONT_SET_USING_JUCE_NAMESPACE
 // If your code uses a lot of JUCE classes, then this will obviously save you
 // a lot of typing, but can be disabled by setting DONT_SET_USING_JUCE_NAMESPACE.
 using namespace juce;
#endif

#if ! JUCE_DONT_DECLARE_PROJECTINFO
namespace ProjectInfo
{
    const char* const  projectName    = "Syndicate";
    const char* const  companyName    = "White Elephant Audio";
    const char* const  versionString  = "1.5.0";
    const int          versionNumber  = 0x10500;
}
#endif
