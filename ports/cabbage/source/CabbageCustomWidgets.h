/*
  Copyright (C) 2010 Rory Walsh, Damien Rennick

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

#ifndef _CabbageWidgets_H__
#define _CabbageWidgets_H__

#include "JuceHeader.h"
#include "CabbageLookAndFeel.h"
#include "CabbageUtils.h"
#include "CabbageTable.h"
#include "XYPad.h"
#include "Soundfiler.h"
#include "DirectoryContentsComponent.h"

class InfoWindow   : public DocumentWindow
{
ScopedPointer<WebBrowserComponent> htmlInfo;

public:
    //==============================================================================
    InfoWindow(LookAndFeel* look, String file)  : DocumentWindow (String("Information"),
                      Colours::black,
                      DocumentWindow::allButtons)
	{
	setSize(600, 400);
	this->setTitleBarHeight(20);
	htmlInfo = new WebBrowserComponent(false);
	htmlInfo->setBounds(0, 0, 600, 400);
	setContentOwned(htmlInfo, true);
	htmlInfo->goToURL(file);
	}

    ~InfoWindow(){};

	void closeButtonPressed(){
	setVisible(false);
	};

};

//==============================================================================
// custom button component with optional surrounding groupbox
//==============================================================================
class CabbageButton : public Component
{
int offX, offY, offWidth, offHeight;
String buttonType;

public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<Button> button;
//---- constructor -----
CabbageButton(String name, String caption, String buttonText, String colour, String fontcolour)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	button = new TextButton(name);
	addAndMakeVisible(groupbox);
	addAndMakeVisible(button);
	groupbox->setVisible(false);
	groupbox->getProperties().set("groupLine", var(1));
	Logger::writeToLog(buttonText);
	button->setButtonText(buttonText);
	if(caption.length()>0){
		offX=10;
		offY=35;
		offWidth=-20;
		offHeight=-45;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}

	if(fontcolour.length()>0)
	button->getProperties().set("fontcolour", fontcolour);
	if(colour.length()>0)
	button->getProperties().set("colour", colour);
}
//---------------------------------------------
~CabbageButton(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageButton);
};

//==============================================================================
// custom slider component with optional surrounding groupbox
//==============================================================================
class CabbageSlider : public Component,
					  public ChangeBroadcaster
{
int offX, offY, offWidth, offHeight, plantX, plantY;
String sliderType, compName, cl;
int resizeCount;
String tracker;

//subclass slider here to expose mouse listener method
		class cSlider : public Slider
		{
		public:
			cSlider(String text, CabbageSlider* _slider):Slider(text), 
														slider(_slider){}
			~cSlider(){};
			
		private:
			void mouseMove(const MouseEvent& event){
				slider->sendChangeMessage();
			}	
			

		CabbageSlider* slider;
		};

public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<Slider> slider;
//---- constructor -----
CabbageSlider(String name, String text, String caption, String kind, String colour, String fontColour, int textBox, String trackerFill, int decPlaces)
	: plantX(-99), plantY(-99), sliderType(kind), compName(caption), cl(colour), tracker(trackerFill)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	slider = new Slider(text);
	slider->toFront(true);
	addAndMakeVisible(slider);
	addAndMakeVisible(groupbox);
	groupbox->setVisible(false);
	groupbox->getProperties().set("groupLine", var(1));
	if(tracker.length()>0)
	slider->getProperties().set("tracker", tracker);


    slider->getProperties().set("decimalPlaces", decPlaces);
	//Logger::writeToLog(fontColour);
	slider->getProperties().set("fontcolour", fontColour);
	//slider->getProperties().set("valueFontColour", Colour::fromString(cl).contrasting().toString()); 

	if(textBox<1) 
		slider->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);

	this->setWantsKeyboardFocus(false);

	resizeCount = 0;
}//--- end of constructor ----

//---------------------------------------------
~CabbageSlider()
{
}

//---------------------------------------------
void resized()
{
	//if rotary
	if (sliderType.contains("rotary")) {
		if(cl.length() > 0)
			slider->setColour(0x1001200, Colour::fromString(cl));
		slider->setSliderStyle(Slider::Rotary);
		getProperties().set("type", var("rslider"));
		slider->setSliderStyle(Slider::RotaryVerticalDrag);
		slider->setRotaryParameters(float_Pi * 1.2f, float_Pi * 2.8f, false);
		//if using group caption
		if (compName.length() > 0) {  
			groupbox->setBounds(0, 0, getWidth(), getHeight());
			int textHeight = 20;
			int availableHeight = getHeight()-textHeight;
			offY = textHeight + 5;
			int sliderHeight = availableHeight - 10;
			int sliderWidth = sliderHeight;
			offX = (getWidth()/2) - (sliderWidth/2);
			slider->setBounds(offX, offY, sliderWidth, sliderHeight); 
			groupbox->setText (compName);
			groupbox->setVisible(true);
			slider->toFront(true);
		}
		//else if no group caption then the slider takes the whole area available
		else 
			slider->setBounds(0, 0, getWidth(), getHeight());
	}
	//else if vertical
	else if (sliderType.contains("vertical")) {
		slider->setSliderStyle(Slider::LinearVertical);
		if(cl.length() > 0)
			slider->setColour(Slider::thumbColourId, Colour::fromString(cl));
		if (compName.length() > 0) {
			groupbox->setBounds(0, 0, getWidth(), getHeight());
			int textHeight = 20;
			int availableHeight = getHeight()-textHeight;
			offY = textHeight + 5;
			int sliderHeight = availableHeight - 10;
			int sliderWidth = getWidth() - 20;
			offX = (getWidth()/2) - (sliderWidth/2);
			slider->setBounds(offX, offY, sliderWidth, sliderHeight); 
			groupbox->setVisible(true);
			groupbox->setText(compName);
			slider->toFront(true);
		}
		else 
			slider->setBounds(0, 0, getWidth(), getHeight());
	}
	//else if horizontal
	else {
		slider->setSliderStyle(Slider::LinearHorizontal);
		if(cl.length() > 0)
			slider->setColour(Slider::thumbColourId, Colour::fromString(cl));
		if (compName.length() > 0) {
			groupbox->setBounds(0, 0, getWidth(), getHeight());
			int textHeight = 20;
			int availableHeight = getHeight()-textHeight;
			offY = textHeight + 5;
			int sliderHeight = availableHeight - 10;
			int sliderWidth = getWidth() - 20;
			offX = (getWidth()/2) - (sliderWidth/2);
			slider->setBounds(offX, offY, sliderWidth, sliderHeight); 
			groupbox->setText(compName);
			groupbox->setVisible(true);
			slider->toFront(true);
		}
		else 
			slider->setBounds(0, 0, getWidth(), getHeight());
	}

	//We only store the original dimensions the first time resized() is called.
	//Otherwise we would be passing incorrect values to the slider l+f methods...
	if (resizeCount == 0) {
		slider->getProperties().set(String("origHeight"), getHeight());
		slider->getProperties().set(String("origWidth"), getWidth());
		slider->getProperties().set(String("origX"), getX());
		slider->getProperties().set(String("origY"), getY());
	}
	resizeCount++;
	this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageSlider);
};

//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbageCheckbox : public Component
{
int offX, offY, offWidth, offHeight;

public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<ToggleButton> button;
//---- constructor -----
CabbageCheckbox(String name, String caption, String buttonText, String colour, String fontcolour, bool isRect)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	groupbox = new GroupComponent(String("groupbox_")+name);
	button = new ToggleButton(name);

	addAndMakeVisible(groupbox);
	addAndMakeVisible(button);
	groupbox->setVisible(false);
	groupbox->getProperties().set("groupLine", var(1));
	
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colour::fromString(colour));
	//text colour ID
	groupbox->setColour(0x1005410,
		Colour::fromString(colour));

	button->setButtonText(buttonText);
	if(caption.length()>0){
		offX=10;
		offY=20;
		offWidth=-20;
		offHeight=-30;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}

	button->getProperties().set("isRect", isRect);
	button->getProperties().set("colour", colour);
	button->getProperties().set("fontcolour", fontcolour);
	//text colour id
	button->setColour(0x1006501,
			Colour::fromString(colour));
	button->setButtonText(buttonText);
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbageCheckbox(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
button->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageCheckbox);
};


//==============================================================================
// custom checkbox component with optional surrounding groupbox
//==============================================================================
class CabbageComboBox : public Component
{
//ScopedPointer<LookAndFeel> lookFeel;
int offX, offY, offWidth, offHeight;
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<ComboBox> combo;
//---- constructor -----
CabbageComboBox(String name, String caption, String text, String colour, String fontColour)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	combo = new ComboBox(name);
	
	addAndMakeVisible(groupbox);
	addAndMakeVisible(combo);
	groupbox->setVisible(false);
	groupbox->getProperties().set("groupLine", var(1));
	combo->setColour(ComboBox::textColourId, Colour::fromString(fontColour));
	combo->getProperties().set("colour", colour);
	combo->getProperties().set("fontcolour", fontColour);
	//combo->setColour(ComboBox::ColourIds::textColourId, Colour::fromString(fontColour));
	//outline colour IDE
	groupbox->setColour(0x1005400,
		Colour::fromString(colour));
	//text colour ID
	groupbox->setColour(0x1005410,
		Colour::fromString(colour));
	
	if(caption.length()>0){
		offX=10;
		offY=35;
		offWidth=-20;
		offHeight=-45;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	//text colour ID
	combo->setColour(0x1000a00,
			Colours::findColourForName("white", Colours::grey));


	combo->setEditableText (false);
    combo->setJustificationType (Justification::centredLeft);
	combo->setTextWhenNothingSelected(text);
	this->setWantsKeyboardFocus(false);

}
//---------------------------------------------
~CabbageComboBox(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
combo->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageComboBox);
};

//==============================================================================
// custom image component
//==============================================================================
class CabbageImage : public Component
{
public:
	CabbageImage(String name, String file, String outline, String fill, String shape, int line):
	Component(name), picFile(file), file(file), fill(fill), outline(outline), shape(shape), line(line){
		setName(name);
		//toBack();
		img = ImageCache::getFromFile (File (file));
		this->setWantsKeyboardFocus(false);
		//this->setInterceptsMouseClicks(false, true);
	}
	~CabbageImage(){
	}

private:
	Image img;
	int top, left, width, height, line;
	String fill, outline, shape, file;
	File picFile;	

	void paint (Graphics& g){
		if(file.length()>5){
		g.drawImage(img, 0, 0, width, height, 0, 0, img.getWidth(), img.getHeight());
		}
		else{
			if(shape.contains("rounded")){
				g.setColour(Colour::fromString(outline));
				g.drawRoundedRectangle(0,0, width, height, width*.02, line);
				g.setColour(Colour::fromString(fill));
				g.fillRoundedRectangle(line,line, width-(line*2), height-(line*2), width*.02);				
			}
			else if(shape.contains("ellipse")){
				g.setColour(Colour::fromString(outline));
				g.drawEllipse(0,0, width, height, line);
				g.setColour(Colour::fromString(fill));
				g.fillEllipse(line,line, width-(line*2), height-(line*2));				
			}
			else if(shape.contains("sharp")){
				g.setColour(Colour::fromString(outline));
				g.drawRect(0,0, width, height, line);
				g.setColour(Colour::fromString(fill));
				g.fillRect(line,line, width-(line*2), height-(line*2));				
			}
		}

	}

	void resized(){
	top = getY();
	left = getX();
	width = getWidth();
	height = getHeight();
	}
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageImage);
};

//==============================================================================
// custom groupbox component, this can act as a plant for other components 
//==============================================================================
class CabbageGroupbox : public GroupComponent
{
OwnedArray<Component> comps;
int offX, offY, offWidth, offHeight;
public:
//---- constructor -----
CabbageGroupbox(String name, String caption, String text, String colour, String fontColour, int line):GroupComponent(name)
{
		toBack();
        offX=offY=offWidth=offHeight=0;
        if(colour.length()>0){
		//outline colour iD
        setColour(0x1005400,
                Colour::fromString(colour));
		//text colour iD
        setColour(0x1005410,
                Colour::fromString(colour));
        }
		this->getProperties().set("colour", colour);
		
		this->getProperties().set("fontcolour", fontColour);

        this->setText(text);
		this->setWantsKeyboardFocus(false);
		if(line==0)
		this->getProperties().set("groupLine", var(0));
		else
		this->getProperties().set("groupLine", var(1));
		this->repaint();

		setName(name);

}
//---------------------------------------------
~CabbageGroupbox(){

}

//---------------------------------------------
void resized()
{
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGroupbox);
};

//==============================================================================
// custom directorylist
//==============================================================================
class CabbageDirectoryList	:	public Component
{
public:
ScopedPointer<DirectoryContentsComponent> directoryList;
String name, channel, workingDir, fileType;
//---- constructor -----
public:
	CabbageDirectoryList (String _name, String _channel, String _workingDir, String _fileType):
																			name(_name),
																			channel(_channel),
																			workingDir(_workingDir),
																			fileType(_fileType)
	{
	setName(name);
	directoryList = new DirectoryContentsComponent(workingDir, fileType);
	addAndMakeVisible(directoryList);
	}

	~CabbageDirectoryList(){}

	void resized()
	{
	directoryList->setBounds(0, 0, getWidth(), getHeight());
	}

	void paint(Graphics& g){}
	const StringArray getListContents(){
		StringArray test = directoryList->getFunctionTables();
		//Logger::writeToLog(test.joinIntoString("\n"));
		
		return directoryList->getFunctionTables();
	}

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageDirectoryList);
};

//==============================================================================
// custom multitab
//==============================================================================
class CabbageMultiTab	:	public Component
{
public:
ScopedPointer<TabbedComponent> tabComp;
String name, fontcolour, bgcolour;
//---- constructor -----
public:
	CabbageMultiTab (String _name,  String _fontcolour, String _bgcolour):
										name(_name),
										fontcolour(_fontcolour),
										bgcolour(_bgcolour)
	{
	setName(name);
	tabComp = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	tabComp->setOutline(0);	
	addAndMakeVisible(tabComp);
	}

	~CabbageMultiTab(){}

	void resized()
	{
	tabComp->setBounds(0, 0, getWidth(), getHeight());
	}

	void paint(Graphics& g){}


private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageMultiTab);
};

//==============================================================================
// custom CabbageXYController
//==============================================================================
class CabbageXYController : public Component
{
//ScopedPointer<LookAndFeel> lookFeel;
int offX, offY, offWidth, offHeight;
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<XYPad> xypad;
int XYAutoIndex;
//---- constructor -----
CabbageXYController(XYPadAutomation* xyAuto, 
					String name, 	
					String text, 
					String caption, 
					int minX, int maxX, int minY, int maxY, int index, int dec, 
					String colour, 
					String fontcolour,
					float xValue, 
					float yValue)
{
	setName(name);
	XYAutoIndex = index;
	offX=offY=offWidth=offHeight=0;
	caption="";
	groupbox = new GroupComponent(String("groupbox_")+name);
	groupbox->setWantsKeyboardFocus(false);
	xypad = new XYPad(xyAuto, text, minX, maxX, minY, maxY, dec, Colour::fromString(colour), Colour::fromString(fontcolour), xValue, yValue);
	xypad->setWantsKeyboardFocus(false);
	addAndMakeVisible(xypad);
	addAndMakeVisible(groupbox);

	groupbox->setVisible(false);
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colours::findColourForName("white", Colours::white));
	//outline text ID
	groupbox->setColour(0x1005410,
		Colours::findColourForName("white", Colours::white));
	
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbageXYController(){

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
xypad->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageXYController);
};


//==============================================================================
// custom Csound message console
//==============================================================================
class CabbageMessageConsole : public Component
{
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<LookAndFeel_V1> lookAndFeel;
String text;
int offX, offY, offWidth, offHeight;
public:
	ScopedPointer<TextEditor> editor;
//---- constructor -----
CabbageMessageConsole(String name, String caption, String text):
		  editor(new TextEditor(String("editor_")+name)), 
		  groupbox(new GroupComponent(String("groupbox_")+name)),
		  lookAndFeel(new LookAndFeel_V1()),
		  offX(0), 
		  offY(0), 
		  offWidth(0), 
		  offHeight(0),
		  text(text)
{	
	editor->setLookAndFeel(lookAndFeel);
	addAndMakeVisible(editor);
	editor->setMultiLine(true);
	editor->setScrollbarsShown(true);
	editor->setReturnKeyStartsNewLine(true);
	editor->setReadOnly(true);
	//background colour ID
	editor->setColour(0x1000200, Colours::black);
	//text colour ID
	editor->setColour(0x1000201, Colours::green);

	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbageMessageConsole(){

}

void paint(Graphics &g){
	//----- For drawing the border 
	g.setColour(CabbageUtils::getComponentSkin());
	g.fillRoundedRectangle (0, -3, getWidth(), getHeight(), 8.f);
	g.setColour(Colours::black);
	g.fillRoundedRectangle (5, getHeight()-35, getWidth()-10, 20, 8.f);

	//----- For drawing the title
	g.setColour (Colours::whitesmoke);
	g.setOpacity (0.8);
	g.setFont (15);
	Justification just(1);
	g.drawText(text, 10, -5, getWidth()-20, 35, just, false); 

}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
editor->setBounds(offX+5, offY+20, (getWidth()+offWidth)-10, (getHeight()+offHeight)-45); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageMessageConsole);
};

//==============================================================================
// custom VUComponent
//==============================================================================
class VUComponent	:	public Component
{
public:


	//=================================================================================
	//		 Nested class VU Meter
	//=================================================================================
	class VUMeter	:	public Component
	{
	public:
		//=============================================================================
		VUMeter(int input1, bool input2)
		{
			//-----1 = Mono, 2 = Left Ch., 3 = Right Ch.
			type = input1;	

			//----- Horizontal or Vertical
			style = input2;	

			clipFlag = 0;	//0 is off, 1 is on

			//----- Declarations for each db value. The range will go logarithmically from -infinity to +3db. 
			//A level of 0.9 is being assigned to 0db. 
			plus3DB = 1;
			zeroDB = 0.9;
			minus3DB = 0.71 * zeroDB; //0.639
			minus10DB = 0.3162 * zeroDB; //0.285
			minus20DB = 0.1 * zeroDB; //0.09
			minus40DB = 0.01 * zeroDB; //0.009
		}

		//====================================================================================
		~VUMeter(){}

		//========== Resizing method =========================================================
		void resized ()
		{
			//----- Declaring dimensional properties
			top = 5;
			clipZone = 2;									//Leaving a gap for the clip marker at the top
			bottom = getHeight() - 5;

			if (type == 1)
				levelWidth = getWidth()/5;					//width of mono level				
			else if ((type == 2) || (type == 3))
				levelWidth = (getWidth()/1.25) / 5;			//width of stereo level should be the same as if it were mono

			levelHeight = bottom - (top + clipZone);		//height available to the VU part
			xOffset = getWidth()/5;							//Mono and Left Ch. VU's will be drawn 1/5 of totalWidth from left
			yOffset = 0;
			prevLevel = bottom;
			currentLevel = bottom;

			//----- Setting the location of the db markers
			plus3Mark = bottom - levelHeight;
			zeroMark = bottom - (0.9 * levelHeight);
			minus3Mark = bottom - (0.8 * levelHeight);
			minus10Mark = bottom - (0.5 * levelHeight);
			minus20Mark = bottom - (0.25 * levelHeight);
			minus40Mark = bottom - (0.1 * levelHeight);

			//----- Setting up the gradient fill for the meter level
			cg = ColourGradient (Colours::transparentBlack, 0, bottom, 
								Colours::transparentBlack, 0, top, false);
			cg.addColour (0.07, Colours::aqua);
			cg.addColour (0.2, Colours::lime);
			cg.addColour (0.5, Colours::lime);
			cg.addColour (0.6, Colours::yellow);
			cg.addColour (0.75, Colours::yellow);
			cg.addColour (0.85, Colours::orange);
			cg.addColour (0.999, Colours::orangered);

			clClip = (Colours::red);

			//----- Calling function to draw image background. This only applies if the VU is mono or the 
			//left channel of a stereo meter. Right channel meters will not use a background as they can just
			//draw over the left channel background image. 
			if ((type == 1) || (type == 2))
					verticalBackground();
		}


		//===== Vertical Background =======================================================================
		void verticalBackground ()
		{
			//----- This function draws the background onto a blank image and then loads it into cache. The 
			//cached image is then reused in the paint() method. This is a more efficient way to redrawing something
			//that is static. 

			// Creating a blank canvas
			img = Image(Image::ARGB, getWidth(), getHeight(), true);
			
			Graphics g (img);
			Colour bg = Colour::fromRGBA (10, 10, 15, 255);
			g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 2);
		
			//----- Painting the db level markers
			g.setColour (Colours::white);

			int fontSize = getHeight()*0.05;
			if (fontSize > 15)
				fontSize = 15;
			g.setFont (fontSize);

			int startText;	//starting x value
			if (type == 1)
				startText = getWidth() * 0.4;
			else if (type == 2)
				startText = (getWidth() * 0.4) * 1.25; //stereo VU's are 1.25 times the size of a mono VU

			Justification just (4);			//Centered
			g.drawText ("+3", startText, plus3Mark-5, getWidth()*.4, 10, just, false);
			g.drawText ("0", startText, zeroMark-5, getWidth()*.4, 10, just, false);
			g.drawText ("-3", startText, minus3Mark-5, getWidth()*.4, 10, just, false);
			g.drawText ("-10", startText, minus10Mark-5, getWidth()*.4, 10, just, false);
			g.drawText ("-20", startText, minus20Mark-5, getWidth()*.4, 10, just, false);
			g.drawText ("-40", startText, minus40Mark-5, getWidth()*.4, 10, just, false);

			//----- Painting the level background
			g.setGradientFill (cg);
			g.setOpacity (0.1);
			g.fillRect (xOffset, clipZone, levelWidth, levelHeight);

			if (type == 2)   //if stereo we need another meter level
				g.fillRect (xOffset+levelWidth+1, clipZone, levelWidth, levelHeight);

			//---- For the clip zone
			g.setColour (clClip);
			g.setOpacity (0.2);
			g.fillRect (xOffset, top, levelWidth, clipZone);

			if (type == 2)   //if stereo we need another clip zone
				g.fillRect (xOffset+levelWidth+1, top, levelWidth, clipZone);

			//----- Loading image into cache
			if (type == 1)
				ImageCache::addImageToCache (img, 11);
			else if (type == 2)
				ImageCache::addImageToCache (img, 12);
		}

				
		//========= Paint Method ====================================================================
		void paint (Graphics &g)
		{
			Image bg;
			//----- Drawing the background from the imagecache
			if (type == 1)
				bg = ImageCache::getFromHashCode(11);
			if (type == 2)
				bg = ImageCache::getFromHashCode(12);

			g.drawImage (bg, 0, 0, getWidth(), getHeight(), 0, 0, bg.getWidth(), bg.getHeight(), false);
			

			//----- Drawing the meter level. When paintFlag is 1 the meter level is to be increased. This
			//new bit of the level is painted using the gradient fill cg. If paintFlag is 0, the level is 
			//to be decreased. Because there is no colour or drawing tool used for paintFlag=0, this bit of 
			//the level is cleared. 
			if (paintFlag == 1) {
				g.setGradientFill (cg);
				g.setOpacity (0.7);
				if ((type == 1) || (type == 2))
					g.fillRect (xOffset, currentLevel, levelWidth, diff);
				else
					g.fillRect ((xOffset+levelWidth+1), currentLevel, levelWidth, diff);
			}
			

			//----- Determining if the clipmarker should be shown. It is set back to 0 immediately as it
			//does not need to repainted over and over. If the clipFlag is 0, the marker will stay on if 
			//the repaint() bounds are not inclusive of the clipZone. When the user clicks on the VU meter
			//the clipFlag will be 0 and the repaint() bounds will include the clipZone (see mouseDown()), 
			//therefore turning off the clipmarker. This happens because there is no colour or draw function 
			//used for clipFlag = 0, and therefore this zone is cleared exposing the background again. 
			if (clipFlag == 1) {
				g.setColour (clClip);
				if (type == 1)
					g.fillRect (xOffset, top, levelWidth, clipZone);
				else
					g.fillRect(xOffset, top, (levelWidth*2) + 1, clipZone);

				clipFlag = 0;
			}	
		}

		
		//========= Set Level =========================================================================
		void setLevel (float value)
		{
			level = value;

			// If level is 1 or more...
			if (level >= 1) {
				clipFlag = 1;
				level = 1;
			}

			//----- The following if statements determine the offset on the y axis. Each zone 
			//itself has to be treated independently to the rest of the level range.
			if ((level >= 0) && (level < minus40DB)){
				currentLevel = (bottom - minus40Mark) * (level / minus40DB);
				currentLevel = bottom - currentLevel;
			}
			//----- If level is between 0.009 and 0.09
			else if ((level >= minus40DB) && (level < minus20DB)){
				currentLevel = (minus40Mark-minus20Mark) * ((level - minus40DB) / (minus20DB - minus40DB));
				currentLevel = minus40Mark - currentLevel;
			}
			//----- If level is between 0.285 and 0.09
			else if ((level >= minus20DB) && (level < minus10DB)){
				currentLevel = (minus20Mark-minus10Mark) * ((level - minus20DB) / (minus10DB - minus20DB));
				currentLevel = minus20Mark - currentLevel;
			}
			//----- If level is between 0.285 and 0.639
			else if ((level >= minus10DB) && (level < minus3DB)){
				currentLevel = (minus10Mark-minus3Mark) * ((level - minus10DB) / (minus3DB - minus10DB)); 
				currentLevel = minus10Mark - currentLevel;
			}
			//----- If level is between 0.639 and 0.9
			else if ((level >= minus3DB) && (level < zeroDB)){
				currentLevel = (minus3Mark-zeroMark) * ((level - minus3DB) / (zeroDB - minus3DB));
				currentLevel = minus3Mark - currentLevel;
			}
			//----- If level is 0.9 or over
			else if (level >= zeroDB) {
				currentLevel = (zeroMark-plus3Mark) * ((level*10)-9);
				currentLevel = zeroMark - currentLevel;
			}

			//----- We only need to repaint the level difference between this value and the previous.
			//This is much more efficient than repainting the entire component each time. 
			diff = prevLevel - currentLevel;

			if (diff > 0) {
				paintFlag = 1; // to indicate that we are adding to the current meter level
				if ((type == 1) || (type == 2))
					repaint(xOffset, currentLevel, levelWidth, diff);
				else
					repaint((xOffset+levelWidth+1), currentLevel, levelWidth, diff);
			}
			else if (diff < 0) {
				diff *= -1;
				paintFlag = 0;	//to  indicate that we are subtracting from the current meter level
				if ((type == 1) || (type == 2))
					repaint(xOffset, prevLevel, levelWidth, diff);
				else
					repaint((xOffset+levelWidth+1), prevLevel, levelWidth, diff);
			}
			//else if diff = 0 then do nothing!

			//----- To see if clip marker should be shown.....
			if (clipFlag == 1) {
				if ((type == 1) || (type == 2))
					repaint (xOffset, top, levelWidth, clipZone);
				else
					repaint((xOffset+levelWidth+1), top, levelWidth, clipZone);
			}
			
			
			// Making the current level the previous level, for the next pass through....
			prevLevel = currentLevel;
		}


		//======== If mouse is clicked ================================================================
		void mouseDown (const MouseEvent& e)
		{
			//----- Getting (x,y) of the mouse click
			int x = e.getPosition().getX();
			int y = e.getPosition().getY();

			//----- If the mouse is clicked over the VU, then the clip marker will be
			//turned off. 
			if ((x >= getWidth()/5) && (x <= getWidth()*0.8) &&
				(y >= 0) && (y <= getHeight())) { 
					clipFlag = 0;
					// Only need to repaint the clipzone. Stereo VU's just repaint both
					//clip zones as it will not make much difference in terms of CPU usage.
					
					if (type == 1) //mono
						repaint (xOffset, top, levelWidth, clipZone);
					else	//stereo
						repaint(xOffset, top, (levelWidth*2) + 1, clipZone);
			}
		}

	private:
		bool style;
		int type, widthMeter, heightMeter;
		float level;
		int clipFlag, clipZone;
		int top, bottom, xOffset, yOffset, levelWidth, levelHeight;
		float plus3DB, zeroDB, minus3DB, minus10DB, minus20DB, minus40DB;
		int plus3Mark, zeroMark, minus3Mark, minus10Mark, minus20Mark, minus40Mark;
		Colour clClip;
		ColourGradient cg;
		Image img;

		int paintFlag;
		int currentLevel, prevLevel;
		int diff;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter);

	};
	//====== End of VUMeter class ===========================================


	//===== VU Component Constructor ========================================
	VUComponent (Array<int> conArr, bool useVertical)
	{
		style = useVertical;
		numArrElements = 0;
		totalNumLevels = 0;
		numMeters = 0;

		for (int i=0; i<conArr.size(); i++) {
			//Ignoring anything that is not either 1 or 2...
			if ((conArr[i] == 1) || (conArr[i] == 2)) {
				config.add (conArr[i]);
				numArrElements++;
				totalNumLevels += conArr[i]; //needed for indexing
				//numMeters allows us to get the correct width of each meter, as stereo
				//meters are 1.25 times as wide as mono ones...
				if (conArr[i] == 1)
					numMeters += 1;
				else
					numMeters += 1.25;
			}
		}	
	}

	//===== Destructor ====================================================
	~VUComponent()
	{
	}

	//===== Resize ========================================================
	void resized()
	{
		float gap = getWidth() * 0.07; //gap between meters
		float totalGap = gap * (numArrElements-1);	//all gaps added up
		float availableWidth = getWidth() - totalGap;
		float widthMonoMeter = availableWidth / numMeters;
		float widthStereoMeter;
		
		//If there is only 1 element in the array it should take up the whole width
		if ((numArrElements == 1) && (totalNumLevels ==2))
			widthStereoMeter = availableWidth;
		else
			widthStereoMeter = widthMonoMeter * 1.25;

		//----- The following for loop adds each level with a unique index. xOffset is the 
		//x coordinate at which they are drawn. 
		int arrElement = 0;
		float xOffset = 0;
		for (int i=0; i<totalNumLevels; i++) {
			//----- Adding a mono meter
			if (config[arrElement] == 1 ) {
				meters.add (new VUMeter(1, style));
				meters[i]->setBounds (xOffset, 0, widthMonoMeter, getHeight());
				addAndMakeVisible (meters[i]);
				xOffset += widthMonoMeter;
				if (arrElement != numArrElements) //if not the last element then add a gap
					xOffset += gap;
				arrElement++;
			}
			//----- Adding a stereo meter
			else if (config[arrElement] == 2 ) {
				//Left side
				meters.add (new VUMeter(2, style));
				meters[i]->setBounds (xOffset, 0, widthStereoMeter, getHeight());
				addAndMakeVisible (meters[i]);

				//Right side
				i++; //i needs to be incremented again to make sure that the index numbers are correct
				meters.add (new VUMeter(3, style));
				meters[i]->setBounds (xOffset, 0, widthStereoMeter, getHeight());
				addAndMakeVisible (meters[i]);

				xOffset += widthStereoMeter;
				if (arrElement != numArrElements) //if not the last element then add a gap
					xOffset += gap;
				arrElement++;
			}			
		}
		
	}

	//===== Set VU Level Function =========================================
	void setVULevel (int VUIndex, float value)
	{
		meters[VUIndex]->setLevel (value);
	}

private:
	OwnedArray<VUMeter> meters;
	bool style;
	int widthMeter, heightMeter;
	int totalNumLevels;
	float numMeters;
	Array<int> config;
	int numArrElements;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUComponent);
};



//==============================================================================
// custom CabbageVUMeter
//==============================================================================
class CabbageVUMeter : public Component
{
//ScopedPointer<LookAndFeel> lookFeel;
int offX, offY, offWidth, offHeight, numMeters, width, height;
String name;
public:
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<VUComponent> vuMeter;
//---- constructor -----
CabbageVUMeter(String name, String text, String caption, Array<int> config):
numMeters(0)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	numMeters = 0;
	for(int i =0;i<config.size();i++)
		numMeters = numMeters+config[i];

	vuMeter = new VUComponent(config, true);
	addAndMakeVisible(vuMeter);
	addAndMakeVisible(groupbox);

	groupbox->setVisible(false);
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colours::findColourForName("white", Colours::white));
	//text colour ID
	groupbox->setColour(0x1005410,
		Colours::findColourForName("white", Colours::white));
	
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbageVUMeter(){

}

int getNoMeters(){
	return numMeters;
}

//---------------------------------------------
void resized()
{
groupbox->setBounds(0, 0, getWidth(), getHeight()); 
vuMeter->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageVUMeter);
};

//==============================================================================
// custom CabbageTable, uses the Table class
//==============================================================================
class CabbageTable : public Component
{
//ScopedPointer<LookAndFeel> lookFeel;
int offX, offY, offWidth, offHeight, tableSize;
StringArray colours, channels;
Array<int> tableSizes, tableNumbers, drawingModes, resizingModes;
Array< Point<float> > minMax;
float alpha;
public:
bool readOnly;
ScopedPointer<GroupComponent> groupbox;
ScopedPointer<CabbageTableManager> table;
ScopedPointer<TextButton> button;
ScopedPointer<ChangeListener> listener;
//---- constructor -----
CabbageTable(String name, String text, String caption, StringArray chans, 
													   Array<int> tblNumbers, 
													   Array<int> tblSize, 
													   Array<int> drawingModes,
													   Array<int> resizingModes,
													   Array<float> ampRanges,
													   StringArray Colours, 
													   bool readOnly, 
													   ChangeListener* listen)
: tableSizes(tblSize), 
  colours(Colours), 
  channels(chans),
  readOnly(readOnly),
  listener(listen),
  tableNumbers(tblNumbers),
  drawingModes(drawingModes),
  resizingModes(resizingModes)
{
	setName(name);

	for(int i=0;i<ampRanges.size();i+=2){
		minMax.add(Point<float>(ampRanges[i], ampRanges[i+1]));
	}


	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	groupbox->setWantsKeyboardFocus(false);
	tableSize = tableSizes[0];
	table = new CabbageTableManager(tableSize);
	
	//add extra colours if user never specified them
	//for(int i=colours.size();i<=tableSizes.size();i++)
			//colours.add(Colour::fromString(colours[0]).withBrightness(1).toString());
	//		colours.add(Colours::lime.withBrightness(float(i)/tableSizes.size()).toString());
	
	
	// set up drawing modes for each table
	// 0: default mode, draws everything normally
	// 1: drawHoriztonalSegments
	// 2: make fixed point envelope
	// 3: fixed points and horizontal
	// 4: fixed points and horizontal and toggle max/min, editable from the onst
	// ....varous fill variations...

	for(int i=drawingModes.size();i<tableSizes.size();i++)
		drawingModes.add(0);
	
	addAndMakeVisible(table);
	addAndMakeVisible(groupbox);

	groupbox->setVisible(false);
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colours::findColourForName("white", Colours::white));
	//text colour ID
	groupbox->setColour(0x1005410,
		Colours::findColourForName("white", Colours::white));
	
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	this->setWantsKeyboardFocus(false);

}


void setDrawingModeBooleans(bool &fixedEnv, bool &drawHorizontal, bool &toggleMaxMin, bool &drawOriginal, bool &drawFill, int mode)
{
if(mode==1){
	drawHorizontal = true;
	fixedEnv = false;
	toggleMaxMin= false;
	drawOriginal = true;
	drawFill = false;
	}
else if(mode==2){
	drawHorizontal = false;
	fixedEnv = true;	
	toggleMaxMin= false;
	drawOriginal = true;
	drawFill = false;
	}
else if(mode==3){
	drawHorizontal = true;
	fixedEnv = true;
	toggleMaxMin= false;
	drawOriginal = true;
	drawFill = false;
	}
else if(mode==4){
	drawHorizontal = true;
	fixedEnv = true;
	toggleMaxMin= true;	
	drawOriginal = false;
	drawFill = true;
	}
else{
	drawHorizontal = false;
	fixedEnv = false;
	toggleMaxMin= false;
	drawOriginal = true;
	drawFill = false;
	}
} 

//---------------------------------------------
~CabbageTable(){
listener.release();
}

void paint(Graphics& g){
//Logger::writeToLog("test");	
}


void addTables(){
bool fixed, horizontal, toggleMaxMin, 	drawOriginal, drawFill;
if(table->getNumberOfTables()<tableNumbers.size())
if(tableNumbers.size()>1)
	for(int i=0;i<tableNumbers.size();i++){
		String name = "table"+String(tableNumbers[i]);
		setDrawingModeBooleans(fixed, horizontal, toggleMaxMin, drawOriginal, drawFill, drawingModes[i]);
		table->addTable(name, channels[i], tableNumbers[i], tableSizes[i], fixed, 
						horizontal, drawOriginal, toggleMaxMin, drawFill, 
						resizingModes[i], minMax[i], Colours::findColourForName(colours[i], Colours::white), readOnly, listener);
	}
else{	
	setDrawingModeBooleans(fixed, horizontal, toggleMaxMin, drawOriginal, drawFill, drawingModes[0]);
	String name = "table"+String(tableNumbers[0]);
	Logger::writeToLog(name);
	table->addTable("table0", channels[0], tableNumbers[0], tableSizes[0], fixed, 
					horizontal, drawOriginal, toggleMaxMin, drawFill, 
					resizingModes[0], minMax[0],
					Colours::findColourForName(colours[0], Colours::white), readOnly, listener);
}	
	
}
//---------------------------------------------
void fillTable(int ID, Array<double, CriticalSection> yValues){
	if(yValues.size()>1)
	table->fillTable(ID, yValues);
	table->repaint();
}

void setScrubberPosition(int ID, float position){
	//if(position>0)
	//table->setScrubberPosition(ID, position);
}
//---------------------------------------------
void resized()
{	

groupbox->setBounds(0, 0, getWidth(), getHeight()); 
table->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight);
//check that we have drawn enough tables


this->setWantsKeyboardFocus(false);

}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTable);
};

//==============================================================================
// custom PVSView, for viewing pvs
//==============================================================================
#ifndef Cabbage_No_Csound
class CabbagePVSTableData       :       public Component
{
public:
        CabbagePVSTableData(int frameSize, int fftsize, int overlap, PVSDATEXT* inSpecData, int minFreq, int maxFreq) :
                                                                                                                                                                                        specData(inSpecData),
                                                                                                                                                                                        frameSize(frameSize),
                                                                                                                                                                         minFreq(minFreq),
                                                                                                                                                                                        maxFreq(maxFreq)
        {
                freqRange = (maxFreq-minFreq);
        }

        ~CabbagePVSTableData()
        {
        }

        void resized()
        {
                if (freqRange >= getWidth())
                        lineThickness = 1;
                else
                        lineThickness = getWidth() / (float)freqRange;

                cg = ColourGradient (Colours::transparentBlack, 0, getHeight(), 
                                                                Colours::transparentBlack, 0, 0, false);
                cg.addColour (0.07, Colours::aqua);
                cg.addColour (0.2, Colours::lime);
                cg.addColour (0.5, Colours::lime);
                cg.addColour (0.6, Colours::yellow);
                cg.addColour (0.75, Colours::yellow);
                cg.addColour (0.85, Colours::orange);
                cg.addColour (0.999, Colours::orangered);

                cacheGridImage();

                currPoint.setXY(0, getHeight());

                frameSize = maxFreq/getWidth();
        }

        void cacheGridImage()
        {
                //----- This method stores the grid in cache
                grid = Image(Image::ARGB, getWidth(), getHeight(), true);
                        
                Graphics g (grid);
                g.setColour(CabbageUtils::getComponentFontColour());
                for (float i=0.0; i<=1.0; i+=0.125) {
                        g.drawLine(getWidth()*i, 0, getWidth()*i, getHeight(), .1);
                        g.drawLine(0, getHeight()*i, getWidth(), getHeight()*i, .1);
                }

                ImageCache::addImageToCache(grid, 19);
        }

        void paint(Graphics& g)
        {
                Image bgGrid = ImageCache::getFromHashCode(19);
                g.drawImage(bgGrid, 0, 0, getWidth(), getHeight(), 0, 0, bgGrid.getWidth(), bgGrid.getHeight(), false);

                g.setGradientFill(cg);

                //draw PVS data
                for (int i=0; i<specData->N*2; i+=2) {
                        if (specData->frame[i]) {
                                amp = specData->frame[i];
                                freq = int(specData->frame[i+1]);
                        
                                if ((amp > 0) && (amp <= 1) && (freq >= minFreq) && (freq <= maxFreq)) {
                                        //      g.fillEllipse ((((freq-minFreq)/(float)freqRange)*getWidth())-1, 
                                        //      (getHeight()-(amp*getHeight()))-1, 2, 2);

                                        g.drawLine(((freq-minFreq)/(float)freqRange)*getWidth(), getHeight()-(amp*getHeight()), 
                                                ((freq-minFreq)/(float)freqRange)*getWidth(), getHeight(), 1);

                                        //prevPoint = currPoint;
                                        //currPoint.setXY(((freq-minFreq)/(float)freqRange)*getWidth(), getHeight()-(amp*getHeight()));
                                        //if (currPoint.getX() > prevPoint.getX())
                                        //      g.drawLine(prevPoint.getX(), prevPoint.getY(), currPoint.getX(), currPoint.getY(), 2);

                                }

                        }
                }
        }

private:
        PVSDATEXT* specData;
        Array<int> freqValues;
        Array<MYFLT> ampValues;
        int frameSize, minFreq, maxFreq, freqRange;
        float lineThickness;
        ColourGradient cg;
        Image grid;
        float amp, maxAmp, freq;
        Point<float> currPoint, prevPoint;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePVSTableData);
};

//---------------------------------------------------------------------
class CabbagePVSView    :       public Component
{
public:
        CabbagePVSView(String name, String caption, String text, int frameSize, int fftsize, int overlap, 
                                                                                                                                                                PVSDATEXT* inSpecData): 
                                                                                                                                                                text(text)

        {
                setName(name);
                maxFreq = 2000;
                minFreq = 0;
                maxAmp = 1.0;
                pvsTableData = new CabbagePVSTableData (frameSize, fftsize, overlap, inSpecData, minFreq, maxFreq);             
        }

        ~CabbagePVSView()
        {
        }

        void resized()
        {
                this->setWantsKeyboardFocus(false);
                pvsTableData->setBounds(getWidth()*0.2, getHeight()*0.1, getWidth()*0.7, getHeight()*0.7);
                addAndMakeVisible(pvsTableData);

                cacheBackground();
        }

        void updatePVSStruct()
        {
                pvsTableData->repaint();
        }

        void cacheBackground()
        {
                bg = Image(Image::ARGB, getWidth(), getHeight(), true);
                Graphics g (bg);

                //background
                g.setColour(CabbageUtils::getDarkerBackgroundSkin());
                g.fillRoundedRectangle (0, 0, getWidth(), getHeight(), 5);

                //border
                g.setColour (CabbageUtils::getComponentSkin());
                g.drawRoundedRectangle (.5, .5, getWidth()-1, getHeight()-1, 5, 1);

                //Markers etc...
                g.setColour (Colours::lime);
                Font font = CabbageUtils::getSmallerValueFont();
                g.setFont(font);
                String strX, strY;
                float strWidth;
                for (float i=0.0; i<=1.0; i+=0.25) {
                        //x-axis markers
                        strX = String(maxFreq*i);
                        strWidth = font.getStringWidthFloat(strX);
                        g.drawText(strX, pvsTableData->getX()+((pvsTableData->getWidth()*i)-(strWidth/2)), pvsTableData->getBottom()+10,
                                strWidth, font.getHeight(), 36, false);

                        //y-axis markers
                        strY = String(maxAmp*i);
                        strWidth = font.getStringWidthFloat(strY); 
                        g.drawText(strY, pvsTableData->getX()-(strWidth+10), 
                                (pvsTableData->getBottom()-(pvsTableData->getHeight()*i)) - (font.getHeight()/2),
                                strWidth, font.getHeight(), 36, false);
                }               

                g.setColour (CabbageUtils::getComponentFontColour());
                Font labelFont = CabbageUtils::getComponentFont();
                String xLabel = "Frequency (Hz)";
                strWidth = labelFont.getStringWidthFloat(xLabel);
                g.setFont(labelFont);
                g.drawText(xLabel, pvsTableData->getX()+((pvsTableData->getWidth()/2)-strWidth/2), getHeight()-(labelFont.getHeight()+10), 
                        strWidth, labelFont.getHeight(), 36, false);

                String yLabel = "Amplitude";
                strWidth = labelFont.getStringWidthFloat(yLabel);
                //g.drawTextAsPath(yLabel, AffineTransform::identity.rotated(4.7, 150, pvsTableData->getHeight()*0.66));

                ImageCache::addImageToCache(bg, 20);
        }

        void paint(Graphics &g)
        { 
                Image img = ImageCache::getFromHashCode(20);
                g.drawImage(img, 0, 0, getWidth(), getHeight(), 0, 0, img.getWidth(), img.getHeight(), false);
        }

        
private:
        ScopedPointer<CabbagePVSTableData> pvsTableData;
        String text;
        int minFreq, maxFreq;
        float maxAmp;
        Image bg;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePVSView);
};

#endif

//==============================================================================
// custom SnapshotControl, used for saving and recalling presets
//==============================================================================
class CabbageSnapshot : public Component,
						public Button::Listener,
						public ActionBroadcaster,
						public ComboBox::Listener
{

ScopedPointer<TextButton> button;
ScopedPointer<GroupComponent> groupbox;
String name, preset, presetFileText;
int offX, offY, offWidth, offHeight;
bool masterSnapshot;

public:
ScopedPointer<ComboBox> combobox;
		
	CabbageSnapshot(String compName, String caption, String presetName, bool master):masterSnapshot(master), name(compName), preset(presetName){
	name << "|snapshot:";
	preset << "|snapshot:";
	setName(name);
	offX=offY=offWidth=offHeight=0;
	
	groupbox = new GroupComponent(String("groupbox_")+name);
	combobox = new ComboBox(name);
	combobox->addListener(this);
	button = new TextButton("Snapshot");
	button->addListener(this);
	


	addAndMakeVisible(combobox);
	addAndMakeVisible(groupbox);
	addAndMakeVisible(button);

	groupbox->setVisible(false);
	//outline colour ID
	groupbox->setColour(0x1005400,
		Colours::findColourForName("white", Colours::white));
	//text colour ID
	groupbox->setColour(0x1005410,
		Colours::findColourForName("white", Colours::white));
	
	if(caption.length()>0){
		offX=10;
		offY=15;
		offWidth=-20;
		offHeight=-25;
		groupbox->setVisible(true);
		groupbox->setText(caption);
	}
	}
	~CabbageSnapshot(){}

	void buttonClicked(Button* button){
	//CabbageUtils::showMessage(name+"save;"+String(combobox->getText().trim()));
		sendActionMessage(preset+"save;"+String(combobox->getText().trim())+String("?")+String(masterSnapshot));
	}

	void comboBoxChanged(ComboBox* combo){
		sendActionMessage(preset+"load;"+String(combobox->getText().trim())+String("?")+String(masterSnapshot));
	}
	//---------------------------------------------
	void resized()
	{
	groupbox->setBounds(0, 0, getWidth(), getHeight()); 
	combobox->setBounds(offX, offY+2, (getWidth()+offWidth)*.60, (getHeight()+offHeight)*.8); 
	button->setBounds(offX+(getWidth()+offWidth)*.60+5, offY, (getWidth()+offWidth)*.35, getHeight()+offHeight);
	}

};
//==============================================================================
// custom NumToggleButton
//==============================================================================
class CabbageNumToggle : public Component,
						 public ActionBroadcaster
{
String name;
Colour colour;
ScopedPointer<Label> label;
int width, height, value, lastValue;
bool buttonState, mouseButtonState;
public:
	  CabbageNumToggle(String name, int width, int height): 
					width(width),
					height(height),
					value(0),
					lastValue(0),
					buttonState(false),
					mouseButtonState(false),
					colour(Colours::lime)
	  {
		setBounds(0, 0, width, height);
		label = new Label();
		label->setText("", dontSendNotification);
		label->getProperties().set("textColour", "black");
		label->setBounds(0, 0, width, height);
		label->setEditable(false, true);
		label->setInterceptsMouseClicks(false, true);
		addAndMakeVisible(label);
		label->toFront(true);  
	  }

	  void mouseDrag (const MouseEvent &e){
		  if(e.mods.isRightButtonDown()&&buttonState)
		  if(e.getOffsetFromDragStart().getY()<0){
			  label->setText(String(value+(e.getDistanceFromDragStart()/2)), dontSendNotification);
			  lastValue = e.getDistanceFromDragStart()/2;
			  sendActionMessage(String("button:")+getName()+String("|state:")+String(buttonState)+String("|value:")+label->getText());
		  }
		  else{
			  label->setText(String(value-(e.getDistanceFromDragStart()/2)), dontSendNotification);
			  lastValue = value-(e.getDistanceFromDragStart()/2);
			  //send a message with the button number and its value
			  sendActionMessage(String("button:")+getName()+String("|state:")+String(buttonState)+String("|value:")+label->getText());
		  }

	  }

	  void mouseExit (const MouseEvent &e){
		if(e.mods.isRightButtonDown())	  
			value = lastValue;
	  }

	void mouseDown(const MouseEvent &e){
		if(e.mods.isLeftButtonDown())
		if(getToggleState()){
			buttonState=false;
			repaint();
			//send a message with the button number and its state
			  sendActionMessage(String("button:")+getName()+String("|state:")+String(buttonState)+String("|value:")+label->getText());
		}
		else{
			buttonState=true;
			repaint();
			  sendActionMessage(String("button:")+getName()+String("|state:")+String(buttonState)+String("|value:")+label->getText());
		}
	}

	bool getToggleState(){
		return buttonState;
	}

	void setToggleState(bool val){
		buttonState = val;
	}

	int getCurrentValue(){
		  return label->getText().getIntValue();
	  }

	void setActiveColour(String inColour){
		  colour = Colours::findColourForName(inColour, Colours::lime);
	  }

	void paint(Graphics &g){
		Image newButton = CabbageLookAndFeel::drawToggleImage (width, height, buttonState, colour, true);
		g.drawImage(newButton, 0, 0, width, height, 0, 0, width, height);
	}

};

//==============================================================================
// custom PatternMatrix
//==============================================================================
class PatternMatrix : public Component, 
					  public Timer,
					  public ButtonListener,
					  public SliderListener,
					  public ActionListener
{
ScopedPointer<CabbageCheckbox> onoffButton;
ScopedPointer<Slider> bpm;
OwnedArray<Slider> pSliders;
float buttonIndex, updateVar, offX, offY, offWidth, offHeight, 
noPatterns, noSteps, beat, currentStepButton, rCtrls;
bool timerActive;

public:
OwnedArray<CabbageNumToggle> stepButton;
CabbagePluginAudioProcessor* myFilter;
//---- constructor -----
	PatternMatrix(String caption, int width, int height, StringArray patterns, int steps, int rctrls, CabbagePluginAudioProcessor* filter)
							: timerActive(false), noSteps(steps), 
								noPatterns(patterns.size()), beat(0), myFilter(filter),
								currentStepButton(0),
								buttonIndex(0),
								updateVar(0),
								rCtrls(rctrls)
	{
		onoffButton = new CabbageCheckbox("Active", "", "", Colours::red.toString(), CabbageUtils::getComponentFontColour().toString(), true);

		onoffButton->button->setButtonText("");
		onoffButton->button->addListener(this);
		//onoffButton->getProperties().set(Colours, "red");
		onoffButton->setBounds(60, 25, 20, 20);

		if(myFilter->patMatrixActive)
			onoffButton->button->setToggleState(true, sendNotification);

		addAndMakeVisible(onoffButton);

		bpm = new Slider("bpm");
		bpm->setSliderStyle(Slider::LinearBar);
		bpm->setBounds(185, 27, 180, 15);
		bpm->setRange(0, 1000, 1);
		bpm->setValue(120);
		bpm->addListener(this);
		addAndMakeVisible(bpm);

		myFilter->noSteps = noSteps;
		myFilter->noPatterns = noPatterns;
		myFilter->patternNames = patterns;
		Rectangle<int> pattRect;
		//set bounds for pattern rectangle
		pattRect.setBounds(60, 50, width-(rCtrls*40)-60, (height-50)*.95);	
		//set bounds for slider rectangle
		Rectangle<int> slidersRect;
		slidersRect.setBounds(pattRect.getWidth()+60, 50, width-pattRect.getWidth()+60, (height-50)*.95); 

		//populate matrix with step buttons
		if(myFilter->patStepMatrix.size()==0)
		{
		for(int pats=0;pats<patterns.size();pats++)
			for(int beats=0;beats<steps;beats++){
				stepButton.add(new CabbageNumToggle("", (pattRect.getWidth()/noSteps), (pattRect.getHeight()/patterns.size())));
				stepButton[stepButton.size()-1]->addActionListener(this);
				stepButton[stepButton.size()-1]->setName(String(stepButton.size()-1));
//				stepButton[stepButton.size()-1]->setToggleState(myFilter->patStepMatrix[stepButton.size()-1].state);			
				CabbagePatternMatrixStepData patMat;
				myFilter->patStepMatrix.add(patMat);
				stepButton[stepButton.size()-1]->setTopLeftPosition(pattRect.getX()+(beats*(pattRect.getWidth()/noSteps)), pats*(pattRect.getHeight())/patterns.size()+pattRect.getY());
				addAndMakeVisible(stepButton[stepButton.size()-1]);
			}
		}
		else{
		for(int pats=0;pats<patterns.size();pats++)
			for(int beats=0;beats<steps;beats++){
				stepButton.add(new CabbageNumToggle("", (pattRect.getWidth()/noSteps), (pattRect.getHeight()/patterns.size())));
				stepButton[stepButton.size()-1]->addActionListener(this);
				stepButton[stepButton.size()-1]->setName(String(stepButton.size()-1));
				stepButton[stepButton.size()-1]->setToggleState(myFilter->patStepMatrix[stepButton.size()-1].state);			
				//CabbagePatternMatrixStepData patMat;
				//myFilter->patStepMatrix.add(patMat);
				stepButton[stepButton.size()-1]->setTopLeftPosition(pattRect.getX()+(beats*(pattRect.getWidth()/noSteps)), pats*(pattRect.getHeight())/patterns.size()+pattRect.getY());
				addAndMakeVisible(stepButton[stepButton.size()-1]);
			}
		}
		//add p-field controls if needed
	   for(int pats=0;pats<patterns.size();pats++){
			for(int i=0;i<rCtrls;i++){
				pSliders.add(new Slider());
				pSliders[pSliders.size()-1]->setSliderStyle(Slider::RotaryVerticalDrag);	
				pSliders[pSliders.size()-1]->addListener(this);
				pSliders[pSliders.size()-1]->setRange(0, 127, 1);
				pSliders[pSliders.size()-1]->setTextBoxIsEditable(true);
				pSliders[pSliders.size()-1]->showTextBox();
				String sliderName("row:"+String(pats)+"|ctrl:"+String(i));
				CabbagePatternMatrixPfieldData patMat;
				myFilter->patPfieldMatrix.add(patMat);
				pSliders[pSliders.size()-1]->getProperties().set("data", var(sliderName));
				pSliders[pSliders.size()-1]->setBounds(slidersRect.getX()+(i*40), (((pats*(slidersRect.getHeight())/patterns.size())))+slidersRect.getY(), 40, 40);
				addAndMakeVisible(pSliders[pSliders.size()-1]);
				}
			}
			
		}

	~PatternMatrix(){
		}


	void actionListenerCallback(const juce::String &value)
		{
			//button:1|state:1|value:127
			//sort out buttons first
			if(value.contains("button")){
			String buttonNum(value.substring(value.indexOf("button:")+7, value.indexOf("|st")));
			String buttonState(value.substring(value.indexOf("|state:")+7, value.indexOf("|val")));
			String buttonValue(value.substring(value.indexOf("|value:")+7, value.length()));
			if(buttonState.getIntValue()==1){
				myFilter->patStepMatrix.getReference(buttonNum.getIntValue()).state = buttonState.getIntValue();
				myFilter->patStepMatrix.getReference(buttonNum.getIntValue()).p4 = buttonValue.getIntValue();
			}
			else
				myFilter->patStepMatrix.getReference(buttonNum.getIntValue()).state = buttonState.getIntValue();
			}
			//and now sliders
		}
		
	void sliderValueChanged(juce::Slider *slider){
		String data = slider->getProperties().getWithDefault("data", String("").contains("row"));
		if(data.contains("row")){
		String sliderRow(data.substring(data.indexOf("row:")+4, data.indexOf("|ctr")));
		String sliderCtrl(data.substring(data.indexOf("|ctrl:")+6, data.length()));		
		if(sliderCtrl=="0")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p5 = slider->getValue();
		else if(sliderCtrl=="1")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p6 = slider->getValue();
		else if(sliderCtrl=="2")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p7 = slider->getValue();
		else if(sliderCtrl=="3")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p8 = slider->getValue();
		else if(sliderCtrl=="4")
		myFilter->patPfieldMatrix.getReference(sliderRow.getIntValue()).p9 = slider->getValue();
		}

	}

	void sliderDragEnded(Slider* slider)
		{
			if(slider->getName()=="bpm"){
			myFilter->bpm = slider->getValue();
			myFilter->timeCounter = 0;
			this->startTimer(15);
			}
		}

	void buttonClicked(Button *button)
		{
			if(button->getName()=="Active"){
				if(button->getToggleState()){
				this->startTimer(15);
				myFilter->patMatrixActive=1;
				}
				else{
				this->stopTimer();
				timerActive = false;
				myFilter->patMatrixActive=0;
				}
			}
		}

	void timerCallback(){
			for(int u=0;u<stepButton.size();u++){
			if(stepButton[u]->getToggleState()==1){
			stepButton[u]->setActiveColour("lime");
			stepButton[u]->repaint();
			}
			}
	
			for(int y=0;y<noPatterns;y++){
			//Logger::writeToLog(String(myFilter->beat+(y*noSteps)));
			if(stepButton[myFilter->beat+(y*noSteps)]->getToggleState()==1){			
			stepButton[myFilter->beat+(y*noSteps)]->setActiveColour("yellow");
			stepButton[myFilter->beat+(y*noSteps)]->repaint();
			}
			}

	}
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatternMatrix);
};

//==============================================================================
// CabbagePatternMatrix
//==============================================================================
class CabbagePatternMatrix : public Component
{
int offX, offY, offWidth, offHeight, tableSize, width, height, rCtrls;
String caption;
StringArray patterns;
public:
ScopedPointer<PatternMatrix> patternMatrix;
CabbagePluginAudioProcessor* myFilter;
//---- constructor -----
CabbagePatternMatrix(CabbagePluginAudioProcessor* filter, String name, int width, int height, String caption, StringArray patterns, int steps, int rctrls)
	: myFilter(filter), height(height), width(width), caption(caption), patterns(patterns), rCtrls(rctrls)
{
	setName(name);
	offX=offY=offWidth=offHeight=0;

	
	patternMatrix = new PatternMatrix(caption, width, height, patterns, steps, rCtrls, myFilter);

	addAndMakeVisible(patternMatrix);
	this->setWantsKeyboardFocus(false);
}
//---------------------------------------------
~CabbagePatternMatrix(){

}

void paint(Graphics& g){
	//----- Background
	Colour bg = Colours::white;
	g.setColour (bg);
	g.fillRoundedRectangle (0, 0, width, height, 5);

	//----- Outline
	g.setColour (CabbageUtils::getComponentFontColour());
	g.setOpacity (0.1);
	g.drawRoundedRectangle (0.5, 0.5, width-1, height-1, 5, 1);

	Font font (String("Impact"), 14, 0);
	font.setFallbackFontName (String("Verdana")); //in case the user doesn't have the first font installed
	g.setFont (font);
	Justification just (4);
	g.setColour (Colours::black);
	String name = CabbageUtils::cabbageString (caption, font, width);
	g.drawText (name, 0, 5, width, 13, just, false);
	g.drawLine (10, 20, width-10, 20, 0.3);

	for(int i=0;i<patterns.size();i++){
	Justification just (4);
	g.setColour (Colours::black);
	String name = CabbageUtils::cabbageString (patterns[i], font, 60);
	
	g.drawText(name, 0, (i*((height-50)/patterns.size()*.90))+50, 60, (height/patterns.size())*.90, just, false);
	}

	g.drawText("Active", 70, 25, 60, 20, just, false);
	g.drawText("BPM", 140, 25, 60, 20, just, false);


}

//---------------------------------------------
void resized()
{
patternMatrix->setBounds(offX, offY, getWidth()+offWidth, getHeight()+offHeight); 
this->setWantsKeyboardFocus(false);
}

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePatternMatrix);
};

//==============================================================================
// custom soundfiler
//==============================================================================
class CabbageSoundfiler	:	public Component
{
ScopedPointer<Soundfiler> soundFiler;

//---- constructor -----
public:
	CabbageSoundfiler (String name, String file, String colour, String fontcolour, CabbageAudioSource &audioSource, int sr)
	{
	setName(name);
	
	soundFiler = new Soundfiler(audioSource, file, sr, Colour::fromString(colour), Colour::fromString(fontcolour));
	addAndMakeVisible(soundFiler);
	}

	~CabbageSoundfiler()
	{
	}

	void resized()
	{
	soundFiler->setBounds(0, 0, getWidth(), getHeight());
	}

	void paint(Graphics& g)
	{

	}


private:
	String text, colour;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageSoundfiler);
};


//==============================================================================
// custom CabbageLabel
//==============================================================================
class CabbageLabel	:	public Component
{

public:
	CabbageLabel (String text, String colour)
		: text(text), colour(colour)
	{
	}

	~CabbageLabel()
	{
	}

	void resized()
	{

	}

	void paint(Graphics& g)
	{
		g.setColour(Colour::fromString(colour));
		g.setFont(CabbageUtils::getComponentFont());
		g.setFont(getHeight());
		g.drawFittedText(text, 0, 0, getWidth(), getHeight(), Justification::left, 1, 1);
	}


private:
	String text, colour;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLabel);
};


//==============================================================================
// custom CabbageTransportControl
//==============================================================================
/*
 * this will need a fast forward, skip to start, play/pause and 
 */ 
