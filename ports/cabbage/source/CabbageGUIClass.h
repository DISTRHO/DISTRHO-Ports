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

//   You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
    
*/
#ifndef CABBPARSE_H
#define CABBPARSE_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "CabbageUtils.h"


class CabbageGUIClass : public CabbageUtils
{
        double width, height, top, left, isRect, min, max, minX, minY, maxX, tabbed, maxY, comboRange, fftSize, overlapSize, frameSize,
		noOfMenus, onoff, midiChan, midiCtrl, sliderRange, xypadRangeY, xypadRangeX, noSteps, noPatterns, pvsChannel, alpha,
		line, anchor, linkTo, scaleX, scaleY, value, valueX, valueY, maxItems, sliderIncr, sliderSkew, decimalPlaces, rCtrls, lineIsVertical;
        StringArray items, onoffcaptions, key, channels, snapshotData, colours;

        String channel, name, sizeText, posText, boundsText, text, type, plant, reltoplant, bounds, range, fileType, workingDir,
        shape, beveltype, caption, kind, topitem, yChannel, xChannel, author, native, basetype,
        exit, csstdout, cssetup, file, debugMessage, xyChannel, pluginID, tabpage, preset;
		Colour outline, fill, fontcolour, textcolour, colour, trackerFill;
		int tableNum, textBox, numPresets, masterSnap, plantButton, xyAutoIndex, paramIndex, numTables, soundfilerIndex;
		Array<int> vuConfig;
		Array<int> tableNumbers;
		Array<float> tableChannelValues;
		

		//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageGUIClass);
public:
	//constructor
	NamedValueSet cabbageIdentifiers;
	CabbageGUIClass(String str, int ID);
	CabbageGUIClass(){};
    ~CabbageGUIClass();
	int parse(String str);
	float getNumProp(String prop);
	float getNumProp(String prop, int index);
	void setNumProp(String prop, float val);
	void setTableChannelValues(int index, float val);
	float getTableChannelValues(int index);
	void addTableChannelValues();
	void setStringProp(String prop, String val);
	void setStringProp(String prop, int index, String value);
	String getStringProp(String prop);
	String getStringProp(String prop, int index);
	String getPropsString();
	String getColourProp(String prop);
	float getNumPropVal(String prop);
	void setNumPropVal(String prop, float val);
	static String getCabbageCodeFromIdentifiers(NamedValueSet props);
	static String getStringForIdentifier(var props, String identifier, String type);
	
	Rectangle<int> getBounds(){
	Rectangle<int> bounds(left, top, width, height);
	return bounds;	
	}
	
	void setBounds(Rectangle<int> bounds){
	left = bounds.getX();
	top = bounds.getY();
	width = bounds.getWidth();
	height = bounds.getHeight();
	}	
	
	static StringArray getIdentifiers(){
		StringArray test;
		return test;
	}
	
	Rectangle<int> getComponentBounds();
	
	
	StringArray getStringArrayProp(String prop);
	String getStringArrayPropValue(String prop, int index);
	
	int getIntArrayPropValue(String prop, int index);
	Array<int> getIntArrayProp(String prop);
	
	float getFloatArrayPropValue(String prop, int index);
	Array<float> getFloatArrayProp(String prop);
	
	
	
	
	StringArray getChannels(){
		return channels;
	}


	inline int getNumPresets(){
		return snapshotData.size();
	}

	inline void clearPresets(){
		snapshotData.clear();
	}

	inline String setOnOffcaptions(int index, String str){
		if(index<=onoffcaptions.size())
		onoffcaptions.getReference(index)=str;
	}

	inline String getOnOffcaptions(int index){
		if(index<=onoffcaptions.size())
		return onoffcaptions.getReference(index);
		else return "";
	}

	inline String setItems(int index, String str){
		if(index<=items.size())
		items.getReference(index)=str;
		else
			return "";
	}

	inline String getItems(int index){
	if(isPositiveAndBelow (index, items.size()))
		return items.getReference(index);
		else return "";
	}

	inline String getColours(int index){
		if(colours.size())
		return colours.getReference(index);
		else return "";
	}
	
	inline int getNumberOfColours(){
		return colours.size();

	}	

	inline StringArray getItemArray(){
		if(items.size())
		return items;
		else return StringArray();
	}

	inline int getItemsSize(){
		return items.size();
	}

	inline Array<int> getVUConfig(){
		return vuConfig;
	}

//	inline String getChannel(int index){
//		return channels[index];
//	}

	inline void addDummyChannel(String dummyChan){
		return channels.add(dummyChan);
	}

//	inline String getTableChannel(int index){
//		return channels[index];
//	}

//	inline int getNumberOfTableChannels(){
//		return channels.size();
//	}

	inline int getKeySize(){
		return key.size();
	}

	inline String getkey(int index){
		return key.getReference(index);
	}

//	inline int getNumberOfTables(){
//		return numTables;
//	}

//	int getTableNumbers(int index)
//	{
//		if(index<tableNumbers.size())
//		return tableNumbers[index];		
//	}

//	inline Array<int> getTableNumbersArray()
//	{
//		return tableNumbers;		
//	}
	
};

#endif
