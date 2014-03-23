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

//==============================================================================

CsoundCodeEditor::CsoundCodeEditor(String type, CodeDocument &document, CodeTokeniser *codeTokeniser)
					: CodeEditorComponent(document, codeTokeniser), type(type), textChanged(false)
{
	document.addListener(this);
	setColour(CodeEditorComponent::backgroundColourId, Colour::fromRGB(20, 20, 20));
	setColour(CodeEditorComponent::lineNumberBackgroundId, Colours::black);
	setColour(CodeEditorComponent::highlightColourId, Colours::green.withAlpha(.6f)); 
	setColour(CaretComponent::caretColourId, Colours::white);
	setColour(TextEditor::backgroundColourId, Colours::black);
	setColour(TextEditor::textColourId, Colours::white);
	setLineNumbersShown(true);
	setColour(CodeEditorComponent::highlightColourId, Colours::cornflowerblue); 
	setColour(CodeEditorComponent::lineNumberTextId, Colours::whitesmoke);
	setLineNumbersShown(true);
	setFont(Font(String("Courier New"), 14, 1));			
}

CsoundCodeEditor::~CsoundCodeEditor()
{
	
}

void CsoundCodeEditor::highlightLine(String line){
		String temp = getDocument().getAllContent();
		Range<int> range;
		moveCaretTo(CodeDocument::Position (getDocument(), temp.indexOf(line)+line.length()), false);
		moveCaretTo(CodeDocument::Position (getDocument(), temp.indexOf(line)), true);
		//range.setStart(temp.indexOf(line)+line.length());
		//range.setEnd(temp.indexOf(line));	
		//range.setStart(1);
		//range.setEnd(100);		
		//setHighlightedRegion(range);
}

bool CsoundCodeEditor::keyPressed (const KeyPress& key)
{
	//Logger::writeToLog(String(key.getKeyCode()));	
	if (key.getTextDescription().contains("cursor up") || key.getTextDescription().contains("cursor down") 
        || key.getTextDescription().contains("cursor left") || key.getTextDescription().contains("cursor right"))  
	handleEscapeKey();

	if (! TextEditorKeyMapper<CodeEditorComponent>::invokeKeyFunction (*this, key))
    {
			
        if (key == KeyPress::returnKey)
			handleReturnKey();                               
			
        else if (key == KeyPress::escapeKey)                                
			handleEscapeKey();
        //else if (key == KeyPress ('[', ModifierKeys::commandModifier, 0))   unindentSelection();
        //else if (key == KeyPress (']', ModifierKeys::commandModifier, 0))   indentSelection();
        else if (key.getTextCharacter() >= ' ')                            
		insertTextAtCaret (String::charToString (key.getTextCharacter()));
		//insertMultiTextAtCaret(String::charToString (key.getTextCharacter()));

		else if(key.getKeyCode() ==  268435488)
			handleTabKey("backwards");
		else if(key ==  KeyPress::tabKey)
			handleTabKey("forwards");
			
			
        else                                                                
		return false;
    }
    //handleUpdateNowIfNeeded();
    return true;
}

void CsoundCodeEditor::handleReturnKey (){
if(type=="csound"){
	insertNewLine("\n");
	sendActionMessage("make popup invisible");		
}		
}	


void CsoundCodeEditor::insertText(String text){
	pos1 = getCaretPos();
	getDocument().insertText(pos1, text);
}


void CsoundCodeEditor::insertNewLine(String text){
	pos1 = getCaretPos();
	StringArray csdArray;
	csdArray.addLines(getAllText());
	String curLine = csdArray[pos1.getLineNumber()];	
	int numberOfTabs=0;
	String tabs;
	while(curLine.substring(numberOfTabs, numberOfTabs+1).equalsIgnoreCase("\t")){
		tabs.append("\t", 8);
		numberOfTabs++;
	}
	Logger::writeToLog("Number of tabs:"+String(numberOfTabs));
	getDocument().insertText(pos1, text+tabs);
}

