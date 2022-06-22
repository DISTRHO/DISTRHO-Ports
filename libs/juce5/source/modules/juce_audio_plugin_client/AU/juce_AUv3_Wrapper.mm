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
#include "../utility/juce_CheckSettingMacros.h"

#if JucePlugin_Build_AUv3

#import <CoreAudioKit/CoreAudioKit.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

#if JUCE_MAC
 #if (! defined MAC_OS_X_VERSION_MIN_REQUIRED) || (! defined MAC_OS_X_VERSION_10_11) || (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_11)
  #error AUv3 needs Deployment Target OS X 10.11 or higher to compile
 #endif
 #if (defined MAC_OS_X_VERSION_10_13) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_13)
  #define JUCE_AUV3_MIDI_OUTPUT_SUPPORTED 1
  #define JUCE_AUV3_VIEW_CONFIG_SUPPORTED 1
 #endif
#endif

#if JUCE_IOS
 #if (! defined __IPHONE_OS_VERSION_MIN_REQUIRED) || (! defined __IPHONE_9_0) || (__IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_9_0)
  #error AUv3 needs Deployment Target iOS 9.0 or higher to compile
 #endif
 #if (defined __IPHONE_11_0) && (__IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_11_0)
  #define JUCE_AUV3_MIDI_OUTPUT_SUPPORTED 1
  #define JUCE_AUV3_VIEW_CONFIG_SUPPORTED 1
 #endif
#endif

#ifndef __OBJC2__
 #error AUv3 needs Objective-C 2 support (compile with 64-bit)
#endif

#define JUCE_CORE_INCLUDE_OBJC_HELPERS 1

#include "../utility/juce_IncludeSystemHeaders.h"
#include "../utility/juce_IncludeModuleHeaders.h"
#include "../../juce_graphics/native/juce_mac_CoreGraphicsHelpers.h"

#include "../../juce_audio_basics/native/juce_mac_CoreAudioLayouts.h"
#include "../../juce_audio_processors/format_types/juce_AU_Shared.h"

#define JUCE_VIEWCONTROLLER_OBJC_NAME(x) JUCE_JOIN_MACRO (x, FactoryAUv3)

#if ! JUCE_COMPILER_SUPPORTS_VARIADIC_TEMPLATES
 #error AUv3 wrapper requires variadic template support
#endif

#if JUCE_IOS
 #define JUCE_IOS_MAC_VIEW  UIView
#else
 #define JUCE_IOS_MAC_VIEW  NSView
#endif

#define JUCE_AUDIOUNIT_OBJC_NAME(x) JUCE_JOIN_MACRO (x, AUv3)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

using namespace juce;

