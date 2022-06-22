/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#include "../../juce_core/system/juce_TargetPlatform.h"

//==============================================================================
#if JucePlugin_Build_VST3 && (__APPLE_CPP__ || __APPLE_CC__ || _WIN32 || _WIN64)

#if JUCE_PLUGINHOST_VST3 && (JUCE_MAC || JUCE_WINDOWS)
 #undef JUCE_VST3HEADERS_INCLUDE_HEADERS_ONLY
 #define JUCE_VST3HEADERS_INCLUDE_HEADERS_ONLY 1
#endif

#include "../../juce_audio_processors/format_types/juce_VST3Headers.h"

#undef JUCE_VST3HEADERS_INCLUDE_HEADERS_ONLY

#include "../utility/juce_CheckSettingMacros.h"
#include "../utility/juce_IncludeModuleHeaders.h"
#include "../utility/juce_WindowsHooks.h"
#include "../utility/juce_FakeMouseMoveGenerator.h"
#include "../../juce_audio_processors/format_types/juce_VST3Common.h"

#ifndef JUCE_VST3_CAN_REPLACE_VST2
 #define JUCE_VST3_CAN_REPLACE_VST2 1
#endif

#if JUCE_VST3_CAN_REPLACE_VST2
#include "../../juce_audio_processors/format_types/juce_VSTInterface.h"
#endif

#ifndef JUCE_VST3_EMULATE_MIDI_CC_WITH_PARAMETERS
 #define JUCE_VST3_EMULATE_MIDI_CC_WITH_PARAMETERS 1
#endif

#if JUCE_VST3_CAN_REPLACE_VST2
 #if JUCE_MSVC
  #pragma warning (push)
  #pragma warning (disable: 4514 4996)
 #endif

 #if JUCE_MSVC
  #pragma warning (pop)
 #endif
#endif

namespace juce
{

using namespace Steinberg;

//==============================================================================
#if JUCE_MAC
  extern void initialiseMacVST();

 #if ! JUCE_64BIT
  extern void updateEditorCompBoundsVST (Component*);
 #endif

  extern JUCE_API void* attachComponentToWindowRefVST (Component*, void* parentWindowOrView, bool isNSView);
  extern JUCE_API void detachComponentFromWindowRefVST (Component*, void* nsWindow, bool isNSView);
  extern JUCE_API void setNativeHostWindowSizeVST (void* window, Component*, int newWidth, int newHeight, bool isNSView);
#endif

//==============================================================================
class JuceAudioProcessor  : public FUnknown
{
public:
    JuceAudioProcessor (AudioProcessor* source) noexcept  : audioProcessor (source) {}
    virtual ~JuceAudioProcessor() {}

    AudioProcessor* get() const noexcept      { return audioProcessor; }

    JUCE_DECLARE_VST3_COM_QUERY_METHODS
    JUCE_DECLARE_VST3_COM_REF_METHODS

    static const FUID iid;

    bool isBypassed = false;

private:
    Atomic<int> refCount;
    ScopedPointer<AudioProcessor> audioProcessor;
    ScopedJuceInitialiser_GUI libraryInitialiser;

    JuceAudioProcessor() JUCE_DELETED_FUNCTION;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceAudioProcessor)
};

class JuceVST3Component;

//==============================================================================
class JuceVST3EditController : public Vst::EditController,
                               public Vst::IMidiMapping,
                               public Vst::ChannelContext::IInfoListener,
                               public AudioProcessorListener
{
public:
    JuceVST3EditController (Vst::IHostApplication* host)
    {
        if (host != nullptr)
            host->queryInterface (FUnknown::iid, (void**) &hostContext);
    }

    //==============================================================================
    static const FUID iid;

    //==============================================================================
   #if JUCE_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Winconsistent-missing-override"
   #endif

    REFCOUNT_METHODS (ComponentBase)

   #if JUCE_CLANG
    #pragma clang diagnostic pop
   #endif

    tresult PLUGIN_API queryInterface (const TUID targetIID, void** obj) override
    {
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, FObject)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, JuceVST3EditController)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::IEditController)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::IEditController2)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::IConnectionPoint)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::IMidiMapping)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::ChannelContext::IInfoListener)
        TEST_FOR_COMMON_BASE_AND_RETURN_IF_VALID (targetIID, IPluginBase, Vst::IEditController)
        TEST_FOR_COMMON_BASE_AND_RETURN_IF_VALID (targetIID, IDependent, Vst::IEditController)
        TEST_FOR_COMMON_BASE_AND_RETURN_IF_VALID (targetIID, FUnknown, Vst::IEditController)

        if (doUIDsMatch (targetIID, JuceAudioProcessor::iid))
        {
            audioProcessor->addRef();
            *obj = audioProcessor;
            return kResultOk;
        }

        *obj = nullptr;
        return kNoInterface;
    }

    //==============================================================================
    tresult PLUGIN_API initialize (FUnknown* context) override
    {
        if (hostContext != context)
        {
            if (hostContext != nullptr)
                hostContext->release();

            hostContext = context;

            if (hostContext != nullptr)
                hostContext->addRef();
        }

        return kResultTrue;
    }

    tresult PLUGIN_API terminate() override
    {
        if (auto* pluginInstance = getPluginInstance())
            pluginInstance->removeListener (this);

        audioProcessor = nullptr;

        return EditController::terminate();
    }

    //==============================================================================
    enum InternalParameters
    {
        paramPreset               = 0x70727374, // 'prst'
        paramBypass               = 0x62797073, // 'byps'
        paramMidiControllerOffset = 0x6d636d00  // 'mdm*'
    };

    struct Param  : public Vst::Parameter
    {
        Param (AudioProcessor& p, int index, Vst::ParamID paramID)  : owner (p), paramIndex (index)
        {
            info.id = paramID;

            toString128 (info.title, p.getParameterName (index));
            toString128 (info.shortTitle, p.getParameterName (index, 8));
            toString128 (info.units, p.getParameterLabel (index));

            info.stepCount = (Steinberg::int32) 0;

           #if ! JUCE_FORCE_LEGACY_PARAMETER_AUTOMATION_TYPE
            if (p.isParameterDiscrete (index))
           #endif
            {
                const int numSteps = p.getParameterNumSteps (index);
                info.stepCount = (Steinberg::int32) (numSteps > 0 && numSteps < 0x7fffffff ? numSteps - 1 : 0);
            }

            info.defaultNormalizedValue = p.getParameterDefaultValue (index);
            jassert (info.defaultNormalizedValue >= 0 && info.defaultNormalizedValue <= 1.0f);
            info.unitId = Vst::kRootUnitId;

            // Is this a meter?
            if (((p.getParameterCategory (index) & 0xffff0000) >> 16) == 2)
                info.flags = Vst::ParameterInfo::kIsReadOnly;
            else
                info.flags = p.isParameterAutomatable (index) ? Vst::ParameterInfo::kCanAutomate : 0;

            valueNormalized = info.defaultNormalizedValue;
        }

        virtual ~Param() {}

        bool setNormalized (Vst::ParamValue v) override
        {
            v = jlimit (0.0, 1.0, v);

            if (v != valueNormalized)
            {
                valueNormalized = v;
                owner.setParameter (paramIndex, static_cast<float> (v));

                changed();
                return true;
            }

            return false;
        }

        void toString (Vst::ParamValue value, Vst::String128 result) const override
        {
            if (auto* p = owner.getParameters()[paramIndex])
                toString128 (result, p->getText ((float) value, 128));
            else
                // remain backward-compatible with old JUCE code
                toString128 (result, owner.getParameterText (paramIndex, 128));
        }

        bool fromString (const Vst::TChar* text, Vst::ParamValue& outValueNormalized) const override
        {
            if (auto* p = owner.getParameters()[paramIndex])
            {
                outValueNormalized = p->getValueForText (getStringFromVstTChars (text));
                return true;
            }

            return false;
        }

        static String getStringFromVstTChars (const Vst::TChar* text)
        {
            return juce::String (juce::CharPointer_UTF16 (reinterpret_cast<const juce::CharPointer_UTF16::CharType*> (text)));
        }

        Vst::ParamValue toPlain (Vst::ParamValue v) const override       { return v; }
        Vst::ParamValue toNormalized (Vst::ParamValue v) const override  { return v; }

    private:
        AudioProcessor& owner;
        int paramIndex;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Param)
    };

    //==============================================================================
    struct BypassParam  : public Vst::Parameter
    {
        BypassParam (Vst::ParamID vstParamID)
        {
            info.id = vstParamID;
            toString128 (info.title, "Bypass");
            toString128 (info.shortTitle, "Bypass");
            toString128 (info.units, "");
            info.stepCount = 1;
            info.defaultNormalizedValue = 0.0f;
            info.unitId = Vst::kRootUnitId;
            info.flags = Vst::ParameterInfo::kIsBypass | Vst::ParameterInfo::kCanAutomate;
        }

        virtual ~BypassParam() {}

        bool setNormalized (Vst::ParamValue v) override
        {
            bool bypass = (v != 0.0f);
            v = (bypass ? 1.0f : 0.0f);

            if (valueNormalized != v)
            {
                valueNormalized = v;
                changed();
                return true;
            }

            return false;
        }

        void toString (Vst::ParamValue value, Vst::String128 result) const override
        {
            bool bypass = (value != 0.0f);
            toString128 (result, bypass ? "On" : "Off");
        }

        bool fromString (const Vst::TChar* text, Vst::ParamValue& outValueNormalized) const override
        {
            auto paramValueString = getStringFromVstTChars (text);

            if (paramValueString.equalsIgnoreCase ("on")
                 || paramValueString.equalsIgnoreCase ("yes")
                 || paramValueString.equalsIgnoreCase ("true"))
            {
                outValueNormalized = 1.0f;
                return true;
            }

            if (paramValueString.equalsIgnoreCase ("off")
                 || paramValueString.equalsIgnoreCase ("no")
                 || paramValueString.equalsIgnoreCase ("false"))
            {
                outValueNormalized = 0.0f;
                return true;
            }

            var varValue = JSON::fromString (paramValueString);

            if (varValue.isDouble() || varValue.isInt()
                 || varValue.isInt64() || varValue.isBool())
            {
                double value = varValue;
                outValueNormalized = (value != 0.0) ? 1.0f : 0.0f;
                return true;
            }

            return false;
        }

        static String getStringFromVstTChars (const Vst::TChar* text)
        {
            return juce::String (juce::CharPointer_UTF16 (reinterpret_cast<const juce::CharPointer_UTF16::CharType*> (text)));
        }

        Vst::ParamValue toPlain (Vst::ParamValue v) const override       { return v; }
        Vst::ParamValue toNormalized (Vst::ParamValue v) const override  { return v; }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BypassParam)
    };

    //==============================================================================
    struct ProgramChangeParameter  : public Vst::Parameter
    {
        ProgramChangeParameter (AudioProcessor& p)  : owner (p)
        {
            jassert (owner.getNumPrograms() > 1);

            info.id = paramPreset;
            toString128 (info.title, "Program");
            toString128 (info.shortTitle, "Program");
            toString128 (info.units, "");
            info.stepCount = owner.getNumPrograms() - 1;
            info.defaultNormalizedValue = static_cast<Vst::ParamValue> (owner.getCurrentProgram())
                                            / static_cast<Vst::ParamValue> (info.stepCount);
            info.unitId = Vst::kRootUnitId;
            info.flags = Vst::ParameterInfo::kIsProgramChange | Vst::ParameterInfo::kCanAutomate;
        }

        virtual ~ProgramChangeParameter() {}

        bool setNormalized (Vst::ParamValue v) override
        {
            Vst::ParamValue program = v * info.stepCount;

            if (! isPositiveAndBelow ((int) program, owner.getNumPrograms()))
                return false;

            if (valueNormalized != v)
            {
                valueNormalized = v;
                changed();
                return true;
            }

            return false;
        }

        void toString (Vst::ParamValue value, Vst::String128 result) const override
        {
            toString128 (result, owner.getProgramName (static_cast<int> (value * info.stepCount)));
        }

        bool fromString (const Vst::TChar* text, Vst::ParamValue& outValueNormalized) const override
        {
            auto paramValueString = getStringFromVstTChars (text);
            auto n = owner.getNumPrograms();

            for (int i = 0; i < n; ++i)
            {
                if (paramValueString == owner.getProgramName (i))
                {
                    outValueNormalized = static_cast<Vst::ParamValue> (i) / info.stepCount;
                    return true;
                }
            }

            return false;
        }

        static String getStringFromVstTChars (const Vst::TChar* text)
        {
            return String (CharPointer_UTF16 (reinterpret_cast<const CharPointer_UTF16::CharType*> (text)));
        }

        Vst::ParamValue toPlain (Vst::ParamValue v) const override       { return v * info.stepCount; }
        Vst::ParamValue toNormalized (Vst::ParamValue v) const override  { return v / info.stepCount; }

    private:
        AudioProcessor& owner;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgramChangeParameter)
    };

    //==============================================================================
    tresult PLUGIN_API setChannelContextInfos (Vst::IAttributeList* list) override
    {
        if (auto* instance = getPluginInstance())
        {
            if (list != nullptr)
            {
                AudioProcessor::TrackProperties trackProperties;

                {
                    Vst::String128 channelName;
                    if (list->getString (Vst::ChannelContext::kChannelNameKey, channelName, sizeof (channelName)) == kResultTrue)
                        trackProperties.name = toString (channelName);
                }

                {
                    int64 colour;
                    if (list->getInt (Vst::ChannelContext::kChannelColorKey, colour) == kResultTrue)
                        trackProperties.colour = Colour (Vst::ChannelContext::GetRed ((uint32) colour),  Vst::ChannelContext::GetGreen ((uint32) colour),
                                                         Vst::ChannelContext::GetBlue ((uint32) colour), Vst::ChannelContext::GetAlpha ((uint32) colour));
                }



                if (MessageManager::getInstance()->isThisTheMessageThread())
                    instance->updateTrackProperties (trackProperties);
                else
                    MessageManager::callAsync ([trackProperties, instance] ()
                                               { instance->updateTrackProperties (trackProperties); });
            }
        }

        return kResultOk;
    }

    //==============================================================================
    tresult PLUGIN_API setComponentState (IBStream* stream) override
    {
        // Cubase and Nuendo need to inform the host of the current parameter values
        if (auto* pluginInstance = getPluginInstance())
        {
            auto numParameters = pluginInstance->getNumParameters();

            for (int i = 0; i < numParameters; ++i)
                setParamNormalized (getVSTParamIDForIndex (i), (double) pluginInstance->getParameter (i));

            setParamNormalized (bypassParamID, audioProcessor->isBypassed ? 1.0f : 0.0f);

            auto numPrograms = pluginInstance->getNumPrograms();

            if (numPrograms > 1)
                setParamNormalized (paramPreset, static_cast<Vst::ParamValue> (pluginInstance->getCurrentProgram())
                                                  / static_cast<Vst::ParamValue> (numPrograms - 1));
        }

        if (auto* handler = getComponentHandler())
            handler->restartComponent (Vst::kParamValuesChanged);

        return Vst::EditController::setComponentState (stream);
    }

    void setAudioProcessor (JuceAudioProcessor* audioProc)
    {
        if (audioProcessor != audioProc)
        {
            audioProcessor = audioProc;
            setupParameters();
        }
    }

    tresult PLUGIN_API connect (IConnectionPoint* other) override
    {
        if (other != nullptr && audioProcessor == nullptr)
        {
            auto result = ComponentBase::connect (other);

            if (! audioProcessor.loadFrom (other))
                sendIntMessage ("JuceVST3EditController", (Steinberg::int64) (pointer_sized_int) this);
            else
                setupParameters();

            return result;
        }

        jassertfalse;
        return kResultFalse;
    }

    //==============================================================================
    tresult PLUGIN_API getMidiControllerAssignment (Steinberg::int32 /*busIndex*/, Steinberg::int16 channel,
                                                    Vst::CtrlNumber midiControllerNumber, Vst::ParamID& resultID) override
    {
        resultID = midiControllerToParameter[channel][midiControllerNumber];

        return kResultTrue; // Returning false makes some hosts stop asking for further MIDI Controller Assignments
    }

    // Converts an incoming parameter index to a MIDI controller:
    bool getMidiControllerForParameter (Vst::ParamID index, int& channel, int& ctrlNumber)
    {
        auto mappedIndex = static_cast<int> (index - parameterToMidiControllerOffset);

        if (isPositiveAndBelow (mappedIndex, numElementsInArray (parameterToMidiController)))
        {
            auto& mc = parameterToMidiController[mappedIndex];

            if (mc.channel != -1 && mc.ctrlNumber != -1)
            {
                channel = jlimit (1, 16, mc.channel + 1);
                ctrlNumber = mc.ctrlNumber;
                return true;
            }
        }

        return false;
    }

    inline bool isMidiControllerParamID (Vst::ParamID paramID) const noexcept
    {
        return (paramID >= parameterToMidiControllerOffset
                    && isPositiveAndBelow (paramID - parameterToMidiControllerOffset,
                                           static_cast<Vst::ParamID> (numElementsInArray (parameterToMidiController))));
    }

    //==============================================================================
    IPlugView* PLUGIN_API createView (const char* name) override
    {
        if (auto* pluginInstance = getPluginInstance())
        {
            if (pluginInstance->hasEditor() && name != nullptr
                 && strcmp (name, Vst::ViewType::kEditor) == 0)
            {
                return new JuceVST3Editor (*this, *pluginInstance);
            }
        }

        return nullptr;
    }

    //==============================================================================
    void audioProcessorParameterChangeGestureBegin (AudioProcessor*, int index) override        { beginEdit (getVSTParamIDForIndex (index)); }
    void audioProcessorParameterChangeGestureEnd (AudioProcessor*, int index) override          { endEdit (getVSTParamIDForIndex (index)); }

    void audioProcessorParameterChanged (AudioProcessor*, int index, float newValue) override
    {
        // NB: Cubase has problems if performEdit is called without setParamNormalized
        EditController::setParamNormalized (getVSTParamIDForIndex (index), (double) newValue);
        performEdit (getVSTParamIDForIndex (index), (double) newValue);
    }

    void audioProcessorChanged (AudioProcessor*) override
    {
        if (auto* pluginInstance = getPluginInstance())
        {
            if (pluginInstance->getNumPrograms() > 1)
                EditController::setParamNormalized (paramPreset, static_cast<Vst::ParamValue> (pluginInstance->getCurrentProgram())
                                                               / static_cast<Vst::ParamValue> (pluginInstance->getNumPrograms() - 1));
        }

        if (componentHandler != nullptr)
            componentHandler->restartComponent (Vst::kLatencyChanged | Vst::kParamValuesChanged);
    }

    //==============================================================================
    AudioProcessor* getPluginInstance() const noexcept
    {
        if (audioProcessor != nullptr)
            return audioProcessor->get();

        return nullptr;
    }

