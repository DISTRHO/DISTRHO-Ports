/*

    IMPORTANT! This file is auto-generated each time you save your
    project - if you alter its contents, your changes may be overwritten!

    There's a section below where you can add your own custom code safely, and the
    Projucer will preserve the contents of that block, but the best way to change
    any of these definitions is by using the Projucer's project settings.

    Any commented-out settings will assume their default values.

*/

#pragma once

//==============================================================================

#ifndef  JucePlugin_Enable_IAA
 #define JucePlugin_Enable_IAA             0
#endif
#ifndef  JucePlugin_Name
 #define JucePlugin_Name                   "Temper"
#endif
#ifndef  JucePlugin_Desc
 #define JucePlugin_Desc                   "Modern digital distortion."
#endif
#ifndef  JucePlugin_Manufacturer
 #define JucePlugin_Manufacturer           "Creative Intent"
#endif
#ifndef  JucePlugin_ManufacturerWebsite
 #define JucePlugin_ManufacturerWebsite    "http://www.creativeintent.co"
#endif
#ifndef  JucePlugin_ManufacturerEmail
 #define JucePlugin_ManufacturerEmail      ""
#endif
#ifndef  JucePlugin_ManufacturerCode
 #define JucePlugin_ManufacturerCode       0x4376696e // 'Cvin'
#endif
#ifndef  JucePlugin_PluginCode
 #define JucePlugin_PluginCode             0x546d7072 // 'Tmpr'
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
 #define JucePlugin_Version                1.0.3
#endif
#ifndef  JucePlugin_VersionCode
 #define JucePlugin_VersionCode            0x10003
#endif
#ifndef  JucePlugin_VersionString
 #define JucePlugin_VersionString          "1.0.3"
#endif
#ifndef  JucePlugin_VSTUniqueID
 #define JucePlugin_VSTUniqueID            JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_VSTCategory
 #define JucePlugin_VSTCategory            kPlugCategEffect
#endif
#ifndef  JucePlugin_AUMainType
 #define JucePlugin_AUMainType             kAudioUnitType_Effect
#endif
#ifndef  JucePlugin_AUSubType
 #define JucePlugin_AUSubType              JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_AUExportPrefix
 #define JucePlugin_AUExportPrefix         TemperAU
#endif
#ifndef  JucePlugin_AUExportPrefixQuoted
 #define JucePlugin_AUExportPrefixQuoted   "TemperAU"
#endif
#ifndef  JucePlugin_AUManufacturerCode
 #define JucePlugin_AUManufacturerCode     JucePlugin_ManufacturerCode
#endif
#ifndef  JucePlugin_CFBundleIdentifier
 #define JucePlugin_CFBundleIdentifier     com.creativeintent.temper
#endif
#ifndef  JucePlugin_RTASCategory
 #define JucePlugin_RTASCategory           ePlugInCategory_None
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
 #define JucePlugin_AAXIdentifier          com.creativeintent.temper
#endif
#ifndef  JucePlugin_AAXManufacturerCode
 #define JucePlugin_AAXManufacturerCode    JucePlugin_ManufacturerCode
#endif
#ifndef  JucePlugin_AAXProductId
 #define JucePlugin_AAXProductId           JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_AAXCategory
 #define JucePlugin_AAXCategory            AAX_ePlugInCategory_Dynamics
#endif
#ifndef  JucePlugin_AAXDisableBypass
 #define JucePlugin_AAXDisableBypass       0
#endif
#ifndef  JucePlugin_AAXDisableMultiMono
 #define JucePlugin_AAXDisableMultiMono    0
#endif
#ifndef  JucePlugin_IAAType
 #define JucePlugin_IAAType                0x61757278 // 'aurx'
#endif
#ifndef  JucePlugin_IAASubType
 #define JucePlugin_IAASubType             JucePlugin_PluginCode
#endif
#ifndef  JucePlugin_IAAName
 #define JucePlugin_IAAName                "Creative Intent: Temper"
#endif

#define JucePlugin_LV2URI                   "https://github.com/creativeintent/temper"
#define JucePlugin_LV2Category              "DistortionPlugin"
#define JucePlugin_WantsLV2Latency          0
#define JucePlugin_WantsLV2State            0
#define JucePlugin_WantsLV2StateString      0
#define JucePlugin_WantsLV2Presets          1
#define JucePlugin_WantsLV2TimePos          0
