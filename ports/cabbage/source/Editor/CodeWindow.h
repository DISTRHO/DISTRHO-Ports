#ifndef __MAINCOMPONENT_H_615ECE56__
#define __MAINCOMPONENT_H_615ECE56__

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
*/

#include "CodeEditor.h"
#include "KeyboardShortcuts.h"
#include "../Plugin/CabbagePluginProcessor.h"
//class LiveCsound;
class PythonEditor;
class PopupDisplay;

//==============================================================================
//Main window. This window holds a 2 textEditors. One for the code, and the other for
//displaying Csound output messages
//==============================================================================
class CodeWindow : public DocumentWindow,
					 public ApplicationCommandTarget,
					 public MenuBarModel,
					 public ActionListener,
					 public ActionBroadcaster,
					 public CodeDocument::Listener,
					 public Timer
{
public:
	CodeWindow(String name);
	~CodeWindow();
		CodeDocument csoundDoc;
	//==============================================================================
	StringArray getMenuBarNames()
	{
		const char* const names[] = { "File", "Edit", "Help", 0 };
		return StringArray (names);
	}
	
	//==============================================================================
	void getAllCommands (Array <CommandID>& commands)
	{
		const CommandID ids[] = {
									CommandIDs::fileNew,
									CommandIDs::fileOpen,
									CommandIDs::fileSave,
									CommandIDs::fileSaveAs,	
									CommandIDs::fileQuit,
									CommandIDs::fileKeyboardShorts,	
									CommandIDs::editUndo,
									CommandIDs::editCopy,
									CommandIDs::editCut,
									CommandIDs::editPaste,
									CommandIDs::editRedo,
									CommandIDs::editToggleComments,
									CommandIDs::editZoomIn,
									CommandIDs::editZoomOut,
									CommandIDs::whiteBackground,
									CommandIDs::blackBackground,
									CommandIDs::startSession,
									CommandIDs::insertFromRepo,
									CommandIDs::AudioSettings,
									CommandIDs::addFromRepo,
									CommandIDs::insertRecentEvent,
									CommandIDs::openPythonEditor,
									CommandIDs::commOrchUpdateInstrument,
									CommandIDs::commOrchUpdateMultiLine,
									CommandIDs::commOrchUpdateSingleLine,
									CommandIDs::commScoUpdateMultiLine,
									CommandIDs::commScoUpdateSingleLine,
									CommandIDs::commOrcUpdateChannel,
									CommandIDs::viewCsoundHelp,	
									CommandIDs::viewCabbageHelp	
									};
		commands.addArray (ids, sizeof (ids) / sizeof (ids [0]));
	}
	
	//==============================================================================
	void menuItemSelected (int menuItemID, int topLevelMenuIndex){
		if (menuItemID >= 100 && menuItemID < 200)
			{
			RecentlyOpenedFilesList recentFiles;
			recentFiles.restoreFromString (appProperties->getUserSettings()
												->getValue ("recentlyOpenedFiles"));

			csdFile = recentFiles.getFile (menuItemID - 100);
			textEditor->getDocument().replaceAllContent(csdFile.loadFileAsString());
			setName(csdFile.getFullPathName());
			}		
	}
	
	//==============================================================================
	void setFontSize(String zoom)
	{
		if(zoom==String("in"))
		textEditor->setFont(Font(String("Courier New"), ++fontSize, 1));
		else
		textEditor->setFont(Font(String("Courier New"), --fontSize, 1));
	}	
	
	
	//================= command methods ====================
	ApplicationCommandTarget* getNextCommandTarget(){
		return findFirstTargetParentComponent();
	}

	void getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result);
	PopupMenu getMenuForIndex (int topLevelMenuIndex, const String& menuName);
	bool perform (const InvocationInfo& info);
	void toggleManuals(String manual);

	void setEditorColourScheme(String theme);
	void actionListenerCallback(const String &message);

	void timerCallback();
	void toggleTextWindows();
	
	Rectangle<int> getCaretScreenPosition(){
		Rectangle<int> rect(textEditor->getCaretPoisition());
		rect.setLeft(rect.getX()+this->getTopLevelComponent()->getX()+100);
		rect.setTop(rect.getY()+this->getTopLevelComponent()->getY()+45);
		return rect;		
	}


	void closeButtonPressed(){
	//JUCEApplication::getInstance()->systemRequestedQuit();
	this->setVisible(false);
	}

	void codeDocumentTextDeleted(int,int){}
	void codeDocumentTextInserted(const juce::String &,int){}
	void codeDocumentChanged (const CodeDocument::Position &affectedTextStart, const CodeDocument::Position &affectedTextEnd);
	void insertFromRepo();
	
	void setText(String file){
	csoundDoc.replaceAllContent(file);
	}


	String getText(){
	 return csoundDoc.getAllContent();	
	}
	
	
	void setColourScheme(String theme){
	if(theme=="white"){
			textEditor->setColourScheme(csoundToker.getDefaultColourScheme());
			textEditor->setColour(CodeEditorComponent::backgroundColourId, Colours::white);
			textEditor->setColour(CaretComponent::caretColourId, Colours::black);
			textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue);
			appProperties->getUserSettings()->setValue("EditorColourScheme", 0); 			
			}
	else if(theme=="dark"){
			textEditor->setColourScheme(csoundToker.getDarkColourScheme());
			textEditor->setColour(CaretComponent::caretColourId, Colours::white);
			textEditor->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(20, 20, 20));
			textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::green.withAlpha(.6f)); 
			appProperties->getUserSettings()->setValue("EditorColourScheme", 1);
			}	
	}
	
	void newFile(String type);
	bool unSaved;

	StringArray recentEvents;
	//ScopedPointer<CabbageLookAndFeel> lookAndFeel;

	String csoundOutputText;
	bool firstTime;
	String debugMessage;
	bool showOutput;
	CommandManager commandManager;
	File csdFile;
	int fontSize;
	String ASCIICabbage;

	StringArray opcodeStrings;
	ScopedPointer<PopupDisplay> popupDisplay;
	CsoundCodeEditor* textEditor;
	CsoundTokeniser csoundToker;
	Font font;
	ScopedPointer<WebBrowserComponent> htmlHelp;
	bool showingHelp;
};


