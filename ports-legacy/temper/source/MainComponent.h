/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.3.0

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_9002020A4DD09B20__
#define __JUCE_HEADER_9002020A4DD09B20__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainComponent  : public Component
{
public:
    //==============================================================================
    MainComponent (AudioProcessorValueTreeState& vts);
    ~MainComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    AudioProcessorValueTreeState& m_vts;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> filterFreqAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> filterResoAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> satAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> curveAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> feedbackAttachment;
    ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> levelAttachment;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> m_cutoffSlider;
    ScopedPointer<Slider> m_resoSlider;
    ScopedPointer<Slider> m_driveSlider;
    ScopedPointer<Slider> m_curveSlider;
    ScopedPointer<Slider> m_satSlider;
    ScopedPointer<Slider> m_feedbackSlider;
    ScopedPointer<Slider> m_gainSlider;
    ScopedPointer<Label> m_cutoffLabel;
    ScopedPointer<Label> m_resoLabel;
    ScopedPointer<Label> m_driveLabel;
    ScopedPointer<Label> m_curveLabel;
    ScopedPointer<Label> m_satLabel;
    ScopedPointer<Label> m_feedbackLabel;
    ScopedPointer<Label> m_gainLabel;
    ScopedPointer<Drawable> drawable1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_9002020A4DD09B20__
