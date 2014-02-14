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

void CsoundCodeEditor::addPopupMenuItems (PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent){
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

void CsoundCodeEditor::insertText(String text){
	pos1 = getCaretPos();
	getDocument().insertText(pos1, text);
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

int val = CabbageUtils::getPreference(appProperties, "EnablePopupDisplay");
if(val){
	String parsedString;
	StringArray syntaxTokens, csdLineTokens;
	csdLineTokens.clear();
	csdLineTokens.addTokens(lineFromCsd, " ", "\n");

		for(int i=0;i<opcodeStrings.size();i++){
			parsedString = opcodeStrings[i];
			syntaxTokens.clear();	
			syntaxTokens.addTokens(parsedString, ";", "\"");
			if(syntaxTokens.size()>3)
			for(int x=0;x<csdLineTokens.size();x++){
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