/*

    IMPORTANT! This file is auto-generated each time you save your
    project - if you alter its contents, your changes may be overwritten!

*/

#pragma once

//==============================================================================
// Audio plugin settings..

#ifndef  JucePlugin_Enable_IAA
 #define JucePlugin_Enable_IAA             0
#endif
#ifndef  JucePlugin_Name
 #define JucePlugin_Name                   "Roth-AIR"
#endif
#ifndef  JucePlugin_Desc
 #define JucePlugin_Desc                   "A mixing tool for adding airy, crispy presence"
#endif
#ifndef  JucePlugin_Manufacturer
 #define JucePlugin_Manufacturer           "Rothmann"
#endif
#ifndef  JucePlugin_ManufacturerWebsite
 #define JucePlugin_ManufacturerWebsite    "www.danielrothmann.com"
#endif
#ifndef  JucePlugin_ManufacturerEmail
 #define JucePlugin_ManufacturerEmail      "daniel@danielrothmann.com"
#endif
#ifndef  JucePlugin_ManufacturerCode
 #define JucePlugin_ManufacturerCode       0x526f7468
#endif
#ifndef  JucePlugin_PluginCode
 #define JucePlugin_PluginCode             0x4a367663
#endif
#ifndef  JucePlugin_IsSynth
 #define JucePlugin_IsSynth                0
#endif
#ifndef  JucePlugin_WantsMidiInput
 #define JucePlugin_WantsMidiInput         0
#endif
#ifndef  JucePlugin_ProducesMidiOutput
 #define JucePlugin_ProducesMidiOutput     0
#endif
#ifndef  JucePlugin_IsMidiEffect
 #define JucePlugin_IsMidiEffect           0
#endif
#ifndef  JucePlugin_EditorRequiresKeyboardFocus
 #define JucePlugin_EditorRequiresKeyboardFocus  0
#endif
#ifndef  JucePlugin_Version
 #define JucePlugin_Version                1.1.1
#endif
#ifndef  JucePlugin_VersionCode
 #define JucePlugin_VersionCode            0x10101
#endif
#ifndef  JucePlugin_VersionString
 #define JucePlugin_VersionString          "1.1.1"
#endif
#ifndef  JucePlugin_VSTUniqueID
 #define JucePlugin_VSTUniqueID            JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_VSTCategory
 #define JucePlugin_VSTCategory            kPlugCategEffect
#endif
#ifndef  JucePlugin_Vst3Category
 #define JucePlugin_Vst3Category           "Fx|Dynamics"
#endif
#ifndef  JucePlugin_AUMainType
 #define JucePlugin_AUMainType             'aufx'
#endif
#ifndef  JucePlugin_AUSubType
 #define JucePlugin_AUSubType              JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_AUExportPrefix
 #define JucePlugin_AUExportPrefix         AIRAU
#endif
#ifndef  JucePlugin_AUExportPrefixQuoted
 #define JucePlugin_AUExportPrefixQuoted   "AIRAU"
#endif
#ifndef  JucePlugin_AUManufacturerCode
 #define JucePlugin_AUManufacturerCode     JucePlugin_ManufacturerCode
#endif
#ifndef  JucePlugin_CFBundleIdentifier
 #define JucePlugin_CFBundleIdentifier     com.Rothmann.Roth-AIR
#endif
#ifndef  JucePlugin_RTASCategory
 #define JucePlugin_RTASCategory           0
#endif
#ifndef  JucePlugin_RTASManufacturerCode
 #define JucePlugin_RTASManufacturerCode   JucePlugin_ManufacturerCode
#endif
#ifndef  JucePlugin_RTASProductId
 #define JucePlugin_RTASProductId          JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_RTASDisableBypass
 #define JucePlugin_RTASDisableBypass      0
#endif
#ifndef  JucePlugin_RTASDisableMultiMono
 #define JucePlugin_RTASDisableMultiMono   0
#endif
#ifndef  JucePlugin_AAXIdentifier
 #define JucePlugin_AAXIdentifier          com.Rothmann.Roth-AIR
#endif
#ifndef  JucePlugin_AAXManufacturerCode
 #define JucePlugin_AAXManufacturerCode    JucePlugin_ManufacturerCode
#endif
#ifndef  JucePlugin_AAXProductId
 #define JucePlugin_AAXProductId           JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_AAXCategory
 #define JucePlugin_AAXCategory            2
#endif
#ifndef  JucePlugin_AAXDisableBypass
 #define JucePlugin_AAXDisableBypass       0
#endif
#ifndef  JucePlugin_AAXDisableMultiMono
 #define JucePlugin_AAXDisableMultiMono    0
#endif
#ifndef  JucePlugin_IAAType
 #define JucePlugin_IAAType                0x61757278
#endif
#ifndef  JucePlugin_IAASubType
 #define JucePlugin_IAASubType             JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_IAAName
 #define JucePlugin_IAAName                "Rothmann: Roth-AIR"
#endif
#ifndef  JucePlugin_VSTNumMidiInputs
 #define JucePlugin_VSTNumMidiInputs       16
#endif
#ifndef  JucePlugin_VSTNumMidiOutputs
 #define JucePlugin_VSTNumMidiOutputs      16
#endif
#ifndef  JucePlugin_MaxNumInputChannels
 #define JucePlugin_MaxNumInputChannels    2
#endif
#ifndef  JucePlugin_MaxNumOutputChannels
 #define JucePlugin_MaxNumOutputChannels   2
#endif
#ifndef  JucePlugin_PreferredChannelConfigurations
 #define JucePlugin_PreferredChannelConfigurations  {1, 1},{2, 2}
#endif

#define JucePlugin_LV2URI                  "http://danielrothmann.com/Roth-AIR"
#define JucePlugin_LV2Category             "DynamicsPlugin"
#define JucePlugin_WantsLV2Latency         0
#define JucePlugin_WantsLV2State           0
#define JucePlugin_WantsLV2TimePos         0
#define JucePlugin_WantsLV2Presets         0
