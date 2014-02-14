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

#ifndef __MAINCOMPONENT_H_1F3294A__
#define __MAINCOMPONENT_H_1F3294A__

#include "JuceHeader.h"
#include "CabbageLookAndFeel.h"
#include "CabbageUtils.h"

//==============================================================================
/* one of these is fitted inside each cell of the table. It contains a text editor
 * for the functoin tables number, and a string displaying the file name */
  //==============================================================================
class listBoxItem : public Component
{
String text;
Colour highlightColour;
int ftableNumber;
ScopedPointer<LookAndFeel_V1> oldSchool;
ScopedPointer<TextEditor> textEditor;
public:
	listBoxItem():highlightColour(Colours::white), isHighlighted(false){
		oldSchool = new LookAndFeel_V1();
		textEditor = new TextEditor();
		setInterceptsMouseClicks(false, true);
		textEditor->setLookAndFeel(oldSchool);
		addAndMakeVisible(textEditor);
		setSize(getWidth(), 50);
	}
	~listBoxItem(){	}

	//paint out
	void paint(Graphics &g){
		g.fillAll(Colours::black.withAlpha(.5f));
		g.setColour(highlightColour);
		g.drawRect(1, 1, getWidth()-1, getHeight()-1, 2);
		g.setColour(Colours::whitesmoke);
		g.drawText (text, 55, 0, getWidth() - 4, getHeight(), Justification::centredLeft, true);	
	}

	void resized(){
		textEditor->setBounds(3, 3, 40, getHeight()-5);
	}

	
	void setText(int _ftableNumber, String _text){
		text = _text;
		textEditor->setText(String(_ftableNumber));
	}	
	
	bool isHighlighted;
};

//==============================================================================
/* this is the table list box model, it' gets passed to our TableListBox object 
this class can send action messages to MainContentComponent whenever something
happens to one of its cells...*/
 //==============================================================================
class FunctionTableList : public TableListBoxModel,
							public ActionBroadcaster
{

public:
	FunctionTableList():font(14.f), rowData(), numHighlighted(0){
		numRows=0;
		ftableCounter=0;		
	}
	
	~FunctionTableList(){
		listBoxItems.clear();
	}
	
	void paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
    {
	g.fillAll (CabbageUtils::getBackgroundSkin());
	g.setFont (font);
	g.setColour(Colours::white);
	//g.drawText (rowData[rowNumber], 2, 0, width - 4, height, Justification::centredLeft, true);

        if (rowIsSelected)
            g.fillAll (Colours::cornflowerblue);
    }

	//add or modify exiting funcoitn table entries
	void addOrModifyRows(int _bankNumber, StringArray functionRows){
		rowData.clear();
		rowData = functionRows;
		bankNumber = _bankNumber+1;
		ftableCounter = 0;
		numRows = rowData.size();
	}
		
		
	// This is overloaded from TableListBoxModel, and must update any custom components that we're using
	Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
											Component* existingComponentToUpdate)
	{
		if (columnId == 1) // If it's the main column, we'll return our custom component..
		{			
			listBoxItem* boxItem = (listBoxItem*) existingComponentToUpdate;
			// If an existing component is being passed-in for updating, we'll re-use it, but
			// if not, we'll have to create one.
			if (boxItem == 0){
				boxItem = new listBoxItem();
			}
			boxItem->setText(rowNumber+(bankNumber*50), rowData[rowNumber]);
			return boxItem;
		}
		else
		{
			// for any other column, just return 0, as we'll be painting these columns directly.
			jassert (existingComponentToUpdate == 0);
			return 0;
		}
	}
										
	//determine actions to take place when users click on a cell...
    void cellClicked (int rowNumber, int columnId, const MouseEvent &e){
		if(e.mods.isRightButtonDown()){
		PopupMenu m;
		CabbageLookAndFeel cLAK;
		m.setLookAndFeel(&cLAK);
		m.addItem(1, "Delete selected");
		int choice = m.show();
		if(choice==1)
			sendActionMessage("delete selected");
		}
		
	}

    void backgroundClicked(){
		sendActionMessage("deselect");
	} 	

	int getNumRows(){
		return numRows;
	}

    void setNumRows(int _numRows){
        numRows = _numRows;
    }
		
	void paintCell(juce::Graphics& g, int, int, int, int, bool){
		g.fillAll(Colours::pink);
		}

	
private:
    Font font;
	StringArray highlightedItems;
	int numRows;
	int ftableCounter;
	int bankNumber;
	int numHighlighted;
	StringArray rowData;
	OwnedArray<listBoxItem> listBoxItems;
 
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FunctionTableList)
};

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content. This is the main parent for all objects...
*/
//==============================================================================

class DirectoryContentsComponent   : public Component,
								public FileBrowserListener,
								public ButtonListener,
								public ActionListener,
								public ActionBroadcaster
{
	OwnedArray<FunctionTableList> tablesList;
	OwnedArray<TableListBox> tables;  
    ScopedPointer<CabbageLookAndFeel> lookAndFeel;
    ScopedPointer<CabbageLookAndFeelBasic> basicLookAndFeel;
	StretchableLayoutManager horizontalLayout;
	ScopedPointer<Viewport> viewPort;
    ScopedPointer<StretchableLayoutResizerBar> horizontalDividerBar;
	ScopedPointer<TabbedComponent> tabComp;
	ScopedPointer<TextEditor> editor;
	ScopedPointer<TextButton> updateTablesButton;
	ScopedPointer<ToggleButton> autoUpdate;
	ScopedPointer<TextButton> addBankButton;
	StringArray functionTableStrings;
	DirectoryContentsList directoryList;
	TimeSliceThread thread;
	int ftableCounter;
	ScopedPointer<FileTreeComponent> fileTreeComp;	
	WildcardFileFilter filter;
	//Array of stireng arrays....
	OwnedArray<StringArray> functionRowData;
	String fileTypes, rootDir;
	StringArray previousScoreEvents;
	
public:
    //==============================================================================
    DirectoryContentsComponent(String dir, String files);
    ~DirectoryContentsComponent();


    bool isFileSuitable(const juce::File&) const
	{
		return true;
	}

    bool isDirectorySuitable(const juce::File&) const
	{
		return true;
	}
			
    void fileDoubleClicked (const File& file){
		Logger::writeToLog("file double clicked");
		}
		
    void browserRootChanged (const File&) {
		Logger::writeToLog("root changed");
		}
		
	void updateSelection(const File& file);
    void showFile (const File& file){}       
    void paint (Graphics&);
    void resized();  
	void buttonClicked(Button* button);
	void fileClicked(const File& file, const MouseEvent& e);  
	void actionListenerCallback(const juce::String& string);
	void selectionChanged();
	const StringArray getFunctionTables();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DirectoryContentsComponent)
};


#endif  // __MAINCOMPONENT_H_1F3294A__
