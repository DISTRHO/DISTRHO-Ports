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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "MainComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
//[/MiscUserDefs]

//==============================================================================
MainComponent::MainComponent (AudioProcessorValueTreeState& vts)
    : m_vts(vts)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (m_cutoffSlider = new Slider ("Cutoff"));
    m_cutoffSlider->setRange (100, 20000, 0);
    m_cutoffSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    m_cutoffSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);

    addAndMakeVisible (m_resoSlider = new Slider ("Resonance"));
    m_resoSlider->setRange (1, 8, 0);
    m_resoSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    m_resoSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);

    addAndMakeVisible (m_driveSlider = new Slider ("Drive"));
    m_driveSlider->setRange (-10, 10, 0);
    m_driveSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    m_driveSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);

    addAndMakeVisible (m_curveSlider = new Slider ("Curve"));
    m_curveSlider->setRange (0.1, 4, 0);
    m_curveSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    m_curveSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);

    addAndMakeVisible (m_satSlider = new Slider ("Saturation"));
    m_satSlider->setRange (0, 1, 0);
    m_satSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    m_satSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);

    addAndMakeVisible (m_feedbackSlider = new Slider ("Feedback"));
    m_feedbackSlider->setRange (-60, -24, 0);
    m_feedbackSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    m_feedbackSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);

    addAndMakeVisible (m_gainSlider = new Slider ("Level"));
    m_gainSlider->setRange (-24, 24, 0);
    m_gainSlider->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    m_gainSlider->setTextBoxStyle (Slider::NoTextBox, true, 80, 20);

    addAndMakeVisible (m_cutoffLabel = new Label ("Cutoff Label",
                                                  TRANS("CUTOFF")));
    m_cutoffLabel->setFont (Font (15.00f, Font::plain));
    m_cutoffLabel->setJustificationType (Justification::centred);
    m_cutoffLabel->setEditable (false, false, false);
    m_cutoffLabel->setColour (Label::textColourId, Colour (0xffff8917));
    m_cutoffLabel->setColour (TextEditor::textColourId, Colours::black);
    m_cutoffLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (m_resoLabel = new Label ("Resonance Label",
                                                TRANS("RESONANCE")));
    m_resoLabel->setFont (Font (15.00f, Font::plain));
    m_resoLabel->setJustificationType (Justification::centred);
    m_resoLabel->setEditable (false, false, false);
    m_resoLabel->setColour (Label::textColourId, Colour (0xffff8917));
    m_resoLabel->setColour (TextEditor::textColourId, Colours::black);
    m_resoLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (m_driveLabel = new Label ("Drive Label",
                                                 TRANS("DRIVE")));
    m_driveLabel->setFont (Font (15.00f, Font::plain));
    m_driveLabel->setJustificationType (Justification::centred);
    m_driveLabel->setEditable (false, false, false);
    m_driveLabel->setColour (Label::textColourId, Colour (0xffff8917));
    m_driveLabel->setColour (TextEditor::textColourId, Colours::black);
    m_driveLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (m_curveLabel = new Label ("Curve Label",
                                                 TRANS("CURVE")));
    m_curveLabel->setFont (Font (15.00f, Font::plain));
    m_curveLabel->setJustificationType (Justification::centred);
    m_curveLabel->setEditable (false, false, false);
    m_curveLabel->setColour (Label::textColourId, Colour (0xffff8917));
    m_curveLabel->setColour (TextEditor::textColourId, Colours::black);
    m_curveLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (m_satLabel = new Label ("Saturation Label",
                                               TRANS("SATURATION")));
    m_satLabel->setFont (Font (15.00f, Font::plain));
    m_satLabel->setJustificationType (Justification::centred);
    m_satLabel->setEditable (false, false, false);
    m_satLabel->setColour (Label::textColourId, Colour (0xffff8917));
    m_satLabel->setColour (TextEditor::textColourId, Colours::black);
    m_satLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (m_feedbackLabel = new Label ("Feedback Label",
                                                    TRANS("FEEDBACK")));
    m_feedbackLabel->setFont (Font (15.00f, Font::plain));
    m_feedbackLabel->setJustificationType (Justification::centred);
    m_feedbackLabel->setEditable (false, false, false);
    m_feedbackLabel->setColour (Label::textColourId, Colour (0xffff8917));
    m_feedbackLabel->setColour (TextEditor::textColourId, Colours::black);
    m_feedbackLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (m_gainLabel = new Label ("Gain Label",
                                                TRANS("LEVEL")));
    m_gainLabel->setFont (Font (15.00f, Font::plain));
    m_gainLabel->setJustificationType (Justification::centred);
    m_gainLabel->setEditable (false, false, false);
    m_gainLabel->setColour (Label::textColourId, Colour (0xffff8917));
    m_gainLabel->setColour (TextEditor::textColourId, Colours::black);
    m_gainLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    drawable1 = Drawable::createFromImageData (BinaryData::Background_png, BinaryData::Background_pngSize);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (744, 476);


    //[Constructor] You can add your own custom stuff here..
    filterFreqAttachment = new SliderAttachment(m_vts, "Cutoff", *m_cutoffSlider);
    filterResoAttachment = new SliderAttachment(m_vts, "Resonance", *m_resoSlider);
    driveAttachment = new SliderAttachment(m_vts, "Drive", *m_driveSlider);
    curveAttachment = new SliderAttachment(m_vts, "Curve", *m_curveSlider);
    satAttachment = new SliderAttachment(m_vts, "Saturation", *m_satSlider);
    feedbackAttachment = new SliderAttachment(m_vts, "Feedback", *m_feedbackSlider);
    levelAttachment = new SliderAttachment(m_vts, "Level", *m_gainSlider);

    m_cutoffSlider->setTextValueSuffix("Hz");
    m_cutoffSlider->setSkewFactorFromMidPoint(800.0);
    m_feedbackSlider->setTextValueSuffix("dB");
    m_gainSlider->setTextValueSuffix("dB");
    //[/Constructor]
}