//============================================================
// window for Python editor
//============================================================
class PythonEditor: public DocumentWindow,
					public ActionListener,
					public ActionBroadcaster
{
public:
	PythonEditor(String name):DocumentWindow (name, Colours::black,
							  DocumentWindow::closeButton)
	{
		textEditor = new CsoundCodeEditor("python", csoundDocu, &csoundToker);
		textEditor->setColour(TextEditor::textColourId, Colours::white);
		textEditor->setFont(Font(String("Courier New"), 15, 1));
		textEditor->addActionListener(this);
		textEditor->setSize(600, 400);

		if(!appProperties->getUserSettings()->getValue("EditorColourScheme", var(0)).getIntValue())
			setEditorColourScheme("white");
		else if(appProperties->getUserSettings()->getValue("EditorColourScheme", var(0)).getIntValue())
			setEditorColourScheme("dark");		
		
		centreWithSize(600, 400);
		setContentNonOwned(textEditor, true);
		setResizable(true, false);
	}
	
	~PythonEditor()
	{
		 delete textEditor;
	} 
	 
	void codeDocumentChanged (const CodeDocument::Position &affectedTextStart, const CodeDocument::Position &affectedTextEnd)
	{
	}	 
	 
	void actionListenerCallback(const String &message)
	{
	if(message=="make invisible"){
		//this->setVisible(false);
		sendActionMessage("sendPythonEvent");
		//if(textEditor->getm)
	}	
	}
	
	void closeButtonPressed(){
	setVisible(false);
	}	
	

	
	void setEditorColourScheme(String theme){
		if(theme=="white"){
				textEditor->setColourScheme(csoundToker.getDefaultColourScheme());
				textEditor->setColour(CodeEditorComponent::backgroundColourId, Colours::white);
				textEditor->setColour(CaretComponent::caretColourId, Colours::black);
				textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue);
				appProperties->getUserSettings()->setValue("EditorColourScheme", 0); 			
				}
		else if(theme=="dark"){
				textEditor->setColourScheme(csoundToker.getDarkColourScheme());
				textEditor->setColour(CaretComponent::caretColourId, Colours::white);
				textEditor->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(20, 20, 20));
				textEditor->setColour(CodeEditorComponent::highlightColourId, Colours::green.withAlpha(.6f)); 
				appProperties->getUserSettings()->setValue("EditorColourScheme", 1);
				}	
	}
	
	void codeDocumentTextDeleted(int,int){}
	void codeDocumentTextInserted(const juce::String &,int){}	
	
	CsoundCodeEditor* textEditor;

	CodeDocument csoundDocu;
	PythonTokeniser csoundToker;
	  
};

