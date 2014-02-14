/*
  Copyright (C) 2007 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
   
  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
  
*/

#ifndef __PLUGINCHARACTERISTICS_81BD1C82__
#define __PLUGINCHARACTERISTICS_81BD1C82__

#define JucePlugin_Name                 "CabbageEffectNam"
#define JucePlugin_Desc                 "CabbagePlugin"
#define JucePlugin_Manufacturer         "CabbageAudio"
#define JucePlugin_ManufacturerCode     'Cabb'
#define JucePlugin_PluginCode           'RORY'

#if defined(Cabbage_Plugin_Midi)
 #define JucePlugin_IsSynth              0
 #define JucePlugin_MaxNumInputChannels  0
 #define JucePlugin_MaxNumOutputChannels 0
 #define JucePlugin_PreferredChannelConfigurations {0, 0}
 #define JucePlugin_WantsMidiInput       1
 #define JucePlugin_ProducesMidiOutput   1
//#define JucePlugin_LV2Category          "MidiPlugin"
#elif defined(Cabbage_Plugin_Synth)
 #define JucePlugin_IsSynth              1
 #define JucePlugin_MaxNumInputChannels  0
 #define JucePlugin_MaxNumOutputChannels 2
 #define JucePlugin_PreferredChannelConfigurations {0, 2}
 #define JucePlugin_WantsMidiInput       1
 #define JucePlugin_ProducesMidiOutput   0
 #define JucePlugin_LV2Category          "InstrumentPlugin"
#else
 #define JucePlugin_IsSynth              0
 #define JucePlugin_MaxNumInputChannels  2
 #define JucePlugin_MaxNumOutputChannels 2
 #define JucePlugin_PreferredChannelConfigurations {1, 1}, {2, 2}
 #define JucePlugin_WantsMidiInput       0
 #define JucePlugin_ProducesMidiOutput   0
#endif

#ifdef Cabbage_Plugin_Synth
 #define JucePlugin_AUMainType kAudioUnitType_MusicDevice
#else
 #define JucePlugin_AUMainType kAudioUnitType_Effect
#endif

#define JucePlugin_SilenceInProducesSilenceOut  1
#define JucePlugin_TailLengthSeconds    0
#define JucePlugin_EditorRequiresKeyboardFocus  1
#define JucePlugin_VersionCode          0x10000
#define JucePlugin_VersionString        "0.0.0"
#define JucePlugin_VSTUniqueID          JucePlugin_PluginCode
#define JucePlugin_VSTCategory          kPlugCategEffect
#define JucePlugin_AUSubType            JucePlugin_PluginCode
#define JucePlugin_AUExportPrefix       JuceProjectAU
#define JucePlugin_AUExportPrefixQuoted "CaabageAU"
#define JucePlugin_AUManufacturerCode   JucePlugin_ManufacturerCode
#define JucePlugin_CFBundleIdentifier   com.CabbageFoundation.CabbagePlugin
#define JucePlugin_AUCocoaViewClassName CabbageProjectAU_V1
#define JucePlugin_RTASCategory         ePlugInCategory_SWGenerators
#define JucePlugin_RTASManufacturerCode JucePlugin_ManufacturerCode
#define JucePlugin_RTASProductId        JucePlugin_PluginCode
#define JucePlugin_AAXIdentifier        com.yourcompany.CabbagePlugin
#define JucePlugin_AAXManufacturerCode  JucePlugin_ManufacturerCode
#define JucePlugin_AAXProductId         JucePlugin_PluginCode
#define JucePlugin_AAXPluginId          JucePlugin_PluginCode
#define JucePlugin_AAXCategory          AAX_ePlugInCategory_Dynamics

#define JucePlugin_LV2URI               "urn:cabbage:plugin"
#define JucePlugin_WantsLV2TimePos      1

#endif   // __PLUGINCHARACTERISTICS_81BD1C82__
