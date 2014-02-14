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
    : AudioProcessorEditor (ownerFilter),
	noButtons(3)
{
	// set look and feel
	customLookAndFeel = new dRowLookAndFeel;
	setLookAndFeel(customLookAndFeel);
	customLookAndFeel->setColour(Label::textColourId, (Colours::black).withBrightness(0.9f));
	
	// load title image to memory cache
//	cachedTitleImage = ImageCache::getFromMemory (Resource::flanger_title, Resource::flanger_title_size);
	
	addAndMakeVisible( comboBox = new ComboBox("comboBox") );
	
	for (int i = 0; i < noParams; i++)
	{
		sliders.add( new Slider(String("param") + String(i)) );
		addAndMakeVisible( sliders[i]);
		
		String labelName = ownerFilter->getParameterName(i);
		sliderLabels.add(new Label(String("Label") + String(i), labelName));
		addAndMakeVisible(sliderLabels[i]);
		sliderLabels[i]->setJustificationType(Justification::left);
		sliderLabels[i]->attachToComponent(sliders[i], false);
			
		sliders[i]->addListener (this);
		ownerFilter->getParameterPointer(i)->setupSlider(*sliders[i]);
		
		sliders[i]->setTextBoxStyle(Slider::TextBoxRight, false, 60, 18);
		sliders[i]->setColour (Slider::thumbColourId, Colours::grey);
		sliders[i]->setColour (Slider::textBoxTextColourId, Colour (0xff78f4ff));
		sliders[i]->setColour (Slider::textBoxBackgroundColourId, Colour(0xFF455769).withBrightness(0.15f));
		sliders[i]->setColour (Slider::textBoxOutlineColourId, (Colour (0xffffffff)).withBrightness(0.5f));
	}
	sliders[INGAIN]->setSliderStyle(Slider::RotaryVerticalDrag);
	sliders[INGAIN]->setColour(Slider::rotarySliderFillColourId, Colours::grey);
	sliders[INGAIN]->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 18);
	sliderLabels[INGAIN]->attachToComponent(sliders[INGAIN], false);
	sliderLabels[INGAIN]->setJustificationType(Justification::centred);
	sliders[COLOUR]->setSliderStyle(Slider::RotaryVerticalDrag);
	sliders[COLOUR]->setColour(Slider::rotarySliderFillColourId, Colours::grey);
	sliders[COLOUR]->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 18);
	sliderLabels[COLOUR]->attachToComponent(sliders[COLOUR], false);
	sliderLabels[COLOUR]->setJustificationType(Justification::centred);
	sliders[OUTGAIN]->setSliderStyle(Slider::RotaryVerticalDrag);
	sliders[OUTGAIN]->setColour(Slider::rotarySliderFillColourId, Colours::grey);
	sliders[OUTGAIN]->setTextBoxStyle(Slider::TextBoxBelow, false, 60, 18);
	sliderLabels[OUTGAIN]->attachToComponent(sliders[OUTGAIN], false);
	sliderLabels[OUTGAIN]->setJustificationType(Justification::centred);


		
	for ( int i = 0; i < noButtons; i++ )
	{
		buttons.add(new TextButton(String("Button ") + String(i)));
		addAndMakeVisible(buttons[i]);
	}
	buttons[0]->setClickingTogglesState(true);
	buttons[0]->addListener(this);
	
	buttons[1]->setClickingTogglesState(true);
	buttons[1]->addListener(this);
	
	buttons[2]->setClickingTogglesState(true);
	buttons[2]->addListener(this);
	
	// set up the meters
	addAndMakeVisible(meterLeft = new MeterComponent(&ownerFilter->RMSLeft, &ownerFilter->peakLeft, &ownerFilter->getCallbackLock()));
	addAndMakeVisible(meterRight = new MeterComponent(&ownerFilter->RMSRight, &ownerFilter->peakRight, &ownerFilter->getCallbackLock()));
	
    // set our component's initial size to be the last one that was stored in the filter's settings
    setSize (250, 200);

    // register ourselves with the filter - it will use its ChangeBroadcaster base
    // class to tell us when something has changed, and this will call our changeListenerCallback()
    // method.
    ownerFilter->addChangeListener (this);
}

