/*
  Copyright (C) 20139 Rory Walsh

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
*/

#ifndef SOUNDFILEWAVEFORM_H
#define SOUNDFILEWAVEFORM_H

#include "JuceHeader.h" 
#include "CabbageUtils.h"
#include "CabbageLookAndFeel.h"


//=================================================================
// display a sound file as a waveform..
//=================================================================
class WaveformDisplay : public Component,
						public Timer
{
public:
	WaveformDisplay(BufferingAudioSource &source, int sr, Colour col):
	thumbnailCache (5), 
	source(&source), 
	colour(col),
	sampleRate(sr),
	currentPlayPosition(0),
	mouseDownX(0),
	mouseUpX(0),
	drawWaveform(true),
	numOfSamples(0)
	{	
    formatManager.registerBasicFormats();  
	thumbnail = new AudioThumbnail(512, formatManager, thumbnailCache); 
	setSize(100, 100);
	}
	
	~WaveformDisplay()
	{	
		source = nullptr;
	}
	
	void resized()
	{
	}
	
    void setFile (const File& file, bool firstTime)
    {
		if(file.existsAsFile()){
			if(firstTime){
			thumbnail->setSource (new FileInputSource(file));
			}
			else{
			//FileInputStream* inFileStream = new FileInputStream(file); // jatFile is the saved thumbnail file
			thumbnail = nullptr;
			thumbnail = new AudioThumbnail(512, formatManager, thumbnailCache); 
			thumbnail->setSource (new FileInputSource(file));
			//delete inFileStream;
			}
			drawWaveform=true;
		}
		else
			drawWaveform = false;

		startTime = 0;
        endTime = thumbnail->getTotalLength();
		repaint();
    }

    void setZoomFactor (double amount)
    {
        if (thumbnail->getTotalLength() > 0)
        {
            const double newScale = jmax (0.001, thumbnail->getTotalLength() * (1.0 - jlimit (0.0, 0.99, amount)));
            const double timeAtCentre = xToTime (getWidth() / 2.0f);
            //startTime = timeAtCentre - newScale * 0.5;
            //endTime = timeAtCentre + newScale * 0.5;
            repaint();
        }
    }

    void paint (Graphics& g)
    {
        g.fillAll (Colours::black);
        g.setColour (colour);
		if(drawWaveform){
			if (thumbnail->getTotalLength() > 0)
			{
				thumbnail->drawChannels (g, getLocalBounds(),
										startTime, endTime, 2.0f);
			}
			else
			{
				g.setFont (14.0f);
				g.drawFittedText ("(No audio file selected)", getLocalBounds(), Justification::centred, 2);
			}
			g.setColour(colour.brighter(.4f));
			g.drawLine(timeToX(currentPlayPosition), 0, timeToX(currentPlayPosition), getHeight(), 2);	
		}
    }

    void timerCallback()
    {
		Viewport* const viewport = findParentComponentOfClass <Viewport> (); //Get the parent viewport
		currentPlayPosition = source->getNextReadPosition()/sampleRate;
		if(viewport != nullptr) //Check for nullness
		viewport->setViewPosition(jmax(0.f, timeToX(currentPlayPosition)-100), 0);
		repaint();
    }

    void mouseDown (const MouseEvent& e)
    {
		if(e.mods.isCommandDown()){
		if(e.mods.isLeftButtonDown())
			this->setSize(jmin(1000, this->getWidth()+100), getHeight());
		else
			this->setSize(jmax(400, this->getWidth()-100), getHeight());
			Logger::writeToLog("command down");
		}
			
		source->setNextReadPosition (jmax (0.0, xToTime ((float) e.x)*sampleRate));
		currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
		mouseDownX = e.x;
		repaint();
    }

    void mouseDrag(const MouseEvent& e)
    {
        mouseUpX = e.x;
		currentPlayPosition = jmax (0.0, xToTime ((float) e.x));
        repaint();
    }
	
	void rewindToStart(){
		if(source)
		source->setNextReadPosition (0);
		currentPlayPosition = 0.f;	
}
	
	void skipForward(){
		currentPlayPosition = currentPlayPosition+1.f; 
	}
	
    AudioThumbnailCache thumbnailCache;
    ScopedPointer<AudioThumbnail> thumbnail;
    ScopedPointer<BufferingAudioSource> source;
	float sampleRate;

private:
	AudioFormatManager formatManager;
	int64 numOfSamples;
	Colour colour;
	int mouseDownX, mouseUpX;
    double startTime, endTime;
	Rectangle<int> localBounds;
	
	double currentPlayPosition;
	bool drawWaveform;

    DrawableRectangle currentPositionMarker;

    float timeToX (const double time) const
    {
        return getWidth() * (float) ((time - startTime) / (endTime - startTime));
    }

    double xToTime (const float x) const
    {
        return (x / getWidth()) * (endTime - startTime) + startTime;
    }
};


class Soundfiler : public Component,
					public Button::Listener,
					public ChangeListener
{
public:
	Soundfiler(CabbageAudioSource& audioSource, String fileName, int sr, Colour colour, Colour fontcolour);
	~Soundfiler(){
	cabbageAudioSource->isSourcePlaying = false;
	cabbageAudioSource->audioSourceBuffer = nullptr;
	cabbageAudioSource->removeAllChangeListeners();
	cabbageAudioSource = nullptr;	
	};
	
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
	void changeListenerCallback(ChangeBroadcaster *source);
	void resized();
	void buttonClicked(Button *button);
	ScopedPointer<WaveformDisplay> waveformDisplay;
	ScopedPointer<ImageButton> playButton;	
	ScopedPointer<ImageButton> skipToStartButton;
	ScopedPointer<ImageButton> skipToEndButton;
	ScopedPointer<TextButton> loadFile;
	ScopedPointer<ToggleButton> loopFile;
	ScopedPointer<Viewport> viewport;
	CabbageAudioSource* cabbageAudioSource;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Soundfiler);
	
};

#endif // SOUNDFILEWAVEFORM_H
