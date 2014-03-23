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

#include "CodeWindow.h"

//==============================================================================
CodeWindow::CodeWindow(String name):DocumentWindow (name, Colours::black,
							  DocumentWindow::allButtons),
							  fontSize(15),
							  showOutput(true),
							  csoundOutputText(""),
							  debugMessage(""),
							  firstTime(true),
							  font(String("Courier New"), 15, 1),
							  showingHelp(false)
{  
	setApplicationCommandManagerToWatch(&commandManager);
	commandManager.registerAllCommandsForTarget(this);
	addKeyListener(commandManager.getKeyMappings());
	
	textEditor = new CsoundCodeEditor("csound", csoundDoc, &csoundToker);

	restoreWindowStateFromString (appProperties->getUserSettings()->getValue ("mainWindowPos"));


	textEditor->addActionListener(this);

	
	this->setTitleBarHeight(20);
	this->setColour(DocumentWindow::backgroundColourId, CabbageUtils::getBackgroundSkin());
	

	setMenuBar(this, 25);	
	
	setVisible(true);

	this->setWantsKeyboardFocus(false);


	if(!appProperties->getUserSettings()->getValue("EditorColourScheme", var(0)).getIntValue())
		setEditorColourScheme("white");
	else if(appProperties->getUserSettings()->getValue("EditorColourScheme", var(0)).getIntValue())
		setEditorColourScheme("dark");

	RecentlyOpenedFilesList recentFiles;
	recentFiles.restoreFromString (appProperties->getUserSettings()
										->getValue ("recentlyOpenedFiles"));

	csdFile = recentFiles.getFile (0);
	csoundDoc.replaceAllContent(csdFile.loadFileAsString());
	
	//cabbageTimer->addActionListener(this);
	//cabbageTimer->startTimedEvent(10, "splash");
	setResizable(true, false);

	String opcodeFile = File(File::getSpecialLocation(File::currentExecutableFile)).getParentDirectory().getFullPathName(); 
	opcodeFile += "/opcodes.txt";
	Logger::writeToLog(opcodeFile);

	if(File(opcodeFile).existsAsFile())
		textEditor->setOpcodeStrings(File(opcodeFile).loadFileAsString());
	//else csound->Message("Could not open opcodes.txt file, parameter display disabled..");

	//set up popup for displaying info regarding opcodes..
	popupDisplay = new PopupDisplay("Poppy");
	popupDisplay->addActionListener(this);
	popupDisplay->setTitleBarHeight(0);
	popupDisplay->addToDesktop(0);
	popupDisplay->setVisible(false);
	
	
htmlHelp = new WebBrowserComponent(false);

setContentNonOwned(textEditor, false);
}

CodeWindow::~CodeWindow(){
	setMenuBar(0);
	setApplicationCommandManagerToWatch(nullptr);
	commandManager.deleteInstance();
	deleteAndZero(textEditor);
}


