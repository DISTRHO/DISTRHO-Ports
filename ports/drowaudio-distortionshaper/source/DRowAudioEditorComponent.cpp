/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#include "DRowAudioEditorComponent.h"

//==============================================================================
DRowAudioEditorComponent::DRowAudioEditorComponent (DRowAudioFilter* const ownerFilter)
    : AudioProcessorEditor (ownerFilter)
{
	// set up the custom look and feel
	lookAndFeel = new dRowLookAndFeel();
	setLookAndFeel(lookAndFeel);
	lookAndFeel->setColour(Label::textColourId, (Colours::black).withBrightness(0.9f));
	lookAndFeel->setColour (Slider::thumbColourId, Colours::grey);
	lookAndFeel->setColour (Slider::textBoxTextColourId, Colour (0xff78f4ff));
	lookAndFeel->setColour (Slider::textBoxBackgroundColourId, Colours::black);
	lookAndFeel->setColour (Slider::textBoxOutlineColourId, Colour (0xff0D2474));
	
	for (int i = 0; i < noParams; i++) {
		values.add(new Value);
		values[i]->addListener(this);
	}
	
	// set up the sliders and labels based on their parameters
	for (int i = 0; i < X1; i++)
	{
		sliders.add( new Slider(String("param") + String(i)) );
		addAndMakeVisible( sliders[i]);
		sliders[i]->getValueObject().referTo(*values[i]);
		
		String labelName = parameterNames[i];
		labels.add(new Label(String("Label") + String(i), labelName));
		labels[i]->setJustificationType(Justification::topLeft);
		labels[i]->attachToComponent(sliders[i], false);
		labels[i]->setFont(12);
			
		sliders[i]->addListener (this);
		sliders[i]->setRange (0.0, 1.0, 0.01);
		sliders[i]->setTextBoxStyle(Slider::TextBoxRight, false, 50, 20);
		sliders[i]->setValue (ownerFilter->getParameter (i), dontSendNotification);
		
		ownerFilter->getParameterPointer(i)->setupSlider(*sliders[i]);
	}

	
	// add a few labels for the section headings
//	labels.add(new Label(String(T("General")), String(T("General"))));
//	labels.add(new Label(String(T("Early Reflections")), String(T("Early Reflections"))));
//	labels.add(new Label(String(T("Reverb")), String(T("Reverb"))));
//	labels.add(new Label(String(T("Output")), String(T("Output"))));
//	for(int i = LABELGENERAL; i < noLabels; ++i) {
//		addAndMakeVisible(labels[i]);
//		labels[i]->setJustificationType(Justification::centredTop);
//	}

	sliders[INGAIN]->setSliderStyle(Slider::LinearVertical);
	sliders[INGAIN]->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	sliders[OUTGAIN]->setSliderStyle(Slider::LinearVertical);
	sliders[OUTGAIN]->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);

	sliders[PREFILTER]->setSliderStyle(Slider::LinearVertical);
	sliders[PREFILTER]->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	sliders[POSTFILTER]->setSliderStyle(Slider::LinearVertical);
	sliders[POSTFILTER]->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
	

	addAndMakeVisible( graphComponent = new GraphComponent(ownerFilter->getDistortionBuffer(), ownerFilter->getDistortionBufferSize()) );
	graphComponent->setValuesToReferTo(*values[X1], *values[Y1], *values[X2], *values[Y2]);
	
    // set the UI component's size
    setSize (500, 220);

    // register ourselves with the filter - it will use its ChangeBroadcaster base
    // class to tell us when something has changed, and this will call our changeListenerCallback()
    // method.
    ownerFilter->addChangeListener (this);
	
	updateParametersFromFilter();
}

DRowAudioEditorComponent::~DRowAudioEditorComponent()
{
    getFilter()->removeChangeListener (this);
	values.clear();
	sliders.clear();
	labels.clear();
    deleteAllChildren();
	
	delete lookAndFeel;
}

//==============================================================================
void DRowAudioEditorComponent::paint (Graphics& g)
{
	// draw rounded background with highlight at top
	Colour backgroundColour(0xFF455769);
	backgroundColour = backgroundColour.withBrightness(0.4f);

	g.setColour(backgroundColour);
	g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 10);
	
	ColourGradient topHighlight(Colours::white.withAlpha(0.3f),
								0, 0,
								Colours::white.withAlpha(0.0f),
								0, 0 + 15,
								false);
	g.setGradientFill(topHighlight);
	g.fillRoundedRectangle(0, 0, getWidth(), 30, 10);	
	
	ColourGradient outlineGradient(Colours::white,
								  0, 00,
								  backgroundColour.withBrightness(0.5f),
								  0, 20,
								  false);
	g.setGradientFill(outlineGradient);
	g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 10, 1.0f);
	

	int height = getHeight();
	int width = getWidth();

	// draw section dividing lines
