/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "JuceHeader.h"
#include "TemperLookAndFeel.h"
#include "PluginProcessor.h"
#include "MainComponent.h"
#include "SpectroscopeComponent.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

//==============================================================================
/**
*/
class TemperAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    TemperAudioProcessorEditor (TemperAudioProcessor&, AudioProcessorValueTreeState&);
    ~TemperAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    ScopedPointer<SpectroscopeComponent> m_vizPre;
    ScopedPointer<SpectroscopeComponent> m_vizPost;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TemperAudioProcessor& processor;
    TemperLookAndFeel laf;

    OpenGLContext m_glContext;
    AudioProcessorValueTreeState& m_vts;

    ScopedPointer<MainComponent> m_main;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TemperAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
