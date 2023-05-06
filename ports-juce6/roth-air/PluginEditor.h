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

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
// Custom look and feel class for bitmap sliders
class RothLookAndFeel : public LookAndFeel_V3
{
public:
	RothLookAndFeel()
	{
	}

	void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
	{
		// Declare big knob images from Binary Data
		Image bigKnob = ImageCache::getFromMemory(BinaryData::bigKnob_png, BinaryData::bigKnob_pngSize);
		Image bigLight = ImageCache::getFromMemory(BinaryData::bigKnob_light_png, BinaryData::bigKnob_light_pngSize);

		// Declare small knob images from Binary Data
		Image smallKnob = ImageCache::getFromMemory(BinaryData::smallKnob_png, BinaryData::smallKnob_pngSize);
		Image smallLight = ImageCache::getFromMemory(BinaryData::smallKnob_light_png, BinaryData::smallKnob_light_pngSize);

		//value between 0 and 1 for current amount of rotation
		const double fractRotation = (slider.getValue() - slider.getMinimum()) / (slider.getMaximum() - slider.getMinimum());

		// Apply rotation transform to knob graphic context
		AffineTransform rotation = AffineTransform::rotation(fractRotation * 5 - 45, width / 2, height / 2);
		g.addTransform(rotation);

		// If it's a big knob
		if (width >= 200)
		{
			// Multiply light alphas by fractRotation (0-1)
			Image bigLightCopy = bigLight.createCopy();
			bigLightCopy.multiplyAllAlphas(fractRotation);

			// Draw big light
			g.drawImage(bigLightCopy,
			x,
			y,
			width,
			height,
			0,
			0,
			bigLight.getWidth(),
			bigLight.getHeight());

			// Draw big knob
			g.drawImage(bigKnob,
			x,
			y,
			width,
			height,
			0,
			0,
			bigKnob.getWidth(),
			bigKnob.getHeight());
		}
		else 
		{
			// Multiply light alphas by fractRotation (0-1)
			Image smallLightCopy = smallLight.createCopy();
			smallLightCopy.multiplyAllAlphas(fractRotation);

			// Draw small light
			g.drawImage(smallLightCopy,
			x,
			y,
			width,
			height,
			0,
			0,
			smallLight.getWidth(),
			smallLight.getHeight());

			// Draw small knob
			g.drawImage(smallKnob,
			x,
			y,
			width,
			height,
			0,
			0,
			smallKnob.getWidth(),
			smallKnob.getHeight());
		}
	}
};

//==============================================================================
/**
*/
class AirAudioProcessorEditor  : public AudioProcessorEditor,
									public Slider::Listener,
									public Timer
{
public:
    AirAudioProcessorEditor (AirAudioProcessor&);
    ~AirAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	//============================================================================
	void sliderValueChanged(Slider* slider) override;
	void sliderDragStarted(Slider* slider) override;
	void sliderDragEnded(Slider* slider) override;
	void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AirAudioProcessor& processor;
	
	// Slider crossSlider;
	Slider dryWetSlider;
	Slider crossSlider;
	Slider threshSlider;
	Slider gainSlider;
	Slider airSlider;

	// Custom look and feel for gui stuff (subclass in PluginEditor)
	RothLookAndFeel rothLookAndFeel;

	// GUI image references
	Image title = ImageCache::getFromMemory(BinaryData::title_png, BinaryData::title_pngSize);
	Image airText = ImageCache::getFromMemory(BinaryData::airText_png, BinaryData::airText_pngSize);
	Image website = ImageCache::getFromMemory(BinaryData::website_png, BinaryData::website_pngSize);

	Image threshLabel = ImageCache::getFromMemory(BinaryData::label_thresh_png, BinaryData::label_thresh_pngSize);
	Image freqLabel = ImageCache::getFromMemory(BinaryData::label_freq_png, BinaryData::label_freq_pngSize);
	Image gainLabel = ImageCache::getFromMemory(BinaryData::label_gain_png, BinaryData::label_freq_pngSize);
	Image mixLabel = ImageCache::getFromMemory(BinaryData::label_mix_png, BinaryData::label_mix_pngSize);

	Image redLight = ImageCache::getFromMemory(BinaryData::bigKnob_red_png, BinaryData::bigKnob_red_pngSize);

	bool showCross = false;
	bool showMix = false;
	bool showThresh = false;
	bool showGain = false;
	bool showAir = false;

	double gainReduction;
	int gainIterator = 0;
	Array<double> arrGainReduction;
	int meterBufferSize = 8;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AirAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