void CsoundCodeEditor::insertMultiTextAtCaret (String text)
{
	sendActionMessage("make popup invisible");
	StringArray csdArray;
	csdArray.addLines(getAllText());
	String curLine;	
	CodeDocument::Position newPos, indexPos;
	newPos = getCaretPos();//getSelectionStartCaretPos();
	int currentLine = getCaretPos().getLineNumber();
	int index = newPos.getIndexInLine();
	Logger::writeToLog(String(index));
	
	StringArray selectedText;
	selectedText.addLines(getTextInRange(this->getHighlightedRegion()));
	
	
	for(int i=0;i<selectedText.size();i++){
		curLine = newPos.getLineText();
		Logger::writeToLog(String(curLine.length()));
		/* need to check for tabs and add four spaces!!*/		
		for(int y=curLine.length();y<index+2;y++){
			getDocument().insertText(CodeDocument::Position(getDocument(), newPos.getLineNumber(), curLine.length()), " ");
			newPos = newPos.movedBy(1);
			//curLine = csdArray[currentLine+i];
		}

		getDocument().insertText(newPos, text);	
		newPos = newPos.movedByLines(1);
		}
	sendActionMessage("make popup invisible");
}

void CsoundCodeEditor::handleTabKey(String direction)
{	
	/*multi line action, get highlited text, find the position of
	 * it within the text editor, remove it from editor and reinsert it with
	 * formatting
	 */
	 
	StringArray selectedText, csdArray;
	selectedText.addLines(getSelectedText());
	csdArray.addLines(getAllText());
	String csdText;
	String currentLine;	

	if(direction.equalsIgnoreCase("forwards")){
		//single line tab
		if(selectedText.size()<1){
			insertTabAtCaret();
			return;
		}
		else{ 
		//multiline tab
		int indexOfText = getAllText().indexOf(getSelectedText());
		csdText = getAllText().replace(getSelectedText(), "");
		for(int i=0;i<selectedText.size();i++)
						selectedText.set(i, "\t"+selectedText[i]);	

		csdText = csdText.replaceSection(indexOfText, 0, selectedText.joinIntoString("\n"));
		}
		
	}
	else if(direction.equalsIgnoreCase("backwards"))
	//single line back tab
	if(selectedText.size()<1){
		pos1 = getCaretPos();
		//Logger::writeToLog(csdArray[pos1.getLineNumber()]);
		currentLine = csdArray[pos1.getLineNumber()];
		if(csdArray[pos1.getLineNumber()].substring(0, 1).contains("\t")){
			csdArray.set(pos1.getLineNumber(), currentLine.substring(1));
			csdText = csdArray.joinIntoString("\n");
		}
		else
			return;
	}
	//multiline back tab
		else{ 
		//multiline tab
		int indexOfText = getAllText().indexOf(getSelectedText());
		csdText = getAllText().replace(getSelectedText(), "");
		for(int i=0;i<selectedText.size();i++)
			if(selectedText[i].substring(0, 1).equalsIgnoreCase("\t"))
						selectedText.set(i, selectedText[i].substring(1));	

		csdText = csdText.replaceSection(indexOfText, 0, selectedText.joinIntoString("\n"));
		}

	//Logger::writeToLog(newTextArray.joinIntoString("\n"));
	setAllText(csdText);	
	if(selectedText.size()>0)
		highlightLine(selectedText.joinIntoString("\n"));
	else
		moveCaretTo(CodeDocument::Position (getDocument(), getAllText().indexOf(currentLine.substring(1))), false);

	sendActionMessage("make popup invisible");	
}

void CsoundCodeEditor::toggleComments()
{
	StringArray selectedText;
	selectedText.addLines(getSelectedText());
	StringArray csdArray;
	csdArray.addLines(this->getAllText());
	String lastLine;

	for(int i=0;i<csdArray.size();i++)
		for(int y=0;y<selectedText.size();y++)
			if(selectedText[y]==csdArray[i]){
				if(!csdArray[i].equalsIgnoreCase("")){
					if(selectedText[y].substring(0, 1).equalsIgnoreCase(";"))
					csdArray.set(i, selectedText[y].substring(1));
					else
					csdArray.set(i, ";"+selectedText[y]);	
					lastLine = selectedText[y].substring(1);
				}
			}
			
	this->setAllText(csdArray.joinIntoString("\n"));
	moveCaretTo(CodeDocument::Position (getDocument(), getAllText().indexOf(lastLine)+lastLine.length()), false);
}