class CabbageTransportControl	:	public Component
{
ScopedPointer<ImageButton> playButton;	
ScopedPointer<ImageButton> skipToStartButton;
ScopedPointer<ImageButton> skipToEndButton;

public:
	CabbageTransportControl(int width, int height){
	playButton = new ImageButton("Play button");
	addAndMakeVisible(playButton);	
	skipToStartButton = new ImageButton("Skip to start button");
	addAndMakeVisible(skipToStartButton);
	skipToEndButton = new ImageButton("Skip to end button");
	addAndMakeVisible(skipToEndButton);
	
	//playButton->setToggleState(false, true);
	playButton->setClickingTogglesState(true);
	//playButton->setState(Button::buttonDown);
/*
	playButton->setImages(false, true, true, 
		CabbageUtils::drawSoundfilerButton("play_normal"), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("play_hover"), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("play_down"), 1.0f, Colours::transparentBlack);

	skipToStartButton->setImages(false, true, true, 
		CabbageUtils::drawSoundfilerButton("skip_start_normal"), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("skip_start_hover"), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("skip_start_down"), 1.0f, Colours::transparentBlack);

	skipToEndButton->setImages(false, true, true, 
		CabbageUtils::drawSoundfilerButton("skip_end_normal"), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("skip_end_hover"), 1.0f, Colours::transparentBlack,
		CabbageUtils::drawSoundfilerButton("skip_end_down"), 1.0f, Colours::transparentBlack);	
		 */
	}
	
