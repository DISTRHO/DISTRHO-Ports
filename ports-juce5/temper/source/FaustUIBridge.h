/*
  ==============================================================================

    FaustUIBridge.h
    Created: 9 Feb 2017 5:59:55pm
    Author:  Nick Thompson

  ==============================================================================
*/

#ifndef FAUSTUIBRIDGE_H_INCLUDED
#define FAUSTUIBRIDGE_H_INCLUDED

#include "JuceHeader.h"

#include "faust/gui/UI.h"

//==============================================================================
/**
 This class interfaces with the Faust UI paradigm, mapping control parameters
 into AudioProcessorParameters on a provided AudioProcessorValueTreeState.
 */
class FaustUIBridge : public UI
{
public:

    /** Construct a FaustUIBridge instance with the associated AudioProcessorValueTreeState. */
    FaustUIBridge(AudioProcessorValueTreeState& vts);

    /** Destructor. */
    virtual ~FaustUIBridge() override;

    //==============================================================================
    /** Widget Layout

        The following methods implement the widget layout functions.
        Because we intend for an AudioProcessorEditor to manage the
        visual aspect of the plugin, these methods are no-ops.
    */
    virtual void openTabBox(const char* label) override {};
    virtual void openHorizontalBox(const char* label) override {};
    virtual void openVerticalBox(const char* label) override {};
    virtual void closeBox() override {};

    //==============================================================================
    /** Active Widgets

        The following methods implement the active widget functions
        by constructing an AudioProcessorParameter and installing a
        FaustUIBridgeListener to propagate changes back to the corresponding
        zone.
    */
    virtual void addButton(const char* label, FAUSTFLOAT* zone) override {};
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) override {};
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override {};
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override;
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) override {};

    //==============================================================================
    /** Passive Widgets

        The following methods implement the passive widget functions.
        Again, because we intend for an AudioProcessorEditor to manage the
        visual aspect of the plugin, these methods are no-ops.
    */
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) override {};
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) override {};

    //==============================================================================
    /** Metadata declarations

        An additional no-op to implement the last piece of the Faust UI interface.
    */
    virtual void declare(FAUSTFLOAT*, const char*, const char*) override {};

    //==============================================================================
    /** This class implements a simple callback which reacts to a value change on a
        given AudioProcessorParameter.

        We construct an instance of this class for each Faust zone declared via the
        active widget methods above.
     */
    class FaustUIBridgeListener : public AudioProcessorValueTreeState::Listener
    {
    public:
        FaustUIBridgeListener(float* zone) : m_zone(zone) {};
        virtual ~FaustUIBridgeListener() override {};

        virtual void parameterChanged (const String& parameterID, float newValue) override
        {
            *m_zone = newValue;
        };

    private:
        float* m_zone;
    };

private:
    //==============================================================================
    /**
     This struct associates an AudioProcessorParameter id with a FaustUIBridgeListener
     object which as been attached to the valueTreeState.
     */
    struct ParameterListenerPair
    {
        //==============================================================================
        /** Constructor.

         @param paramId        The String Id of the AudioProcessorParameter.
         @param listener       The attached FaustUIBridgeListener.
         */
        ParameterListenerPair(String paramId, FaustUIBridgeListener* listener)
        : paramId(paramId), listener(listener) {};

        String paramId;
        FaustUIBridgeListener* listener;
    };

    // A reference to the AudioProcessor's value tree so that we can map the faust
    // UI parameters to AudioProcessor parameters.
    AudioProcessorValueTreeState& valueTreeState;

    // Maintain an array associating AudioProcessorParameters to the Listeners that have been
    // installed on those parameters.
    Array<ParameterListenerPair> listenerAssignments;

    // And an array of listeners to ensure the mapping between internal value tree values
    // match the float* zone members of the faust implementation.
    OwnedArray<AudioProcessorValueTreeState::Listener> listeners;
};

#endif  // FAUSTUIBRIDGE_H_INCLUDED
