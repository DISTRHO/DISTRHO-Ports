/*
------------------------------------------------------------------------------

Plugin editor
================
A Juce class containing the Roth-AIR GUI elements
By Daniel Rothmann

Provided under GNU General Public license:
http://www.gnu.org/licenses/

------------------------------------------------------------------------------
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AirAudioProcessorEditor::AirAudioProcessorEditor (AirAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (581, 345);
	setResizable(false, false);

	// Set Threshold slider params
	threshSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	threshSlider.setRange(processor.cThreshold->range.start, processor.cThreshold->range.end);
	threshSlider.setTextBoxStyle(Slider::NoTextBox, true, 50, 15);
	threshSlider.setPopupDisplayEnabled(false, false, this);
	threshSlider.setTextValueSuffix(" dB");
	threshSlider.setValue(*processor.cThreshold);
	threshSlider.setLookAndFeel(&rothLookAndFeel);

	// Set crossover freq slider params
	crossSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	crossSlider.setRange(processor.crossFreq->range.start, processor.crossFreq->range.end);
	crossSlider.setTextBoxStyle(Slider::NoTextBox, true, 50, 15);
	crossSlider.setPopupDisplayEnabled(false, false, this);
	crossSlider.setTextValueSuffix(" Hz");
	crossSlider.setValue(*processor.crossFreq);
	crossSlider.setLookAndFeel(&rothLookAndFeel);

	// Set Gain slider params
	gainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	gainSlider.setRange(processor.hpGain->range.start, processor.hpGain->range.end);
	gainSlider.setTextBoxStyle(Slider::NoTextBox, true, 50, 15);
	gainSlider.setPopupDisplayEnabled(false, false, this);
	gainSlider.setTextValueSuffix(" Rms");
	gainSlider.setValue(*processor.hpGain);
	gainSlider.setLookAndFeel(&rothLookAndFeel);

	// Set Dry/Wet slider params
	dryWetSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	dryWetSlider.setRange(processor.dryWet->range.start, processor.dryWet->range.end);
	dryWetSlider.setTextBoxStyle(Slider::NoTextBox, true, 50, 15);
	dryWetSlider.setPopupDisplayEnabled(false, false, this);
	dryWetSlider.setTextValueSuffix(" %");
	dryWetSlider.setValue(*processor.dryWet);
	dryWetSlider.setLookAndFeel(&rothLookAndFeel);

	// Set AIR slider params
	airSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
	airSlider.setRange(processor.airAmt->range.start, processor.airAmt->range.end);
	airSlider.setTextBoxStyle(Slider::NoTextBox, true, 50, 15);
	airSlider.setPopupDisplayEnabled(false, false, this);
	airSlider.setTextValueSuffix(" %");
	airSlider.setValue(*processor.airAmt);
	airSlider.setLookAndFeel(&rothLookAndFeel);

	// Add and make sliders visible
	addAndMakeVisible(&threshSlider);
	addAndMakeVisible(&crossSlider);
	addAndMakeVisible(&gainSlider);
	addAndMakeVisible(&dryWetSlider);
	addAndMakeVisible(&airSlider);
	
	// Add listeners for sliders
	threshSlider.addListener(this);
	crossSlider.addListener(this);
	gainSlider.addListener(this);
	dryWetSlider.addListener(this);
	airSlider.addListener(this);

	for (int i = 0; i < meterBufferSize; i++)
	{
		arrGainReduction.add(0.);
	}

	// Start a timer - this is how often the GUI will be updated when params are automated from host (60"fps")
	startTimerHz(60);
}

AirAudioProcessorEditor::~AirAudioProcessorEditor()
{
}

//==============================================================================
void AirAudioProcessorEditor::paint (Graphics& g)
{
    // g.fillAll (Colours::white);
	g.fillAll(Colour::fromRGB(62, 75, 89));

    g.setColour (Colour::fromRGB(152, 193, 217));
    g.setFont (17.0f);

	// Draw GUI images
	g.drawImage(title, 0, 5, 252, 62, 0, 0, 252, 62);
	g.drawImage(website, 354, 34, 215, 25, 0, 0, 215, 25);

	// draws labels if the "show-x-" is false - if its true it draws param value as text (truncated string)
	if (!showThresh)
		g.drawImage(threshLabel, 10, 64, 130, 130, 0, 0, 130, 130);
	else
		g.drawText(std::to_string(processor.getThreshold()).substr(0,5) + " dB", 10, 64, 130, 130, juce::Justification::centred);

	if (!showCross)
		g.drawImage(freqLabel, 10, 204, 130, 130, 0, 0, 130, 130);
	else
		g.drawText(std::to_string(processor.getCrossFreq()).substr(0, 4) + " Hz", 10, 204, 130, 130, juce::Justification::centred);

	if (!showGain)
		g.drawImage(gainLabel, 440, 64, 130, 130, 0, 0, 130, 130);
	else
		g.drawText(std::to_string(processor.getHpGain()).substr(0, 4), 440, 64, 130, 130, juce::Justification::centred);

	if(!showMix)
		g.drawImage(mixLabel, 440, 204, 130, 130, 0, 0, 130, 130);
	else
		g.drawText(std::to_string(processor.getDryWet() * 100).substr(0, 4) + "%", 440, 204, 130, 130, juce::Justification::centred);

	// Image redLightCopy = redLight.createCopy();
	// redLightCopy.multiplyAllAlphas(gainReduction);
	g.beginTransparencyLayer(gainReduction);
	g.drawImage(redLight, 145, 54, 290, 290, 0, 0, 290, 290);
	g.endTransparencyLayer();

	g.drawImage(airText, 230, 164, 120, 72, 0, 0, 120, 72);
}

void AirAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

	// Place the knobs (sliders) and set size
	threshSlider.setBounds(10, 64, 130, 130);
	crossSlider.setBounds(10, 204, 130, 130);
	gainSlider.setBounds(440, 64, 130, 130);
	dryWetSlider.setBounds(440, 204, 130, 130);
	airSlider.setBounds(145, 54, 290, 290);
}

// When a slider is changed, set according parameter to new slider value
void AirAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider == &crossSlider)
	{
		*processor.crossFreq = crossSlider.getValue();
	}
	else if (slider == &dryWetSlider)
	{
		*processor.dryWet = dryWetSlider.getValue();
	}
	else if (slider == &threshSlider)
	{
		*processor.cThreshold = threshSlider.getValue();
	}
	else if (slider == &gainSlider)
	{
		*processor.hpGain = gainSlider.getValue();
	}
	else if (slider == &airSlider)
	{
		*processor.airAmt = airSlider.getValue();
	}
}

// Tell the parameter that user has started dragging slider
void AirAudioProcessorEditor::sliderDragStarted(Slider* slider)
{
	if (slider == &crossSlider)
	{
		processor.crossFreq->beginChangeGesture();
		showCross = true;
	}
	else if (slider == &dryWetSlider)
	{
		processor.dryWet->beginChangeGesture();
		showMix = true;
	}
	else if (slider == &threshSlider)
	{
		processor.cThreshold->beginChangeGesture();
		showThresh = true;
	}
	else if (slider == &gainSlider)
	{
		processor.hpGain->beginChangeGesture();
		showGain = true;
	}
	else if (slider == &airSlider)
	{
		processor.airAmt->beginChangeGesture();
		showAir = true;
	}
}

// Tell the parameter that user has stopped dragging slider
void AirAudioProcessorEditor::sliderDragEnded(Slider* slider)
{
	if (slider == &crossSlider)
	{
		processor.crossFreq->endChangeGesture();
		showCross = false;
	}
	else if (slider == &dryWetSlider)
	{
		processor.dryWet->endChangeGesture();
		showMix = false;
	}
	else if (slider == &threshSlider)
	{
		processor.cThreshold->endChangeGesture();
		showThresh = false;
	}
	else if (slider == &gainSlider)
	{
		processor.hpGain->endChangeGesture();
		showGain = false;
	}
	else if (slider == &airSlider)
	{
		processor.airAmt->endChangeGesture();
		showAir = false;
	}
}

// Update sliders with parameter values with frequency from timer
void AirAudioProcessorEditor::timerCallback()
{
	// get parameter values from the processor each timerCallback - to check for automation changes
	crossSlider.setValue(processor.getCrossFreq(), dontSendNotification);
	dryWetSlider.setValue(processor.getDryWet(), dontSendNotification);
	threshSlider.setValue(processor.getThreshold(), dontSendNotification);
	gainSlider.setValue(processor.getHpGain(), dontSendNotification);
	airSlider.setValue(processor.getAirAmt(), dontSendNotification);

	// get current amount of gain reduction from compressor, divide it by 20(dB) and waveshape it for low values to be pushed up
	double tempGainReduction = processor.getTempGainReduction() / 15;
	tempGainReduction = (1.0 + 4) * tempGainReduction / (1.0 + 4 * abs(tempGainReduction));

	// if the gain reduction has changed, do the calculation and order a repaint
	if (tempGainReduction != gainReduction)
	{ 
		// iterate index (for averaging meter values)
		gainIterator++;

		if (gainIterator >= arrGainReduction.size())
			gainIterator = 0;

		// store temporary gain reduction in array at index
		// arrGainReduction.getReference(gainIterator) = tempGainReduction;
		arrGainReduction.setUnchecked(gainIterator, tempGainReduction);

		// init gainReduction value
		gainReduction = 0;

		// loop through all values in array and add them to gainreduction
		for (int i = 0; i < arrGainReduction.size(); i++)
		{
			// gainReduction += arrGainReduction.getReference(gainIterator);
			gainReduction += arrGainReduction.getUnchecked(i);
		}

		// divide by the size of the array (get the mean of the array)
		gainReduction /= arrGainReduction.size();

		// make sure it will stay within alpha boundaries
		if (gainReduction > 1)
			gainReduction = 1;
		else if (gainReduction < 0)
			gainReduction = 0;

		// order repaint of the button area
		repaint(145, 54, 290, 290);
	}
}