private:
    friend class JuceVST3Component;

    //==============================================================================
    ComSmartPtr<JuceAudioProcessor> audioProcessor;
    ScopedJuceInitialiser_GUI libraryInitialiser;

    struct MidiController
    {
        int channel = -1, ctrlNumber = -1;
    };

    enum { numMIDIChannels = 16 };
    Vst::ParamID parameterToMidiControllerOffset;
    MidiController parameterToMidiController[numMIDIChannels * Vst::kCountCtrlNumber];
    Vst::ParamID midiControllerToParameter[numMIDIChannels][Vst::kCountCtrlNumber];

    //==============================================================================
   #if ! JUCE_FORCE_USE_LEGACY_PARAM_IDS
    bool usingManagedParameter = false;
    Array<Vst::ParamID> vstParamIDs;
   #endif
    Vst::ParamID bypassParamID;


    //==============================================================================
    void setupParameters()
    {
        if (auto* pluginInstance = getPluginInstance())
        {
            pluginInstance->addListener (this);

           #if JUCE_FORCE_USE_LEGACY_PARAM_IDS
            const bool usingManagedParameter = false;
           #endif

            if (parameters.getParameterCount() <= 0)
            {
                auto numParameters = pluginInstance->getNumParameters();

              #if ! JUCE_FORCE_USE_LEGACY_PARAM_IDS
                usingManagedParameter = (pluginInstance->getParameters().size() == numParameters);
              #endif

                for (int i = 0; i < numParameters; ++i)
                {
                   #if JUCE_FORCE_USE_LEGACY_PARAM_IDS
                    const Vst::ParamID vstParamID = static_cast<Vst::ParamID> (i);
                   #else
                    const Vst::ParamID vstParamID = generateVSTParamIDForIndex (pluginInstance, i);
                    vstParamIDs.add (vstParamID);
                   #endif

                    parameters.addParameter (new Param (*pluginInstance, i, vstParamID));
                }

                bypassParamID = static_cast<Vst::ParamID> (usingManagedParameter ? paramBypass : numParameters);
                parameters.addParameter (new BypassParam (bypassParamID));

                if (pluginInstance->getNumPrograms() > 1)
                    parameters.addParameter (new ProgramChangeParameter (*pluginInstance));
            }

           #if JUCE_VST3_EMULATE_MIDI_CC_WITH_PARAMETERS
            parameterToMidiControllerOffset = static_cast<Vst::ParamID> (usingManagedParameter ? paramMidiControllerOffset
                                                                                               : parameters.getParameterCount());

            initialiseMidiControllerMappings();
           #endif

            audioProcessorChanged (pluginInstance);
        }
    }

    void initialiseMidiControllerMappings()
    {
        for (int c = 0, p = 0; c < numMIDIChannels; ++c)
        {
            for (int i = 0; i < Vst::kCountCtrlNumber; ++i, ++p)
            {
                midiControllerToParameter[c][i] = static_cast<Vst::ParamID> (p) + parameterToMidiControllerOffset;
                parameterToMidiController[p].channel = c;
                parameterToMidiController[p].ctrlNumber = i;

                parameters.addParameter (new Vst::Parameter (toString ("MIDI CC " + String (c) + "|" + String (i)),
                                          static_cast<Vst::ParamID> (p) + parameterToMidiControllerOffset, 0, 0, 0,
                                          Vst::ParameterInfo::kCanAutomate, Vst::kRootUnitId));
            }
        }
    }

    void sendIntMessage (const char* idTag, const Steinberg::int64 value)
    {
        jassert (hostContext != nullptr);

        if (auto* message = allocateMessage())
        {
            const FReleaser releaser (message);
            message->setMessageID (idTag);
            message->getAttributes()->setInt (idTag, value);
            sendMessage (message);
        }
    }

    //==============================================================================
   #if JUCE_FORCE_USE_LEGACY_PARAM_IDS
    inline Vst::ParamID getVSTParamIDForIndex (int paramIndex) const noexcept   { return static_cast<Vst::ParamID> (paramIndex); }
   #else
    static Vst::ParamID generateVSTParamIDForIndex (AudioProcessor* const pluginFilter, int paramIndex)
    {
        jassert (pluginFilter != nullptr);

        const int n = pluginFilter->getNumParameters();
        const bool managedParameter = (pluginFilter->getParameters().size() == n);

        if (isPositiveAndBelow (paramIndex, n))
        {
            auto juceParamID = pluginFilter->getParameterID (paramIndex);
            auto paramHash = static_cast<Vst::ParamID> (juceParamID.hashCode());

           #if JUCE_USE_STUDIO_ONE_COMPATIBLE_PARAMETERS
            // studio one doesn't like negative parameters
            paramHash &= ~(1 << (sizeof (Vst::ParamID) * 8 - 1));
           #endif

            return managedParameter ? paramHash
                                    : static_cast<Vst::ParamID> (juceParamID.getIntValue());
        }

        return static_cast<Vst::ParamID> (-1);
    }

    inline Vst::ParamID getVSTParamIDForIndex (int paramIndex) const noexcept
    {
        return usingManagedParameter ? vstParamIDs.getReference (paramIndex)
                                     : static_cast<Vst::ParamID> (paramIndex);
    }
   #endif

    //==============================================================================
    class JuceVST3Editor  : public Vst::EditorView,
                            public Steinberg::IPlugViewContentScaleSupport,
                            private Timer
    {
    public:
        JuceVST3Editor (JuceVST3EditController& ec, AudioProcessor& p)
          : Vst::EditorView (&ec, nullptr),
            owner (&ec), pluginInstance (p)
        {
            component = new ContentWrapperComponent (*this, p);
        }

        tresult PLUGIN_API queryInterface (const TUID targetIID, void** obj) override
        {
            TEST_FOR_AND_RETURN_IF_VALID (targetIID, Steinberg::IPlugViewContentScaleSupport)
            return Vst::EditorView::queryInterface (targetIID, obj);
        }

        REFCOUNT_METHODS (Vst::EditorView)

        //==============================================================================
        tresult PLUGIN_API isPlatformTypeSupported (FIDString type) override
        {
            if (type != nullptr && pluginInstance.hasEditor())
            {
               #if JUCE_WINDOWS
                if (strcmp (type, kPlatformTypeHWND) == 0)
               #else
                if (strcmp (type, kPlatformTypeNSView) == 0 || strcmp (type, kPlatformTypeHIView) == 0)
               #endif
                    return kResultTrue;
            }

            return kResultFalse;
        }

        tresult PLUGIN_API attached (void* parent, FIDString type) override
        {
            if (parent == nullptr || isPlatformTypeSupported (type) == kResultFalse)
                return kResultFalse;

            if (component == nullptr)
                component = new ContentWrapperComponent (*this, pluginInstance);

           #if JUCE_WINDOWS
            component->addToDesktop (0, parent);
            component->setOpaque (true);
            component->setVisible (true);
           #else
            isNSView = (strcmp (type, kPlatformTypeNSView) == 0);
            macHostWindow = juce::attachComponentToWindowRefVST (component, parent, isNSView);
           #endif

            component->resizeHostWindow();
            systemWindow = parent;
            attachedToParent();

            // Life's too short to faff around with wave lab
            if (getHostType().isWavelab())
                startTimer (200);

            return kResultTrue;
        }

        tresult PLUGIN_API removed() override
        {
            if (component != nullptr)
            {
               #if JUCE_WINDOWS
                component->removeFromDesktop();
               #else
                if (macHostWindow != nullptr)
                {
                    juce::detachComponentFromWindowRefVST (component, macHostWindow, isNSView);
                    macHostWindow = nullptr;
                }
               #endif

                component = nullptr;
            }

            return CPluginView::removed();
        }

        tresult PLUGIN_API onSize (ViewRect* newSize) override
        {
            if (newSize != nullptr)
            {
                rect = *newSize;

                if (component != nullptr)
                {
                    component->setSize (rect.getWidth(), rect.getHeight());

                    if (auto* peer = component->getPeer())
                        peer->updateBounds();
                }

                return kResultTrue;
            }

            jassertfalse;
            return kResultFalse;
        }

        tresult PLUGIN_API getSize (ViewRect* size) override
        {
            if (size != nullptr && component != nullptr)
            {
                *size = ViewRect (0, 0, component->getWidth(), component->getHeight());
                return kResultTrue;
            }

            return kResultFalse;
        }

        tresult PLUGIN_API canResize() override
        {
            if (component != nullptr)
                if (auto* editor = component->pluginEditor.get())
                    return editor->isResizable() ? kResultTrue : kResultFalse;

            return kResultFalse;
        }

        tresult PLUGIN_API checkSizeConstraint (ViewRect* rectToCheck) override
        {
            if (rectToCheck != nullptr && component != nullptr)
            {
                if (auto* editor = component->pluginEditor.get())
                {
                    // checkSizeConstraint
                    auto juceRect = editor->getLocalArea (component, Rectangle<int>::leftTopRightBottom (rectToCheck->left, rectToCheck->top,
                                                                                                         rectToCheck->right, rectToCheck->bottom));
                    if (auto* constrainer = editor->getConstrainer())
                    {
                        Rectangle<int> limits (0, 0, constrainer->getMaximumWidth(), constrainer->getMaximumHeight());
                        constrainer->checkBounds (juceRect, editor->getBounds(), limits, false, false, false, false);

                        juceRect = component->getLocalArea (editor, juceRect);
                        rectToCheck->right  = rectToCheck->left + juceRect.getWidth();
                        rectToCheck->bottom = rectToCheck->top  + juceRect.getHeight();
                    }
                }

                return kResultTrue;
            }

            jassertfalse;
            return kResultFalse;
        }

        tresult PLUGIN_API setContentScaleFactor (Steinberg::IPlugViewContentScaleSupport::ScaleFactor factor) override
        {
           #if (JUCE_MAC || JUCE_IOS)
            ignoreUnused (factor);
           #else
            if (auto* editor = component->pluginEditor.get())
            {
                editor->setScaleFactor (factor);
                return kResultTrue;
            }
           #endif

            return kResultFalse;
        }

    private:
        void timerCallback() override
        {
            stopTimer();

            ViewRect viewRect;
            getSize (&viewRect);
            onSize (&viewRect);
        }

        //==============================================================================
        struct ContentWrapperComponent  : public Component
        {
            ContentWrapperComponent (JuceVST3Editor& editor, AudioProcessor& plugin)
               : pluginEditor (plugin.createEditorIfNeeded()),
                 owner (editor)
            {
                setOpaque (true);
                setBroughtToFrontOnMouseClick (true);

                // if hasEditor() returns true then createEditorIfNeeded has to return a valid editor
                jassert (pluginEditor != nullptr);

                if (pluginEditor != nullptr)
                {
                    addAndMakeVisible (pluginEditor);

                    pluginEditor->setTopLeftPosition (0, 0);
                    lastBounds = getSizeToContainChild();
                    isResizingParentToFitChild = true;
                    setBounds (lastBounds);
                    isResizingParentToFitChild = false;

                    resizeHostWindow();
                }

                ignoreUnused (fakeMouseGenerator);
            }

            ~ContentWrapperComponent()
            {
                if (pluginEditor != nullptr)
                {
                    PopupMenu::dismissAllActiveMenus();
                    pluginEditor->processor.editorBeingDeleted (pluginEditor);
                }
            }

            void paint (Graphics& g) override
            {
                g.fillAll (Colours::black);
            }

            juce::Rectangle<int> getSizeToContainChild()
            {
                if (pluginEditor != nullptr)
                    return getLocalArea (pluginEditor, pluginEditor->getLocalBounds());

                return {};
            }

            void childBoundsChanged (Component*) override
            {
                if (isResizingChildToFitParent)
                    return;

                auto b = getSizeToContainChild();

                if (lastBounds != b)
                {
                    lastBounds = b;
                    isResizingParentToFitChild = true;
                    resizeHostWindow();
                    isResizingParentToFitChild = false;
                }
            }

            void resized() override
            {
                if (pluginEditor != nullptr)
                {
                    if (! isResizingParentToFitChild)
                    {
                        lastBounds = getLocalBounds();
                        isResizingChildToFitParent = true;
                        pluginEditor->setTopLeftPosition (0, 0);
                        pluginEditor->setBounds (pluginEditor->getLocalArea (this, getLocalBounds()));
                        isResizingChildToFitParent = false;
                    }
                }
            }

            void resizeHostWindow()
            {
                if (pluginEditor != nullptr)
                {
                    auto b = getSizeToContainChild();
                    auto w = b.getWidth();
                    auto h = b.getHeight();
                    auto host = getHostType();

                   #if JUCE_WINDOWS
                    setSize (w, h);
                   #else
                    if (owner.macHostWindow != nullptr && ! (host.isWavelab() || host.isReaper() || host.isBitwigStudio()))
                        juce::setNativeHostWindowSizeVST (owner.macHostWindow, this, w, h, owner.isNSView);
                   #endif

                    if (owner.plugFrame != nullptr)
                    {
                        ViewRect newSize (0, 0, w, h);
                        isResizingParentToFitChild = true;
                        owner.plugFrame->resizeView (&owner, &newSize);
                        isResizingParentToFitChild = false;

                       #if JUCE_MAC
                        if (host.isWavelab() || host.isReaper())
                       #else
                        if (host.isWavelab())
                       #endif
                            setBounds (0, 0, w, h);
                    }
                }
            }

            ScopedPointer<AudioProcessorEditor> pluginEditor;

        private:
            JuceVST3Editor& owner;
            FakeMouseMoveGenerator fakeMouseGenerator;
            Rectangle<int> lastBounds;
            bool isResizingChildToFitParent = false;
            bool isResizingParentToFitChild = false;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ContentWrapperComponent)
        };

        //==============================================================================
        ComSmartPtr<JuceVST3EditController> owner;
        AudioProcessor& pluginInstance;

        ScopedPointer<ContentWrapperComponent> component;
        friend struct ContentWrapperComponent;

       #if JUCE_MAC
        void* macHostWindow = nullptr;
        bool isNSView = false;
       #endif

       #if JUCE_WINDOWS
        WindowsHooks hooks;
       #endif

        ScopedJuceInitialiser_GUI libraryInitialiser;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceVST3Editor)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceVST3EditController)
};