// TODO: use SFINAE to automatically generate this for all NSObjects
template <> struct ContainerDeletePolicy<AUAudioUnitBusArray>                   { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<AUParameterTree>                       { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<NSMutableArray<AUParameterNode *> >    { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<AUParameter>                           { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<NSMutableArray<AUAudioUnitBus*> >      { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<AUAudioUnitBus>                        { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<AVAudioFormat>                         { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<AVAudioPCMBuffer>                      { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<NSMutableArray<NSNumber*> >            { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<NSNumber>                              { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<NSMutableArray<AUAudioUnitPreset*> >   { static void destroy (NSObject* o) { [o release]; } };
template <> struct ContainerDeletePolicy<AUAudioUnitPreset>                     { static void destroy (NSObject* o) { [o release]; } };

//==============================================================================
struct AudioProcessorHolder  : public ReferenceCountedObject
{
    AudioProcessorHolder() {}
    AudioProcessorHolder (AudioProcessor* p) : processor (p) {}
    AudioProcessor& operator*() noexcept        { return *processor; }
    AudioProcessor* operator->() noexcept       { return processor; }
    AudioProcessor* get() noexcept              { return processor; }

    struct ViewConfig
    {
        double width;
        double height;
        bool hostHasMIDIController;
    };

    ScopedPointer<ViewConfig> viewConfiguration;

    typedef ReferenceCountedObjectPtr<AudioProcessorHolder> Ptr;

private:
    ScopedPointer<AudioProcessor> processor;

    AudioProcessorHolder& operator= (AudioProcessor*) JUCE_DELETED_FUNCTION;
    AudioProcessorHolder (AudioProcessorHolder&) JUCE_DELETED_FUNCTION;
    AudioProcessorHolder& operator= (AudioProcessorHolder&) JUCE_DELETED_FUNCTION;
};

//==============================================================================
class JuceAudioUnitv3Base
{
public:
    JuceAudioUnitv3Base (const AudioComponentDescription& descr,
                         AudioComponentInstantiationOptions options,
                         NSError** error)
       #pragma clang diagnostic push
       #pragma clang diagnostic ignored "-Wobjc-method-access"
        : au ([audioUnitObjCClass.createInstance() initWithComponentDescription: descr
                                                                        options: options
                                                                          error: error
                                                                      juceClass: this])
       #pragma clang diagnostic pop
    {}

    JuceAudioUnitv3Base (AUAudioUnit* audioUnit) : au (audioUnit)
    {
        jassert (MessageManager::getInstance()->isThisTheMessageThread());
        initialiseJuce_GUI();
    }

    virtual ~JuceAudioUnitv3Base() {}

    //==============================================================================
    AUAudioUnit* getAudioUnit() noexcept                                   { return au; }

    //==============================================================================
    virtual void reset()                                                   = 0;

    //==============================================================================
    virtual AUAudioUnitPreset* getCurrentPreset()                          = 0;
    virtual void setCurrentPreset(AUAudioUnitPreset*)                      = 0;
    virtual NSArray<AUAudioUnitPreset*>* getFactoryPresets()               = 0;

    virtual NSDictionary<NSString*, id>* getFullState()
    {
        objc_super s = { getAudioUnit(), [AUAudioUnit class] };
        return ObjCMsgSendSuper<NSDictionary<NSString*, id>*> (&s, @selector (fullState));
    }

    virtual void setFullState (NSDictionary<NSString*, id>* state)
    {
        objc_super s = { getAudioUnit(), [AUAudioUnit class] };
        ObjCMsgSendSuper<void, NSDictionary<NSString*, id>*> (&s, @selector (setFullState:), state);
    }

    virtual AUParameterTree* getParameterTree()                            = 0;
    virtual NSArray<NSNumber*>* parametersForOverviewWithCount (int)       = 0;

    //==============================================================================
    virtual NSTimeInterval getLatency()                                    = 0;
    virtual NSTimeInterval getTailTime()                                   = 0;

    //==============================================================================
    virtual AUAudioUnitBusArray* getInputBusses()                          = 0;
    virtual AUAudioUnitBusArray* getOutputBusses()                         = 0;
    virtual NSArray<NSNumber*>* getChannelCapabilities()                   = 0;
    virtual bool shouldChangeToFormat (AVAudioFormat*, AUAudioUnitBus*)    = 0;

    //==============================================================================
    virtual int getVirtualMIDICableCount()                                 = 0;
    virtual bool getSupportsMPE()                                          = 0;
    virtual NSArray<NSString*>* getMIDIOutputNames()                       = 0;

    //==============================================================================
    virtual AUInternalRenderBlock getInternalRenderBlock()                 = 0;
    virtual bool getCanProcessInPlace()                                    { return false; }
    virtual bool getRenderingOffline()                                     = 0;
    virtual void setRenderingOffline (bool offline)                        = 0;

    //==============================================================================
    virtual NSString* getContextName()      const                          = 0;
    virtual void setContextName (NSString*)                                = 0;

    virtual bool allocateRenderResourcesAndReturnError (NSError **outError)
    {
        objc_super s = { getAudioUnit(), [AUAudioUnit class] };
        return (ObjCMsgSendSuper<BOOL, NSError**> (&s, @selector (allocateRenderResourcesAndReturnError:), outError) == YES);
    }

    virtual void deallocateRenderResources()
    {
        objc_super s = { getAudioUnit(), [AUAudioUnit class] };
        ObjCMsgSendSuper<void> (&s, @selector (deallocateRenderResources));
    }

    //==============================================================================
   #if JUCE_AUV3_VIEW_CONFIG_SUPPORTED
    virtual NSIndexSet* getSupportedViewConfigurations (NSArray<AUAudioUnitViewConfiguration*>*) = 0;
    virtual void selectViewConfiguration (AUAudioUnitViewConfiguration*)   = 0;
   #endif

private:
    struct Class  : public ObjCClass<AUAudioUnit>
    {
        Class() : ObjCClass<AUAudioUnit> ("AUAudioUnit_")
        {
            addIvar<JuceAudioUnitv3Base*> ("cppObject");

           #pragma clang diagnostic push
           #pragma clang diagnostic ignored "-Wundeclared-selector"
            addMethod (@selector (initWithComponentDescription:options:error:juceClass:),
                       initWithComponentDescriptionAndJuceClass, "@@:",
                       @encode (AudioComponentDescription),
                       @encode (AudioComponentInstantiationOptions), "^@@");
           #pragma clang diagnostic pop

            addMethod (@selector (initWithComponentDescription:options:error:),
                       initWithComponentDescription, "@@:",
                       @encode (AudioComponentDescription),
                       @encode (AudioComponentInstantiationOptions), "^@");

            addMethod (@selector (dealloc),                         dealloc,                        "v@:");

            //==============================================================================
            addMethod (@selector (reset),                           reset,                          "v@:");

            //==============================================================================
            addMethod (@selector (currentPreset),                   getCurrentPreset,               "@@:");
            addMethod (@selector (setCurrentPreset:),               setCurrentPreset,               "v@:@");
            addMethod (@selector (factoryPresets),                  getFactoryPresets,              "@@:");
            addMethod (@selector (fullState),                       getFullState,                   "@@:");
            addMethod (@selector (setFullState:),                   setFullState,                   "v@:@");
            addMethod (@selector (parameterTree),                   getParameterTree,               "@@:");
            addMethod (@selector (parametersForOverviewWithCount:), parametersForOverviewWithCount, "@@:", @encode (NSInteger));

            //==============================================================================
            addMethod (@selector (latency),                         getLatency,                     @encode (NSTimeInterval), "@:");
            addMethod (@selector (tailTime),                        getTailTime,                    @encode (NSTimeInterval),  "@:");

            //==============================================================================
            addMethod (@selector (inputBusses),                     getInputBusses,                 "@@:");
            addMethod (@selector (outputBusses),                    getOutputBusses,                "@@:");
            addMethod (@selector (channelCapabilities),             getChannelCapabilities,         "@@:");
            addMethod (@selector (shouldChangeToFormat:forBus:),    shouldChangeToFormat,           "B@:@@");

            //==============================================================================
            addMethod (@selector (virtualMIDICableCount),           getVirtualMIDICableCount,       @encode (NSInteger), "@:");
            addMethod (@selector (supportsMPE),                     getSupportsMPE,                 @encode (BOOL),      "@:");

           #if JUCE_AUV3_MIDI_OUTPUT_SUPPORTED
            addMethod (@selector (MIDIOutputNames),                 getMIDIOutputNames,             "@@:");
           #endif

            //==============================================================================
            addMethod (@selector (internalRenderBlock),             getInternalRenderBlock,         @encode (AUInternalRenderBlock), "@:");
            addMethod (@selector (canProcessInPlace),               getCanProcessInPlace,           @encode (BOOL), "@:");
            addMethod (@selector (isRenderingOffline),              getRenderingOffline,            @encode (BOOL),  "@:");
            addMethod (@selector (setRenderingOffline:),            setRenderingOffline,            "v@:", @encode (BOOL));
            addMethod (@selector (allocateRenderResourcesAndReturnError:),  allocateRenderResourcesAndReturnError, "B@:^@");
            addMethod (@selector (deallocateRenderResources),       deallocateRenderResources,      "v@:");

            //==============================================================================
            addMethod (@selector (contextName),                     getContextName,                 "@@:");
            addMethod (@selector (setContextName:),                  setContextName,                 "v@:@");

            //==============================================================================
           #if JUCE_AUV3_VIEW_CONFIG_SUPPORTED
            addMethod (@selector (supportedViewConfigurations:),    getSupportedViewConfigurations, "@@:@");
            addMethod (@selector (selectViewConfiguration:),        selectViewConfiguration,        "v@:@");
           #endif

            registerClass();
        }

        //==============================================================================
        static JuceAudioUnitv3Base* _this (id self)                     { return getIvar<JuceAudioUnitv3Base*> (self, "cppObject"); }
        static void setThis (id self, JuceAudioUnitv3Base* cpp)         { object_setInstanceVariable           (self, "cppObject", cpp); }

        //==============================================================================
        static id initWithComponentDescription (id _self, SEL, AudioComponentDescription descr, AudioComponentInstantiationOptions options, NSError** error)
        {
            AUAudioUnit* self = _self;

            objc_super s = { self, [AUAudioUnit class] };
            self = ObjCMsgSendSuper<AUAudioUnit*, AudioComponentDescription,
                                    AudioComponentInstantiationOptions, NSError**> (&s, @selector(initWithComponentDescription:options:error:), descr, options, error);

            JuceAudioUnitv3Base* juceAU = JuceAudioUnitv3Base::create (self, descr, options, error);

            setThis (self, juceAU);
            return self;
        }

        static id initWithComponentDescriptionAndJuceClass (id _self, SEL, AudioComponentDescription descr, AudioComponentInstantiationOptions options, NSError** error, JuceAudioUnitv3Base* juceAU)
        {
            AUAudioUnit* self = _self;

            objc_super s = { self, [AUAudioUnit class] };
            self = ObjCMsgSendSuper<AUAudioUnit*, AudioComponentDescription,
                                    AudioComponentInstantiationOptions, NSError**> (&s, @selector(initWithComponentDescription:options:error:), descr, options, error);


            setThis (self, juceAU);
            return self;
        }

        static void dealloc (id self, SEL)                                                          { delete _this (self); }

        //==============================================================================
        static void reset (id self, SEL)                                                            { _this (self)->reset(); }

        //==============================================================================
        static AUAudioUnitPreset* getCurrentPreset (id self, SEL)                                   { return _this (self)->getCurrentPreset(); }
        static void setCurrentPreset (id self, SEL, AUAudioUnitPreset* preset)                      { return _this (self)->setCurrentPreset (preset); }
        static NSArray<AUAudioUnitPreset*>* getFactoryPresets (id self, SEL)                        { return _this (self)->getFactoryPresets(); }
        static NSDictionary<NSString*, id>* getFullState (id self, SEL)                             { return _this (self)->getFullState(); }
        static void setFullState (id self, SEL, NSDictionary<NSString *, id>* state)                { return _this (self)->setFullState (state); }
        static AUParameterTree*     getParameterTree (id self, SEL)                                 { return _this (self)->getParameterTree(); }
        static NSArray<NSNumber*>* parametersForOverviewWithCount (id self, SEL, NSInteger count)   { return _this (self)->parametersForOverviewWithCount (static_cast<int> (count)); }

        //==============================================================================
        static NSTimeInterval getLatency (id self, SEL)                                             { return _this (self)->getLatency(); }
        static NSTimeInterval getTailTime (id self, SEL)                                            { return _this (self)->getTailTime(); }

        //==============================================================================
        static AUAudioUnitBusArray* getInputBusses   (id self, SEL)                                 { return _this (self)->getInputBusses(); }
        static AUAudioUnitBusArray* getOutputBusses  (id self, SEL)                                 { return _this (self)->getOutputBusses(); }
        static NSArray<NSNumber*>* getChannelCapabilities (id self, SEL)                            { return _this (self)->getChannelCapabilities(); }
        static BOOL shouldChangeToFormat (id self, SEL, AVAudioFormat* format, AUAudioUnitBus* bus) { return _this (self)->shouldChangeToFormat (format, bus) ? YES : NO; }

        //==============================================================================
        static NSInteger getVirtualMIDICableCount (id self, SEL)                                    { return _this (self)->getVirtualMIDICableCount(); }
        static BOOL getSupportsMPE (id self, SEL)                                                   { return _this (self)->getSupportsMPE() ? YES : NO; }
        static NSArray<NSString*>* getMIDIOutputNames (id self, SEL)                                { return _this (self)->getMIDIOutputNames(); }

        //==============================================================================
        static AUInternalRenderBlock getInternalRenderBlock (id self, SEL)                          { return _this (self)->getInternalRenderBlock();  }
        static BOOL getCanProcessInPlace (id self, SEL)                                             { return _this (self)->getCanProcessInPlace() ? YES : NO; }
        static BOOL getRenderingOffline (id self, SEL)                                              { return _this (self)->getRenderingOffline() ? YES : NO; }
        static void setRenderingOffline (id self, SEL, BOOL renderingOffline)                       { _this (self)->setRenderingOffline (renderingOffline); }
        static BOOL allocateRenderResourcesAndReturnError (id self, SEL, NSError** error)           { return _this (self)->allocateRenderResourcesAndReturnError (error) ? YES : NO; }
        static void deallocateRenderResources (id self, SEL)                                        { _this (self)->deallocateRenderResources(); }

        //==============================================================================
        static NSString* getContextName (id self, SEL)                                              { return _this (self)->getContextName(); }
        static void setContextName (id self, SEL, NSString* str)                                    { return _this (self)->setContextName (str); }

        //==============================================================================
       #if JUCE_AUV3_VIEW_CONFIG_SUPPORTED
        static NSIndexSet* getSupportedViewConfigurations (id self, SEL, NSArray<AUAudioUnitViewConfiguration*>* configs) { return _this (self)->getSupportedViewConfigurations (configs); }
        static void selectViewConfiguration (id self, SEL, AUAudioUnitViewConfiguration* config)    { _this (self)->selectViewConfiguration (config); }
       #endif
    };

    static JuceAudioUnitv3Base* create (AUAudioUnit*, AudioComponentDescription, AudioComponentInstantiationOptions, NSError**);

    //==============================================================================
    static Class audioUnitObjCClass;

protected:
    AUAudioUnit* au;
};

//==============================================================================
JuceAudioUnitv3Base::Class JuceAudioUnitv3Base::audioUnitObjCClass;

//==============================================================================
//=========================== The actual AudioUnit =============================
//==============================================================================
class JuceAudioUnitv3  : public JuceAudioUnitv3Base,
                         public AudioProcessorListener,
                         public AudioPlayHead
{
public:
    JuceAudioUnitv3 (const AudioProcessorHolder::Ptr& processor,
                     const AudioComponentDescription& descr,
                     AudioComponentInstantiationOptions options,
                     NSError** error)
        : JuceAudioUnitv3Base (descr, options, error),
          processorHolder (processor),
          mapper (*processorHolder->get())
    {
        init();
    }

    JuceAudioUnitv3 (AUAudioUnit* audioUnit, AudioComponentDescription, AudioComponentInstantiationOptions, NSError**)
        : JuceAudioUnitv3Base (audioUnit),
          processorHolder (new AudioProcessorHolder (createPluginFilterOfType (AudioProcessor::wrapperType_AudioUnitv3))),
          mapper (*processorHolder->get())
    {
        init();
    }

    ~JuceAudioUnitv3()
    {
        auto& processor = getAudioProcessor();
        processor.removeListener (this);

        removeEditor (processor);

        if (editorObserverToken != nullptr)
        {
            [paramTree removeParameterObserver: editorObserverToken];
            editorObserverToken = nullptr;
        }
    }

    //==============================================================================
    void init()
    {
        AudioProcessor& processor = getAudioProcessor();
        const AUAudioFrameCount maxFrames = [getAudioUnit() maximumFramesToRender];

       #ifdef JucePlugin_PreferredChannelConfigurations
        short configs[][2] = {JucePlugin_PreferredChannelConfigurations};
        const int numConfigs = sizeof (configs) / sizeof (short[2]);

        jassert (numConfigs > 0 && (configs[0][0] > 0 || configs[0][1] > 0));
        processor.setPlayConfigDetails (configs[0][0], configs[0][1], kDefaultSampleRate, static_cast<int> (maxFrames));

        Array<AUChannelInfo> channelInfos;

        for (int i = 0; i < numConfigs; ++i)
        {
            AUChannelInfo channelInfo;

            channelInfo.inChannels  = configs[i][0];
            channelInfo.outChannels = configs[i][1];

            channelInfos.add (channelInfo);
        }
       #else
        Array<AUChannelInfo> channelInfos = AudioUnitHelpers::getAUChannelInfo (processor);
       #endif

        processor.setPlayHead (this);

        totalInChannels  = processor.getTotalNumInputChannels();
        totalOutChannels = processor.getTotalNumOutputChannels();

        {
            channelCapabilities = [[NSMutableArray<NSNumber*> alloc] init];


            for (int i = 0; i < channelInfos.size(); ++i)
            {
                AUChannelInfo& info = channelInfos.getReference (i);

                [channelCapabilities addObject: [NSNumber numberWithInteger: info.inChannels]];
                [channelCapabilities addObject: [NSNumber numberWithInteger: info.outChannels]];
            }
        }

        editorObserverToken = nullptr;
        internalRenderBlock = CreateObjCBlock (this, &JuceAudioUnitv3::renderCallback);

        processor.setRateAndBufferSizeDetails (kDefaultSampleRate, static_cast<int> (maxFrames));
        processor.prepareToPlay (kDefaultSampleRate, static_cast<int> (maxFrames));
        processor.addListener (this);

        addParameters();
        addPresets();

        addAudioUnitBusses (true);
        addAudioUnitBusses (false);
    }

    AudioProcessor& getAudioProcessor() const noexcept        { return **processorHolder; }

    //==============================================================================
    void reset() override
    {
        midiMessages.clear();
        lastTimeStamp.mSampleTime = std::numeric_limits<Float64>::max();
    }

    //==============================================================================
    AUAudioUnitPreset* getCurrentPreset() override
    {
        const int n = static_cast<int> ([factoryPresets count]);
        const int idx = static_cast<int> (getAudioProcessor().getCurrentProgram());

        if (idx < n)
            return [factoryPresets objectAtIndex:static_cast<unsigned int> (idx)];

        return nullptr;
    }

    void setCurrentPreset(AUAudioUnitPreset* preset) override
    {
        const int n = static_cast<int> ([factoryPresets count]);
        const int idx = static_cast<int> ([preset number]);

        if (isPositiveAndBelow (idx, n))
            getAudioProcessor().setCurrentProgram (idx);
    }

    NSArray<AUAudioUnitPreset*>* getFactoryPresets() override
    {
        return factoryPresets;
    }

    NSDictionary<NSString*, id>* getFullState() override
    {
        NSMutableDictionary<NSString*, id>* retval = [[NSMutableDictionary<NSString*, id> alloc] init];

        {
            NSDictionary<NSString*, id>* superRetval = JuceAudioUnitv3Base::getFullState();

            if (superRetval != nullptr)
                [retval addEntriesFromDictionary:superRetval];
        }

        juce::MemoryBlock state;
        getAudioProcessor().getCurrentProgramStateInformation (state);

        if (state.getSize() > 0)
        {
            NSData* ourState = [[NSData alloc] initWithBytes: state.getData()
                                                      length: state.getSize()];

            NSString* nsKey = [[NSString alloc] initWithUTF8String: JUCE_STATE_DICTIONARY_KEY];

            [retval setObject: ourState
                       forKey: nsKey];

            [nsKey    release];
            [ourState release];
        }

        return [retval autorelease];
    }

    void setFullState (NSDictionary<NSString*, id>* state) override
    {
        if (state == nullptr)
            return;

        NSMutableDictionary<NSString*, id>* modifiedState = [[NSMutableDictionary<NSString*, id> alloc] init];
        [modifiedState addEntriesFromDictionary: state];

        NSString* nsPresetKey = [[NSString alloc] initWithUTF8String: kAUPresetDataKey];
        [modifiedState removeObjectForKey: nsPresetKey];
        [nsPresetKey release];

        JuceAudioUnitv3Base::setFullState (modifiedState);

        NSString* nsKey = [[NSString alloc] initWithUTF8String: JUCE_STATE_DICTIONARY_KEY];
        NSObject* obj = [modifiedState objectForKey: nsKey];
        [nsKey release];

        if (obj != nullptr)
        {
            if ([obj isKindOfClass:[NSData class]])
            {
                NSData* data = reinterpret_cast<NSData*> (obj);
                const int numBytes = static_cast<int> ([data length]);
                const juce::uint8* const rawBytes = reinterpret_cast< const juce::uint8* const> ([data bytes]);

                if (numBytes > 0)
                    getAudioProcessor().setCurrentProgramStateInformation (rawBytes, numBytes);
            }
        }

        [modifiedState release];
    }

    AUParameterTree* getParameterTree() override
    {
        return paramTree;
    }

    NSArray<NSNumber*>* parametersForOverviewWithCount (int count) override
    {
        const int n = static_cast<int> ([overviewParams count]);

        if (count >= n)
            return overviewParams;

        NSMutableArray<NSNumber*>* retval = [[NSMutableArray<NSNumber*>alloc] initWithArray: overviewParams];
        [retval removeObjectsInRange: NSMakeRange (static_cast<unsigned int> (count), static_cast<unsigned int> (n - count))];

        return [retval autorelease];
    }

    //==============================================================================
    NSTimeInterval getLatency() override
    {
        auto& p = getAudioProcessor();
        return p.getLatencySamples() / p.getSampleRate();
    }

    NSTimeInterval getTailTime() override
    {
        return getAudioProcessor().getTailLengthSeconds();
    }

    //==============================================================================
    AUAudioUnitBusArray* getInputBusses() override            { return inputBusses;  }
    AUAudioUnitBusArray* getOutputBusses() override           { return outputBusses; }
    NSArray<NSNumber*>* getChannelCapabilities() override     { return channelCapabilities; }

    bool shouldChangeToFormat (AVAudioFormat* format, AUAudioUnitBus* auBus) override
    {
        const bool isInput = ([auBus busType] == AUAudioUnitBusTypeInput);
        const int busIdx = static_cast<int> ([auBus index]);
        const int newNumChannels = static_cast<int> ([format channelCount]);

        AudioProcessor& processor = getAudioProcessor();

        if (AudioProcessor::Bus* bus = processor.getBus (isInput, busIdx))
        {
          #ifdef JucePlugin_PreferredChannelConfigurations
            ignoreUnused (bus);

            short configs[][2] = {JucePlugin_PreferredChannelConfigurations};

            if (! AudioUnitHelpers::isLayoutSupported (processor, isInput, busIdx, newNumChannels, configs))
                return false;
          #else
            const AVAudioChannelLayout* layout    = [format channelLayout];
            const AudioChannelLayoutTag layoutTag = (layout != nullptr ? [layout layoutTag] : 0);

            if (layoutTag != 0)
            {
                AudioChannelSet newLayout = CoreAudioLayouts::fromCoreAudio (layoutTag);

                if (newLayout.size() != newNumChannels)
                    return false;

                if (! bus->isLayoutSupported (newLayout))
                    return false;
            }
            else
            {
                if (! bus->isNumberOfChannelsSupported (newNumChannels))
                    return false;
            }
           #endif

            return true;
        }

        return false;
    }

    //==============================================================================
    int getVirtualMIDICableCount() override
    {
       #if JucePlugin_WantsMidiInput
        return 1;
       #else
        return 0;
       #endif
    }

    bool getSupportsMPE() override
    {
        return getAudioProcessor().supportsMPE();
    }

    NSArray<NSString*>* getMIDIOutputNames() override
    {
       #if JucePlugin_ProducesMidiOutput
        return @[@"MIDI Out"];
       #else
        return @[];
       #endif
    }

    //==============================================================================
    AUInternalRenderBlock getInternalRenderBlock() override   { return internalRenderBlock;  }
    bool getRenderingOffline() override                       { return getAudioProcessor().isNonRealtime(); }
    void setRenderingOffline (bool offline) override          { getAudioProcessor().setNonRealtime (offline); }

    //==============================================================================
    NSString* getContextName() const    override              { return juceStringToNS (contextName); }
    void setContextName (NSString* str) override
    {
        if (str != nullptr)
        {
            AudioProcessor::TrackProperties props;
            props.name = nsStringToJuce (str);

            getAudioProcessor().updateTrackProperties (props);
        }
    }

    //==============================================================================
    bool allocateRenderResourcesAndReturnError (NSError **outError) override
    {
        AudioProcessor& processor = getAudioProcessor();
        const AUAudioFrameCount maxFrames = [getAudioUnit() maximumFramesToRender];

        if (! JuceAudioUnitv3Base::allocateRenderResourcesAndReturnError (outError))
            return false;

        if (outError != nullptr)
            *outError = nullptr;

        AudioProcessor::BusesLayout layouts;
        for (int dir = 0; dir < 2; ++dir)
        {
            const bool isInput = (dir == 0);
            const int n = AudioUnitHelpers::getBusCount (&processor, isInput);
            Array<AudioChannelSet>& channelSets = (isInput ? layouts.inputBuses : layouts.outputBuses);

            AUAudioUnitBusArray* auBuses = (isInput ? [getAudioUnit() inputBusses] : [getAudioUnit() outputBusses]);
            jassert ([auBuses count] == static_cast<NSUInteger> (n));

            for (int busIdx = 0; busIdx < n; ++busIdx)
            {
                AudioProcessor::Bus* bus = processor.getBus (isInput, busIdx);
                AVAudioFormat* format = [[auBuses objectAtIndexedSubscript:static_cast<NSUInteger> (busIdx)] format];

                AudioChannelSet newLayout;
                const AVAudioChannelLayout* layout    = [format channelLayout];
                const AudioChannelLayoutTag layoutTag = (layout != nullptr ? [layout layoutTag] : 0);

                if (layoutTag != 0)
                    newLayout = CoreAudioLayouts::fromCoreAudio (layoutTag);
                else
                    newLayout = bus->supportedLayoutWithChannels (static_cast<int> ([format channelCount]));

                if (newLayout.isDisabled())
                    return false;

                channelSets.add (newLayout);
            }
        }

       #ifdef JucePlugin_PreferredChannelConfigurations
        short configs[][2] = {JucePlugin_PreferredChannelConfigurations};

        if (! AudioProcessor::containsLayout (layouts, configs))
        {
            if (outError != nullptr)
                *outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:kAudioUnitErr_FormatNotSupported userInfo:nullptr];

            return false;
        }
       #endif

        if (! AudioUnitHelpers::setBusesLayout (&getAudioProcessor(), layouts))
        {
            if (outError != nullptr)
                *outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:kAudioUnitErr_FormatNotSupported userInfo:nullptr];

            return false;
        }

        totalInChannels  = processor.getTotalNumInputChannels();
        totalOutChannels = processor.getTotalNumOutputChannels();

        allocateBusBuffer (true);
        allocateBusBuffer (false);

        mapper.alloc();

        audioBuffer.prepare (totalInChannels, totalOutChannels, static_cast<int> (maxFrames));

        double sampleRate = (jmax (AudioUnitHelpers::getBusCount (&processor, true), AudioUnitHelpers::getBusCount (&processor, false)) > 0 ?
                             [[[([inputBusses count] > 0 ? inputBusses : outputBusses) objectAtIndexedSubscript: 0] format] sampleRate] : 44100.0);

        processor.setRateAndBufferSizeDetails (sampleRate, static_cast<int> (maxFrames));
        processor.prepareToPlay (sampleRate, static_cast<int> (maxFrames));

        zeromem (&lastAudioHead, sizeof (lastAudioHead));
        hostMusicalContextCallback = [getAudioUnit() musicalContextBlock];
        hostTransportStateCallback = [getAudioUnit() transportStateBlock];

        reset();

        return true;
    }

    void deallocateRenderResources() override
    {
        hostMusicalContextCallback = nullptr;
        hostTransportStateCallback = nullptr;

        getAudioProcessor().releaseResources();
        audioBuffer.release();

        inBusBuffers. clear();
        outBusBuffers.clear();

        mapper.release();

        JuceAudioUnitv3Base::deallocateRenderResources();
    }

    //==============================================================================
   #if JUCE_AUV3_VIEW_CONFIG_SUPPORTED
    NSIndexSet* getSupportedViewConfigurations (NSArray<AUAudioUnitViewConfiguration*>* configs) override
    {
        auto supportedViewIndecies = [[NSMutableIndexSet alloc] init];
        auto n = [configs count];

        if (auto* editor = getAudioProcessor().createEditorIfNeeded())
        {
            // If you hit this assertion then your plug-in's editor is reporting that it doesn't support
            // any host MIDI controller configurations!
            jassert (editor->supportsHostMIDIControllerPresence (true) || editor->supportsHostMIDIControllerPresence (false));

            for (auto i = 0u; i < n; ++i)
            {
                if (auto* viewConfiguration = [configs objectAtIndex:i])
                {
                    if (editor->supportsHostMIDIControllerPresence ([viewConfiguration hostHasController] == YES))
                    {
                        auto* constrainer = editor->getConstrainer();
                        auto height = (int) [viewConfiguration height];
                        auto width  = (int) [viewConfiguration width];

                        if (height <= constrainer->getMaximumHeight() && height >= constrainer->getMinimumHeight()
                         && width  <= constrainer->getMaximumWidth()  && width  >= constrainer->getMinimumWidth())
                            [supportedViewIndecies addIndex: i];
                    }
                }
            }
        }

        return [supportedViewIndecies autorelease];
    }

    void selectViewConfiguration (AUAudioUnitViewConfiguration* config) override
    {
        processorHolder->viewConfiguration = new AudioProcessorHolder::ViewConfig { [config width], [config height], [config hostHasController] == YES };
    }
   #endif

    //==============================================================================
    void audioProcessorChanged (AudioProcessor* processor) override
    {
        ignoreUnused (processor);

        [au willChangeValueForKey: @"allParameterValues"];
        addPresets();
        [au didChangeValueForKey: @"allParameterValues"];
    }

    void audioProcessorParameterChanged (AudioProcessor*, int idx, float newValue) override
    {
        if (isPositiveAndBelow (idx, getAudioProcessor().getNumParameters()))
        {
            if (AUParameter* param = [paramTree parameterWithAddress: getAUParameterAddressForIndex (idx)])
            {
                if (editorObserverToken != nullptr)
                    [param setValue: newValue  originator: editorObserverToken];
                else
                    [param setValue: newValue];
            }
        }
    }

    //==============================================================================
    bool getCurrentPosition (CurrentPositionInfo& info) override
    {
        bool musicContextCallSucceeded = false;
        bool transportStateCallSucceeded = false;

        info = lastAudioHead;
        info.timeInSamples = (int64) (lastTimeStamp.mSampleTime + 0.5);
        info.timeInSeconds = info.timeInSamples / getAudioProcessor().getSampleRate();

        switch (lastTimeStamp.mSMPTETime.mType)
        {
            case kSMPTETimeType2398:        info.frameRate = AudioPlayHead::fps23976; break;
            case kSMPTETimeType24:          info.frameRate = AudioPlayHead::fps24; break;
            case kSMPTETimeType25:          info.frameRate = AudioPlayHead::fps25; break;
            case kSMPTETimeType2997:        info.frameRate = AudioPlayHead::fps2997; break;
            case kSMPTETimeType2997Drop:    info.frameRate = AudioPlayHead::fps2997drop; break;
            case kSMPTETimeType30Drop:      info.frameRate = AudioPlayHead::fps30drop; break;
            case kSMPTETimeType30:          info.frameRate = AudioPlayHead::fps30; break;
            case kSMPTETimeType60Drop:      info.frameRate = AudioPlayHead::fps60drop; break;
            case kSMPTETimeType60:          info.frameRate = AudioPlayHead::fps60; break;
            default:                        info.frameRate = AudioPlayHead::fpsUnknown; break;
        }

        double num;
        NSInteger den;
        NSInteger outDeltaSampleOffsetToNextBeat;
        double outCurrentMeasureDownBeat, bpm;
        double ppqPosition;

        if (hostMusicalContextCallback != nullptr)
        {
            AUHostMusicalContextBlock musicalContextCallback = hostMusicalContextCallback;

            if (musicalContextCallback (&bpm, &num, &den, &ppqPosition, &outDeltaSampleOffsetToNextBeat, &outCurrentMeasureDownBeat))
            {
                musicContextCallSucceeded = true;

                info.timeSigNumerator   = (int) num;
                info.timeSigDenominator = (int) den;
                info.ppqPositionOfLastBarStart = outCurrentMeasureDownBeat;
                info.bpm = bpm;
                info.ppqPosition = ppqPosition;
                info.ppqPositionOfLastBarStart = outCurrentMeasureDownBeat;
            }
        }

        double outCurrentSampleInTimeLine, outCycleStartBeat = 0, outCycleEndBeat = 0;
        AUHostTransportStateFlags flags;

        if (hostTransportStateCallback != nullptr)
        {
            AUHostTransportStateBlock transportStateCallback = hostTransportStateCallback;

            if (transportStateCallback (&flags, &outCurrentSampleInTimeLine, &outCycleStartBeat, &outCycleEndBeat))
            {
                transportStateCallSucceeded = true;

                info.timeInSamples  = (int64) (outCurrentSampleInTimeLine + 0.5);
                info.timeInSeconds  = info.timeInSamples / getAudioProcessor().getSampleRate();
                info.isPlaying      = ((flags & AUHostTransportStateMoving) != 0);
                info.isLooping      = ((flags & AUHostTransportStateCycling) != 0);
                info.isRecording    = ((flags & AUHostTransportStateRecording) != 0);
                info.ppqLoopStart   = outCycleStartBeat;
                info.ppqLoopEnd     = outCycleEndBeat;
            }
        }

        if (musicContextCallSucceeded && transportStateCallSucceeded)
            lastAudioHead = info;

        return true;
    }

    //==============================================================================
    static void removeEditor (AudioProcessor& processor)
    {
        ScopedLock editorLock (processor.getCallbackLock());

        if (AudioProcessorEditor* editor = processor.getActiveEditor())
        {
            processor.editorBeingDeleted (editor);
            delete editor;
        }
    }

private:
    //==============================================================================
    struct BusBuffer
    {
        BusBuffer (AUAudioUnitBus* bus, int maxFramesPerBuffer)
            : auBus (bus),
              maxFrames (maxFramesPerBuffer),
              numberOfChannels (static_cast<int> ([[auBus format] channelCount])),
              isInterleaved ([[auBus format] isInterleaved])
        {
            alloc();
        }

        //==============================================================================
        void alloc()
        {
            const int numBuffers = isInterleaved ? 1 : numberOfChannels;
            int bytes = static_cast<int> (sizeof (AudioBufferList))
                          + ((numBuffers - 1) * static_cast<int> (sizeof (::AudioBuffer)));
            jassert (bytes > 0);

            bufferListStorage.calloc (static_cast<size_t> (bytes));
            bufferList = reinterpret_cast<AudioBufferList*> (bufferListStorage.getData());

            const int bufferChannels = isInterleaved ? numberOfChannels : 1;
            scratchBuffer.setSize (numBuffers, bufferChannels * maxFrames);
        }

        void dealloc()
        {
            bufferList = nullptr;
            bufferListStorage.free();
            scratchBuffer.setSize (0, 0);
        }

        //==============================================================================
        int numChannels() const noexcept                { return numberOfChannels; }
        bool interleaved() const noexcept               { return isInterleaved; }
        AudioBufferList* get() const noexcept           { return bufferList; }

        //==============================================================================
        void prepare (UInt32 nFrames, const AudioBufferList* other = nullptr) noexcept
        {
            const int numBuffers = isInterleaved ? 1 : numberOfChannels;
            const bool isCompatible = isCompatibleWith (other);

            bufferList->mNumberBuffers = static_cast<UInt32> (numBuffers);

            for (int i = 0; i < numBuffers; ++i)
            {
                const UInt32 bufferChannels = static_cast<UInt32> (isInterleaved ? numberOfChannels : 1);
                bufferList->mBuffers[i].mNumberChannels = bufferChannels;
                bufferList->mBuffers[i].mData = (isCompatible ? other->mBuffers[i].mData
                                                              : scratchBuffer.getWritePointer (i));
                bufferList->mBuffers[i].mDataByteSize = nFrames * bufferChannels * sizeof (float);
            }
        }

        //==============================================================================
        bool isCompatibleWith (const AudioBufferList* other) const noexcept
        {
            if (other == nullptr)
                return false;

            if (other->mNumberBuffers > 0)
            {
                const bool otherInterleaved = AudioUnitHelpers::isAudioBufferInterleaved (*other);
                const int otherChannels = static_cast<int> (otherInterleaved ? other->mBuffers[0].mNumberChannels
                                                                             : other->mNumberBuffers);

                return otherInterleaved == isInterleaved
                    && numberOfChannels == otherChannels;
            }

            return numberOfChannels == 0;
        }

    private:
        AUAudioUnitBus* auBus;
        HeapBlock<char> bufferListStorage;
        AudioBufferList* bufferList = nullptr;
        int maxFrames, numberOfChannels;
        bool isInterleaved;
        AudioSampleBuffer scratchBuffer;
    };

    //==============================================================================
    void addAudioUnitBusses (bool isInput)
    {
        ScopedPointer<NSMutableArray<AUAudioUnitBus*> > array = [[NSMutableArray<AUAudioUnitBus*> alloc] init];
        AudioProcessor& processor = getAudioProcessor();
        const int n = AudioUnitHelpers::getBusCount (&processor, isInput);

        for (int i = 0; i < n; ++i)
        {
            ScopedPointer<AUAudioUnitBus> audioUnitBus;

            {
                ScopedPointer<AVAudioFormat> defaultFormat = [[AVAudioFormat alloc] initStandardFormatWithSampleRate: kDefaultSampleRate
                                                                                                            channels: static_cast<AVAudioChannelCount> (processor.getChannelCountOfBus (isInput, i))];

                audioUnitBus = [[AUAudioUnitBus alloc] initWithFormat: defaultFormat
                                                                error: nullptr];
            }

            [array addObject: audioUnitBus];
        }

        (isInput ? inputBusses : outputBusses) = [[AUAudioUnitBusArray alloc] initWithAudioUnit: au
                                                                                        busType: (isInput ? AUAudioUnitBusTypeInput : AUAudioUnitBusTypeOutput)
                                                                                         busses: array];
    }

    void addParameters()
    {
        ScopedPointer<NSMutableArray<AUParameterNode*> > params = [[NSMutableArray<AUParameterNode*> alloc] init];

        paramObserver = CreateObjCBlock (this, &JuceAudioUnitv3::valueChangedFromHost);
        paramProvider = CreateObjCBlock (this, &JuceAudioUnitv3::getValue);

        overviewParams = [[NSMutableArray<NSNumber*> alloc] init];

        auto& processor = getAudioProcessor();
        const int n = processor.getNumParameters();

       #if ! JUCE_FORCE_USE_LEGACY_PARAM_IDS
        // check if all parameters are managed?
        usingManagedParameter = (processor.getParameters().size() == processor.getNumParameters());
       #endif

        for (int idx = 0; idx < n; ++idx)
        {
            const String identifier (idx);
            const String name = processor.getParameterName (idx);

            AudioUnitParameterUnit unit = kAudioUnitParameterUnit_Generic;
            AudioUnitParameterOptions flags = (UInt32) (kAudioUnitParameterFlag_IsWritable
                                                      | kAudioUnitParameterFlag_IsReadable
                                                      | kAudioUnitParameterFlag_HasCFNameString
                                                      | kAudioUnitParameterFlag_ValuesHaveStrings);

           #if ! JUCE_FORCE_LEGACY_PARAMETER_AUTOMATION_TYPE
            flags |= (UInt32) kAudioUnitParameterFlag_IsHighResolution;
           #endif

            // set whether the param is automatable (unnamed parameters aren't allowed to be automated)
            if (name.isEmpty() || ! processor.isParameterAutomatable (idx))
                flags |= kAudioUnitParameterFlag_NonRealTime;

            if (processor.isMetaParameter (idx))
                flags |= kAudioUnitParameterFlag_IsGlobalMeta;

            // is this a meter?
            if (((processor.getParameterCategory (idx) & 0xffff0000) >> 16) == 2)
            {
                flags &= ~kAudioUnitParameterFlag_IsWritable;
                flags |= kAudioUnitParameterFlag_MeterReadOnly | kAudioUnitParameterFlag_DisplayLogarithmic;
                unit = kAudioUnitParameterUnit_LinearGain;
            }

           #if JUCE_FORCE_USE_LEGACY_PARAM_IDS
            AUParameterAddress address = static_cast<AUParameterAddress> (idx);
           #else
            AUParameterAddress address = generateAUParameterAddressForIndex (idx);

            // Consider yourself very unlucky if you hit this assertion. The hash code of your
            // parameter ids are not unique.
            jassert (! paramMap.contains (static_cast<int64> (address)));

            paramAddresses.add (address);
            paramMap.set (static_cast<int64> (address), idx);
          #endif

            // create methods in AUParameterTree return unretained objects (!) -> see Apple header AUAudioUnitImplementation.h

            ScopedPointer<AUParameter> param = [[AUParameterTree createParameterWithIdentifier: juceStringToNS (identifier)
                                                                                          name: juceStringToNS (name)
                                                                                       address: address
                                                                                           min: 0.0f
                                                                                           max: 1.0f
                                                                                          unit: unit
                                                                                      unitName: nullptr
                                                                                         flags: flags
                                                                                  valueStrings: nullptr
                                                                           dependentParameters: nullptr] retain];

            [params addObject: param];
            [overviewParams addObject: [NSNumber numberWithUnsignedLongLong:address]];
        }

        // create methods in AUParameterTree return unretained objects (!) -> see Apple header AUAudioUnitImplementation.h
        paramTree = [[AUParameterTree createTreeWithChildren: params] retain];

        [paramTree setImplementorValueObserver: paramObserver];
        [paramTree setImplementorValueProvider: paramProvider];

        if (processor.hasEditor())
        {
            editorParamObserver = CreateObjCBlock (this, &JuceAudioUnitv3::valueChangedForObserver);
            editorObserverToken = [paramTree tokenByAddingParameterObserver: editorParamObserver];
        }
    }

    void addPresets()
    {
        factoryPresets = [[NSMutableArray<AUAudioUnitPreset*> alloc] init];

        const int n = getAudioProcessor().getNumPrograms();

        for (int idx = 0; idx < n; ++idx)
        {
            String name = getAudioProcessor().getProgramName (idx);

            ScopedPointer<AUAudioUnitPreset> preset = [[AUAudioUnitPreset alloc] init];
            [preset setName: juceStringToNS (name)];
            [preset setNumber: static_cast<NSInteger> (idx)];

            [factoryPresets addObject: preset];
        }
    }

    //==============================================================================
    void allocateBusBuffer (bool isInput)
    {
        OwnedArray<BusBuffer>& busBuffers = isInput ? inBusBuffers : outBusBuffers;
        busBuffers.clear();

        const int n = AudioUnitHelpers::getBusCount (&getAudioProcessor(), isInput);
        const AUAudioFrameCount maxFrames = [getAudioUnit() maximumFramesToRender];

        for (int busIdx = 0; busIdx < n; ++busIdx)
            busBuffers.add (new BusBuffer ([(isInput ? inputBusses : outputBusses) objectAtIndexedSubscript: static_cast<unsigned int> (busIdx)],
                                           static_cast<int> (maxFrames)));
    }

    //==============================================================================
    void processEvents (const AURenderEvent *__nullable realtimeEventListHead, int numParams, AUEventSampleTime startTime)
    {
        for (const AURenderEvent* event = realtimeEventListHead; event != nullptr; event = event->head.next)
        {
            switch (event->head.eventType)
            {
                case AURenderEventMIDI:
                {
                    const AUMIDIEvent& midiEvent = event->MIDI;
                    midiMessages.addEvent (midiEvent.data, midiEvent.length, static_cast<int> (midiEvent.eventSampleTime - startTime));
                }
                break;

                case AURenderEventParameter:
                case AURenderEventParameterRamp:
                {
                    const AUParameterEvent& paramEvent = event->parameter;
                    const int idx = getJuceParameterIndexForAUAddress (paramEvent.parameterAddress);

                    if (isPositiveAndBelow (idx, numParams))
                        getAudioProcessor().setParameter (idx, paramEvent.value);
                }
                break;

                default:
                    break;
            }
        }
    }

    AUAudioUnitStatus renderCallback (AudioUnitRenderActionFlags* actionFlags, const AudioTimeStamp* timestamp, AUAudioFrameCount frameCount,
                                      NSInteger outputBusNumber, AudioBufferList* outputData, const AURenderEvent *__nullable realtimeEventListHead,
                                      AURenderPullInputBlock __nullable pullInputBlock)
    {
        auto& processor = getAudioProcessor();
        jassert (static_cast<int> (frameCount) <= getAudioProcessor().getBlockSize());

        // process params
        const int numParams = processor.getNumParameters();
        processEvents (realtimeEventListHead, numParams, static_cast<AUEventSampleTime> (timestamp->mSampleTime));

        if (lastTimeStamp.mSampleTime != timestamp->mSampleTime)
        {
            lastTimeStamp = *timestamp;

            const int numInputBuses  = inBusBuffers. size();
            const int numOutputBuses = outBusBuffers.size();

            // prepare buffers
            {
                for (int busIdx = 0; busIdx < numOutputBuses; ++busIdx)
                {
                     BusBuffer& busBuffer = *outBusBuffers[busIdx];
                     const bool canUseDirectOutput =
                         (busIdx == outputBusNumber && outputData != nullptr && outputData->mNumberBuffers > 0);

                    busBuffer.prepare (frameCount, canUseDirectOutput ? outputData : nullptr);
                }

                for (int busIdx = 0; busIdx < numInputBuses; ++busIdx)
                {
                    BusBuffer& busBuffer = *inBusBuffers[busIdx];
                    busBuffer.prepare (frameCount, busIdx < numOutputBuses ? outBusBuffers[busIdx]->get() : nullptr);
                }

                audioBuffer.reset();
            }

            // pull inputs
            {
                for (int busIdx = 0; busIdx < numInputBuses; ++busIdx)
                {
                    BusBuffer& busBuffer = *inBusBuffers[busIdx];
                    AudioBufferList* buffer = busBuffer.get();

                    if (pullInputBlock == nullptr || pullInputBlock (actionFlags, timestamp, frameCount, busIdx, buffer) != noErr)
                        AudioUnitHelpers::clearAudioBuffer (*buffer);

                    if (actionFlags != nullptr && (*actionFlags & kAudioUnitRenderAction_OutputIsSilence) != 0)
                        AudioUnitHelpers::clearAudioBuffer (*buffer);
                }
            }

            // set buffer pointer to minimize copying
            {
                int chIdx = 0;

                for (int busIdx = 0; busIdx < numOutputBuses; ++busIdx)
                {
                    BusBuffer& busBuffer = *outBusBuffers[busIdx];
                    AudioBufferList* buffer = busBuffer.get();

                    const bool interleaved = busBuffer.interleaved();
                    const int numChannels = busBuffer.numChannels();

                    const int* outLayoutMap = mapper.get (false, busIdx);

                    for (int ch = 0; ch < numChannels; ++ch)
                        audioBuffer.setBuffer (chIdx++, interleaved ? nullptr : static_cast<float*> (buffer->mBuffers[outLayoutMap[ch]].mData));
                }

                // use input pointers on remaining channels

                for (int busIdx = 0; chIdx < totalInChannels;)
                {
                    const int channelOffset = processor.getOffsetInBusBufferForAbsoluteChannelIndex (true, chIdx, busIdx);

                    BusBuffer& busBuffer = *inBusBuffers[busIdx];
                    AudioBufferList* buffer = busBuffer.get();

                    const int* inLayoutMap = mapper.get (true, busIdx);
                    audioBuffer.setBuffer (chIdx++,  busBuffer.interleaved() ? nullptr : static_cast<float*> (buffer->mBuffers[inLayoutMap[channelOffset]].mData));
                }
            }

            // copy input
            {
                for (int busIdx = 0; busIdx < numInputBuses; ++busIdx)
                    audioBuffer.push (*inBusBuffers[busIdx]->get(), mapper.get (true, busIdx));

                // clear remaining channels
                for (int i = totalInChannels; i < totalOutChannels; ++i)
                    zeromem (audioBuffer.push(), sizeof (float) * frameCount);
            }

            // process audio
            processBlock (audioBuffer.getBuffer (frameCount), midiMessages);

            // send MIDI
           #if JucePlugin_ProducesMidiOutput && JUCE_AUV3_MIDI_OUTPUT_SUPPORTED
            auto midiOut = [au MIDIOutputEventBlock];
            MidiMessage msg;
            int samplePosition;

            for (MidiBuffer::Iterator it (midiMessages); it.getNextEvent (msg, samplePosition);)
                midiOut (samplePosition, 0, msg.getRawDataSize(), msg.getRawData());
           #endif

            midiMessages.clear();
        }

        // copy back
        audioBuffer.pop (*outBusBuffers[(int) outputBusNumber]->get(),
                         mapper.get (false, (int) outputBusNumber));

        return noErr;
    }

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiBuffer) noexcept
    {
        auto& processor = getAudioProcessor();
        const ScopedLock sl (processor.getCallbackLock());

        if (processor.isSuspended())
            buffer.clear();
        else if ([au shouldBypassEffect])
            processor.processBlockBypassed (buffer, midiBuffer);
        else
            processor.processBlock (buffer, midiBuffer);
    }

    //==============================================================================
    void valueChangedFromHost (AUParameter* param, AUValue value)
    {
        if (param != nullptr)
        {
            const int idx = getJuceParameterIndexForAUAddress ([param address]);
            auto& processor = getAudioProcessor();

            if (isPositiveAndBelow (idx, processor.getNumParameters()))
                processor.setParameter (idx, value);
        }
    }

    AUValue getValue (AUParameter* param)
    {
        if (param != nullptr)
        {
            const int idx = getJuceParameterIndexForAUAddress ([param address]);
            auto& processor = getAudioProcessor();

            if (isPositiveAndBelow (idx, processor.getNumParameters()))
                return processor.getParameter (idx);
        }

        return 0;
    }

    void valueChangedForObserver(AUParameterAddress, AUValue)
    {
        // this will have already been handled by valueChangedFromHost
    }

    //==============================================================================
   #if JUCE_FORCE_USE_LEGACY_PARAM_IDS
    inline AUParameterAddress getAUParameterAddressForIndex (int paramIndex) const noexcept     { return static_cast<AUParameterAddress> (paramIndex); }
    inline int getJuceParameterIndexForAUAddress (AUParameterAddress address) const noexcept    { return static_cast<int> (address); }
   #else
    AUParameterAddress generateAUParameterAddressForIndex (int paramIndex) const
    {
        auto& processor = getAudioProcessor();
        const int n = processor.getNumParameters();

        if (isPositiveAndBelow (paramIndex, n))
        {
            const String& juceParamID = processor.getParameterID (paramIndex);
            return usingManagedParameter ? static_cast<AUParameterAddress> (juceParamID.hashCode64())
                                         : static_cast<AUParameterAddress> (juceParamID.getIntValue());
        }

        return static_cast<AUParameterAddress> (-1);
    }

    inline AUParameterAddress getAUParameterAddressForIndex (int paramIndex) const noexcept
    {
        return usingManagedParameter ? paramAddresses.getReference (paramIndex)
                                     : static_cast<AUParameterAddress> (paramIndex);
    }

    inline int getJuceParameterIndexForAUAddress (AUParameterAddress address) const noexcept
    {
        return usingManagedParameter ? paramMap[static_cast<int64> (address)]
                                     : static_cast<int> (address);
    }
   #endif

    //==============================================================================
    static const double kDefaultSampleRate;

    AudioProcessorHolder::Ptr processorHolder;

    int totalInChannels, totalOutChannels;

    ScopedPointer<AUAudioUnitBusArray> inputBusses;
    ScopedPointer<AUAudioUnitBusArray> outputBusses;

    ObjCBlock<AUImplementorValueObserver> paramObserver;
    ObjCBlock<AUImplementorValueProvider> paramProvider;

   #if ! JUCE_FORCE_USE_LEGACY_PARAM_IDS
    bool usingManagedParameter;
    Array<AUParameterAddress> paramAddresses;
    HashMap<int64, int> paramMap;
   #endif

    // to avoid recursion on parameter changes, we need to add an
    // editor observer to do the parameter changes
    ObjCBlock<AUParameterObserver> editorParamObserver;
    AUParameterObserverToken editorObserverToken;

    ScopedPointer<AUParameterTree> paramTree;
    ScopedPointer<NSMutableArray<NSNumber*>> overviewParams;
    ScopedPointer<NSMutableArray<NSNumber*>> channelCapabilities;

    ScopedPointer<NSMutableArray<AUAudioUnitPreset*> > factoryPresets;

    ObjCBlock<AUInternalRenderBlock> internalRenderBlock;

    AudioUnitHelpers::CoreAudioBufferList audioBuffer;
    AudioUnitHelpers::ChannelRemapper mapper;

    OwnedArray<BusBuffer> inBusBuffers, outBusBuffers;
    MidiBuffer midiMessages;

    ObjCBlock<AUHostMusicalContextBlock> hostMusicalContextCallback;
    ObjCBlock<AUHostTransportStateBlock> hostTransportStateCallback;

    AudioTimeStamp lastTimeStamp;
    CurrentPositionInfo lastAudioHead;

    String contextName;
};

const double JuceAudioUnitv3::kDefaultSampleRate = 44100.0;

JuceAudioUnitv3Base* JuceAudioUnitv3Base::create (AUAudioUnit* audioUnit, AudioComponentDescription descr, AudioComponentInstantiationOptions options, NSError** error)
{
    PluginHostType::jucePlugInClientCurrentWrapperType = AudioProcessor::wrapperType_AudioUnitv3;
    return new JuceAudioUnitv3 (audioUnit, descr, options, error);
}

//==============================================================================
class JuceAUViewController
{
public:
    JuceAUViewController (AUViewController<AUAudioUnitFactory>* p)
        : myself (p)
    {
        jassert (MessageManager::getInstance()->isThisTheMessageThread());

        PluginHostType::jucePlugInClientCurrentWrapperType = AudioProcessor::wrapperType_AudioUnitv3;
        initialiseJuce_GUI();
    }

    ~JuceAUViewController()
    {
        jassert (MessageManager::getInstance()->isThisTheMessageThread());

        if (processorHolder != nullptr)
            JuceAudioUnitv3::removeEditor (getAudioProcessor());
    }

    //==============================================================================
    void loadView()
    {
        jassert (MessageManager::getInstance()->isThisTheMessageThread());

        if (AudioProcessor* p = createPluginFilterOfType (AudioProcessor::wrapperType_AudioUnitv3))
        {
            processorHolder = new AudioProcessorHolder (p);
            auto& processor = getAudioProcessor();

            if (processor.hasEditor())
            {
                if (AudioProcessorEditor* editor = processor.createEditorIfNeeded())
                {
                    preferredSize = editor->getBounds();

                    JUCE_IOS_MAC_VIEW* view = [[[JUCE_IOS_MAC_VIEW alloc] initWithFrame: convertToCGRect (editor->getBounds())] autorelease];
                    [myself setView: view];

                    editor->setVisible (true);
                    editor->addToDesktop (0, view);
                }
            }
        }
    }

    void viewDidLayoutSubviews()
    {
        if (processorHolder != nullptr && [myself view] != nullptr)
        {
            if (AudioProcessorEditor* editor = getAudioProcessor().getActiveEditor())
            {
                if (processorHolder->viewConfiguration != nullptr)
                    editor->hostMIDIControllerIsAvailable (processorHolder->viewConfiguration->hostHasMIDIController);

                editor->setBounds (convertToRectInt ([[myself view] bounds]));

                if (JUCE_IOS_MAC_VIEW* peerView = [[[myself view] subviews] objectAtIndex: 0])
                {
                   #if JUCE_IOS
                    [peerView setNeedsDisplay];
                   #else
                    [peerView setNeedsDisplay: YES];
                   #endif
                }
            }
        }
    }

    CGSize getPreferredContentSize() const
    {
        return CGSizeMake (static_cast<float> (preferredSize.getWidth()),
                           static_cast<float> (preferredSize.getHeight()));
    }

    //==============================================================================
    AUAudioUnit* createAudioUnit (const AudioComponentDescription& descr, NSError** error)
    {
        AUAudioUnit* retval = nil;

        if (! MessageManager::getInstance()->isThisTheMessageThread())
        {
            WaitableEvent creationEvent;

            // AUv3 headers say that we may block this thread and that the message thread is guaranteed
            // to be unblocked
            struct AUCreator  : public CallbackMessage
            {
                JuceAUViewController& owner;
                AudioComponentDescription pDescr;
                NSError** pError;
                AUAudioUnit*& outAU;
                WaitableEvent& e;

                AUCreator (JuceAUViewController& parent, const AudioComponentDescription& paramDescr, NSError** paramError,
                           AUAudioUnit*& outputAU, WaitableEvent& event)
                    : owner (parent), pDescr (paramDescr), pError (paramError), outAU (outputAU), e (event)
                {}

                void messageCallback() override
                {
                    outAU = owner.createAudioUnitOnMessageThread (pDescr, pError);
                    e.signal();
                }
            };

            (new AUCreator (*this, descr, error, retval, creationEvent))->post();
            creationEvent.wait (-1);
        }
        else
            retval = createAudioUnitOnMessageThread (descr, error);

        return [retval autorelease];
    }

private:
    //==============================================================================
    AUViewController<AUAudioUnitFactory>* myself;
    AudioProcessorHolder::Ptr processorHolder = nullptr;
    Rectangle<int> preferredSize { 1, 1 };

    //==============================================================================
    AUAudioUnit* createAudioUnitOnMessageThread (const AudioComponentDescription& descr, NSError** error)
    {
        jassert (MessageManager::getInstance()->isThisTheMessageThread());

        [myself view];  // this will call [view load] and ensure that the AudioProcessor has been instantiated

        if (processorHolder == nullptr)
            return nullptr;

        return (new JuceAudioUnitv3 (processorHolder, descr, 0, error))->getAudioUnit();
    }

    AudioProcessor& getAudioProcessor() const noexcept       { return **processorHolder; }
};

//==============================================================================
// necessary glue code
@interface JUCE_VIEWCONTROLLER_OBJC_NAME (JucePlugin_AUExportPrefix) : AUViewController<AUAudioUnitFactory>
@end

@implementation JUCE_VIEWCONTROLLER_OBJC_NAME (JucePlugin_AUExportPrefix)
{
    ScopedPointer<JuceAUViewController> cpp;
}

- (instancetype) initWithNibName: (nullable NSString*) nib bundle: (nullable NSBundle*) bndl { self = [super initWithNibName: nib bundle: bndl]; cpp = new JuceAUViewController (self); return self;}
- (void) loadView               { cpp->loadView(); }
- (AUAudioUnit *)createAudioUnitWithComponentDescription:(AudioComponentDescription)desc error:(NSError **)error { return cpp->createAudioUnit (desc, error); }
- (CGSize) preferredContentSize { return cpp->getPreferredContentSize(); }
- (void)viewDidLayoutSubviews   { return cpp->viewDidLayoutSubviews(); }
@end

//==============================================================================
#if JUCE_IOS
bool JUCE_CALLTYPE juce_isInterAppAudioConnected() { return false; }
void JUCE_CALLTYPE juce_switchToHostApplication()  {}
#if JUCE_MODULE_AVAILABLE_juce_gui_basics
Image JUCE_CALLTYPE juce_getIAAHostIcon (int)      { return Image(); }
#endif
#endif

#pragma clang diagnostic pop
#endif
