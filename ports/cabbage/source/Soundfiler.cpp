/*
  Copyright (C) 2009 Rory Walsh

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

#include "Soundfiler.h"


//==============================================================================
// soundfiler component
//==============================================================================
Soundfiler::Soundfiler(CabbageAudioSource& _audioSource, String fileName, int sr, Colour colour, Colour fontColour)
:cabbageAudioSource(&_audioSource)
{

	viewport = new Viewport();
	Logger::writeToLog("sample Rate:"+String(sr));
	waveformDisplay = new WaveformDisplay(*cabbageAudioSource->audioSourceBuffer, sr, colour);
	//register a listener callback so that we know when to update our audioSource object
	cabbageAudioSource->addChangeListener(this);

	playButton = new ImageButton("Play button");
	playButton->addListener(this);
	addAndMakeVisible(playButton);	
	skipToStartButton = new ImageButton("Skip to start button");
	skipToStartButton->addListener(this);
	addAndMakeVisible(skipToStartButton);
	
	skipToEndButton = new ImageButton("Skip to end button");
	skipToEndButton->addListener(this);
	//addAndMakeVisible(skipToEndButton);
	
	//playButton->setToggleState(false, true);
	playButton->setClickingTogglesState(true);
	//playButton->setState(Button::buttonDown);

	playButton->setImages(false, true, true, 
		CabbageUtils::drawSoundfilerButton("play_normal", fontColour.toString()), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("play_hover", fontColour.toString()), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("play_down", fontColour.toString()), 1.0f, Colours::transparentBlack);

	skipToStartButton->setImages(false, true, true, 
		CabbageUtils::drawSoundfilerButton("skip_start_normal", fontColour.toString()), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("skip_start_hover", fontColour.toString()), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("skip_start_down", fontColour.toString()), 1.0f, Colours::transparentBlack);

	skipToEndButton->setImages(false, true, true, 
		CabbageUtils::drawSoundfilerButton("skip_end_normal", fontColour.toString()), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("skip_end_hover", fontColour.toString()), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("skip_end_down", fontColour.toString()), 1.0f, Colours::transparentBlack);	


	loadFile = new TextButton("Open File");
	loadFile->getProperties().set("fontcolour", fontColour.toString());
	
	
	loopFile = new ToggleButton("Loop");
	
	loopFile->getProperties().set("isRect", 1);
	loopFile->getProperties().set("colour", fontColour.toString());
	loopFile->getProperties().set("fontcolour", fontColour.toString());
	addAndMakeVisible(loopFile);
	loadFile->addListener(this);
	loopFile->addListener(this);

	addAndMakeVisible(loadFile);


	addAndMakeVisible(viewport);
    setSize (400, 300);

	waveformDisplay->setFile(File(fileName), true);
	waveformDisplay->setZoomFactor(1);

	viewport->setViewedComponent(waveformDisplay, false);
	viewport->setScrollBarsShown(true, true);
}



void Soundfiler::changeListenerCallback(ChangeBroadcaster *source)
{
CabbageAudioSource* const cabbageAudio = dynamic_cast <CabbageAudioSource*> (source);
if(cabbageAudio){
	waveformDisplay->source = cabbageAudio->audioSourceBuffer;
	}
}

void Soundfiler::buttonClicked(Button *button)
{
	if(button->getName()=="Play button"){
		if(!cabbageAudioSource->isSourcePlaying && cabbageAudioSource->isValidFile){
			waveformDisplay->startTimer(100);
			//startStop->setButtonText("Stop..");
			//startStop->setState(Button::buttonDown);
		}
		else{
			waveformDisplay->stopTimer();
			//startStop->setButtonText("Play");
		}
		cabbageAudioSource->isSourcePlaying=!cabbageAudioSource->isSourcePlaying;
	}
	else if(button->getName()=="Skip to start button"){
		waveformDisplay->rewindToStart();
	}
	else if(button->getName()=="Skip to end button"){
		waveformDisplay->skipForward();
	}
	else if(button->getName()=="Loop"){
		if(cabbageAudioSource->isValidFile)
		if(button->getToggleState()==true)
		cabbageAudioSource->audioSource->setLooping(true);
		else
		cabbageAudioSource->audioSource->setLooping(false);	
	}
	else{
		FileChooser openFC(String("Open a Cabbage sound file..."), File::nonexistent, String("*.wav;*.mp3"));
		if(openFC.browseForFileToOpen())
			if(cabbageAudioSource->setFile(openFC.getResult().getFullPathName(), 2)){
					waveformDisplay->sampleRate = cabbageAudioSource->sampleRate;
					waveformDisplay->setFile(openFC.getResult(), false);					
					waveformDisplay->setZoomFactor(1);
					waveformDisplay->stopTimer();
					playButton->setToggleState(false, sendNotification);
					
			}
		
	}
}

//==============================================================================
void Soundfiler::resized()
{
waveformDisplay->setSize(800, getHeight()-40);
viewport->setBounds(0, 0, getWidth(), getHeight()-20);
//startStop->setBounds(0, getHeight()-20, 40, 20);
skipToStartButton->setBounds(90, getHeight()-20, 20, 20);	
playButton->setBounds(70, getHeight()-20, 20, 20);
skipToEndButton->setBounds(100, getHeight()-20, 20, 20);	
loopFile->setBounds(115, getHeight()-20, 60, 20);
loadFile->setBounds(0, getHeight()-20, 70, 20);
}
//==============================================================================
void Soundfiler::paint (Graphics& g)
{
g.fillAll(Colours::black);
}