namespace
{
    template <typename FloatType> struct AudioBusPointerHelper {};
    template <> struct AudioBusPointerHelper<float>  { static inline float**  impl (Vst::AudioBusBuffers& data) noexcept { return data.channelBuffers32; } };
    template <> struct AudioBusPointerHelper<double> { static inline double** impl (Vst::AudioBusBuffers& data) noexcept { return data.channelBuffers64; } };

    template <typename FloatType> struct ChooseBufferHelper {};
    template <> struct ChooseBufferHelper<float>  { static inline AudioBuffer<float>&  impl (AudioBuffer<float>& f, AudioBuffer<double>&  ) noexcept { return f; } };
    template <> struct ChooseBufferHelper<double> { static inline AudioBuffer<double>& impl (AudioBuffer<float>&  , AudioBuffer<double>& d) noexcept { return d; } };
}


//==============================================================================
class JuceVST3Component : public Vst::IComponent,
                          public Vst::IAudioProcessor,
                          public Vst::IUnitInfo,
                          public Vst::IConnectionPoint,
                          public AudioPlayHead
{
public:
    JuceVST3Component (Vst::IHostApplication* h)
      : pluginInstance (createPluginFilterOfType (AudioProcessor::wrapperType_VST3)),
        host (h)
    {
       #ifdef JucePlugin_PreferredChannelConfigurations
        short configs[][2] = { JucePlugin_PreferredChannelConfigurations };
        const int numConfigs = sizeof (configs) / sizeof (short[2]);

        jassert (numConfigs > 0 && (configs[0][0] > 0 || configs[0][1] > 0));

        pluginInstance->setPlayConfigDetails (configs[0][0], configs[0][1], 44100.0, 1024);
       #endif

        // VST-3 requires your default layout to be non-discrete!
        // For example, your default layout must be mono, stereo, quadrophonic
        // and not AudioChannelSet::discreteChannels (2) etc.
        jassert (checkBusFormatsAreNotDiscrete());

        comPluginInstance = new JuceAudioProcessor (pluginInstance);

        zerostruct (processContext);

        processSetup.maxSamplesPerBlock = 1024;
        processSetup.processMode = Vst::kRealtime;
        processSetup.sampleRate = 44100.0;
        processSetup.symbolicSampleSize = Vst::kSample32;

       #if JUCE_FORCE_USE_LEGACY_PARAM_IDS
        vstBypassParameterId = static_cast<Vst::ParamID> (pluginInstance->getNumParameters());
       #else
        cacheParameterIDs();
       #endif

        pluginInstance->setPlayHead (this);
    }

    ~JuceVST3Component()
    {
        if (pluginInstance != nullptr)
            if (pluginInstance->getPlayHead() == this)
                pluginInstance->setPlayHead (nullptr);
    }

    //==============================================================================
    AudioProcessor& getPluginInstance() const noexcept { return *pluginInstance; }

    //==============================================================================
    static const FUID iid;

    JUCE_DECLARE_VST3_COM_REF_METHODS

    tresult PLUGIN_API queryInterface (const TUID targetIID, void** obj) override
    {
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, IPluginBase)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, JuceVST3Component)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::IComponent)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::IAudioProcessor)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::IUnitInfo)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::IConnectionPoint)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, Vst::ChannelContext::IInfoListener)
        TEST_FOR_COMMON_BASE_AND_RETURN_IF_VALID (targetIID, FUnknown, Vst::IComponent)

        if (doUIDsMatch (targetIID, JuceAudioProcessor::iid))
        {
            comPluginInstance->addRef();
            *obj = comPluginInstance;
            return kResultOk;
        }

        *obj = nullptr;
        return kNoInterface;
    }

    //==============================================================================
    tresult PLUGIN_API initialize (FUnknown* hostContext) override
    {
        if (host != hostContext)
            host.loadFrom (hostContext);

        processContext.sampleRate = processSetup.sampleRate;
        preparePlugin (processSetup.sampleRate, (int) processSetup.maxSamplesPerBlock);

        return kResultTrue;
    }

    tresult PLUGIN_API terminate() override
    {
        getPluginInstance().releaseResources();
        return kResultTrue;
    }

    //==============================================================================
    tresult PLUGIN_API connect (IConnectionPoint* other) override
    {
        if (other != nullptr && juceVST3EditController == nullptr)
            juceVST3EditController.loadFrom (other);

        return kResultTrue;
    }

    tresult PLUGIN_API disconnect (IConnectionPoint*) override
    {
        juceVST3EditController = nullptr;
        return kResultTrue;
    }

    tresult PLUGIN_API notify (Vst::IMessage* message) override
    {
        if (message != nullptr && juceVST3EditController == nullptr)
        {
            Steinberg::int64 value = 0;

            if (message->getAttributes()->getInt ("JuceVST3EditController", value) == kResultTrue)
            {
                juceVST3EditController = (JuceVST3EditController*) (pointer_sized_int) value;

                if (juceVST3EditController != nullptr)
                    juceVST3EditController->setAudioProcessor (comPluginInstance);
                else
                    jassertfalse;
            }
        }

        return kResultTrue;
    }

    tresult PLUGIN_API getControllerClassId (TUID classID) override
    {
        memcpy (classID, JuceVST3EditController::iid, sizeof (TUID));
        return kResultTrue;
    }

    //==============================================================================
    tresult PLUGIN_API setActive (TBool state) override
    {
        if (! state)
        {
            getPluginInstance().releaseResources();

            deallocateChannelListAndBuffers (channelListFloat,  emptyBufferFloat);
            deallocateChannelListAndBuffers (channelListDouble, emptyBufferDouble);
        }
        else
        {
            auto sampleRate = getPluginInstance().getSampleRate();
            auto bufferSize = getPluginInstance().getBlockSize();

            sampleRate = processSetup.sampleRate > 0.0
                            ? processSetup.sampleRate
                            : sampleRate;

            bufferSize = processSetup.maxSamplesPerBlock > 0
                            ? (int) processSetup.maxSamplesPerBlock
                            : bufferSize;

            allocateChannelListAndBuffers (channelListFloat,  emptyBufferFloat);
            allocateChannelListAndBuffers (channelListDouble, emptyBufferDouble);

            preparePlugin (sampleRate, bufferSize);
        }

        return kResultOk;
    }

    tresult PLUGIN_API setIoMode (Vst::IoMode) override                                 { return kNotImplemented; }
    tresult PLUGIN_API getRoutingInfo (Vst::RoutingInfo&, Vst::RoutingInfo&) override   { return kNotImplemented; }

    bool isBypassed()                   { return comPluginInstance->isBypassed; }
    void setBypassed (bool bypassed)    { comPluginInstance->isBypassed = bypassed; }

    //==============================================================================
    void writeJucePrivateStateInformation (MemoryOutputStream& out)
    {
        ValueTree privateData (kJucePrivateDataIdentifier);

        // for now we only store the bypass value
        privateData.setProperty ("Bypass", var (isBypassed()), nullptr);

        privateData.writeToStream (out);
    }

    void setJucePrivateStateInformation (const void* data, int sizeInBytes)
    {
        auto privateData = ValueTree::readFromData (data, static_cast<size_t> (sizeInBytes));
        setBypassed (static_cast<bool> (privateData.getProperty ("Bypass", var (false))));
    }

    void getStateInformation (MemoryBlock& destData)
    {
        pluginInstance->getStateInformation (destData);

        // With bypass support, JUCE now needs to store private state data.
        // Put this at the end of the plug-in state and add a few null characters
        // so that plug-ins built with older versions of JUCE will hopefully ignore
        // this data. Additionally, we need to add some sort of magic identifier
        // at the very end of the private data so that JUCE has some sort of
        // way to figure out if the data was stored with a newer JUCE version.
        MemoryOutputStream extraData;

        extraData.writeInt64 (0);
        writeJucePrivateStateInformation (extraData);
        auto privateDataSize = (int64) (extraData.getDataSize() - sizeof (int64));
        extraData.writeInt64 (privateDataSize);
        extraData << kJucePrivateDataIdentifier;

        // write magic string
        destData.append (extraData.getData(), extraData.getDataSize());
    }

    void setStateInformation (const void* data, int sizeAsInt)
    {
        int64 size = sizeAsInt;

        // Check if this data was written with a newer JUCE version
        // and if it has the JUCE private data magic code at the end
        auto jucePrivDataIdentifierSize = std::strlen (kJucePrivateDataIdentifier);

        if ((size_t) size >= jucePrivDataIdentifierSize + sizeof (int64))
        {
            auto buffer = static_cast<const char*> (data);

            String magic (CharPointer_UTF8 (buffer + size - jucePrivDataIdentifierSize),
                          CharPointer_UTF8 (buffer + size));

            if (magic == kJucePrivateDataIdentifier)
            {
                // found a JUCE private data section
                uint64 privateDataSize;

                std::memcpy (&privateDataSize,
                             buffer + ((size_t) size - jucePrivDataIdentifierSize - sizeof (uint64)),
                             sizeof (uint64));

                privateDataSize = ByteOrder::swapIfBigEndian (privateDataSize);
                size -= privateDataSize + jucePrivDataIdentifierSize + sizeof (uint64);

                if (privateDataSize > 0)
                    setJucePrivateStateInformation (buffer + size, static_cast<int> (privateDataSize));

                size -= sizeof (uint64);
            }
        }

        if (size >= 0)
            pluginInstance->setStateInformation (data, static_cast<int> (size));
    }

    //==============================================================================
   #if JUCE_VST3_CAN_REPLACE_VST2
    void loadVST2VstWBlock (const char* data, int size)
    {
        auto headerLen = static_cast<int> (htonl (*(juce::int32*) (data + 4)));
        auto bank = (const vst2FxBank*) (data + (8 + headerLen));
        auto version = static_cast<int> (htonl (bank->version1)); ignoreUnused (version);

        jassert ('VstW' == htonl (*(juce::int32*) data));
        jassert (1 == htonl (*(juce::int32*) (data + 8))); // version should be 1 according to Steinberg's docs
        jassert ('CcnK' == htonl (bank->magic1));
        jassert ('FBCh' == htonl (bank->magic2));
        jassert (version == 1 || version == 2);
        jassert (JucePlugin_VSTUniqueID == htonl (bank->fxID));

        setStateInformation (bank->chunk,
                             jmin ((int) (size - (bank->chunk - data)),
                                   (int) htonl (bank->chunkSize)));
    }

    bool loadVST3PresetFile (const char* data, int size)
    {
        if (size < 48)
            return false;

        // At offset 4 there's a little-endian version number which seems to typically be 1
        // At offset 8 there's 32 bytes the SDK calls "ASCII-encoded class id"
        auto chunkListOffset = (int) ByteOrder::littleEndianInt (data + 40);
        jassert (memcmp (data + chunkListOffset, "List", 4) == 0);
        auto entryCount = (int) ByteOrder::littleEndianInt (data + chunkListOffset + 4);
        jassert (entryCount > 0);

        for (int i = 0; i < entryCount; ++i)
        {
            auto entryOffset = chunkListOffset + 8 + 20 * i;

            if (entryOffset + 20 > size)
                return false;

            if (memcmp (data + entryOffset, "Comp", 4) == 0)
            {
                // "Comp" entries seem to contain the data.
                auto chunkOffset = ByteOrder::littleEndianInt64 (data + entryOffset + 4);
                auto chunkSize   = ByteOrder::littleEndianInt64 (data + entryOffset + 12);

                if (chunkOffset + chunkSize > static_cast<juce::uint64> (size))
                {
                    jassertfalse;
                    return false;
                }

                loadVST2VstWBlock (data + chunkOffset, (int) chunkSize);
            }
        }

        return true;
    }

    bool loadVST2CompatibleState (const char* data, int size)
    {
        if (size < 4)
            return false;

        if (htonl (*(juce::int32*) data) == 'VstW')
        {
            loadVST2VstWBlock (data, size);
            return true;
        }

        if (memcmp (data, "VST3", 4) == 0)
        {
            // In Cubase 5, when loading VST3 .vstpreset files,
            // we get the whole content of the files to load.
            // In Cubase 7 we get just the contents within and
            // we go directly to the loadVST2VstW codepath instead.
            return loadVST3PresetFile (data, size);
        }

        return false;
    }
   #endif

    bool loadStateData (const void* data, int size)
    {
       #if JUCE_VST3_CAN_REPLACE_VST2
        return loadVST2CompatibleState ((const char*) data, size);
       #else
        setStateInformation (data, size);
        return true;
       #endif
    }

    bool readFromMemoryStream (IBStream* state)
    {
        FUnknownPtr<ISizeableStream> s (state);
        Steinberg::int64 size = 0;

        if (s != nullptr
             && s->getStreamSize (size) == kResultOk
             && size > 0
             && size < 1024 * 1024 * 100) // (some hosts seem to return junk for the size)
        {
            MemoryBlock block (static_cast<size_t> (size));

            // turns out that Cubase 9 might give you the incorrect stream size :-(
            Steinberg::int32 bytesRead = 1;
            int len;

            for (len = 0; bytesRead > 0 && len < static_cast<int> (block.getSize()); len += bytesRead)
                if (state->read (block.getData(), static_cast<int32> (block.getSize()), &bytesRead) != kResultOk)
                    break;

            if (len == 0)
                return false;

            block.setSize (static_cast<size_t> (len));

            // Adobe Audition CS6 hack to avoid trying to use corrupted streams:
            if (getHostType().isAdobeAudition())
                if (block.getSize() >= 5 && memcmp (block.getData(), "VC2!E", 5) == 0)
                    return false;

            return loadStateData (block.getData(), (int) block.getSize());
        }

        return false;
    }

    bool readFromUnknownStream (IBStream* state)
    {
        MemoryOutputStream allData;

        {
            const size_t bytesPerBlock = 4096;
            HeapBlock<char> buffer (bytesPerBlock);

            for (;;)
            {
                Steinberg::int32 bytesRead = 0;
                auto status = state->read (buffer, (Steinberg::int32) bytesPerBlock, &bytesRead);

                if (bytesRead <= 0 || (status != kResultTrue && ! getHostType().isWavelab()))
                    break;

                allData.write (buffer, static_cast<size_t> (bytesRead));
            }
        }

        const size_t dataSize = allData.getDataSize();

        return dataSize > 0 && dataSize < 0x7fffffff
                && loadStateData (allData.getData(), (int) dataSize);
    }

    tresult PLUGIN_API setState (IBStream* state) override
    {
        if (state == nullptr)
            return kInvalidArgument;

        FUnknownPtr<IBStream> stateRefHolder (state); // just in case the caller hasn't properly ref-counted the stream object

        if (state->seek (0, IBStream::kIBSeekSet, nullptr) == kResultTrue)
        {
            if (! getHostType().isFruityLoops() && readFromMemoryStream (state))
                return kResultTrue;

            if (readFromUnknownStream (state))
                return kResultTrue;
        }

        return kResultFalse;
    }

   #if JUCE_VST3_CAN_REPLACE_VST2
    static tresult writeVST2Int (IBStream* state, int n)
    {
        juce::int32 t = (juce::int32) htonl (n);
        return state->write (&t, 4);
    }

    static tresult writeVST2Header (IBStream* state, bool bypassed)
    {
        tresult status = writeVST2Int (state, 'VstW');

        if (status == kResultOk) status = writeVST2Int (state, 8); // header size
        if (status == kResultOk) status = writeVST2Int (state, 1); // version
        if (status == kResultOk) status = writeVST2Int (state, bypassed ? 1 : 0); // bypass

        return status;
    }
   #endif

    tresult PLUGIN_API getState (IBStream* state) override
    {
       if (state == nullptr)
           return kInvalidArgument;

        juce::MemoryBlock mem;
        getStateInformation (mem);

      #if JUCE_VST3_CAN_REPLACE_VST2
        tresult status = writeVST2Header (state, isBypassed());

        if (status != kResultOk)
            return status;

        const int bankBlockSize = 160;
        vst2FxBank bank;

        zerostruct (bank);
        bank.magic1         = (int32) htonl ('CcnK');
        bank.size           = (int32) htonl (bankBlockSize - 8 + (unsigned int) mem.getSize());
        bank.magic2         = (int32) htonl ('FBCh');
        bank.version1       = (int32) htonl (2);
        bank.fxID           = (int32) htonl (JucePlugin_VSTUniqueID);
        bank.version2       = (int32) htonl (JucePlugin_VersionCode);
        bank.chunkSize      = (int32) htonl ((unsigned int) mem.getSize());

        status = state->write (&bank, bankBlockSize);

        if (status != kResultOk)
            return status;
       #endif

        return state->write (mem.getData(), (Steinberg::int32) mem.getSize());
    }

    //==============================================================================
    Steinberg::int32 PLUGIN_API getUnitCount() override
    {
        return 1;
    }

    tresult PLUGIN_API getUnitInfo (Steinberg::int32 unitIndex, Vst::UnitInfo& info) override
    {
        if (unitIndex == 0)
        {
            info.id             = Vst::kRootUnitId;
            info.parentUnitId   = Vst::kNoParentUnitId;
            info.programListId  = Vst::kNoProgramListId;

            toString128 (info.name, TRANS("Root Unit"));

            return kResultTrue;
        }

        zerostruct (info);
        return kResultFalse;
    }

    Steinberg::int32 PLUGIN_API getProgramListCount() override
    {
        if (getPluginInstance().getNumPrograms() > 0)
            return 1;

        return 0;
    }

    tresult PLUGIN_API getProgramListInfo (Steinberg::int32 listIndex, Vst::ProgramListInfo& info) override
    {
        if (listIndex == 0)
        {
            info.id = JuceVST3EditController::paramPreset;
            info.programCount = (Steinberg::int32) getPluginInstance().getNumPrograms();

            toString128 (info.name, TRANS("Factory Presets"));

            return kResultTrue;
        }

        jassertfalse;
        zerostruct (info);
        return kResultFalse;
    }

    tresult PLUGIN_API getProgramName (Vst::ProgramListID listId, Steinberg::int32 programIndex, Vst::String128 name) override
    {
        if (listId == JuceVST3EditController::paramPreset
            && isPositiveAndBelow ((int) programIndex, getPluginInstance().getNumPrograms()))
        {
            toString128 (name, getPluginInstance().getProgramName ((int) programIndex));
            return kResultTrue;
        }

        jassertfalse;
        toString128 (name, juce::String());
        return kResultFalse;
    }

    tresult PLUGIN_API getProgramInfo (Vst::ProgramListID, Steinberg::int32, Vst::CString, Vst::String128) override             { return kNotImplemented; }
    tresult PLUGIN_API hasProgramPitchNames (Vst::ProgramListID, Steinberg::int32) override                                     { return kNotImplemented; }
    tresult PLUGIN_API getProgramPitchName (Vst::ProgramListID, Steinberg::int32, Steinberg::int16, Vst::String128) override    { return kNotImplemented; }
    tresult PLUGIN_API selectUnit (Vst::UnitID) override                                                                        { return kNotImplemented; }
    tresult PLUGIN_API setUnitProgramData (Steinberg::int32, Steinberg::int32, IBStream*) override                              { return kNotImplemented; }
    Vst::UnitID PLUGIN_API getSelectedUnit() override                                                                           { return Vst::kRootUnitId; }

    tresult PLUGIN_API getUnitByBus (Vst::MediaType, Vst::BusDirection, Steinberg::int32, Steinberg::int32, Vst::UnitID& unitId) override
    {
        zerostruct (unitId);
        return kNotImplemented;
    }

    //==============================================================================
    bool getCurrentPosition (CurrentPositionInfo& info) override
    {
        info.timeInSamples              = jmax ((juce::int64) 0, processContext.projectTimeSamples);
        info.timeInSeconds              = static_cast<double> (info.timeInSamples) / processContext.sampleRate;
        info.bpm                        = jmax (1.0, processContext.tempo);
        info.timeSigNumerator           = jmax (1, (int) processContext.timeSigNumerator);
        info.timeSigDenominator         = jmax (1, (int) processContext.timeSigDenominator);
        info.ppqPositionOfLastBarStart  = processContext.barPositionMusic;
        info.ppqPosition                = processContext.projectTimeMusic;
        info.ppqLoopStart               = processContext.cycleStartMusic;
        info.ppqLoopEnd                 = processContext.cycleEndMusic;
        info.isRecording                = (processContext.state & Vst::ProcessContext::kRecording) != 0;
        info.isPlaying                  = (processContext.state & Vst::ProcessContext::kPlaying) != 0;
        info.isLooping                  = (processContext.state & Vst::ProcessContext::kCycleActive) != 0;
        info.editOriginTime             = 0.0;
        info.frameRate                  = AudioPlayHead::fpsUnknown;

        if ((processContext.state & Vst::ProcessContext::kSmpteValid) != 0)
        {
            switch (processContext.frameRate.framesPerSecond)
            {
                case 24:
                {
                    if ((processContext.frameRate.flags & Vst::FrameRate::kPullDownRate) != 0)
                        info.frameRate = AudioPlayHead::fps23976;
                    else
                        info.frameRate = AudioPlayHead::fps24;
                }
                break;
                case 25: info.frameRate = AudioPlayHead::fps25; break;
                case 29: info.frameRate = AudioPlayHead::fps30drop; break;

                case 30:
                {
                    if ((processContext.frameRate.flags & Vst::FrameRate::kDropRate) != 0)
                        info.frameRate = AudioPlayHead::fps30drop;
                    else
                        info.frameRate = AudioPlayHead::fps30;
                }
                break;

                default: break;
            }
        }

        return true;
    }

    //==============================================================================
    int getNumAudioBuses (bool isInput) const
    {
        int busCount = pluginInstance->getBusCount (isInput);

      #ifdef JucePlugin_PreferredChannelConfigurations
        short configs[][2] = {JucePlugin_PreferredChannelConfigurations};
        const int numConfigs = sizeof (configs) / sizeof (short[2]);

        bool hasOnlyZeroChannels = true;

        for (int i = 0; i < numConfigs && hasOnlyZeroChannels == true; ++i)
            if (configs[i][isInput ? 0 : 1] != 0)
                hasOnlyZeroChannels = false;

        busCount = jmin (busCount, hasOnlyZeroChannels ? 0 : 1);
       #endif

        return busCount;
    }

    //==============================================================================
    Steinberg::int32 PLUGIN_API getBusCount (Vst::MediaType type, Vst::BusDirection dir) override
    {
        if (type == Vst::kAudio)
            return getNumAudioBuses (dir == Vst::kInput);

        if (type == Vst::kEvent)
        {
            if (dir == Vst::kInput)
                return isMidiInputBusEnabled ? 1 : 0;

            if (dir == Vst::kOutput)
                return isMidiOutputBusEnabled ? 1 : 0;
        }

        return 0;
    }

    tresult PLUGIN_API getBusInfo (Vst::MediaType type, Vst::BusDirection dir,
                                   Steinberg::int32 index, Vst::BusInfo& info) override
    {
        if (type == Vst::kAudio)
        {
            if (index < 0 || index >= getNumAudioBuses (dir == Vst::kInput))
                return kResultFalse;

            if (auto* bus = pluginInstance->getBus (dir == Vst::kInput, index))
            {
                info.mediaType = Vst::kAudio;
                info.direction = dir;
                info.channelCount = bus->getLastEnabledLayout().size();
                toString128 (info.name, bus->getName());

               #if JucePlugin_IsSynth
                info.busType = (dir == Vst::kInput && index > 0 ? Vst::kAux : Vst::kMain);
               #else
                info.busType = (index == 0 ? Vst::kMain : Vst::kAux);
               #endif

                info.flags = (bus->isEnabledByDefault()) ? Vst::BusInfo::kDefaultActive : 0;
                return kResultTrue;
            }
        }

        if (type == Vst::kEvent)
        {
            info.flags = Vst::BusInfo::kDefaultActive;

           #if JucePlugin_WantsMidiInput
            if (dir == Vst::kInput && index == 0)
            {
                info.mediaType = Vst::kEvent;
                info.direction = dir;
                info.channelCount = 16;
                toString128 (info.name, TRANS("MIDI Input"));
                info.busType = Vst::kMain;
                return kResultTrue;
            }
           #endif

           #if JucePlugin_ProducesMidiOutput
            if (dir == Vst::kOutput && index == 0)
            {
                info.mediaType = Vst::kEvent;
                info.direction = dir;
                info.channelCount = 16;
                toString128 (info.name, TRANS("MIDI Output"));
                info.busType = Vst::kMain;
                return kResultTrue;
            }
           #endif
        }

        zerostruct (info);
        return kResultFalse;
    }

    tresult PLUGIN_API activateBus (Vst::MediaType type, Vst::BusDirection dir, Steinberg::int32 index, TBool state) override
    {
        if (type == Vst::kEvent)
        {
            if (index != 0)
                return kResultFalse;

            if (dir == Vst::kInput)
                isMidiInputBusEnabled = (state != 0);
            else
                isMidiOutputBusEnabled = (state != 0);

            return kResultTrue;
        }

        if (type == Vst::kAudio)
        {
            if (index < 0 || index >= getNumAudioBuses (dir == Vst::kInput))
                return kResultFalse;

            if (auto* bus = pluginInstance->getBus (dir == Vst::kInput, index))
            {
               #ifdef JucePlugin_PreferredChannelConfigurations
                auto newLayout = pluginInstance->getBusesLayout();
                auto targetLayout = (state != 0 ? bus->getLastEnabledLayout() : AudioChannelSet::disabled());

                (dir == Vst::kInput ? newLayout.inputBuses : newLayout.outputBuses).getReference (index) = targetLayout;

                short configs[][2] = { JucePlugin_PreferredChannelConfigurations };
                auto compLayout = pluginInstance->getNextBestLayoutInLayoutList (newLayout, configs);

                if ((dir == Vst::kInput ? compLayout.inputBuses : compLayout.outputBuses).getReference (index) != targetLayout)
                    return kResultFalse;
               #endif

                return bus->enable (state != 0) ? kResultTrue : kResultFalse;
            }
        }

        return kResultFalse;
    }

    bool checkBusFormatsAreNotDiscrete()
    {
        auto numInputBuses  = pluginInstance->getBusCount (true);
        auto numOutputBuses = pluginInstance->getBusCount (false);

        for (int i = 0; i < numInputBuses; ++i)
        {
            auto layout = pluginInstance->getChannelLayoutOfBus (true,  i);

            if (layout.isDiscreteLayout() && ! layout.isDisabled())
                return false;
        }

        for (int i = 0; i < numOutputBuses; ++i)
        {
            auto layout = pluginInstance->getChannelLayoutOfBus (false,  i);

            if (layout.isDiscreteLayout() && ! layout.isDisabled())
                return false;
        }

        return true;
    }

    tresult PLUGIN_API setBusArrangements (Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns,
                                           Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts) override
    {
        auto numInputBuses  = pluginInstance->getBusCount (true);
        auto numOutputBuses = pluginInstance->getBusCount (false);

        if (numIns > numInputBuses || numOuts > numOutputBuses)
            return false;

        auto requested = pluginInstance->getBusesLayout();

        for (int i = 0; i < numIns; ++i)
            requested.getChannelSet (true,  i) = getChannelSetForSpeakerArrangement (inputs[i]);

        for (int i = 0; i < numOuts; ++i)
            requested.getChannelSet (false, i) = getChannelSetForSpeakerArrangement (outputs[i]);

       #ifdef JucePlugin_PreferredChannelConfigurations
        short configs[][2] = { JucePlugin_PreferredChannelConfigurations };
        if (! AudioProcessor::containsLayout (requested, configs))
            return kResultFalse;
       #endif

        return pluginInstance->setBusesLayoutWithoutEnabling (requested) ? kResultTrue : kResultFalse;
    }

    tresult PLUGIN_API getBusArrangement (Vst::BusDirection dir, Steinberg::int32 index, Vst::SpeakerArrangement& arr) override
    {
        if (auto* bus = pluginInstance->getBus (dir == Vst::kInput, index))
        {
            arr = getVst3SpeakerArrangement (bus->getLastEnabledLayout());
            return kResultTrue;
        }

        return kResultFalse;
    }

    //==============================================================================
    tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) override
    {
        return (symbolicSampleSize == Vst::kSample32
                 || (getPluginInstance().supportsDoublePrecisionProcessing()
                       && symbolicSampleSize == Vst::kSample64)) ? kResultTrue : kResultFalse;
    }

    Steinberg::uint32 PLUGIN_API getLatencySamples() override
    {
        return (Steinberg::uint32) jmax (0, getPluginInstance().getLatencySamples());
    }

    tresult PLUGIN_API setupProcessing (Vst::ProcessSetup& newSetup) override
    {
        if (canProcessSampleSize (newSetup.symbolicSampleSize) != kResultTrue)
            return kResultFalse;

        processSetup = newSetup;
        processContext.sampleRate = processSetup.sampleRate;

        getPluginInstance().setProcessingPrecision (newSetup.symbolicSampleSize == Vst::kSample64
                                                        ? AudioProcessor::doublePrecision
                                                        : AudioProcessor::singlePrecision);

        preparePlugin (processSetup.sampleRate, processSetup.maxSamplesPerBlock);

        return kResultTrue;
    }

    tresult PLUGIN_API setProcessing (TBool state) override
    {
        if (! state)
            getPluginInstance().reset();

        return kResultTrue;
    }

    Steinberg::uint32 PLUGIN_API getTailSamples() override
    {
        auto tailLengthSeconds = getPluginInstance().getTailLengthSeconds();

        if (tailLengthSeconds <= 0.0 || processSetup.sampleRate > 0.0)
            return Vst::kNoTail;

        return (Steinberg::uint32) roundToIntAccurate (tailLengthSeconds * processSetup.sampleRate);
    }

    //==============================================================================
    void processParameterChanges (Vst::IParameterChanges& paramChanges)
    {
        jassert (pluginInstance != nullptr);

        auto numParamsChanged = paramChanges.getParameterCount();

        for (Steinberg::int32 i = 0; i < numParamsChanged; ++i)
        {
            if (auto* paramQueue = paramChanges.getParameterData (i))
            {
                auto numPoints = paramQueue->getPointCount();

                Steinberg::int32 offsetSamples = 0;
                double value = 0.0;

                if (paramQueue->getPoint (numPoints - 1, offsetSamples, value) == kResultTrue)
                {
                    auto vstParamID = paramQueue->getParameterId();

                    if (vstParamID == vstBypassParameterId)
                        setBypassed (static_cast<float> (value) != 0.0f);
                   #if JUCE_VST3_EMULATE_MIDI_CC_WITH_PARAMETERS
                    else if (juceVST3EditController->isMidiControllerParamID (vstParamID))
                        addParameterChangeToMidiBuffer (offsetSamples, vstParamID, value);
                   #endif
                    else if (vstParamID == JuceVST3EditController::paramPreset)
                    {
                        auto numPrograms  = pluginInstance->getNumPrograms();
                        auto programValue = roundToInt (value * (jmax (0, numPrograms - 1)));

                        if (numPrograms > 1 && isPositiveAndBelow (programValue, numPrograms)
                             && programValue != pluginInstance->getCurrentProgram())
                            pluginInstance->setCurrentProgram (programValue);
                    }
                    else
                    {
                        auto index = getJuceIndexForVSTParamID (vstParamID);

                        if (isPositiveAndBelow (index, pluginInstance->getNumParameters()))
                            pluginInstance->setParameter (index, static_cast<float> (value));
                    }
                }
            }
        }
    }

    void addParameterChangeToMidiBuffer (const Steinberg::int32 offsetSamples, const Vst::ParamID id, const double value)
    {
        // If the parameter is mapped to a MIDI CC message then insert it into the midiBuffer.
        int channel, ctrlNumber;

        if (juceVST3EditController->getMidiControllerForParameter (id, channel, ctrlNumber))
        {
            if (ctrlNumber == Vst::kAfterTouch)
                midiBuffer.addEvent (MidiMessage::channelPressureChange (channel,
                                                                         jlimit (0, 127, (int) (value * 128.0))), offsetSamples);
            else if (ctrlNumber == Vst::kPitchBend)
                midiBuffer.addEvent (MidiMessage::pitchWheel (channel,
                                                              jlimit (0, 0x3fff, (int) (value * 0x4000))), offsetSamples);
            else
                midiBuffer.addEvent (MidiMessage::controllerEvent (channel,
                                                                   jlimit (0, 127, ctrlNumber),
                                                                   jlimit (0, 127, (int) (value * 128.0))), offsetSamples);
        }
    }

    tresult PLUGIN_API process (Vst::ProcessData& data) override
    {
        if (pluginInstance == nullptr)
            return kResultFalse;

        if ((processSetup.symbolicSampleSize == Vst::kSample64) != pluginInstance->isUsingDoublePrecision())
            return kResultFalse;

        if (data.processContext != nullptr)
            processContext = *data.processContext;
        else
            zerostruct (processContext);

        midiBuffer.clear();

       #if JucePlugin_WantsMidiInput
        if (data.inputEvents != nullptr)
            MidiEventList::toMidiBuffer (midiBuffer, *data.inputEvents);
       #endif

        if (getHostType().isWavelab())
        {
            const int numInputChans  = (data.inputs  != nullptr && data.inputs[0].channelBuffers32 != nullptr)  ? (int) data.inputs[0].numChannels  : 0;
            const int numOutputChans = (data.outputs != nullptr && data.outputs[0].channelBuffers32 != nullptr) ? (int) data.outputs[0].numChannels : 0;

            if ((pluginInstance->getTotalNumInputChannels() + pluginInstance->getTotalNumOutputChannels()) > 0
                 && (numInputChans + numOutputChans) == 0)
                return kResultFalse;
        }

        if      (processSetup.symbolicSampleSize == Vst::kSample32) processAudio<float>  (data, channelListFloat);
        else if (processSetup.symbolicSampleSize == Vst::kSample64) processAudio<double> (data, channelListDouble);
        else jassertfalse;

       #if JucePlugin_ProducesMidiOutput
        if (data.outputEvents != nullptr)
            MidiEventList::toEventList (*data.outputEvents, midiBuffer);
       #endif

        return kResultTrue;
    }

