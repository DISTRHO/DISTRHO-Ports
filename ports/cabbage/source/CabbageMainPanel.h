/*
  Copyright (C) 2007 Rory Walsh

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
#ifndef CABBMAIN_H
#define CABBMAIN_H

#include "JuceHeader.h"
#include "CabbageUtils.h"

class ComponentLayoutEditor;

//for grouping objects when a user drags over them
//-----------------------------------------------------------------------------

//main panel onto which all GUI objects are placed in standalone host
//-----------------------------------------------------------------------------

class CabbageMainPanel   : public Component,
						   public CabbageUtils,
						   public ActionBroadcaster
						 
{
private:	
	
	ScopedPointer<ComponentLayoutEditor> layoutEditor;
	OwnedArray<Component > comps;
	OwnedArray<Component > clipBoard;
	
	Colour panelColour;
	int currentIndex;
	bool LOCKED;
	String mouseState;
	String colour;
	bool doubleClick;


public:
	Rectangle<int> currentBounds;
	Array<Rectangle<int> > childBounds;
	Array<Rectangle<int> > origChildBounds;
	Array<Point<int> > origSelectedCompBounds;
	Array<Point<int> > newSelectedCompBounds;


	int width, height, top, left;
	inline void setCompColour(String col){
		colour = col;
	}

	inline String getColour(){
		return colour;
	}

	inline String getMouseState(){
		return mouseState;
	}

	inline void setMouseState(String state){
		mouseState = state;
	}

	inline void setDoubleClick(bool dblClick){
		doubleClick = dblClick;
	}

	bool isDoubleClick(){
		return doubleClick;
	}

	//String getCurrentBounds(){
	//	return "bounds(" + String(currentBounds.getX()) + String(", ") + String(currentBounds.getY()) + String(", ") + String(currentBounds.getWidth()) + String(", ")
	//		+ String(currentBounds.getHeight()) + String(")");
	//}

	//String getCurrentChildBounds(int i){
	//	return "bounds(" + String(childBounds[i].getX()) + String(", ") + String(childBounds[i].getY()) + String(", ") + String(childBounds[i].getWidth()) + String(", ")
	//		+ String(childBounds[i].getHeight()) + String(")");
	//}

	//String getCurrentOrigChildBounds(int i){
	//	return "bounds(" + String(origChildBounds[i].getX()) + String(", ") + String(origChildBounds[i].getY()) + String(", ") + String(origChildBounds[i].getWidth()) + String(", ")
	//		+ String(origChildBounds[i].getHeight()) + String(")");
	//}
	//	
	//String getCurrentPos(){
	//return String("pos(") + String(left) + String(", ") + String(top) + String(")");
	//}


	String getCurrentSize(){
	return String("size(") + String(width) + String(", ") + String(height) + String(")");
	}

	CabbageMainPanel();
	~CabbageMainPanel();
	void paint(Graphics &g);
	ComponentLayoutEditor* getLayoutEditor();
	void setLayoutEditor(ComponentLayoutEditor* ed);

	inline int getIndex(){
	return currentIndex;
	}

	int getCurrentWidth(){
		return width;
	}

	int getCurrentHeight(){
		return height;
	}

	int getCurrentTop(){
		return top;
	}

	int getCurrentLeft(){
		return left;
	}

	inline void setIndex(int index){
	currentIndex = index;
	}

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageMainPanel);

};

#endif