MainComponent::~MainComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    // This has to happen before we null out the Sliders themselves because
    // when the ScopedPointer<> delete policy tries to remove Listeners from
    // the SliderAttachment it refers to the Sliders themselves. If the Sliders
    // have already been deleted at that point then we get a null pointer error.
    filterFreqAttachment = nullptr;
    filterResoAttachment = nullptr;
    driveAttachment = nullptr;
    curveAttachment = nullptr;
    satAttachment = nullptr;
    feedbackAttachment = nullptr;
    levelAttachment = nullptr;
    //[/Destructor_pre]

    m_cutoffSlider = nullptr;
    m_resoSlider = nullptr;
    m_driveSlider = nullptr;
    m_curveSlider = nullptr;
    m_satSlider = nullptr;
    m_feedbackSlider = nullptr;
    m_gainSlider = nullptr;
    m_cutoffLabel = nullptr;
    m_resoLabel = nullptr;
    m_driveLabel = nullptr;
    m_curveLabel = nullptr;
    m_satLabel = nullptr;
    m_feedbackLabel = nullptr;
    m_gainLabel = nullptr;
    drawable1 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.setColour (Colours::black);
    jassert (drawable1 != 0);
    if (drawable1 != 0)
        drawable1->drawWithin (g, Rectangle<float> (0, 0, 744, 476),
                               RectanglePlacement::stretchToFit, 1.000f);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MainComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    m_cutoffSlider->setBounds (28, 149, 72, 72);
    m_resoSlider->setBounds (28, 277, 72, 72);
    m_driveSlider->setBounds (336, 357, 72, 72);
    m_curveSlider->setBounds (219, 370, 50, 50);
    m_satSlider->setBounds (475, 370, 50, 50);
    m_feedbackSlider->setBounds (640, 148, 72, 72);
    m_gainSlider->setBounds (640, 277, 72, 72);
    m_cutoffLabel->setBounds (24, 111, 80, 20);
    m_resoLabel->setBounds (24, 369, 80, 20);
    m_driveLabel->setBounds (332, 450, 80, 20);
    m_curveLabel->setBounds (204, 440, 80, 20);
    m_satLabel->setBounds (462, 440, 80, 20);
    m_feedbackLabel->setBounds (637, 111, 80, 20);
    m_gainLabel->setBounds (639, 369, 80, 20);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainComponent" componentName=""
                 parentClasses="public Component" constructorParams="AudioProcessorValueTreeState&amp; vts"
                 variableInitialisers="m_vts(vts)" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="744"
                 initialHeight="476">
  <BACKGROUND backgroundColour="ffffff">
    <IMAGE pos="0 0 744 476" resource="BinaryData::Background_png" opacity="1"
           mode="0"/>
  </BACKGROUND>
  <SLIDER name="Cutoff" id="80edd5c38c704dd5" memberName="m_cutoffSlider"
          virtualName="" explicitFocusOrder="0" pos="28 149 72 72" min="100"
          max="20000" int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="0"/>
  <SLIDER name="Resonance" id="3bb5cf0ab68e9733" memberName="m_resoSlider"
          virtualName="" explicitFocusOrder="0" pos="28 277 72 72" min="1"
          max="8" int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="0"/>
  <SLIDER name="Drive" id="9909e31099819864" memberName="m_driveSlider"
          virtualName="" explicitFocusOrder="0" pos="336 357 72 72" min="-10"
          max="10" int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="0"/>
  <SLIDER name="Curve" id="a56ea73f883bdf8f" memberName="m_curveSlider"
          virtualName="" explicitFocusOrder="0" pos="219 370 50 50" min="0.10000000000000000555"
          max="4" int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="0"/>
  <SLIDER name="Saturation" id="a1e434d9a7eda8a0" memberName="m_satSlider"
          virtualName="" explicitFocusOrder="0" pos="475 370 50 50" min="0"
          max="1" int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="0"/>
  <SLIDER name="Feedback" id="ecd475fce33f4b83" memberName="m_feedbackSlider"
          virtualName="" explicitFocusOrder="0" pos="640 148 72 72" min="-60"
          max="-24" int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="0"/>
  <SLIDER name="Level" id="bf47d1ef820213ea" memberName="m_gainSlider"
          virtualName="" explicitFocusOrder="0" pos="640 277 72 72" min="-24"
          max="24" int="0" style="RotaryHorizontalVerticalDrag" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="80" textBoxHeight="20" skewFactor="1"
          needsCallback="0"/>
  <LABEL name="Cutoff Label" id="8c36484c7dd57b99" memberName="m_cutoffLabel"
         virtualName="" explicitFocusOrder="0" pos="24 111 80 20" textCol="ffff8917"
         edTextCol="ff000000" edBkgCol="0" labelText="CUTOFF" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
  <LABEL name="Resonance Label" id="cc33e12b86f2a86a" memberName="m_resoLabel"
         virtualName="" explicitFocusOrder="0" pos="24 369 80 20" textCol="ffff8917"
         edTextCol="ff000000" edBkgCol="0" labelText="RESONANCE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
  <LABEL name="Drive Label" id="195ded46976233cb" memberName="m_driveLabel"
         virtualName="" explicitFocusOrder="0" pos="332 450 80 20" textCol="ffff8917"
         edTextCol="ff000000" edBkgCol="0" labelText="DRIVE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
  <LABEL name="Curve Label" id="30073a37efa500cc" memberName="m_curveLabel"
         virtualName="" explicitFocusOrder="0" pos="204 440 80 20" textCol="ffff8917"
         edTextCol="ff000000" edBkgCol="0" labelText="CURVE" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
  <LABEL name="Saturation Label" id="e8a8527f3f6976cf" memberName="m_satLabel"
         virtualName="" explicitFocusOrder="0" pos="462 440 80 20" textCol="ffff8917"
         edTextCol="ff000000" edBkgCol="0" labelText="SATURATION" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
  <LABEL name="Feedback Label" id="5547950683b7ca1f" memberName="m_feedbackLabel"
         virtualName="" explicitFocusOrder="0" pos="637 111 80 20" textCol="ffff8917"
         edTextCol="ff000000" edBkgCol="0" labelText="FEEDBACK" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
  <LABEL name="Gain Label" id="719d91e31d43fa34" memberName="m_gainLabel"
         virtualName="" explicitFocusOrder="0" pos="639 369 80 20" textCol="ffff8917"
         edTextCol="ff000000" edBkgCol="0" labelText="LEVEL" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