DRowAudioEditorComponent::~DRowAudioEditorComponent()
{
    getFilter()->removeChangeListener (this);
//	ImageCache::release (cachedTitleImage);
	sliders.clear();
	sliderLabels.clear();
	buttons.clear();
    deleteAllChildren();
	delete customLookAndFeel;
}

//==============================================================================
void DRowAudioEditorComponent::paint (Graphics& g)
{
	// just clear the window
	Colour backgroundColour(0xFF455769);
//	Colour backgroundColour(Colours::royalblue);
	backgroundColour = backgroundColour.withBrightness(0.4f);
//	ColourGradient backgroundGradient(backgroundColour.withBrightness(0.0f),
//									  0, 40,
//									  backgroundColour.withBrightness(0.0f),
//									  0, getHeight(),
//									  false);
//	backgroundGradient.addColour(0.025f, backgroundColour);
//	backgroundGradient.addColour(0.975f, backgroundColour);
//	GradientBrush backgroundBrush(backgroundGradient);
//	g.setBrush(&backgroundBrush);
//	g.fillRect(0, 40, getWidth(), getHeight()-40);
	g.setColour(backgroundColour);
	g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 10);
	
	ColourGradient topHighlight(Colours::white.withAlpha(0.3f),
								0, 0,
								Colours::white.withAlpha(0.0f),
								0, 0 + 15,
								false);
	
	ColourGradient topColor(topHighlight);
	g.setGradientFill(topColor);
	g.fillRoundedRectangle(0, 0, getWidth(), 30, 10);
	
	ColourGradient outlineGradient(Colours::white,
								  0, 0,
								  backgroundColour.withBrightness(0.5f),
								  0, 20,
								  false);
	g.setGradientFill(outlineGradient);
	g.drawRoundedRectangle(0, 0, getWidth(), getHeight(), 10, 1.0f);
		
	
//	g.setColour(Colour(0xFFE9E9F4));
//	g.setFont(30);
//	g.drawFittedText(T("Distortion"),
//					 getWidth()/2 - (getWidth()/2), 5,
//					 getWidth(), getHeight(),
//					 Justification::centredTop,
//					 1);
//	g.drawImageWithin (cachedTitleImage,
//                       0, 0, 250, 40,
//                       RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize,
//                       false);
}

void DRowAudioEditorComponent::resized()
{
//    comboBox->setBounds (getWidth()/2 - 100, 40,
//						200, 20);
	
//	for (int i = 0; i < noParams; i++)
//		sliders[i]->setBounds (70, 70 + (30*i), getWidth()-140, 20);
	sliders[PRE]->setBounds(5, 30, getWidth()-10, 20);
	sliders[INGAIN]->setBounds(getWidth()*0.5f - 110, 75, 70, 70);
	sliders[COLOUR]->setBounds(getWidth()*0.5f - 35, 75, 70, 70);
	sliders[OUTGAIN]->setBounds(getWidth()*0.5f + 40, 75, 70, 70);
	sliders[POST]->setBounds(5, getHeight()-25, getWidth()-10, 20);
	
	
//	meterLeft->setBounds(getWidth()-65, 70, 25, 290);
//	meterRight->setBounds(getWidth()-35, 70, 25, 290);
		
//	for ( int i = 0; i < noButtons; i++ )
//		buttons[i]->setBounds( 10 + (i * ((getWidth()-20)/noButtons) + ((noButtons-1)*5)), 370,
//							  ((getWidth()-20)/noButtons)-(((noButtons-1)*5)), 20);
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

void DRowAudioEditorComponent::buttonClicked(Button* clickedButton)
{
//	DRowAudioFilter* currentFilter = getFilter();
	
	if (clickedButton == buttons[0])
	{
	}
	if (clickedButton == buttons[2])
	{
	}
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
	
    // we use this lock to make sure the processBlock() method isn't writing
	// to variables while we're trying to read it them
    filter->getCallbackLock().enter();

	for(int i = 0; i < noParams; i++)
		tempParamVals[i] =  filter->getScaledParameter (i);

    // ..release the lock ASAP
    filter->getCallbackLock().exit();


    /* Update our sliders

       (note that it's important here to tell the slider not to send a change
       message, because that would cause it to call the filter with a parameter
       change message again, and the values would drift out.
    */
	for(int i = 0; i < noParams; i++)
		sliders[i]->setValue (tempParamVals[i], dontSendNotification);
}