//============================================================
// audio settings class (not currently used...)
//============================================================
class AudioSettings: public Component,
					public ActionListener,
					public ActionBroadcaster
{
public:
	AudioSettings(StringArray deviceStrings)
	{
    addAndMakeVisible (label = new Label ("new label",
                                          "Sampling Rate"));
    label->setFont (Font (15.00f, Font::bold));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::lime);
    label->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (samplingRate = new ComboBox ("new combo box"));
	samplingRate->addItem("44100", 1);
	samplingRate->addItem("48000", 2);
	samplingRate->setSelectedId(1, dontSendNotification );
    samplingRate->setEditableText (false);
    samplingRate->setJustificationType (Justification::centredLeft);
    samplingRate->setTextWhenNothingSelected (String::empty);
    samplingRate->setTextWhenNoChoicesAvailable ("(no choices)");
    //samplingRate->addListener (this);

    addAndMakeVisible (label2 = new Label ("new label",
                                           "Ksmps"));
    label2->setFont (Font (20.00f, Font::bold));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::lime);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (ksmps = new ComboBox ("new combo box"));
	ksmps->addItem("32", 1);
	ksmps->addItem("64", 2);
	ksmps->addItem("128", 3);
	ksmps->addItem("512", 4);
	ksmps->addItem("1024", 5);
	ksmps->addItem("2048", 6);
	ksmps->addItem("4096", 7);
	ksmps->setSelectedId(2, dontSendNotification );
    ksmps->setEditableText (false);
    ksmps->setJustificationType (Justification::centredLeft);
    ksmps->setTextWhenNothingSelected (String::empty);
    ksmps->setTextWhenNoChoicesAvailable ("(no choices)");
   // ksmps->addListener (this);

    addAndMakeVisible (label3 = new Label ("new label",
                                           "Audio Device"));
    label3->setFont (Font (15.00f, Font::bold));
    label3->setJustificationType (Justification::centredLeft);
    label3->setEditable (false, false, false);
    label3->setColour (TextEditor::textColourId, Colours::lime);
    label3->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (audioDevice = new ComboBox ("new combo box"));
    audioDevice->setEditableText (false);
	for(int i=0;i<deviceStrings.size();i++)
		audioDevice->addItem(deviceStrings[i], i+1);
		
	audioDevice->setSelectedId(deviceStrings.size(), dontSendNotification );	
	
    audioDevice->setJustificationType (Justification::centredLeft);
    audioDevice->setTextWhenNothingSelected (String::empty);
    audioDevice->setTextWhenNoChoicesAvailable ("(no choices)");
    //audioDevice->addListener (this);

    addAndMakeVisible (label4 = new Label ("new label",
                                           "MIDI Device"));
    label4->setFont (Font (15.00f, Font::bold));
    label4->setJustificationType (Justification::centredLeft);
    label4->setEditable (false, false, false);
    label4->setColour (TextEditor::textColourId, Colours::lime);
    label4->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (midiDevice = new ComboBox ("new combo box"));
    midiDevice->setEditableText (false);
    midiDevice->setJustificationType (Justification::centredLeft);
    midiDevice->setTextWhenNothingSelected (String::empty);
    midiDevice->setTextWhenNoChoicesAvailable ("(no choices)");
    //midiDevice->addListener (this);

	setSize(300, 200);

	}

	void actionListenerCallback(const String &message)
	{
	if(message=="make invisible"){
		//this->setVisible(false);
		sendActionMessage("sendPythonEvent");
		}	
	}
	
	void resized(){
    label->setBounds (28, 16, 96, 24);
    samplingRate->setBounds (130, 16, 118, 24);
    label2->setBounds (58, 52, 80, 24);
    ksmps->setBounds (130, 52, 117, 24);
    label3->setBounds (41, 87, 80, 24);
    audioDevice->setBounds (130, 88, 117, 24);
    label4->setBounds (42, 122, 80, 24);
    midiDevice->setBounds (130, 121, 118, 24);	
	}
	
	void paint(Graphics& g){
		g.fillAll(Colours::black);
	}
	
	~AudioSettings(){
    label = nullptr;
    samplingRate = nullptr;
    label2 = nullptr;
    ksmps = nullptr;
    label3 = nullptr;
    audioDevice = nullptr;
    label4 = nullptr;
    midiDevice = nullptr;	
	}
	