private:
    //==============================================================================
    Atomic<int> refCount { 1 };

    AudioProcessor* pluginInstance;
    ComSmartPtr<Vst::IHostApplication> host;
    ComSmartPtr<JuceAudioProcessor> comPluginInstance;
    ComSmartPtr<JuceVST3EditController> juceVST3EditController;

    /**
        Since VST3 does not provide a way of knowing the buffer size and sample rate at any point,
        this object needs to be copied on every call to process() to be up-to-date...
    */
    Vst::ProcessContext processContext;
    Vst::ProcessSetup processSetup;

    MidiBuffer midiBuffer;
    Array<float*> channelListFloat;
    Array<double*> channelListDouble;

    AudioBuffer<float>  emptyBufferFloat;
    AudioBuffer<double> emptyBufferDouble;

   #if JucePlugin_WantsMidiInput
    bool isMidiInputBusEnabled = true;
   #else
    bool isMidiInputBusEnabled = false;
   #endif

   #if JucePlugin_ProducesMidiOutput
    bool isMidiOutputBusEnabled = true;
   #else
    bool isMidiOutputBusEnabled = false;
   #endif

    ScopedJuceInitialiser_GUI libraryInitialiser;

   #if ! JUCE_FORCE_USE_LEGACY_PARAM_IDS
    bool usingManagedParameter;
    Array<Vst::ParamID> vstParamIDs;
    HashMap<int32, int> paramMap;
   #endif
    Vst::ParamID vstBypassParameterId;

    static const char* kJucePrivateDataIdentifier;

    //==============================================================================
    template <typename FloatType>
    void processAudio (Vst::ProcessData& data, Array<FloatType*>& channelList)
    {
        int totalInputChans = 0, totalOutputChans = 0;
        bool tmpBufferNeedsClearing = false;

        auto plugInInputChannels  = pluginInstance->getTotalNumInputChannels();
        auto plugInOutputChannels = pluginInstance->getTotalNumOutputChannels();

        // Wavelab workaround: wave-lab lies on the number of inputs/outputs so re-count here
        int vstInputs;
        for (vstInputs = 0; vstInputs < data.numInputs; ++vstInputs)
            if (getPointerForAudioBus<FloatType> (data.inputs[vstInputs]) == nullptr
                  && data.inputs[vstInputs].numChannels > 0)
                break;

        int vstOutputs;
        for (vstOutputs = 0; vstOutputs < data.numOutputs; ++vstOutputs)
            if (getPointerForAudioBus<FloatType> (data.outputs[vstOutputs]) == nullptr
                  && data.outputs[vstOutputs].numChannels > 0)
                break;

        {
            auto n = jmax (vstOutputs, getNumAudioBuses (false));

            for (int bus = 0; bus < n && totalOutputChans < plugInOutputChannels; ++bus)
            {
                if (bus < vstOutputs)
                {
                    if (auto** const busChannels = getPointerForAudioBus<FloatType> (data.outputs[bus]))
                    {
                        auto numChans = jmin ((int) data.outputs[bus].numChannels, plugInOutputChannels - totalOutputChans);

                        for (int i = 0; i < numChans; ++i)
                        {
                            if (auto dst = busChannels[i])
                            {
                                if (totalOutputChans >= plugInInputChannels)
                                    FloatVectorOperations::clear (dst, (int) data.numSamples);

                                channelList.set (totalOutputChans++, busChannels[i]);
                            }
                        }
                    }
                }
                else
                {
                    const int numChans = jmin (pluginInstance->getChannelCountOfBus (false, bus), plugInOutputChannels - totalOutputChans);

                    for (int i = 0; i < numChans; ++i)
                    {
                        if (auto* tmpBuffer = getTmpBufferForChannel<FloatType> (totalOutputChans, data.numSamples))\
                        {
                            tmpBufferNeedsClearing = true;
                            channelList.set (totalOutputChans++, tmpBuffer);
                        }
                        else
                            return;
                    }
                }
            }
        }

        {
            auto n = jmax (vstInputs, getNumAudioBuses (true));

            for (int bus = 0; bus < n && totalInputChans < plugInInputChannels; ++bus)
            {
                if (bus < vstInputs)
                {
                    if (auto** const busChannels = getPointerForAudioBus<FloatType> (data.inputs[bus]))
                    {
                        const int numChans = jmin ((int) data.inputs[bus].numChannels, plugInInputChannels - totalInputChans);

                        for (int i = 0; i < numChans; ++i)
                        {
                            if (busChannels[i] != nullptr)
                            {
                                if (totalInputChans >= totalOutputChans)
                                    channelList.set (totalInputChans, busChannels[i]);
                                else
                                {
                                    auto* dst = channelList.getReference (totalInputChans);
                                    auto* src = busChannels[i];

                                    if (dst != src)
                                        FloatVectorOperations::copy (dst, src, (int) data.numSamples);
                                }
                            }

                            ++totalInputChans;
                        }
                    }
                }
                else
                {
                    auto numChans = jmin (pluginInstance->getChannelCountOfBus (true, bus), plugInInputChannels - totalInputChans);

                    for (int i = 0; i < numChans; ++i)
                    {
                        if (auto* tmpBuffer = getTmpBufferForChannel<FloatType> (totalInputChans, data.numSamples))
                        {
                            tmpBufferNeedsClearing = true;
                            channelList.set (totalInputChans++, tmpBuffer);
                        }
                        else
                            return;
                    }
                }
            }
        }

        if (tmpBufferNeedsClearing)
            ChooseBufferHelper<FloatType>::impl (emptyBufferFloat, emptyBufferDouble).clear();

        AudioBuffer<FloatType> buffer;

        if (int totalChans = jmax (totalOutputChans, totalInputChans))
            buffer.setDataToReferTo (channelList.getRawDataPointer(), totalChans, (int) data.numSamples);

        {
            const ScopedLock sl (pluginInstance->getCallbackLock());

            pluginInstance->setNonRealtime (data.processMode == Vst::kOffline);

            if (data.inputParameterChanges != nullptr)
                processParameterChanges (*data.inputParameterChanges);

           #if JUCE_DEBUG && ! JucePlugin_ProducesMidiOutput
            const int numMidiEventsComingIn = midiBuffer.getNumEvents();
           #endif

            if (pluginInstance->isSuspended())
            {
                buffer.clear();
            }
            else
            {
                if (totalInputChans == pluginInstance->getTotalNumInputChannels()
                 && totalOutputChans == pluginInstance->getTotalNumOutputChannels())
                {
                    if (isBypassed())
                        pluginInstance->processBlockBypassed (buffer, midiBuffer);
                    else
                        pluginInstance->processBlock (buffer, midiBuffer);
                }
            }

           #if JUCE_DEBUG && (! JucePlugin_ProducesMidiOutput)
            /*  This assertion is caused when you've added some events to the
                midiMessages array in your processBlock() method, which usually means
                that you're trying to send them somewhere. But in this case they're
                getting thrown away.

                If your plugin does want to send MIDI messages, you'll need to set
                the JucePlugin_ProducesMidiOutput macro to 1 in your
                JucePluginCharacteristics.h file.

                If you don't want to produce any MIDI output, then you should clear the
                midiMessages array at the end of your processBlock() method, to
                indicate that you don't want any of the events to be passed through
                to the output.
            */
            jassert (midiBuffer.getNumEvents() <= numMidiEventsComingIn);
           #endif
        }
    }

    //==============================================================================
    template <typename FloatType>
    void allocateChannelListAndBuffers (Array<FloatType*>& channelList, AudioBuffer<FloatType>& buffer)
    {
        channelList.clearQuick();
        channelList.insertMultiple (0, nullptr, 128);

        auto& p = getPluginInstance();
        buffer.setSize (jmax (p.getTotalNumInputChannels(), p.getTotalNumOutputChannels()), p.getBlockSize() * 4);
        buffer.clear();
    }

    template <typename FloatType>
    void deallocateChannelListAndBuffers (Array<FloatType*>& channelList, AudioBuffer<FloatType>& buffer)
    {
        channelList.clearQuick();
        channelList.resize (0);
        buffer.setSize (0, 0);
    }

    template <typename FloatType>
    static FloatType** getPointerForAudioBus (Vst::AudioBusBuffers& data) noexcept
    {
        return AudioBusPointerHelper<FloatType>::impl (data);
    }

    template <typename FloatType>
    FloatType* getTmpBufferForChannel (int channel, int numSamples) noexcept
    {
        auto& buffer = ChooseBufferHelper<FloatType>::impl (emptyBufferFloat, emptyBufferDouble);

        // we can't do anything if the host requests to render many more samples than the
        // block size, we need to bail out
        if (numSamples > buffer.getNumSamples() || channel >= buffer.getNumChannels())
            return nullptr;

        return buffer.getWritePointer (channel);
    }

    void preparePlugin (double sampleRate, int bufferSize)
    {
        auto& p = getPluginInstance();

        p.setRateAndBufferSizeDetails (sampleRate, bufferSize);
        p.prepareToPlay (sampleRate, bufferSize);
    }

    //==============================================================================
   #if JUCE_FORCE_USE_LEGACY_PARAM_IDS
    inline Vst::ParamID getVSTParamIDForIndex (int paramIndex) const noexcept   { return static_cast<Vst::ParamID> (paramIndex); }
    inline int getJuceIndexForVSTParamID (Vst::ParamID paramID) const noexcept  { return static_cast<int> (paramID); }
   #else
    void cacheParameterIDs()
    {
        const int numParameters = pluginInstance->getNumParameters();
        usingManagedParameter = (pluginInstance->getParameters().size() == numParameters);

        vstBypassParameterId = static_cast<Vst::ParamID> (usingManagedParameter ? JuceVST3EditController::paramBypass : numParameters);

        for (int i = 0; i < numParameters; ++i)
        {
            auto paramID = JuceVST3EditController::generateVSTParamIDForIndex (pluginInstance, i);

            // Consider yourself very unlucky if you hit this assertion. The hash code of your
            // parameter ids are not unique.
            jassert (! vstParamIDs.contains (paramID));

            vstParamIDs.add (paramID);
            paramMap.set (static_cast<int32> (paramID), i);
        }
    }

    inline Vst::ParamID getVSTParamIDForIndex (int paramIndex) const noexcept
    {
        return usingManagedParameter ? vstParamIDs.getReference (paramIndex)
                                     : static_cast<Vst::ParamID> (paramIndex);
    }

    inline int getJuceIndexForVSTParamID (Vst::ParamID paramID) const noexcept
    {
        return usingManagedParameter ? paramMap[static_cast<int32> (paramID)]
                                     : static_cast<int> (paramID);
    }
   #endif

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceVST3Component)
};