//=================== addPopupMenuItems =======================
void CsoundCodeEditor::addPopupMenuItems (PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent)
{
	menuToAddTo.addItem(1, "Cut");
	menuToAddTo.addItem(1, "Copy");
	menuToAddTo.addItem(1, "Paste");
	menuToAddTo.addItem(1, "Select All");
	menuToAddTo.addSeparator();
	menuToAddTo.addItem(1, "Undo");
	menuToAddTo.addItem(1, "Redo");
	menuToAddTo.addItem(10, "Add to repo");
	PopupMenu m;
	int repoIndex = 100;
	ScopedPointer<XmlElement> xmlElement;
	xmlElement = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");
	if(xmlElement)
	forEachXmlChildElement (*xmlElement, e)
		{
		m.addItem(repoIndex, e->getTagName());
		repoEntries.add(e->getTagName());
		repoIndex++;
		}	
	xmlElement =nullptr;	
	menuToAddTo.addSubMenu("Insert from repo", m);
};


Rectangle<int> CsoundCodeEditor::getCaretPoisition()
{
pos1 = getCaretPos();
return getCharacterBounds(pos1);		
}


void CsoundCodeEditor::performPopupMenuAction (int menuItemID){
	if(menuItemID==1000){	
		pos1 = getDocument().findWordBreakBefore(getCaretPos());
		String line = getDocument().getLine(pos1.getLineNumber());			
		Logger::writeToLog(line);
		line = getTextInRange(this->getHighlightedRegion());
		Logger::writeToLog(line);
		sendActionMessage(line);			
	}

	//add to repo
	else if(menuItemID==10){
			addToRepository();
	}

	//insert from repo
	else if(menuItemID>=100){
	ScopedPointer<XmlElement> xmlElement;
	xmlElement = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");

	forEachXmlChildElement (*xmlElement, e)
		{
		if(e->getTagName()==repoEntries[menuItemID-100])
			insertText(e->getAllSubText());
		}
	xmlElement = nullptr;
	}	
	
};

void CsoundCodeEditor::addRepoToSettings()
{

}

void CsoundCodeEditor::updateCaretPosition()
{
	/*
	Logger::writeToLog("Updating caret position");
	int columnEdit = 1;
	
	if(columnEdit==1){
	StringArray selectedText;
	selectedText.addLines(getTextInRange(this->getHighlightedRegion()));
	Rectangle<int> newCaretPosition(getCharacterBounds(getSelectionStartCaretPosition()));
	newCaretPosition.setHeight(getCharacterBounds (getCaretPos()).getHeight()*selectedText.size());
	caret->setCaretPosition (newCaretPosition);
	}
	else*/
	setCaretPos(getCharacterBounds (getCaretPos()));
}

void CsoundCodeEditor::addToRepository()
{
	AlertWindow alert("Add to Repository", "Enter a name and hit 'escape'", AlertWindow::NoIcon, this->getTopLevelComponent()); 
	//CabbageLookAndFeel basicLookAndFeel;
	//alert.setLookAndFeel(&basicLookAndFeel);
	alert.setColour(TextEditor::textColourId, Colours::white);
	alert.setColour(TextEditor::highlightedTextColourId, Colours::lime);
	alert.addTextEditor("textEditor", "enter name", "");
	alert.getTextEditor("textEditor")->setColour(TextEditor::textColourId, Colours::lime);	
	alert.runModalLoop();
	Logger::writeToLog(alert.getTextEditorContents("textEditor"));
	String repoEntryName = alert.getTextEditorContents("textEditor");
	
	if(repoEntryName!="enter name"){
		ScopedPointer<XmlElement> repoXml;
		ScopedPointer<XmlElement> newEntryXml;
		repoXml = appProperties->getUserSettings()->getXmlValue("CopeRepoXmlData");
		newEntryXml = new XmlElement(repoEntryName);
		newEntryXml->addTextElement(getSelectedText());
		if(newEntryXml)
		repoXml->addChildElement(newEntryXml);
		appProperties->getUserSettings()->setValue("CopeRepoXmlData", repoXml);	
		
		repoXml = nullptr;
		newEntryXml = nullptr;
	}
}