	~CabbageTransportControl(){}
	
	void resized()
	{
		int numOfButtons = 3;
		float buttonWidth = getWidth() / (numOfButtons+1); //jmin(getHeight()*0.8f, (float)getWidth()/(numOfButtons+1));
		float buttonGap = buttonWidth / (numOfButtons+1); //(getWidth()-(buttonWidth*numOfButtons)) / numOfButtons;
		float buttonHeight = getHeight() - (buttonGap*2);

		skipToStartButton->setBounds(buttonGap, buttonGap, buttonWidth, buttonHeight);	
		playButton->setBounds(buttonGap*2 + buttonWidth, buttonGap, buttonWidth, buttonHeight);
		skipToEndButton->setBounds(buttonGap*3 + buttonWidth*2, buttonGap, buttonWidth, buttonHeight);	
	}

	void paint(Graphics& g) 
	{
		//g.setColour(CabbageUtils::getDarkerBackgroundSkin());
		g.setColour(Colours::lightgrey);
		g.fillAll();
	}
};
//==============================================================================
// custom CabbageLine
//==============================================================================
class CabbageLine	:	public Component
{
Colour col;
public:
	CabbageLine (bool isHorizontal, String colour)
		: isHorizontal(isHorizontal)
	{
		col = Colour::fromString(colour);
	}

