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

#ifndef __CODE_EDITOR__
#define __CODE_EDITOR__

#include "../JuceHeader.h"
#include "CsoundTokeniser.h"
#include "PythonTokeniser.h"
#include "CommandManager.h"
#include "../CabbageUtils.h"


extern ApplicationProperties* appProperties;
extern CabbageTimer* cabbageTimer;

class CsoundCodeEditor : public CodeEditorComponent,
						 public ActionBroadcaster,
						 public CodeDocument::Listener
	{
	public:
			CodeDocument::Position positionInCode;
			CsoundCodeEditor(String type, CodeDocument &document, CodeTokeniser *codeTokeniser);

			~CsoundCodeEditor();


	bool keyPressed (const KeyPress& key){
	//Logger::writeToLog(key.getTextDescription());
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
        else                                                                
		return false;
    }

    //handleUpdateNowIfNeeded();
    return true;
	}

	void handleDirectionKey(){
		
	}

	void handleEscapeKey(){
	if(type=="python")
		sendActionMessage("make python invisible");
	if(type=="csound")
		sendActionMessage("make popup invisible");	
	}
	
	void handleReturnKey (){
	if(type=="csound"){
		insertText("\n");
		sendActionMessage("make popup invisible");		
	}		
	}	
	
	void addPopupMenuItems (PopupMenu &menuToAddTo, const MouseEvent *mouseClickEvent);
	void performPopupMenuAction (int menuItemID);
	String getLineText();
	String getTempChannelInstr();
	String getSelectedText();
	String getInstrumentText();
	Rectangle<int> getCaretPoisition();
	StringArray getSelectedTextArray();
	void addRepoToSettings();
	void insertText(String text);
	void addToRepository();
	StringArray repoEntries;
	String getAllText();
	void setAllText(String text);
	void highlightLine(String line);
	void codeDocumentTextDeleted(int,int);
	void codeDocumentTextInserted(const juce::String &,int);
	bool textChanged;
	
	void setOpcodeStrings(String opcodes){
	opcodeStrings.addLines(opcodes);	
	}
	
	String getOpcodeToken(int index){
	return opcodeTokens[index];	
	}	
	
	private:
		int xPos, yPos, prevXpos;
		CodeDocument::Position pos1, pos2;
		Colour selectedColour;
		String type;
		StringArray opcodeStrings;
		StringArray opcodeTokens;
	};

#endif