String CsoundCodeEditor::getLineText(){
	StringArray csdLines;
	csdLines.addLines(getDocument().getAllContent());
	pos1 = getDocument().findWordBreakBefore(getCaretPos());
	Logger::writeToLog(csdLines[pos1.getLineNumber()]);	
	return csdLines[pos1.getLineNumber()];
}	

String CsoundCodeEditor::getAllText(){
	return getDocument().getAllContent();
}	

void CsoundCodeEditor::setAllText(String text){
	getDocument().replaceAllContent(text);
}

String CsoundCodeEditor::getTempChannelInstr(){
	String channel = "event_i \"i\", 999.999, 0, .1\n";
	channel << "instr 999\n";
	channel << getLineText() << "\n";
	channel << "endin\n";	
	Logger::writeToLog(channel);
	return channel;
}

String CsoundCodeEditor::getSelectedText(){
	String selectedText = getTextInRange(this->getHighlightedRegion());
	Logger::writeToLog(selectedText);	
	return selectedText;
}

StringArray CsoundCodeEditor::getSelectedTextArray(){
	StringArray tempArray;
	String selectedText = getTextInRange(this->getHighlightedRegion());
	tempArray.addLines(selectedText);
	Logger::writeToLog(selectedText);	
	return tempArray;
}

String CsoundCodeEditor::getInstrumentText(){
	StringArray csdLines;
	csdLines.addLines(getDocument().getAllContent());
	pos1 = getDocument().findWordBreakBefore(getCaretPos());
	int startOfInstrDef, endOfInstrDef;
	int index = pos1.getLineNumber();
	while(index>0){
	if(csdLines[index].contains("instr")){
		startOfInstrDef = index-1;
		break;
		}
	else
		index--;
	}

	index = pos1.getLineNumber();
	while(index>0){
	if(csdLines[index].contains("endin")){
		endOfInstrDef = index;
		break;
		}
	else
		index++;
	}		
	
	String selectedText="";
	for(int i = startOfInstrDef;i<=endOfInstrDef;i++)
	selectedText += csdLines[i]+"\n";
	Logger::writeToLog(selectedText);	
	return selectedText;
}

void CsoundCodeEditor::codeDocumentTextInserted(const juce::String &,int)
{
	
textChanged = true;
pos1 = getDocument().findWordBreakBefore(getCaretPos());
String lineFromCsd = getDocument().getLine(pos1.getLineNumber());

if(CabbageUtils::getPreference(appProperties, "EnablePopupDisplay"))
	{
	String opcodeHelpString;
	StringArray syntaxTokens, csdLineTokens;
	csdLineTokens.clear();
	csdLineTokens.addTokens(lineFromCsd, " ,\t", "");

		for(int i=0;i<opcodeStrings.size();i++){
			opcodeHelpString = opcodeStrings[i];
			syntaxTokens.clear();	
			syntaxTokens.addTokens(opcodeHelpString, ";", "\"");
			if(syntaxTokens.size()>3)
			for(int x=0;x<csdLineTokens.size();x++){
				//Logger::writeToLog(csdLineTokens[x]);
				if(syntaxTokens[0].removeCharacters("\"")==csdLineTokens[x].trim()){
					if(syntaxTokens[0].length()>3){
							//Logger::writeToLog(syntaxTokens[0]);
							sendActionMessage("popupDisplay"+syntaxTokens[2]);
							opcodeTokens = syntaxTokens;
							x=csdLineTokens.size();
							i=opcodeStrings.size();	
						}
					}	
					
			}
			
		}
	}
}

void CsoundCodeEditor::codeDocumentTextDeleted(int,int){
textChanged = true;
sendActionMessage("make popup invisible");	
}