private:
    ScopedPointer<Label> label;
    ScopedPointer<ComboBox> samplingRate;
    ScopedPointer<Label> label2;
    ScopedPointer<ComboBox> ksmps;
    ScopedPointer<Label> label3;
    ScopedPointer<ComboBox> audioDevice;
    ScopedPointer<Label> label4;
    ScopedPointer<ComboBox> midiDevice;
	
};


//============================================================
// class for displaying popup text
//============================================================
class PopupDisplay : public DialogWindow, 
						public Timer,
							public ActionBroadcaster
{

		class Box : public Component
		{
		public:
			Box():firstTime(true)
			{
				this->setInterceptsMouseClicks(false, false);
				setSize(10, 50);
			}	
			
			~Box(){};
			
			void setText(String _info, String _syntax){
				syntax=_syntax;
				info=_info;
				repaint();
			}

			void paint(Graphics& g)
			{
				#ifdef LINUX
				g.fillAll(Colour::fromRGB(40,40,40));
				g.setColour(Colours::yellow);
				g.drawRect(0, 0, getWidth()-1, getHeight()-1, 1);
				g.setFont(Font(String("Arial"), 16, 0));
				g.setColour(Colours::whitesmoke);
				g.drawFittedText(syntax, 10, 10, getWidth(), getHeight(), Justification::topLeft, 100, 1);
				g.setFont(Font(String("Arial"), 15, 0));
				g.setColour(Colours::cornflowerblue);				
				g.drawFittedText(info, 10, 25, getWidth(), getHeight(), Justification::topLeft, 100, 1);
				#else
				g.fillAll(Colour::fromRGB(20, 20, 20));
				g.setColour(Colours::whitesmoke);
				g.drawRect(0, 0, getWidth()-1, getHeight()-1, 1);
				g.setFont(Font(String("Arial"), 16, 0));
				g.setColour(Colours::yellow);
				g.drawFittedText(syntax, 10, 10, getWidth(), getHeight(), Justification::topLeft, 100, 1);
				g.setFont(Font(String("Arial"), 15, 0));
				g.setColour(Colours::lime);				
				g.drawFittedText(info, 10, 25, getWidth(), getHeight(), Justification::topLeft, 100, 1);				
					
					
				#endif
			}
			
			void resized(){
				setSize(getWidth(), getHeight());	
			}
			
	bool firstTime;
	private:

		String syntax, info;
		
		};
		
public:
		PopupDisplay(String name): DialogWindow(name, Colours::black, true, true),
		time(0), seconds(0)
		{
		box = new Box();
		this->setContentNonOwned(box, true);
		#ifndef LINUX
		this->setAlpha(.9f);
		#endif
		}
		
		~PopupDisplay(){}		
		
		void resized(){
		setSize(getWidth(), 50);
		//	box->repaint();
		}
	
		void setText(String opcodeName, String opcodeDescrptor){
			box->setText(opcodeName, opcodeDescrptor);	
		}
	
		int getDesktopWindowStyleFlags() 	const{
			int styleFlags = ComponentPeer::windowAppearsOnTaskbar;

			//if (useDropShadow)       styleFlags |= ComponentPeer::windowHasDropShadow;
			//if (useNativeTitleBar)   styleFlags |= ComponentPeer::windowHasTitleBar;

			return 0;
		}		
		
		void timerCallback(){
			if(time<seconds){
				time++;
				stopTimer();
				setVisible(true);
				sendActionMessage("");
			}		
		}
		
		void killSplash(){
		box->firstTime=false;
		box->repaint();
		}
		
		bool triggerToAppear(int _seconds)
		{
		time=0;
		seconds = _seconds;
		startTimer(1000);
		}
			
		
		void closeButtonPressed(){
		setVisible(false);
		}		
		
		ScopedPointer<Box> box;
		int time;
		int seconds;
		
	
};


#endif  // __MAINCOMPONENT_H_615ECE56__