const char* JuceVST3Component::kJucePrivateDataIdentifier = "JUCEPrivateData";

//==============================================================================
#if JUCE_MSVC
 #pragma warning (push, 0)
 #pragma warning (disable: 4310)
#elif JUCE_CLANG
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wall"
#endif

DECLARE_CLASS_IID (JuceAudioProcessor, 0x0101ABAB, 0xABCDEF01, JucePlugin_ManufacturerCode, JucePlugin_PluginCode)
DEF_CLASS_IID (JuceAudioProcessor)

#if JUCE_VST3_CAN_REPLACE_VST2
 FUID getFUIDForVST2ID (bool forControllerUID)
 {
     TUID uuid;
     extern JUCE_API void getUUIDForVST2ID (bool, uint8[16]);
     getUUIDForVST2ID (forControllerUID, (uint8*) uuid);
     return FUID (uuid);
 }
 const Steinberg::FUID JuceVST3Component     ::iid (getFUIDForVST2ID (false));
 const Steinberg::FUID JuceVST3EditController::iid (getFUIDForVST2ID (true));
#else
 DECLARE_CLASS_IID (JuceVST3EditController, 0xABCDEF01, 0x1234ABCD, JucePlugin_ManufacturerCode, JucePlugin_PluginCode)
 DEF_CLASS_IID (JuceVST3EditController)

 DECLARE_CLASS_IID (JuceVST3Component, 0xABCDEF01, 0x9182FAEB, JucePlugin_ManufacturerCode, JucePlugin_PluginCode)
 DEF_CLASS_IID (JuceVST3Component)
