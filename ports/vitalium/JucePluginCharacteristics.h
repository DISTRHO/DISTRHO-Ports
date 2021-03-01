
#ifndef __JUCE_PLUGIN_CHARACTERISTICS_H__
#define __JUCE_PLUGIN_CHARACTERISTICS_H__

#define JucePlugin_Name                     "Vitalium"
#define JucePlugin_Desc                     "Vitalium"
#define JucePlugin_Manufacturer             "DISTRHO"
#define JucePlugin_ManufacturerCode         'DSTR'
#define JucePlugin_ManufacturerEmail        "info@kx.studio"
#define JucePlugin_ManufacturerWebsite      "vitalium.distrho.kx.studio"
#define JucePlugin_PluginCode               'Vita'

#define JucePlugin_VersionCode              0x10006
#define JucePlugin_VersionString            "1.0.6"

#define JucePlugin_MaxNumInputChannels              0
#define JucePlugin_MaxNumOutputChannels             2
#define JucePlugin_PreferredChannelConfigurations   { 0, 2 }
#define JucePlugin_IsSynth                          1
#define JucePlugin_WantsMidiInput                   1
#define JucePlugin_ProducesMidiOutput               0
#define JucePlugin_SilenceInProducesSilenceOut      0
#define JucePlugin_EditorRequiresKeyboardFocus      1 // pluginEditorRequiresKeys
#define JucePlugin_IsMidiEffect                     0

// aaxIdentifier="audio.vial.synth"
// pluginAUExportPrefix="vial"
// pluginRTASCategory=""
// pluginAAXCategory="2048"

#define JucePlugin_VSTUniqueID              JucePlugin_PluginCode
#define JucePlugin_VSTCategory              kPlugCategSynth

#define JucePlugin_LV2URI                   "urn:distrho:vitalium"
#define JucePlugin_LV2Category              "InstrumentPlugin"
#define JucePlugin_WantsLV2Latency          0
#define JucePlugin_WantsLV2State            1
#define JucePlugin_WantsLV2TimePos          1
#define JucePlugin_WantsLV2Presets          0

#endif
