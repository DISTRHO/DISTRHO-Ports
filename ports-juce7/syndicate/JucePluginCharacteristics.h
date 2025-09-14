#pragma once

#define JucePlugin_Name                   "Syndicate"
#define JucePlugin_Desc                   "Hosts your favourite plugins to allow you to create complex parallel chains of effects"
#define JucePlugin_Manufacturer           "White Elephant Audio"
#define JucePlugin_ManufacturerWebsite    "www.whiteelephantaudio.com"
#define JucePlugin_ManufacturerEmail      "info@whiteelephantaudio.com"
#define JucePlugin_ManufacturerCode       'Wele'
#define JucePlugin_PluginCode             'Wsyn'
#define JucePlugin_IsSynth                0
#define JucePlugin_WantsMidiInput         0
#define JucePlugin_ProducesMidiOutput     0
#define JucePlugin_IsMidiEffect           0
#define JucePlugin_EditorRequiresKeyboardFocus  0
#define JucePlugin_Version                1.5.0
#define JucePlugin_VersionCode            0x10500
#define JucePlugin_VersionString          "1.5.0"
#define JucePlugin_VSTUniqueID            JucePlugin_PluginCode
#define JucePlugin_VSTCategory            kPlugCategEffect
#define JucePlugin_Vst3Category           "Fx"
#define JucePlugin_AUMainType             'aufx'
#define JucePlugin_AUSubType              JucePlugin_PluginCode
#define JucePlugin_AUExportPrefix         SyndicateAU
#define JucePlugin_AUExportPrefixQuoted   "SyndicateAU"
#define JucePlugin_AUManufacturerCode     JucePlugin_ManufacturerCode
#define JucePlugin_CFBundleIdentifier     com.whiteelephantaudio.Syndicate

#define JucePlugin_LV2URI                   "https://whiteelephantaudio.com/plugins/syndicate"