#endif

#if JUCE_MSVC
 #pragma warning (pop)
#elif JUCE_CLANG
 #pragma clang diagnostic pop
#endif

//==============================================================================
bool initModule()
{
   #if JUCE_MAC
    initialiseMacVST();
   #endif

    return true;
}

bool shutdownModule()
{
    return true;
}

#undef JUCE_EXPORTED_FUNCTION

#if JUCE_WINDOWS
    extern "C" __declspec (dllexport) bool InitDll()   { return initModule(); }
    extern "C" __declspec (dllexport) bool ExitDll()   { return shutdownModule(); }
    #define JUCE_EXPORTED_FUNCTION

#else
    #define JUCE_EXPORTED_FUNCTION extern "C" __attribute__ ((visibility ("default")))

    CFBundleRef globalBundleInstance = nullptr;
    juce::uint32 numBundleRefs = 0;
    juce::Array<CFBundleRef> bundleRefs;

    enum { MaxPathLength = 2048 };
    char modulePath[MaxPathLength] = { 0 };
    void* moduleHandle = nullptr;

    JUCE_EXPORTED_FUNCTION bool bundleEntry (CFBundleRef ref)
    {
        if (ref != nullptr)
        {
            ++numBundleRefs;
            CFRetain (ref);

            bundleRefs.add (ref);

            if (moduleHandle == nullptr)
            {
                globalBundleInstance = ref;
                moduleHandle = ref;

                CFURLRef tempURL = CFBundleCopyBundleURL (ref);
                CFURLGetFileSystemRepresentation (tempURL, true, (UInt8*) modulePath, MaxPathLength);
                CFRelease (tempURL);
            }
        }

        return initModule();
    }

    JUCE_EXPORTED_FUNCTION bool bundleExit()
    {
        if (shutdownModule())
        {
            if (--numBundleRefs == 0)
            {
                for (int i = 0; i < bundleRefs.size(); ++i)
                    CFRelease (bundleRefs.getUnchecked (i));

                bundleRefs.clear();
            }

            return true;
        }

        return false;
    }
