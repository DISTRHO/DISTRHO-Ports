/*
  Copyright (C) 2012 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.   

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA

  This file is based on the JUCE library feil of the same name.
  JUCE - "Jules' Utility Class Extensions"
  Copyright 2004-9 by Raw Material Software Ltd.
*/


#ifndef __CABBAGE_GENERICAUDIOPROCESSOREDITOR_JUCEHEADER__
#define __CABBAGE_GENERICAUDIOPROCESSOREDITOR_JUCEHEADER__

//#include "FilterGraph.h"
#include "../CabbageLookAndFeel.h"

class ProcessorParameterPropertyComp   : public PropertyComponent,
                                         public AudioProcessorListener,
                                         public Timer
{
 //==============================================================================
    class ParamSlider  : public Slider,
						 public ActionBroadcaster
						// public ActionListener
    {
    public:
        ParamSlider (AudioProcessor& owner_, const int index_)
            : owner (owner_),
              index (index_)
        {
			//basicLookAndFeel = new CabbageLookAndFeelBasic();
			setLookAndFeel(basicLookAndFeel);
			this->setColour(Slider::textBoxBackgroundColourId, CabbageUtils::getBackgroundSkin());

            setRange (0.0, 1.0, 0.0);
			setSliderStyle (Slider::LinearHorizontal);
			setTextBoxStyle (Slider::TextBoxLeft, false, 80, 15);
            setTextBoxIsEditable (false);
            setScrollWheelEnabled (false);
        }

        void valueChanged()
        {
			//this return the name of the plugin
			//this calls ProcessorParameterPropertyComp::actionListenerCallbck
			this->sendActionMessage("");
            const float newVal = (float) getValue();

            if (owner.getParameter (index) != newVal)
                owner.setParameter (index, newVal);
        }


        String getTextFromValue (double /*value*/)
        {
            return owner.getParameterText (index);
        }


    private:
        //==============================================================================
        AudioProcessor& owner;
        const int index;
		ScopedPointer<CabbageLookAndFeelBasic> basicLookAndFeel;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParamSlider);
    };


public:
	ParamSlider slider;
	Colour highlightColour;
	bool moddedText;

    ProcessorParameterPropertyComp (const String& name, AudioProcessor& owner_, const int index_)
        : PropertyComponent (name),
          owner (owner_),
          index (index_),
          paramHasChanged (false),
          slider (owner_, index_)
    {
		moddedText = false;
		setPreferredHeight(20);
		startTimer (100);
        addAndMakeVisible (&slider);
        owner_.addListener (this);
		highlightColour = CabbageUtils::getDarkerBackgroundSkin();
    }

    ~ProcessorParameterPropertyComp()
    {
        owner.removeListener (this);
    }

    void refresh()
    {
		paramHasChanged = false;
        slider.setValue (owner.getParameter (index), sendNotification);
    }


    void audioProcessorChanged (AudioProcessor*)  {}

    void audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float)
    {
        if (parameterIndex == index)
            paramHasChanged = true;
    }

    void timerCallback()
    {
        if (paramHasChanged)
        {
            refresh();
            startTimer (1000 / 50);
        }
        else
        {
            startTimer (jmin (1000 / 4, getTimerInterval() + 10));
        }
    }

	void paint(Graphics &g)
	{
		g.fillAll(CabbageUtils::getComponentSkin().darker(.4f)); //background
		String text = getName();
		if(moddedText)
			g.setColour(Colours::yellow);
		else
			g.setColour(Colours::whitesmoke);

		Font font (CabbageUtils::getTitleFont());
		//font.setFallbackFontName (String("Verdana")); //in case the user doesn't have the first font installed
		g.setFont (font);
	
		g.drawFittedText(text, 5, 5, 
		font.getStringWidth(text), font.getHeight()-2, Justification::centredTop, 1);
	
	//	g.drawFittedText(text, 5, 5, 
	//	font.getStringWidth(text), font.getHeight()-2, Justification::centredTop, 1);
	}


private:

	ScopedPointer<CabbageLookAndFeelBasic> basicLookAndFeel;
    AudioProcessor& owner;
    const int index;
    bool volatile paramHasChanged;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcessorParameterPropertyComp);
};


class CabbageGenericAudioProcessorEditor      : public AudioProcessorEditor
{
public:
    //==============================================================================

	CabbageGenericAudioProcessorEditor(AudioProcessor* const owner_)
		: AudioProcessorEditor (owner_)
	{
		jassert (owner_ != nullptr);
		setOpaque (true); 
		basicLookAndFeel = new CabbageLookAndFeelBasic();
		this->setLookAndFeel(basicLookAndFeel);
		addAndMakeVisible (&panel);

		Array <PropertyComponent*> params;

		const int numParams = owner_->getNumParameters();
		Logger::writeToLog("NumParameters:" +String(numParams));
		int totalHeight = 0;

		for (int i = 0; i < numParams; ++i)
		{
			String name (owner_->getParameterName (i));
			if (name.trim().isEmpty())
				name = "Unnamed";

			ProcessorParameterPropertyComp* const pc = new ProcessorParameterPropertyComp (name, *owner_, i);
			
			params.add (pc);
			totalHeight += pc->getPreferredHeight();
		}

		panel.addProperties (params);

		setSize (400, jlimit (25, 400, totalHeight));
	}

	~CabbageGenericAudioProcessorEditor()
	{
	}


	void paint (Graphics& g)
	{
		//background
		g.setColour(Colour::fromRGB(20, 20, 20));
		g.fillAll();
	}


	void resized()
	{
		panel.setBounds (getLocalBounds());
	}


	ScopedPointer<CabbageLookAndFeelBasic> basicLookAndFeel;
private:
    //==============================================================================
    PropertyPanel panel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGenericAudioProcessorEditor)
};

#endif

