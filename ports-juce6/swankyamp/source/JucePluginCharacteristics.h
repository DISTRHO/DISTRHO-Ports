#ifndef __JUCE_PLUGIN_CHARACTERISTICS_H__
#define __JUCE_PLUGIN_CHARACTERISTICS_H__

#define JucePlugin_Name                   "Swanky Amp"
#define JucePlugin_Desc                   "Tube amplifiers simulation"
#define JucePlugin_Manufacturer           "Resonant DSP"
#define JucePlugin_ManufacturerWebsite    "https://www.resonantdsp.com/"
#define JucePlugin_ManufacturerEmail      "info@resonantdsp.com"
#define JucePlugin_ManufacturerCode       0x4d616e75
#define JucePlugin_PluginCode             0x63637766
#define JucePlugin_IsSynth                0
#define JucePlugin_WantsMidiInput         0
#define JucePlugin_ProducesMidiOutput     0
#define JucePlugin_IsMidiEffect           0
#define JucePlugin_EditorRequiresKeyboardFocus  0
#define JucePlugin_Version                1.4.0
#define JucePlugin_VersionCode            0x10400
#define JucePlugin_VersionString          "1.4.0"
#define JucePlugin_VSTUniqueID            JucePlugin_PluginCode
#define JucePlugin_VSTCategory            kPlugCategEffect
#define JucePlugin_Vst3Category           "Fx"
#define JucePlugin_AUMainType             'aufx'
#define JucePlugin_AUSubType              JucePlugin_PluginCode
#define JucePlugin_AUExportPrefix         SwankyAmpAU
#define JucePlugin_AUExportPrefixQuoted   "SwankyAmpAU"
#define JucePlugin_AUManufacturerCode     JucePlugin_ManufacturerCode
#define JucePlugin_CFBundleIdentifier     com.ResonantDSP.SwankyAmp
#define JucePlugin_RTASCategory           8192
#define JucePlugin_RTASManufacturerCode   JucePlugin_ManufacturerCode
#define JucePlugin_RTASProductId          JucePlugin_PluginCode
#define JucePlugin_RTASDisableBypass      0
#define JucePlugin_RTASDisableMultiMono   0
#define JucePlugin_AAXIdentifier          com.ResonantDSP.SwankyAmp
#define JucePlugin_AAXManufacturerCode    JucePlugin_ManufacturerCode
#define JucePlugin_AAXProductId           JucePlugin_PluginCode
#define JucePlugin_AAXCategory            8192
#define JucePlugin_AAXDisableBypass       0
#define JucePlugin_AAXDisableMultiMono    0
#define JucePlugin_IAAType                0x61757278
#define JucePlugin_IAASubType             JucePlugin_PluginCode
#define JucePlugin_IAAName                "Resonant DSP: Swanky Amp"
#define JucePlugin_VSTNumMidiInputs       16
#define JucePlugin_VSTNumMidiOutputs      16

#define JucePlugin_LV2URI                   "urn:distrho:SwankyAmp"
#define JucePlugin_LV2Category              "SimulatorPlugin"
#define JucePlugin_WantsLV2Latency          0
#define JucePlugin_WantsLV2State            1
#define JucePlugin_WantsLV2TimePos          0
#define JucePlugin_WantsLV2Presets          0

#endif