	~CabbageLine()
	{
	}

	void resized()
	{
		if (isHorizontal == true)
			this->setBounds(getX(), getY(), getWidth(), getHeight());
		else
			this->setBounds(getX(), getY(), getWidth(), getHeight());	

		this->setAlpha(0.7);
	}

	void paint(Graphics& g)
	{
		g.setColour (col);
		g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 1);

		g.setColour (CabbageUtils::getBackgroundSkin());
		g.fillRoundedRectangle (0, 0, getWidth()-1, getHeight()-1, 1);
	}


private:
	bool isHorizontal;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLine);
};


//==============================================================================
// custom Cabbage display widget
//==============================================================================
class CabbageDisplay	:	public Component
{

public:
	CabbageDisplay ()
	{
#ifndef Cabbage_No_Csound
	winData = nullptr;
#endif
	}

	~CabbageDisplay()
	{
	}

	void resized()
	{

	}
#ifndef Cabbage_No_Csound
	void updateData(WINDAT* windat)
	{
	winData = windat;
	}

	void paint(Graphics& g)
	{
	g.setColour (Colours::white);
	//draw PVS data
	if(winData!=nullptr)
	for (int i=0; i<winData->npts; i++) {
			if (winData->fdata[i]) {
				int amp = winData->fdata[i];
				g.drawLine((i/winData->npts)*getWidth(), getHeight()-(amp*getHeight()), 
				(i/winData->npts)*getWidth(), getHeight(), 1);
			}
		}
	}

	WINDAT* winData;
#endif
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageDisplay);
};
#endif