//	lookAndFeel->drawInsetLine(g, width-170, 2, width-170, height, 2);
//	lookAndFeel->drawInsetLine(g, 0, height-120, width-170, height-120, 2);
//	lookAndFeel->drawInsetLine(g, (width-170)*0.5f, height-120, (width-170)*0.5f, height, 2);
	
	// linea around graph display
	Rectangle<int> outlineRect((0.5*width)-101, 9, 202, 202);
	
	g.setColour(Colours::black);
	g.drawHorizontalLine(9, (0.5*width)-101, (0.5*width)+101);
	g.setColour(Colour().greyLevel(0.5));
	g.drawHorizontalLine(210, (0.5*width)-101, (0.5*width)+101);

	ColourGradient sideGradient(Colour().greyLevel(0.3),
								outlineRect.getX(), outlineRect.getY(),
								Colour().greyLevel(0.5),
								outlineRect.getX(), outlineRect.getBottom(),
								false);
	g.setGradientFill(sideGradient);
	g.fillRect(outlineRect.getX(), outlineRect.getY(), outlineRect.getWidth(), outlineRect.getHeight());
	
//	g.drawLine((0.5*width)-100, 210, (0.5f*width)+100, 210, 4);
	
	// draw section heading lines
//	g.setColour(Colours::white.withAlpha(0.6f));
//	g.drawHorizontalLine(24, 10, 130);
//	g.drawHorizontalLine(24, 200, 320);
//	
//	g.drawHorizontalLine(24, 340, 385);
//	g.drawHorizontalLine(24, 445, 490);
//	
//	g.drawHorizontalLine(194, 10, 25);
//	g.drawHorizontalLine(194, 140, 155);
//
//	g.drawHorizontalLine(194, 175, 215);
//	g.drawHorizontalLine(194, 280, 320);
}

void DRowAudioEditorComponent::resized()
{
	int height = getHeight();
	int width = getWidth();
	
	
	//labels[LABELGENERAL]->setBounds(0, 10, width-170, 20);
//	sliders[PREDELAY]->setBounds(5, 60, 210, 20);
//	sliders[SPREAD]->setBounds(5, 110, 210, 20);
//	sliders[EARLYLATEMIX]->setBounds(5, 155, 210, 20);
//
//	sliders[DELTIME]->setBounds(220, 55, 50, 120); 
//	sliders[FBCOEFF]->setBounds(275, 55, 50, 120);
//	labels[DELTIME]->setSize(50, 24);
//	labels[FBCOEFF]->setSize(50, 24);
//	
//	labels[LABELEARLYREFLECTIONS]->setBounds(0, 180, 165, 20);
//	sliders[ROOMSHAPE]->setBounds(5, 225, 150, 20);
//	sliders[EARLYDECAY]->setBounds(5, 275, 150, 20);
//	
//	labels[LABELREVERB]->setBounds(165, 180, 165, 20);
//	sliders[FILTERCF]->setBounds(170, 225, 150, 20);
//	sliders[DIFFUSION]->setBounds(170, 275, 150, 20);
//
//	labels[LABELOUTPUT]->setBounds(width-170, 10, 170, 20);
	sliders[INGAIN]->setBounds(20, 30, 50, height-40);
	sliders[PREFILTER]->setBounds(80, 30, 50, height-40);
	
	graphComponent->setBounds((0.5*width)-100, 10, 200, height - 20);
	
	sliders[POSTFILTER]->setBounds(width-130, 30, 50, height-40);	
	sliders[OUTGAIN]->setBounds(width-60, 30, 50, height-40);	
}

//==============================================================================
void DRowAudioEditorComponent::sliderValueChanged (Slider* changedSlider)
{
    DRowAudioFilter* currentFilter = getFilter();
	
	for (int i = 0; i < noParams; i++)
		if ( changedSlider == sliders[i] )
			currentFilter->setScaledParameterNotifyingHost (i, (float) sliders[i]->getValue());
}

void DRowAudioEditorComponent::sliderDragStarted(Slider* changedSlider)
{
	DRowAudioFilter* currentFilter = getFilter();
	
	for (int i = 0; i < noParams; i++)
		if ( changedSlider == sliders[i] )
			currentFilter->beginParameterChangeGesture(i);
}
void DRowAudioEditorComponent::sliderDragEnded(Slider* changedSlider)
{
	DRowAudioFilter* currentFilter = getFilter();
	
	for (int i = 0; i < noParams; i++)
		if ( changedSlider == sliders[i] )
			currentFilter->endParameterChangeGesture(i);
}

void DRowAudioEditorComponent::valueChanged(Value& value)
{
	DRowAudioFilter* currentFilter = getFilter();
	
	for (int i = 0; i < noParams; i++)
		if ( value.refersToSameSourceAs(*values[i]) )
			currentFilter->setScaledParameterNotifyingHost (i, (float) values[i]->getValue());
}

void DRowAudioEditorComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    // this is the filter telling us that it's changed, so we'll update our
    // display of the time, midi message, etc.
    updateParametersFromFilter();
}

//==============================================================================
void DRowAudioEditorComponent::updateParametersFromFilter()
{
    DRowAudioFilter* const filter = getFilter();
	
	float tempParamVals[noParams];
	
    // we use this lock to make sure the processBlock() method isn't writing to the
    // lastMidiMessage variable while we're trying to read it, but be extra-careful to
    // only hold the lock for a minimum amount of time..
    filter->getCallbackLock().enter();
	
	for(int i = 0; i < noParams; i++)
		tempParamVals[i] =  filter->getScaledParameter (i);

    // ..release the lock ASAP
    filter->getCallbackLock().exit();

    // Update our sliders
	for(int i = 0; i < noParams; i++)
		values[i]->setValue (tempParamVals[i]);
}