#endif

//==============================================================================
/** This typedef represents VST3's createInstance() function signature */
typedef FUnknown* (*CreateFunction) (Vst::IHostApplication*);

static FUnknown* createComponentInstance (Vst::IHostApplication* host)
{
    return static_cast<Vst::IAudioProcessor*> (new JuceVST3Component (host));
}

static FUnknown* createControllerInstance (Vst::IHostApplication* host)
{
    return static_cast<Vst::IEditController*> (new JuceVST3EditController (host));
}

//==============================================================================
struct JucePluginFactory;
static JucePluginFactory* globalFactory = nullptr;

//==============================================================================
struct JucePluginFactory  : public IPluginFactory3
{
    JucePluginFactory()
        : factoryInfo (JucePlugin_Manufacturer, JucePlugin_ManufacturerWebsite,
                       JucePlugin_ManufacturerEmail, Vst::kDefaultFactoryFlags)
    {
    }

    virtual ~JucePluginFactory()
    {
        if (globalFactory == this)
            globalFactory = nullptr;
    }

    //==============================================================================
    bool registerClass (const PClassInfo2& info, CreateFunction createFunction)
    {
        if (createFunction == nullptr)
        {
            jassertfalse;
            return false;
        }

        auto* entry = classes.add (new ClassEntry (info, createFunction));
        entry->infoW.fromAscii (info);

        return true;
    }