void CodeWindow::getCommandInfo (const CommandID commandID, ApplicationCommandInfo& result)
{	
	KeyboardShortcutKeys shortcuts(appProperties->getUserSettings()->getXmlValue("KeyboardShortcutXmlData"));

	switch (commandID)
	{
	
	//file commands
	case CommandIDs::fileNew:
		result.setInfo (String("New"), String("Create a new file"), CommandCategories::file, 0);
		result.addDefaultKeypress ('n', ModifierKeys::commandModifier);
		break;
	case CommandIDs::fileOpen:
		result.setInfo (String("Open"), String("Open a file"), CommandCategories::file, 0);
		result.addDefaultKeypress ('o', ModifierKeys::commandModifier);
		break;
	case CommandIDs::fileSave:
		result.setInfo (String("Save"), String("Save a file"), CommandCategories::file, 0);
		result.addDefaultKeypress ('s', ModifierKeys::commandModifier);
		break;
	case CommandIDs::fileSaveAs:
		result.setInfo (String("Save as"), String("Save file as.."), CommandCategories::file, 0);
		result.addDefaultKeypress ('s', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
		break;
	case CommandIDs::fileQuit:
		result.setInfo (String("Quit"), String("Quit"), CommandCategories::file, 0);
		result.addDefaultKeypress ('s', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
		break;
	case CommandIDs::fileUpdateGUI:
		result.setInfo (String("Update GUI"), String("Update GUI"), CommandCategories::file, 0);
		result.addDefaultKeypress ('#', ModifierKeys::commandModifier);
		break;		
	case CommandIDs::fileKeyboardShorts:
		result.setInfo (String("Keyboard Shortcuts"), String("Update GUI"), CommandCategories::file, 0);
		break;
	//edit commands
	case CommandIDs::editUndo:
		result.setInfo (String("Undo"), String("Undo last action"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('z', ModifierKeys::commandModifier);
		break;
	case CommandIDs::editRedo:
		result.setInfo (String("Redo"), String("Redo last action"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('z', ModifierKeys::shiftModifier | ModifierKeys::commandModifier);
		break;
	case CommandIDs::editCut:
		result.setInfo (String("Cut"), String("Cut selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('x', ModifierKeys::commandModifier);
		break;
	case CommandIDs::editCopy:
		result.setInfo (String("Copy"), String("Copy selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('c', ModifierKeys::commandModifier);
		break;
	case CommandIDs::editPaste:
		result.setInfo (String("Paste"), String("Paste selection"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('v', ModifierKeys::commandModifier);
		break;
	case CommandIDs::editToggleComments:
		result.setInfo (String("Toggle comments"), String("Toggle comments"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('t', ModifierKeys::commandModifier);
		break;
	case CommandIDs::editZoomIn:
		result.setInfo (String("Zoom in"), String("Zoom in"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('=', ModifierKeys::commandModifier);
		break;
	case CommandIDs::editZoomOut:
		result.setInfo (String("Zoom out"), String("Zoom out"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('-', ModifierKeys::commandModifier);
		break;
	case CommandIDs::whiteBackground:
		result.setInfo (String("White background"), String("White scheme"), CommandCategories::edit, 0);
		break;
	case CommandIDs::blackBackground:
		result.setInfo (String("Dark background"), String("Dark scheme"), CommandCategories::edit, 0);
		break;
	case CommandIDs::insertFromRepo:
		result.setInfo (String("Insert from repo"), String("Insert from repo"), CommandCategories::edit, 0);
		//result.defaultKeypresses.add (KeyPress::createFromDescription(shortcuts.getKeyPress("InsertFromRepo")));
		//result.addDefaultKeypress (KeyPress::createFromDescription(shortcuts.getKeyPress("InsertFromRepo")));
		break;	
	case CommandIDs::addFromRepo:
		result.setInfo (String("Add from repo"), String("Add from repo"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('j', ModifierKeys::commandModifier);
		break;	
	case CommandIDs::insertRecentEvent:
		result.setInfo (String("Insert recent event"), String("Insert recent event"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('e', ModifierKeys::commandModifier | ModifierKeys::altModifier);
		break;	
	case CommandIDs::openPythonEditor:
		result.setInfo (String("Insert Python code"), String("Insert Python code"), CommandCategories::edit, 0);
		result.addDefaultKeypress ('p', ModifierKeys::commandModifier | ModifierKeys::altModifier);
		break;	
	case CommandIDs::AudioSettings:
		result.setInfo (String("Audio Settings"), String("Edit audio settings"), CommandCategories::edit, 0);
		break;			
		
	//interactive commands	
	case CommandIDs::commOrchUpdateInstrument:
		result.setInfo (String("Update entire instr"), String("Update entire instr"), CommandCategories::command, 0);
		result.addDefaultKeypress ('u', ModifierKeys::commandModifier);
		break;	
	case CommandIDs::startSession:
		result.setInfo (String("Restart Session"), String("Restart Session"), CommandCategories::command, 0);
		result.addDefaultKeypress ('r', ModifierKeys::commandModifier);
		break;	
	case CommandIDs::commOrchUpdateMultiLine:
		result.setInfo (String("Update orc multi-line"), String("Update orch mutli-line"), CommandCategories::command, 0);
		result.addDefaultKeypress ('m', ModifierKeys::commandModifier);
		break;	
	case CommandIDs::commOrchUpdateSingleLine:
		result.setInfo (String("Update orc single-line"), String("Update orch single-line"), CommandCategories::command, 0);
		result.addDefaultKeypress ('l', ModifierKeys::commandModifier);
		break;	
	case CommandIDs::commScoUpdateMultiLine:
		result.setInfo (String("Update sco multi-line"), String("Update orch multi-line"), CommandCategories::command, 0);
		result.addDefaultKeypress ('p', ModifierKeys::commandModifier);
		break;	
	case CommandIDs::commScoUpdateSingleLine:
		result.setInfo (String("Update sco single-line"), String("Update orch multi-line"), CommandCategories::command, 0);
		result.addDefaultKeypress ('e', ModifierKeys::commandModifier);
		break;	
	case CommandIDs::commOrcUpdateChannel:
		result.setInfo (String("Update channel"), String("Update channel"), CommandCategories::command, 0);
		result.addDefaultKeypress ('c', ModifierKeys::altModifier | ModifierKeys::commandModifier);
		break;	
		
	case CommandIDs::viewCsoundHelp:
#ifndef MACOSX
		result.setInfo (String("Toggle Csound Manual F1"), String("Toggle Csound Manual"), CommandCategories::help, 0);
		result.defaultKeypresses.add(KeyPress(KeyPress::F1Key));
#else
		result.setInfo (String("Toggle Csound Manual Ctrl+1"), String("Toggle Csound Manual"), CommandCategories::help, 0);
		result.addDefaultKeypress ('1', ModifierKeys::commandModifier);		
#endif
		break;

	case CommandIDs::viewCabbageHelp:
		result.setInfo (String("Toggle Cabbage Manual"), String("Toggle Cabbage Manual"), CommandCategories::help, 0);
		break;
	}
}

//==============================================================================
PopupMenu CodeWindow::getMenuForIndex (int topLevelMenuIndex, const String& menuName)
{
PopupMenu m1, m2;
m1.clear();
m2.clear();
m2.setLookAndFeel(&getLookAndFeel());
if(topLevelMenuIndex==0)
	{		
	 //m1.addCommandItem(&commandManager, CommandIDs::fileNew);	 
	 //m1.addCommandItem(&commandManager, CommandIDs::fileOpen);
	 
	 //RecentlyOpenedFilesList recentFiles;
     //recentFiles.restoreFromString (appProperties->getUserSettings()
     //                                       ->getValue ("recentlyOpenedFiles"));

     //PopupMenu recentFilesMenu;
     //recentFiles.createPopupMenuItems (recentFilesMenu, 100, true, true);
     //m1.addSubMenu ("Open recent file", recentFilesMenu);	 
	 
	 m1.addCommandItem(&commandManager, CommandIDs::fileSave);
	 m1.addCommandItem(&commandManager, CommandIDs::fileSaveAs);
	 m1.addCommandItem(&commandManager, CommandIDs::fileQuit);
	 //m1.addCommandItem(&commandManager, CommandIDs::fileKeyboardShorts);
	 
	 return m1;
	}

else if(topLevelMenuIndex==1)
	{
	m1.addCommandItem(&commandManager, CommandIDs::editUndo);
	m1.addCommandItem(&commandManager, CommandIDs::editRedo);
	m1.addCommandItem(&commandManager, CommandIDs::editCut);
	m1.addCommandItem(&commandManager, CommandIDs::editCopy);
	m1.addCommandItem(&commandManager, CommandIDs::editPaste);
	m1.addCommandItem(&commandManager, CommandIDs::editToggleComments);
	m1.addSeparator();
	m2.addCommandItem(&commandManager, CommandIDs::editZoomIn);
	m2.addCommandItem(&commandManager, CommandIDs::editZoomOut);
	m1.addSubMenu(String("Font Size"), m2);
	m2.clear();
	m2.addCommandItem(&commandManager, CommandIDs::whiteBackground);
	m2.addCommandItem(&commandManager, CommandIDs::blackBackground);
	m1.addSubMenu("Change editor theme", m2);
	m1.addCommandItem(&commandManager, CommandIDs::AudioSettings);
	//m1.addCommandItem(&commandManager, CommandIDs::insertFromRepo);
	//m1.addCommandItem(&commandManager, CommandIDs::addFromRepo);
	//m1.addCommandItem(&commandManager, CommandIDs::insertRecentEvent);
	//m1.addCommandItem(&commandManager, CommandIDs::openPythonEditor);
	return m1;
	}

//else if(topLevelMenuIndex==2)
//	{
	//m1.addCommandItem(&commandManager, CommandIDs::startSession);
	//m1.addCommandItem(&commandManager, CommandIDs::commOrchUpdateInstrument);
	//m1.addCommandItem(&commandManager, CommandIDs::commOrcUpdateChannel);	
	//m1.addCommandItem(&commandManager, CommandIDs::commOrchUpdateMultiLine);
	//m1.addCommandItem(&commandManager, CommandIDs::commOrchUpdateSingleLine);
	//m1.addCommandItem(&commandManager, CommandIDs::commScoUpdateMultiLine);
	//m1.addCommandItem(&commandManager, CommandIDs::commScoUpdateSingleLine);
	
//	return m1;
//	}
 
	
else if(topLevelMenuIndex==2)
	{
	m1.addCommandItem(&commandManager, CommandIDs::viewCsoundHelp);
	m1.addCommandItem(&commandManager, CommandIDs::viewCabbageHelp);
	return m1;
	}

else return m1;
}

bool CodeWindow::perform (const InvocationInfo& info)
{
	Logger::writeToLog(String(info.commandID));
	//---------------------------------------------------------------------------------------------
	if(info.commandID==CommandIDs::fileNew)
		{		
			String tempFile = File::getSpecialLocation(File::userHomeDirectory).getFullPathName()+"/liveCodeSession.csd";
			Logger::writeToLog(tempFile);
			csdFile = tempFile;
			csoundDoc.replaceAllContent(csdFile.loadFileAsString());			
			toggleTextWindows();
		}
	else if(info.commandID==CommandIDs::fileSave)
		{
			Logger::writeToLog("fileSaved");
			sendActionMessage("fileSaved");
		}
	else if(info.commandID==CommandIDs::fileSaveAs)
		{
			Logger::writeToLog("fileSaveAs");
			sendActionMessage("fileSaveAs");
		}
	else if(info.commandID==CommandIDs::fileOpen)
		{			
			Logger::writeToLog("fileOpen");
			sendActionMessage("fileOpen");
		}
	else if(info.commandID==CommandIDs::AudioSettings)
		{			
			sendActionMessage("audioSettings");
		}
	else if(info.commandID==CommandIDs::fileQuit)
		{			
		JUCEApplication::getInstance()->systemRequestedQuit();	
		}
		
	else if(info.commandID==CommandIDs::editUndo)
		{			
			textEditor->undo();
		}
		
	else if(info.commandID==CommandIDs::fileKeyboardShorts)
		{				
			DialogWindow::LaunchOptions o;
			o.content.setOwned(new ShortcutsPanel());
			o.dialogTitle                   = TRANS("Keyboard Shortcuts");
			o.dialogBackgroundColour        = Colours::black;
			o.resizable                     = false;
			o.useNativeTitleBar				= false;
			o.escapeKeyTriggersCloseButton  = true;
			//o.content->setLookAndFeel(&this->getLookAndFeel());
			o.launchAsync();			
		}
		
	else if(info.commandID==CommandIDs::editCut)
		{			
			textEditor->cutToClipboard();
		}
	else if(info.commandID==CommandIDs::editCopy)
		{			
			textEditor->copyToClipboard();
		}
	else if(info.commandID==CommandIDs::editPaste)
		{			
			textEditor->pasteFromClipboard();
		}

	else if(info.commandID==CommandIDs::editRedo)
		{			
			textEditor->redo();
		}
	else if(info.commandID==CommandIDs::editToggleComments)
		{			
		textEditor->toggleComments();
		}
		
	else if(info.commandID==CommandIDs::editZoomIn)
		{			
			setFontSize("in");
		}
	else if(info.commandID==CommandIDs::editZoomOut)
		{			
			setFontSize("out");
		}
	else if(info.commandID==CommandIDs::whiteBackground)
		{			
		setEditorColourScheme("white");
		}

	else if(info.commandID==CommandIDs::blackBackground)
		{			
		setEditorColourScheme("dark");
		}

	else if(info.commandID==CommandIDs::insertRecentEvent)
		{
			
		}
	else if(info.commandID==CommandIDs::insertFromRepo)
		{			
		popupDisplay->setVisible(false);
		CodeWindow::insertFromRepo();
		}

	else if(info.commandID==CommandIDs::openPythonEditor)
		{	
		/*if(pythonEditor==nullptr){
			pythonEditor = new PythonEditor("Python Editor");
			//pythonEditor->textEditor->setLookAndFeel(lookAndFeel);
			//pythonEditor->setLookAndFeel(lookAndFeel);
			pythonEditor->addActionListener(this);
			pythonEditor->setAlwaysOnTop(true);
			pythonEditor->setVisible(true);
			pythonEditor->textEditor->setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(40, 40, 40));
		}
		
		pythonEditor->toFront(true);
		cabbageTimer->startTimedEvent(1, "pythonFocus");
		//pythonEditor->textEditor->highlightLine("oscil 1");
		
		 */
		}	
		
	else if(info.commandID==CommandIDs::addFromRepo)
		{	
		textEditor->addToRepository();
		}	

	else if(info.commandID==CommandIDs::viewCsoundHelp)
		{
		sendActionMessage("toggleCsoundOutput");
		sendActionMessage("hideOutputWindow");
		toggleManuals("Csound");
		}
	
	else if(info.commandID==CommandIDs::viewCabbageHelp)
		{
		sendActionMessage("toggleCsoundOutput");
		sendActionMessage("hideOutputWindow");
		toggleManuals("Cabbage");
		}
				
return true;
}

//=======================================================
// toggle manuals
//=======================================================
void CodeWindow::toggleManuals(String manual)
{
if(!showingHelp)
if(manual=="Csound")
{       
			String helpDir;
			if(manual=="Csound")	
			helpDir = appProperties->getUserSettings()->getValue("CsoundHelpDir", "");
		
			if(helpDir.length()<2)
					CabbageUtils::showMessage("Please set the Csound manual directory in the Preference menu", &getLookAndFeel());                
			else{                        
				CodeDocument::Position pos1, pos2;
				pos1 = textEditor->getDocument().findWordBreakBefore(textEditor->getCaretPos());
				pos2 = textEditor->getDocument().findWordBreakAfter(textEditor->getCaretPos());
				String opcode = csoundDoc.getTextBetween(pos1, pos2);
				URL urlCsound(helpDir+"/"+opcode.trim()+String(".html"));
				String urlCabbage;


				ChildProcess process;
				File temp1(urlCsound.toString(false));
				if(temp1.exists()){
				#ifdef LINUX        
					if(!process.start("xdg-open "+urlCsound.toString(false).toUTF8())) 
						CabbageUtils::showMessage("couldn't show file", &getLookAndFeel());
				#else
					htmlHelp->goToURL(urlCsound.toString(false));
					showingHelp = true;
					setContentNonOwned(nullptr, false);
					setContentNonOwned(htmlHelp, false);
				#endif
				}

			}
		}
else{
		String path;
#if defined(LINUX) || defined(MACOSX)
	path = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"/Docs/cabbage.html";
#else
	path = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName()+"\\Docs\\cabbage.html";
#endif	
	
	
	if(!File(path).existsAsFile())
			CabbageUtils::showMessage(
"Could not find Cabbage manual. Make sure\n\
it is located in the Docs folder in the same\n\
directory as the main Cabbage executable", &getLookAndFeel());
		else{
				ChildProcess process;
				File temp1(path);
				if(temp1.exists()){
				#ifdef LINUX        
					if(!process.start("xdg-open "+path)) 
						CabbageUtils::showMessage("couldn't show file", &getLookAndFeel());
				#else
					htmlHelp->goToURL(path);
					showingHelp = true;
					setContentNonOwned(nullptr, false);
					setContentNonOwned(htmlHelp, false);
				#endif
				}
		}
	}
else{
	setContentNonOwned(nullptr, false);
	setContentNonOwned(textEditor, false);
	showingHelp = false;
}
}

void CodeWindow::toggleTextWindows()
{
/*
if(showOutput){
		setContentNonOwned(textEditor, false);
		setMenuBar(this, 25);
		stopTimer();
		if(firstTime)
		outputTextEditor->setText("");
		firstTime==false;
		repaint();
}
else{
	startTimer(100);	
	setContentNonOwned(outputTextEditor, false);
	setMenuBar(this, 25);
	outputTextEditor->setFont(Font(String("Courier New"), 15, 1));
		repaint();
}				
showOutput=!showOutput;
*/	
}

void CodeWindow::setEditorColourScheme(String theme){
	setColourScheme(theme);
}

void CodeWindow::actionListenerCallback(const String &message){
	Logger::writeToLog(message);
	if(message=="splash")
		toggleTextWindows();
	//else if(message=="pythonFocus")
	//	pythonEditor->textEditor->grabKeyboardFocus();
	else if(message=="make popup invisible"){
		popupDisplay->setTopLeftPosition(1000, 1000);
		popupDisplay->setVisible(false);
	}
	else if(message=="sendPythonEvent"){
	/*	String text = pythonEditor->textEditor->getSelectedText();
		String event = "pyruni {{\n";
		if(text.length()>0)
		event << text;
		event << "\n}}";
		Logger::writeToLog(event);
//		csound->CompileOrc(event.toUTF8());
		this->grabKeyboardFocus();
	
	 */
	}
	else if(message=="return focus to editor"){
	textEditor->grabKeyboardFocus();		
	}
	else if(message.contains("popupDisplay")){
			
	int width = (font.getStringWidth(textEditor->getOpcodeToken(2)) > font.getStringWidth(textEditor->getOpcodeToken(3)) ? 
											font.getStringWidth(textEditor->getOpcodeToken(2)) : 
											font.getStringWidth(textEditor->getOpcodeToken(3)));
	//popupDisplay->killSplash();
	popupDisplay->setSize(width, 50);
	popupDisplay->box->setSize(width, 50);

	popupDisplay->setVisible(true);
	popupDisplay->setAlwaysOnTop(true);
	popupDisplay->addToDesktop(0);

	popupDisplay->setTopLeftPosition(this->getCaretScreenPosition().getTopLeft());
	//popupDisplay->setBounds(this->getCaretScreenPosition().getX(),
	//						this->getCaretScreenPosition().getY()+18.f,
	//						width, 50);
	//popupDisplay->setWantsKeyboardFocus(false);
	popupDisplay->setText(textEditor->getOpcodeToken(2).removeCharacters("\""), 
								   textEditor->getOpcodeToken(3).removeCharacters("\""));
		
	textEditor->toFront(true);
	//cabbageTimer->startTimedEvent(1, "return focus to editor");
	textEditor->grabKeyboardFocus();

	}
	
		
}


void CodeWindow::timerCallback(){
//		if(showOutput){
//			outputTextEditor->setText(csoundOutputText);
//			outputTextEditor->setCaretPosition(outputTextEditor->getText().length());
//		}
}

void CodeWindow::insertFromRepo(){
int repoIndex = 1;
		PopupMenu m;
		StringArray repoEntries;
		m.setLookAndFeel(&this->getLookAndFeel());
		ScopedPointer<XmlElement> xmlElement;
		xmlElement = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");

		forEachXmlChildElement (*xmlElement, e)
			{
			m.addItem(repoIndex, e->getTagName());
			repoEntries.add(e->getTagName());
			repoIndex++;
			}	
			

		const int result = m.showAt(getCaretScreenPosition());

		forEachXmlChildElement (*xmlElement, e)
			{
			if(e->getTagName()==repoEntries[result-1])
			textEditor->insertText(e->getAllSubText());
			}
		xmlElement = nullptr;			
}

//===============================================================================
void CodeWindow::newFile(String type)
{
String untitledCSD;
if(type=="effect"){
untitledCSD= 
"<Cabbage>\n"
"form size(400, 300), caption(\"Untitled\"), pluginID(\"plu1\")\n"
"\n"
"</Cabbage>\n"
"<CsoundSynthesizer>\n"
"<CsOptions>\n"
"-n -d\n"
"</CsOptions>\n"
"<CsInstruments>\n"
"sr = 44100\n"
"ksmps = 64\n"
"nchnls = 2\n"
"0dbfs=1\n"
"\n"
"instr 1\n"
"a1 inch 1\n"
"a2 inch 2\n"
"\n"
"\n"
";outs a1, a2\n"
"endin\n"
"\n"
"</CsInstruments>  \n"
"<CsScore>\n"
"f1 0 1024 10 1\n"
"i1 0 3600\n"
"</CsScore>\n"
"</CsoundSynthesizer>";
}
else if(type=="instrument")
{
untitledCSD= 
"<Cabbage>\n"
"form size(400, 300), caption(\"Untitled\"), pluginID(\"plu1\")\n"
"keyboard bounds(10, 200, 360, 100)\n"
"\n"
"</Cabbage>\n"
"<CsoundSynthesizer>\n"
"<CsOptions>\n"
"-n -d -+rtmidi=NULL -M0 --midi-key-cps=4 --midi-velocity-amp=5\n" 
"</CsOptions>\n"
"<CsInstruments>\n"
"sr = 44100\n"
"ksmps = 64\n"
"nchnls = 2\n"
"0dbfs=1\n"
"\n"
"instr 1\n"
"a1 oscili p5, p4, 1\n"
"outs a1, a1"
"\n"
"endin\n"
"\n"
"</CsInstruments>  \n"
"<CsScore>\n"
"f1 0 1024 10 1\n"
"f0 3600\n"
"</CsScore>\n"
"</CsoundSynthesizer>";
}
csoundDoc.replaceAllContent(untitledCSD);
unSaved = true;
sendActionMessage("fileSavedAs");
}
