/*
  ==============================================================================

  This is an automatically generated file created by the Jucer!

  Creation date:  26 Aug 2012 12:42:01am

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Jucer version: 1.12

  ------------------------------------------------------------------------------

  The Jucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-6 by Raw Material Software ltd.

  ==============================================================================
*/

#ifndef __JUCER_HEADER_DELAYGRAPH_DELAYGRAPH_F9D273E4__
#define __JUCER_HEADER_DELAYGRAPH_DELAYGRAPH_F9D273E4__

//[Headers]	 -- You can add your own extra header files here --
#include "pitcheddelaytab.h"
//[/Headers]



//==============================================================================
/**
																	//[Comments]
	An auto-generated component, created by the Jucer.

	Describe your class and how it works here!
																	//[/Comments]
*/
class DelayGraph  : public Component,
                    public Timer
{
public:
	//==============================================================================
	DelayGraph (OwnedArray<PitchedDelayTab>& tabs_, PitchedDelayAudioProcessor* ownerFilter);
	~DelayGraph();

	//==============================================================================
	//[UserMethods]	 -- You can add your own custom methods in this section.
	void timerCallback();
	void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel);
	//[/UserMethods]

	void paint (Graphics& g);
	void resized();
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);



	//==============================================================================
	juce_UseDebuggingNewOperator

private:
	//[UserVariables]   -- You can add your own custom variables in this section.

	class IntComperator
	{
	public:
		static int compareElements (int first, int second)
		{
			return first - second;
		}
	};

	int getDelayUnderMouse(Point<int> mousePos, float minDistance = 10);

	OwnedArray<PitchedDelayTab>& tabs;
	PitchedDelayAudioProcessor* Proc;
	HeapBlock<double> currentDelays;
	HeapBlock<double> currentVolumes;
	HeapBlock<double> currentPreVolumes;
	HeapBlock<double> currentFeedback;
	HeapBlock<bool> currentEnabled;
	int currentSelected;
	bool dragging;
	//[/UserVariables]

	//==============================================================================


	//==============================================================================
	// (prevent copy constructor and operator= being generated..)
	DelayGraph (const DelayGraph&);
	const DelayGraph& operator= (const DelayGraph&);
};


#endif   // __JUCER_HEADER_DELAYGRAPH_DELAYGRAPH_F9D273E4__