    bool isClassRegistered (const FUID& cid) const
    {
        for (int i = 0; i < classes.size(); ++i)
            if (classes.getUnchecked (i)->infoW.cid == cid)
                return true;

        return false;
    }

    //==============================================================================
    JUCE_DECLARE_VST3_COM_REF_METHODS

    tresult PLUGIN_API queryInterface (const TUID targetIID, void** obj) override
    {
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, IPluginFactory3)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, IPluginFactory2)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, IPluginFactory)
        TEST_FOR_AND_RETURN_IF_VALID (targetIID, FUnknown)

        jassertfalse; // Something new?
        *obj = nullptr;
        return kNotImplemented;
    }

    //==============================================================================
    Steinberg::int32 PLUGIN_API countClasses() override
    {
        return (Steinberg::int32) classes.size();
    }

    tresult PLUGIN_API getFactoryInfo (PFactoryInfo* info) override
    {
        if (info == nullptr)
            return kInvalidArgument;

        memcpy (info, &factoryInfo, sizeof (PFactoryInfo));
        return kResultOk;
    }

    tresult PLUGIN_API getClassInfo (Steinberg::int32 index, PClassInfo* info) override
    {
        return getPClassInfo<PClassInfo> (index, info);
    }

    tresult PLUGIN_API getClassInfo2 (Steinberg::int32 index, PClassInfo2* info) override
    {
        return getPClassInfo<PClassInfo2> (index, info);
    }

    tresult PLUGIN_API getClassInfoUnicode (Steinberg::int32 index, PClassInfoW* info) override
    {
        if (info != nullptr)
        {
            if (auto* entry = classes[(int) index])
            {
                memcpy (info, &entry->infoW, sizeof (PClassInfoW));
                return kResultOk;
            }
        }

        return kInvalidArgument;
    }

    tresult PLUGIN_API createInstance (FIDString cid, FIDString sourceIid, void** obj) override
    {
        *obj = nullptr;

        FUID sourceFuid = sourceIid;

        if (cid == nullptr || sourceIid == nullptr || ! sourceFuid.isValid())
        {
            jassertfalse; // The host you're running in has severe implementation issues!
            return kInvalidArgument;
        }

        TUID iidToQuery;
        sourceFuid.toTUID (iidToQuery);

        for (auto* entry : classes)
        {
            if (doUIDsMatch (entry->infoW.cid, cid))
            {
                if (auto* instance = entry->createFunction (host))
                {
                    const FReleaser releaser (instance);

                    if (instance->queryInterface (iidToQuery, obj) == kResultOk)
                        return kResultOk;
                }

                break;
            }
        }

        return kNoInterface;
    }

    tresult PLUGIN_API setHostContext (FUnknown* context) override
    {
        host.loadFrom (context);

        if (host != nullptr)
        {
            Vst::String128 name;
            host->getName (name);

            return kResultTrue;
        }

        return kNotImplemented;
    }

private:
    //==============================================================================
    ScopedJuceInitialiser_GUI libraryInitialiser;
    Atomic<int> refCount { 1 };
    const PFactoryInfo factoryInfo;
    ComSmartPtr<Vst::IHostApplication> host;

    //==============================================================================
    struct ClassEntry
    {
        ClassEntry() noexcept {}

        ClassEntry (const PClassInfo2& info, CreateFunction fn) noexcept
            : info2 (info), createFunction (fn) {}

        PClassInfo2 info2;
        PClassInfoW infoW;
        CreateFunction createFunction = {};
        bool isUnicode = false;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ClassEntry)
    };

    OwnedArray<ClassEntry> classes;

    //==============================================================================
    template<class PClassInfoType>
    tresult PLUGIN_API getPClassInfo (Steinberg::int32 index, PClassInfoType* info)
    {
        if (info != nullptr)
        {
            zerostruct (*info);

            if (auto* entry = classes[(int) index])
            {
                if (entry->isUnicode)
                    return kResultFalse;

                memcpy (info, &entry->info2, sizeof (PClassInfoType));
                return kResultOk;
            }
        }

        jassertfalse;
        return kInvalidArgument;
    }

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePluginFactory)
};

} // juce namespace

//==============================================================================
#ifndef JucePlugin_Vst3ComponentFlags
 #if JucePlugin_IsSynth
  #define JucePlugin_Vst3ComponentFlags Vst::kSimpleModeSupported
 #else
  #define JucePlugin_Vst3ComponentFlags 0
 #endif
#endif

#ifndef JucePlugin_Vst3Category
 #if JucePlugin_IsSynth
  #define JucePlugin_Vst3Category Vst::PlugType::kInstrumentSynth
 #else
  #define JucePlugin_Vst3Category Vst::PlugType::kFx
 #endif
#endif

//==============================================================================
// The VST3 plugin entry point.
JUCE_EXPORTED_FUNCTION IPluginFactory* PLUGIN_API GetPluginFactory()
{
    PluginHostType::jucePlugInClientCurrentWrapperType = AudioProcessor::wrapperType_VST3;

   #if JUCE_WINDOWS
    // Cunning trick to force this function to be exported. Life's too short to
    // faff around creating .def files for this kind of thing.
    #pragma comment(linker, "/EXPORT:" __FUNCTION__ "=" __FUNCDNAME__)
   #endif

    if (globalFactory == nullptr)
    {
        globalFactory = new JucePluginFactory();

        static const PClassInfo2 componentClass (JuceVST3Component::iid,
                                                 PClassInfo::kManyInstances,
                                                 kVstAudioEffectClass,
                                                 JucePlugin_Name,
                                                 JucePlugin_Vst3ComponentFlags,
                                                 JucePlugin_Vst3Category,
                                                 JucePlugin_Manufacturer,
                                                 JucePlugin_VersionString,
                                                 kVstVersionString);

        globalFactory->registerClass (componentClass, createComponentInstance);

        static const PClassInfo2 controllerClass (JuceVST3EditController::iid,
                                                  PClassInfo::kManyInstances,
                                                  kVstComponentControllerClass,
                                                  JucePlugin_Name,
                                                  JucePlugin_Vst3ComponentFlags,
                                                  JucePlugin_Vst3Category,
                                                  JucePlugin_Manufacturer,
                                                  JucePlugin_VersionString,
                                                  kVstVersionString);

        globalFactory->registerClass (controllerClass, createControllerInstance);
    }
    else
    {
        globalFactory->addRef();
    }

    return dynamic_cast<IPluginFactory*> (globalFactory);
}

//==============================================================================
#if _MSC_VER || JUCE_MINGW
extern "C" BOOL WINAPI DllMain (HINSTANCE instance, DWORD reason, LPVOID) { if (reason == DLL_PROCESS_ATTACH) Process::setCurrentModuleInstanceHandle (instance); return true; }
#endif

#endif //JucePlugin_Build_VST3
