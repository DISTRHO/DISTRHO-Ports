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

namespace CabbageIDs
{
    static const Identifier top = "top";
    static const Identifier left = "left";
    static const Identifier width = "width";
    static const Identifier height = "height";
    static const Identifier min = "min";
    static const Identifier max = "max";
    static const Identifier value = "value";
    static const Identifier channel = "channel";
    static const Identifier colour = "colour";
	static const Identifier tablecolour = "tablecolour";
    static const Identifier fontcolour= "fontcolour";
	static const Identifier items = "items";
	static const Identifier text = "text";
	static const Identifier range = "range";
	static const Identifier sliderrange = "sliderrange";
	static const Identifier amprange = "amprange";
	static const Identifier caption = "caption";
	static const Identifier basetype = "basetype";
	static const Identifier textbox = "textbox";
	static const Identifier name = "name";
	static const Identifier type = "type";
	static const Identifier trackercolour = "trackercolour";
	static const Identifier sliderskew = "sliderskew";
	static const Identifier sliderincr = "sliderince";
	static const Identifier midichan = "midichan";
	static const Identifier midictrl = "midictrl";
	static const Identifier kind = "kind";
	static const Identifier decimalplaces = "decimalplaces";
	static const Identifier mode = "mode";
	static const Identifier shape = "shape";
	static const Identifier channeltype = "channeltype";
	static const Identifier comborange = "comborange";
	static const Identifier populate = "populate";
	static const Identifier outline = "outline";
	static const Identifier popup = "popup";
	static const Identifier plant = "plant";
	static const Identifier line = "line";
	static const Identifier tablenumber = "tablenumber";
	static const Identifier resizemode = "resizemode";
	static const Identifier drawmode = "drawmode";
	static const Identifier readonly = "readonly";
	static const Identifier xyautoindex = "xyautoindex";
	static const Identifier file = "file";
	static const Identifier latched = "latched";
	static const Identifier xchannel = "xchannel";
	static const Identifier ychannel = "ychannel";
	static const Identifier minx = "minx";
	static const Identifier miny = "miny";
	static const Identifier maxx = "maxx";
	static const Identifier maxy = "maxy";
	static const Identifier valuex = "valuex";
	static const Identifier fill = "fill";
	static const Identifier valuey = "valuey";
	static const Identifier textcolour = "textcolour";
	static const Identifier pluginid = "pluginid";
	static const Identifier tabs = "tabs";
	static const Identifier tabbed = "tabbed";
	static const Identifier rangey = "rangey";
	static const Identifier rangex = "rangex";
	static const Identifier tabpage = "tabpage";
	static const Identifier filetype = "filetype";
	static const Identifier workingdir = "workingdir";
	static const Identifier author = "author";
	static const Identifier xychannel = "xychannel";
	static const Identifier guirefresh = "guirefresh";
	
	//type of widgets/controls/messages
	static const String combobox = "combobox";
	static const String rslider = "rslider";
	static const String hslider = "hslider";
	static const String vslider = "vslider";
	static const String checkbox = "checkbox";
	static const String button = "button";
	static const String filebutton = "filebutton";
	static const String table = "table";
	static const String groupbox = "groupbox";
	static const String image = "image";
	static const String form = "form";
	static const String xypad = "xypad";
	static const String stringchannel = "string";
	static const String hostbpm = "HOST_BPM";
	static const String timeinseconds = "TIME_IN_SECONDS";
	static const String isplaying = "IS_PLAYING";
	static const String isrecording = "IS_RECORDING";
	static const String hostppqpos = "HOST_PPQ_POS";
	static const String csoundoutput = "csoundoutput";

};

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
	float getNumProp(Identifier prop);
	float getNumProp(Identifier prop, int index);
	void setNumProp(Identifier prop, float val);
	void setTableChannelValues(int index, float val);
	float getTableChannelValues(int index);
	void addTableChannelValues();
	void setStringProp(Identifier prop, String val);
	void setStringProp(Identifier prop, int index, String value);
	String getStringProp(Identifier prop);
	String getStringProp(Identifier prop, int index);
	String getPropsString();
	String getColourProp(Identifier prop);
	float getNumPropVal(Identifier prop);
	void setNumPropVal(Identifier prop, float val);
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
	
	
	StringArray getStringArrayProp(Identifier prop);
	String getStringArrayPropValue(Identifier prop, int index);
	
	int getIntArrayPropValue(Identifier prop, int index);
	Array<int> getIntArrayProp(Identifier prop);
	
	float getFloatArrayPropValue(Identifier prop, int index);
	Array<float> getFloatArrayProp(Identifier prop);
	
	
	
	
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

