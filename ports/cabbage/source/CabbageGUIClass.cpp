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
  
#include "CabbageGUIClass.h"

CabbageGUIClass::CabbageGUIClass(String compStr, int ID):
								debugMessage(""), 
								plant(""), 
								reltoplant(""), 
								scaleX(1), 
								scaleY(1), 
								midiChan(-99), 
								midiCtrl(-99),
								boundsText(""),
								posText(""),
								sizeText(""),
								preset(""),
								trackerFill(),
								masterSnap(0),
								width(0),
								height(0),
								left(0),
								top(0), 
								fill(Colours::cornflowerblue),
								xyAutoIndex(0),
								fileType(""),
								workingDir(""),
								alpha(1),
								sliderRange(1),
								numTables(0),
								sliderIncr(.01),
								decimalPlaces(0),
								tabbed(0),
								soundfilerIndex(0),
								native("")
{
//Default values are assigned to all attributres 
//before parsing begins
	cabbageIdentifiers.set("scalex", 1);
	cabbageIdentifiers.set("scaley", 1);
	items.clear();
	items.add("");
	key.clear();
	min=0;
	max=1;
	sliderSkew = 1;
	value = 0;
	StringArray strTokens;
	strTokens.addTokens(compStr, " ", "\"");
	//changing case to lower to make syntax non case-sensitive

	//===============sliders==================//
	if(strTokens[0].trim() == "hslider"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "interactive");
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 150);
		cabbageIdentifiers.set(CabbageIDs::height, 50);		
		var channels;
		channels.append("hslider");
		cabbageIdentifiers.set(CabbageIDs::channel, channels);
		cabbageIdentifiers.set(CabbageIDs::min, 0);
		cabbageIdentifiers.set(CabbageIDs::max, 100);	
		cabbageIdentifiers.set(CabbageIDs::value, 50);
		cabbageIdentifiers.set(CabbageIDs::text, "");
		cabbageIdentifiers.set(CabbageIDs::textbox, 0.f);
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::trackercolour, Colours::lime.toString());
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());	
		cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
		cabbageIdentifiers.set(CabbageIDs::sliderincr, .0001);
		cabbageIdentifiers.set(CabbageIDs::midichan, -99);
		cabbageIdentifiers.set(CabbageIDs::midictrl, -99);

		//these don't appear in the props dialog
		cabbageIdentifiers.set(CabbageIDs::name, "hslider");
		cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::kind, "horizontal");
		cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);

	}
    else if(strTokens[0].trim() == "vslider"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "interactive");
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 50);
		cabbageIdentifiers.set(CabbageIDs::height, 150);	
		var channels;
		channels.append("vslider");
		cabbageIdentifiers.set(CabbageIDs::channel, channels);		  
		cabbageIdentifiers.set(CabbageIDs::min, 0);
		cabbageIdentifiers.set(CabbageIDs::max, 100);	
		cabbageIdentifiers.set(CabbageIDs::value, 50);
		cabbageIdentifiers.set(CabbageIDs::text, "");
		cabbageIdentifiers.set(CabbageIDs::textbox, 0.f);
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::trackercolour, Colours::lime.toString());
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());	
		cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
		cabbageIdentifiers.set(CabbageIDs::sliderincr, .0001);
		cabbageIdentifiers.set(CabbageIDs::midichan, -99);
		cabbageIdentifiers.set(CabbageIDs::midictrl, -99);
	
		//these don't appear in the props dialog
		cabbageIdentifiers.set(CabbageIDs::name, "vslider");
		cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::kind, "vertical");
		cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);	
		

	}
    else if(strTokens[0].trim() == "rslider"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "interactive");
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 60);
		cabbageIdentifiers.set(CabbageIDs::height, 60);
		var channels;
		channels.append("rslider");
		cabbageIdentifiers.set(CabbageIDs::channel, channels);	
		cabbageIdentifiers.set(CabbageIDs::min, 0);
		cabbageIdentifiers.set(CabbageIDs::max, 100);	
		cabbageIdentifiers.set(CabbageIDs::value, 50);
		cabbageIdentifiers.set(CabbageIDs::text, "");
		cabbageIdentifiers.set(CabbageIDs::textbox, 0.f);
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::trackercolour, Colours::lime.toString());
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());	
		cabbageIdentifiers.set(CabbageIDs::sliderskew, 1);
		cabbageIdentifiers.set(CabbageIDs::sliderincr, .0001);
		cabbageIdentifiers.set(CabbageIDs::midichan, -99);
		cabbageIdentifiers.set(CabbageIDs::midictrl, -99);
		
		//these don't appear in the props dialog
		cabbageIdentifiers.set(CabbageIDs::name, "rslider");
		cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::kind, "rotary");
		cabbageIdentifiers.set(CabbageIDs::decimalplaces, 1);			
		
	}

    else if(strTokens[0].trim() == "source"){
		  cabbageIdentifiers.set("basetype", "layout");
          top = 10;
          left = 10;
          width = 400;
          height = 240;
          channel = "";
          name = "source";
		  caption = "";
		  type = name;
          //name.append(String(ID), 1024);
	}

	//===============info button==================//
    else if(strTokens[0].trim() == "infobutton"){
		  cabbageIdentifiers.set("basetype", "layout");
          top = 10;
          left = 10;
          width = 600;
          height = 440;
          channel = "";
          name = "infobutton";
		  caption = "";
		  type = name;
          //name.append(String(ID), 1024);
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 80);
		cabbageIdentifiers.set(CabbageIDs::height, 40);	
		cabbageIdentifiers.set(CabbageIDs::text, "");
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());	
		cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());	
		cabbageIdentifiers.set(CabbageIDs::type, "infobutton");
		cabbageIdentifiers.set(CabbageIDs::name, "infobutton");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		
	}

	//===============directory list==================//
    else if(strTokens[0].trim() == "directorylist"){
		  cabbageIdentifiers.set("basetype", "layout");
          top = 10;
          left = 10;
          width = 600;
          height = 440;
          channel = "";
          name = "directorylist";
		  caption = "";
		  type = name;
          //name.append(String(ID), 1024);
	}


    else if(strTokens[0].trim() == "patmatrix"){
		  cabbageIdentifiers.set("basetype", "layout");
          top = 10;
          left = 10;
          width = 400;
          height = 240;
          channel = "";
          name = "patmatrix";
		  caption = "Pattern Matrix";
		  type = name;
		  noSteps = 8;
		  noPatterns=1;
		  rCtrls=0;
          //name.append(String(ID), 1024);
	}

	//===============file button==================//
    else if(strTokens[0].trim() == "filebutton"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 80;
		height = 40;
		channel = "filebuttonchan";
		var array;
		array.append("Open file");
		array.append("Open file");
		items.add("");
		items.add("");
		name = "filebutton";
		max = 1;
		min = 0;
		caption = "";
		type = name;
		name.append(String(ID), 1024);

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 80);
		cabbageIdentifiers.set(CabbageIDs::height, 40);	
		var channels;
		channels.append("filebutton");
		cabbageIdentifiers.set(CabbageIDs::channel, channels);
		cabbageIdentifiers.set(CabbageIDs::value, 1);	
		cabbageIdentifiers.set(CabbageIDs::text, array);
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());	
		cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());	
		cabbageIdentifiers.set(CabbageIDs::type, "filebutton");
		cabbageIdentifiers.set(CabbageIDs::name, "filebutton");
		cabbageIdentifiers.set(CabbageIDs::mode, "open");
		
		
		cabbageIdentifiers.set("name", cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));	
		var populate;
		populate.append("");
		populate.append("");
		cabbageIdentifiers.set("populate", populate);  
	}

	//===============button==================//
    else if(strTokens[0].trim() == "button"){
		cabbageIdentifiers.set("basetype", "interactive");
		top = 10;
		left = 10;
		width = 80;
		height = 40;
		channel = "buttonchan";
		items.add("");
		items.add("");
		name = "button";
		max = 1;
		min = 0;
		caption = "";
		type = name;
		name.append(String(ID), 1024);

		var array;
		array.append("Push me");
		array.append("Push me");

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 80);
		cabbageIdentifiers.set(CabbageIDs::height, 40);	
		var channels;
		channels.append("buttonchan");
		cabbageIdentifiers.set(CabbageIDs::channel, channels);
		cabbageIdentifiers.set(CabbageIDs::value, 1);	
		cabbageIdentifiers.set(CabbageIDs::text, array);
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());	
		cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());	
		cabbageIdentifiers.set(CabbageIDs::latched, 1);
		cabbageIdentifiers.set(CabbageIDs::type, "button");
		cabbageIdentifiers.set(CabbageIDs::name, "button");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		  
	}

	//===============checkbox==================//
    else if(strTokens[0].trim() == "checkbox"){
		cabbageIdentifiers.set("basetype", "interactive");
		top = 10;
		left = 10;
		width = 100;
		height = 22;
		channel = "checkchan";
		type = "checkbox";
		caption = "";
		name = "checkbox";
		caption = "";
		items.add("");

		var array;
		array.append("Check box");

		min = 0;
		max = 1;
		shape = "square";
		name.append(String(ID), 1024);
		//default colour for toggles
		colour = Colours::lime;

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 100);
		cabbageIdentifiers.set(CabbageIDs::height, 22);	
		var channels;
		channels.append("checkchan");
		cabbageIdentifiers.set(CabbageIDs::channel, channels);
		cabbageIdentifiers.set(CabbageIDs::text, array);
		cabbageIdentifiers.set(CabbageIDs::value, 0);	
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::shape, "square");
		
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::lime.toString());	
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::type, "checkbox");
		cabbageIdentifiers.set(CabbageIDs::name, "checkbox");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));		  
		  
	}
	//===============combobox==================//
    else if(strTokens[0].trim() == "combobox"){
		cabbageIdentifiers.set("basetype", "interactive");
		items.clear();
		top = 10;
		left = 10;
		width = 80;
		height = 22;
		channel = "combochan";
		caption = "";
		//add one item by befault, will be overwritten by users values
		var array;
		array.append("Item 1");
		array.append("Item 2");
		array.append("Item 3");
		array.append("Item 4");

		items.add("Item 1");
		items.add("Item 2");
		items.add("Item 3");
		items.add("Item 4");


		name = "combobox";		 
		type = name;
		name.append(String(ID), 1024);
		min=1;
		value = 1;

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 80);
		cabbageIdentifiers.set(CabbageIDs::height, 20);	
		var channels;
		channels.append("combochan");
		cabbageIdentifiers.set(CabbageIDs::channel, channels);
		cabbageIdentifiers.set(CabbageIDs::channeltype, "number");
		cabbageIdentifiers.set(CabbageIDs::text, array);
		cabbageIdentifiers.set(CabbageIDs::value, 1);	
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::black.toString());
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::type, "combobox");
		cabbageIdentifiers.set(CabbageIDs::name, "combobox");		
		cabbageIdentifiers.set(CabbageIDs::comborange, 4);
		var populate;
		populate.append("");
		populate.append("");
		cabbageIdentifiers.set(CabbageIDs::populate, populate);
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		 
	} 
	//===============label==================//
    else if(strTokens[0].trim() == "label"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 100;
		height = 16;
		channel = "label";
		beveltype = "none";
		colour = CabbageUtils::getComponentFontColour();
		fontcolour = CabbageUtils::getComponentFontColour();
		text = "hello";
		name = "label";
		type = name;
		
		name.append(String(ID), 1024);
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 80);
		cabbageIdentifiers.set(CabbageIDs::height, 16);	
		cabbageIdentifiers.set(CabbageIDs::text, "hello");	
		cabbageIdentifiers.set(CabbageIDs::min, 1);
		cabbageIdentifiers.set(CabbageIDs::name, "label");
		cabbageIdentifiers.set(CabbageIDs::type, cabbageIdentifiers.getWithDefault("name", "").toString());
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
	}
	//===============multitab==================//	
    else if(strTokens[0].trim() == "multitab"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 100;
		height = 16;
		channel = "multitab";
		colour = CabbageUtils::getComponentFontColour();
		fontcolour = CabbageUtils::getComponentFontColour();
		items.add("Tab 1");
		name = "multitab";
		type = name;
		name.append(String(ID), 1024);
	}	
	//===============soundfiler==================//	
    else if(strTokens[0].trim() == "soundfiler"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 300;
		height = 200;
		colour = Colours::lime;
		fontcolour = CabbageUtils::getComponentFontColour();
		text = "";
		name = "soundfiler";
		type = name;
		name.append(String(ID), 1024);


		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 300);
		cabbageIdentifiers.set(CabbageIDs::height, 200);	
		var channels;
		channels.append("soundfiler1");
		channels.append("soundfiler2");
		cabbageIdentifiers.set(CabbageIDs::channel, channels);	  
		cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::type, "soundfiler");
		cabbageIdentifiers.set(CabbageIDs::name, "soundfiler");
		cabbageIdentifiers.set(CabbageIDs::file, "");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
	}	
	//===============snapshot==================//	
    else if(strTokens[0].trim() == "snapshot"){
		cabbageIdentifiers.set("basetype", "layout");
		items.clear();
		top = 10;
		left = 10;
		width = 200;
		height = 20;
		channel = "snapshot";
		colour = CabbageUtils::getComponentFontColour();
		fontcolour = Colours::white;
		text = "Snap";
		name = "snapshot";
		type = name;
		name.append(String(ID), 1024);
		items.add("One");
		items.add("Two");
		items.add("Three");
		items.add("Four");
		min=1;
		value=1;
		var val;
		val.append("One");
		val.append("Two");
		val.append("Three");
		val.append("Four");
		val.append("Five");

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 200);
		cabbageIdentifiers.set(CabbageIDs::height, 20);	
		cabbageIdentifiers.set(CabbageIDs::text, val);
		cabbageIdentifiers.set(CabbageIDs::value, value);
		cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getComponentFontColour().toString());
		cabbageIdentifiers.set(CabbageIDs::type, "snapshot");
		cabbageIdentifiers.set(CabbageIDs::name, "snapshot");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));

		  
	}
	//===============image==================//
    else if(strTokens[0].trim() == "image"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 160;
		height = 120;
		channel = "";
		beveltype = "none";
		fontcolour = Colours::white;
		text = "";
		line = 2;
		name = "image";
		type = name;
		shape="rounded";
		outline = Colours::black;
		colour = Colours::white;
		name.append(String(ID), 1024);
		plantButton = 0;


		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 160);
		cabbageIdentifiers.set(CabbageIDs::height, 120);	
		cabbageIdentifiers.set(CabbageIDs::shape, "rounded");		
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::outline, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::popup, 0);
		cabbageIdentifiers.set(CabbageIDs::plant, "");
		cabbageIdentifiers.set(CabbageIDs::type, "image");
		cabbageIdentifiers.set(CabbageIDs::file, "");
		cabbageIdentifiers.set(CabbageIDs::name, "image");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		  
	}
	//===============groupbox==================//
    else if(strTokens[0].trim() == "groupbox"){
		line = 1;
		plantButton = 0;
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 80);
		cabbageIdentifiers.set(CabbageIDs::height, 22);		
		cabbageIdentifiers.set(CabbageIDs::colour, CabbageUtils::getComponentSkin().toString());
		cabbageIdentifiers.set(CabbageIDs::fontcolour, CabbageUtils::getComponentFontColour().toString());	
		cabbageIdentifiers.set(CabbageIDs::popup, 0);
		cabbageIdentifiers.set(CabbageIDs::plant, "");
		cabbageIdentifiers.set(CabbageIDs::line, 1);
		cabbageIdentifiers.set(CabbageIDs::type, "groupbox");
		cabbageIdentifiers.set(CabbageIDs::name, "groupbox");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));		  
		  
	}
	//===============line==================//
    else if(strTokens[0].trim() == "line"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 180;
		height = 2;
		name = "line";
		type = name;
		name.append(String(ID), 1024);

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 160);
		cabbageIdentifiers.set(CabbageIDs::height, 2);	
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::type, "line");
		cabbageIdentifiers.set(CabbageIDs::name, "line");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		  
	}
	//===============transport==================//
    else if(strTokens[0].trim() == "transport"){
		cabbageIdentifiers.set("basetype", "layout");
          top = 0;
          left = 0;
          width = 100;
          height = 40;
          name = "transport";
		  type = name;
          name.append(String(ID), 1024);
	}
	//===============csoundoutput==================//	
    else if(strTokens[0].trim() == "csoundoutput"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 400;
		height = 200;
		colour = Colours::white;
		name = "csoundoutput";
		type = name;
		name.append(String(ID), 1024);

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 400);
		cabbageIdentifiers.set(CabbageIDs::text, "Csound output");
		cabbageIdentifiers.set(CabbageIDs::height, 200);	
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::white.toString());
		cabbageIdentifiers.set(CabbageIDs::name, "csoundoutput");
		cabbageIdentifiers.set(CabbageIDs::type, "csoundoutput");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));

	}
	//===============vemeter==================//
    else if(strTokens[0].trim() == "vumeter"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 400;
		height = 200;
		vuConfig.clear();
		colour = Colours::white;
		name = "vumeter";
		type = name;
		name.append(String(ID), 1024);
	}
	//===============table==================//
    else if(strTokens[0].trim() == "table"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 400;
		height = 200;
		colour = Colours::lime;
		name = "table";
		type = name;
		value = 1;
		tableNum = 1;
		name.append(String(ID), 1024);

		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 400);
		cabbageIdentifiers.set(CabbageIDs::height, 200);	
		cabbageIdentifiers.set(CabbageIDs::tablenumber, 1);
		cabbageIdentifiers.set(CabbageIDs::drawmode, 0);
		cabbageIdentifiers.set(CabbageIDs::resizemode, 0);
		cabbageIdentifiers.set(CabbageIDs::readonly, 0);
		cabbageIdentifiers.set(CabbageIDs::tablecolour, Colours::lime.toString());
		cabbageIdentifiers.set(CabbageIDs::amprange, 0);
		cabbageIdentifiers.set(CabbageIDs::type, "table");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		  
	}
    else if(strTokens[0].trim() == "pvsview"){
		cabbageIdentifiers.set("basetype", "layout");
          top = 10;
          left = 10;
          width = 400;
          height = 200;
          colour = Colours::white;
          name = "pvsview";
		  type = name;
		  value = 1;
		  tableNum = 1;
          name.append(String(ID), 1024);
		  overlapSize = 256;
		  fftSize = 1024;
		  frameSize = 1024;
	}
	//===============xypad==================//
    else if(strTokens[0].trim() == "xypad"){
		cabbageIdentifiers.set("basetype", "interactive");
		top = 10;
		left = 10;
		width = 200;
		height = 200;
		xChannel = "xypadchanX";
		yChannel = "xypadchanY";
		channel = "";
		caption = "";
		name = "xypad";
		caption = "";
		minX = 0;
		maxX = 200;
		minY = 0;
		maxY = 200;
		valueX = 0;
		decimalPlaces = 2;
		valueY = 0;
		type = "xypad";
		name.append(String(ID), 1024);
		xyChannel = "";
		colour = Colours::lime;
		fontcolour = Colours::cornflowerblue;	
		var channels;
		channels.append("xChan");
		channels.append("yChan");
		cabbageIdentifiers.set(CabbageIDs::xyautoindex, 0);	
		cabbageIdentifiers.set(CabbageIDs::xchannel, channels[0]);
		cabbageIdentifiers.set(CabbageIDs::ychannel, channels[1]);
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::width, 200);
		cabbageIdentifiers.set(CabbageIDs::height, 200);
		cabbageIdentifiers.set(CabbageIDs::minx, 0);
		cabbageIdentifiers.set(CabbageIDs::maxx, 200);	
		cabbageIdentifiers.set(CabbageIDs::text, "");	
		cabbageIdentifiers.set(CabbageIDs::miny, 0);
		cabbageIdentifiers.set(CabbageIDs::maxy, 200);		
		cabbageIdentifiers.set(CabbageIDs::valuex, 0);	
		cabbageIdentifiers.set(CabbageIDs::valuey, 0);	
		cabbageIdentifiers.set(CabbageIDs::colour, Colours::lime.toString());	    
		cabbageIdentifiers.set(CabbageIDs::fontcolour, Colours::cornflowerblue.toString());	
		cabbageIdentifiers.set(CabbageIDs::type, "xypad");
		cabbageIdentifiers.set(CabbageIDs::name, "xypad");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));	    
		  
	}
	//===============keyboard==================//
    else if(strTokens[0].trim() == "keyboard"){
		cabbageIdentifiers.set("basetype", "layout");
		top = 10;
		left = 10;
		width = 400;
		height = 100;
		name = "keyboard";
		type = name;
		name.append(String(ID), 1024);
		value=60;


		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 400);
		cabbageIdentifiers.set(CabbageIDs::height, 100);	
		cabbageIdentifiers.set(CabbageIDs::value, 60);	
		cabbageIdentifiers.set(CabbageIDs::type, "keyboard");	
		cabbageIdentifiers.set(CabbageIDs::name, "keyboard");
		cabbageIdentifiers.set(CabbageIDs::name, cabbageIdentifiers.getWithDefault("name", "").toString()+String(ID));
		  
	}
	//===============form==================//
    else if(strTokens[0].trim() == "form"){
	    cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
		cabbageIdentifiers.set(CabbageIDs::top, 10);
		cabbageIdentifiers.set(CabbageIDs::left, 10);
		cabbageIdentifiers.set(CabbageIDs::width, 600);
		cabbageIdentifiers.set(CabbageIDs::height, 300);	
		cabbageIdentifiers.set(CabbageIDs::caption, "");
		cabbageIdentifiers.set(CabbageIDs::name, "form");
		cabbageIdentifiers.set(CabbageIDs::type, "form");
		cabbageIdentifiers.set(CabbageIDs::guirefresh, 20);
	}

    else if(strTokens[0].trim() == "hostbpm"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostbpm");
		cabbageIdentifiers.set(CabbageIDs::type, "hostbpm");
	}

    else if(strTokens[0].trim() == "hostppqpos"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostppqpos");
		cabbageIdentifiers.set(CabbageIDs::type, "hostppqpos");
	}

    else if(strTokens[0].trim() == "hostplaying"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostplaying");
		cabbageIdentifiers.set(CabbageIDs::type, "hostplaying");
	}

    else if(strTokens[0].trim() == "hostrecording"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hostrecording");
		cabbageIdentifiers.set(CabbageIDs::type, "hostrecording");
	}

    else if(strTokens[0].trim() == "hosttime"){
		cabbageIdentifiers.set(CabbageIDs::basetype, "layout");
        cabbageIdentifiers.set(CabbageIDs::name, "hosttime");
		cabbageIdentifiers.set(CabbageIDs::type, "hosttime");
	}

	tabpage = String("");
	
//parse the text now that all default values ahve been assigned
parse(compStr);
}

CabbageGUIClass::~CabbageGUIClass()
{

}
//===========================================================================================
int CabbageGUIClass::parse(String inStr)
{
	//Logger::writeToLog(str);
	//remove any text after a semicolon and take out tabs..
	String str = inStr.removeCharacters("\t");
	if(str.indexOf(0, ";")!=-1)
		str = str.substring(0, str.indexOf(0, ";"));

	
	//making this static would also speed things up!!
	String typeOfWidget="";
	StringArray identArray;
	identArray.clear();
    identArray.add(",colour(");
	identArray.add("tablecolour(");
	identArray.add(" colour(");
    identArray.add(",colours(");
	identArray.add(" colours(");
    identArray.add("fontcolour(");
	identArray.add("size(");
	identArray.add("items(");
    identArray.add("pos(");
    identArray.add("min(");
    identArray.add("max(");
    identArray.add("value(");
	identArray.add("tabpage(");
	identArray.add("guirefresh(");
	identArray.add("midictrl(");
	//check line values with spaces, otherwise
	//it gets confused with the outline identifier
	identArray.add(" line(");
	identArray.add(",line(");
	identArray.add("bounds(");
	identArray.add("populate(");
	identArray.add(",range(");
	identArray.add(" range(");
	identArray.add("amprange(");

	identArray.add("rangex(");
	identArray.add("rangey(");
	identArray.add("plant(");
	identArray.add("alpha(");
    identArray.add(",channel(");
	identArray.add("channel(");
	identArray.add("channeltype(");
	identArray.add(" chan(");
    identArray.add("channels(");
	identArray.add(" chans(");
    identArray.add("name(");
    identArray.add("textbox(");
	identArray.add("instrs(");
    identArray.add("caption(");
    identArray.add("topitem(");
    identArray.add("menuitem(");
    identArray.add("stdout(");
	identArray.add("native(");
    identArray.add("exit(");
    identArray.add("cssetup(");
    identArray.add("kind(");
	identArray.add("config(");
    identArray.add("beveltype(");
	identArray.add("text(");
    identArray.add("runcsound(");
	identArray.add("tabs(");
	identArray.add("tab(");
	identArray.add(" mode(");
	identArray.add(",mode(");
	identArray.add("tablenumber(");
	identArray.add("tablenum(");
	identArray.add("tablenumbers(");
	identArray.add("tablenums(");
	identArray.add("fill(");
	identArray.add("file(");
	identArray.add("outline(");
	identArray.add("master(");
	identArray.add("shape("); 
	identArray.add("textcolour("); 
	identArray.add("scale(");
	identArray.add("chan(");
	identArray.add("key(");
	identArray.add("steps(");
	identArray.add("pluginid(");
	identArray.add("rctrls(");
	identArray.add("trackercolour(");
	identArray.add("preset(");
	identArray.add("popup(");
	identArray.add("fftsize(");
	identArray.add("overlap(");
	identArray.add("framesize(");
	identArray.add("pvschan(");
	identArray.add("author(");
	identArray.add("tabpage(");
	identArray.add("drawmode(");
	identArray.add("resizemode(");
	identArray.add("readonly(");
	identArray.add("latched(");
	//add a few dummy identifiers so we can catch bogus one in the Cabbage code
	identArray.add("");
	identArray.add("");
	identArray.add("");
	//Logger::writeToLog(String(identArray.size()));
	//retrieve paramters consisting of strings
	for(int indx=0;indx<identArray.size();indx++)
	{
		//check to see if identifier is part of input string..turn to lowercase first..
		//Logger::writeToLog("index:"+String(indx)+" arrayItem:"+identArray.getReference(indx)+" line:\n"+str);
		int identPos = str.toLowerCase().indexOf(identArray.getReference(indx));
          if(identPos!=-1){
			StringArray strTokens, fillStrTokens;
			fillStrTokens.addTokens(str, ", \t", "\"");
			typeOfWidget = fillStrTokens[0];
			
			String newString = str.substring(identPos+identArray.getReference(indx).length());
			
			String tstr = newString.substring(0, newString.indexOf(0, ")"));
			
			if(tstr.length()==0) return 0;
			
			strTokens.addTokens(tstr.removeCharacters("\t()\""), ",", "\"");
			
			//for(int i=0;i<strTokens.size();i++)
			//	showMessage(strTokens[i]);

			if(identArray[indx].equalsIgnoreCase("name(")){
					name = strTokens[0].trim();
					cabbageIdentifiers.set(CabbageIDs::name, strTokens[0].trim());
			} 
			else if(identArray[indx].equalsIgnoreCase("plant(")){
				cabbageIdentifiers.set(CabbageIDs::plant, strTokens[0].trim());
				plant = strTokens[0].trim();
			} 
			else if(identArray[indx].equalsIgnoreCase("caption(")){
			caption = strTokens[0].trim();
			cabbageIdentifiers.set(CabbageIDs::caption, strTokens[0].trim());
			} 
			
            else if(identArray[indx].equalsIgnoreCase("channel(")||
				identArray[indx].equalsIgnoreCase(",channel(")||
				identArray[indx].equalsIgnoreCase(" chans(") ||
				identArray[indx].equalsIgnoreCase("channel(") ||
				identArray[indx].equalsIgnoreCase(" channels("))
				{
					channel = strTokens[0].trim();
					channels.add(channel);
					var array;
					array.append(strTokens[0].trim());
					if(str.containsIgnoreCase("xypad"))
					{
					xChannel = strTokens[0].trim();
					yChannel = strTokens[1].trim();
					cabbageIdentifiers.set(CabbageIDs::xchannel, strTokens[0].trim());
					cabbageIdentifiers.set(CabbageIDs::ychannel, strTokens[1].trim());
					}
					else if(str.containsIgnoreCase("vumeter"))
						{
						channels.clear();
						array.resize(0);					
						for(int u=0;u<strTokens.size();u++)
							{
							channels.add(strTokens[u].trim());
							array.append(strTokens[u].trim());
							}
						}
					else{ 
						if(strTokens.size()>1)
						for(int i=1;i<strTokens.size();i++)
							{
							channels.add(strTokens[i].trim());
							array.append(strTokens[i].trim());						
							}
						}
					cabbageIdentifiers.set(CabbageIDs::channel, array);	
				
				}

            else if(identArray[indx].equalsIgnoreCase(" colour(")||
				identArray[indx].equalsIgnoreCase(",colour(")||
				identArray[indx].equalsIgnoreCase(" colours(")||
				identArray[indx].equalsIgnoreCase(", colours(")){
					if(strTokens.size()<2){
						colour = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					}
						
					else if(strTokens.size()==4)
						colour = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						colour = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
									
					for(int i=0;i<strTokens.size();i++)
						colours.add(Colours::findColourForName(strTokens[i].trim(), Colours::white).toString());

					//var RGBA;
					//RGBA.append(colour.getRed());
					//RGBA.append(colour.getGreen());
					//RGBA.append(colour.getBlue());
					//RGBA.append(colour.getAlpha());
					cabbageIdentifiers.set(CabbageIDs::colour, colour.toString());
									
			}


            else if(identArray[indx].equalsIgnoreCase("fontcolour(")){
					if(strTokens.size()<2)
						fontcolour = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						fontcolour = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						fontcolour = Colour::fromRGBA (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue(), 
									255);
									
					cabbageIdentifiers.set(CabbageIDs::fontcolour, fontcolour.toString());				
			}

            else if(identArray[indx].equalsIgnoreCase("tablecolour(")){
					var colours;
					for(int i=0;i<strTokens.size();i++)
						colours.append(strTokens[i].trim());
						
					cabbageIdentifiers.set(CabbageIDs::tablecolour, colours);	
					}

            else if(identArray[indx].equalsIgnoreCase("trackercolour(")){

					if(strTokens.size()<2){
						if(strTokens[0].trim().length()>2)
						trackerFill = Colours::findColourForName(strTokens[0].trim(), Colours::white);
						else 
						trackerFill = Colour::fromRGBA (255, 255, 255, 0);
					}
					else if(strTokens.size()==4)
						trackerFill = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						trackerFill = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
									
					cabbageIdentifiers.set(CabbageIDs::trackercolour, trackerFill.toString());					
			}

			else if(identArray[indx].equalsIgnoreCase("kind(")){
				kind = strTokens[0].trim();
				cabbageIdentifiers.set(CabbageIDs::kind, strTokens[0].trim());	
			} 
			else if(identArray[indx].equalsIgnoreCase("file(")){
				file = strTokens[0].trim();
				cabbageIdentifiers.set(CabbageIDs::file, strTokens[0].trim());	
			} 
			else if(identArray[indx].equalsIgnoreCase("fill(")){
					if(strTokens.size()<2)
						fill = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						fill = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						fill = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
									
					cabbageIdentifiers.set(CabbageIDs::fill, trackerFill.toString());		

			}
			else if(identArray[indx].equalsIgnoreCase("shape(")){
				shape = strTokens[0].trim();
				cabbageIdentifiers.set(CabbageIDs::shape, strTokens[0].trim());	
			} 
			else if(identArray[indx].equalsIgnoreCase("outline(")){
					if(strTokens.size()<2)
						outline = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						outline = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						outline = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
									
					cabbageIdentifiers.set(CabbageIDs::outline, trackerFill.toString());	
			}
			else if(identArray[indx].equalsIgnoreCase("textcolour(")){
					if(strTokens.size()<2)
						textcolour = Colours::findColourForName(strTokens[0].trim(), Colours::white);
					else if(strTokens.size()==4)
						textcolour = Colour::fromRGBA (strTokens[0].getIntValue(),
															strTokens[1].getIntValue(), 
															strTokens[2].getIntValue(),
															strTokens[3].getIntValue());
					else if(strTokens.size()==3)
						textcolour = Colour::fromRGB (strTokens[0].getIntValue(),
									strTokens[1].getIntValue(), 
									strTokens[2].getIntValue());
					cabbageIdentifiers.set(CabbageIDs::textcolour, textcolour.toString());	
			}
			
			//!!! This haven't been set for the generic data system
            else if(identArray[indx].equalsIgnoreCase("key(")){
				key.clear();
				if(strTokens.size()==1){
					key.add(strTokens[0].trim());
				}
				else if(strTokens.size()==2){
					key.add(strTokens[0].trim());
					key.add(strTokens[1].trim());
				}
				else
					debugMessage ="WARNING: Too many parameters passed to key(): usage key(\"key1\", \"key2\"\")";

			}

			else if(identArray[indx].equalsIgnoreCase("pluginid(")){
				pluginID = strTokens[0].trim();
				cabbageIdentifiers.set(CabbageIDs::pluginid, strTokens[0].trim());
			}

            else if(identArray[indx].equalsIgnoreCase("items(")||
					identArray[indx].equalsIgnoreCase("text(")){
              items.clear();//clear any unwanted items
			  text = strTokens[0];
			  //CabbageUtils::showMessage(typeOfWidget+":"+text);
			  var value;
			  for(int i= 0;i<(int)strTokens.size();i++)
					{
					//Logger::writeToLog(strTokens[i]);
					String test = strTokens[i]; 
					items.add(strTokens[i]);	
					value.append(strTokens[i].trim());
					//if((int)strTokens.size()<2) {
					//	items.add(strTokens[i]);
					//	value.append(strTokens[i].trim());
					//	i++;
					//	}
					comboRange = i+1;
					}
					//Logger::writeToLog(String(value.size())+typeOfWidget);
					if((value.size()==1) && ((typeOfWidget=="button") || (typeOfWidget=="filebutton"))){
						value.append(strTokens[0]);
						//Logger::writeToLog(strTokens[0].trim());
					}
						
			  cabbageIdentifiers.set(CabbageIDs::text, value);
			  cabbageIdentifiers.set(CabbageIDs::comborange, comboRange);
			}

			//!!! This haven't been set for the generic data system
			else if(identArray[indx].equalsIgnoreCase("instrs(")){
              items.clear();//clear any unwanted items
			  for(int i= 0;i<(int)strTokens.size();i++){
				String test = strTokens[i]; 
				items.add(strTokens[i]);	
				}
			}

			else if(identArray[indx].equalsIgnoreCase("preset(")){
				//CabbageUtils::showMessage("preset is no longer supported, see docs");
			}

			else if(identArray[indx].equalsIgnoreCase(" mode(") ||
					identArray[indx].equalsIgnoreCase(",mode(")){
				cabbageIdentifiers.set(CabbageIDs::mode, strTokens[0].trim());	
			}
			
			else if(identArray[indx].equalsIgnoreCase("native(")){
				native = strTokens[0].trim();
				cabbageIdentifiers.set("native", strTokens[0].trim());	
				//Logger::writeToLog(native);
			}
			
			else if(identArray[indx].equalsIgnoreCase("channeltype(")){
				cabbageIdentifiers.set(CabbageIDs::channeltype, strTokens[0].trim());	
			}
			
			else if(identArray[indx].equalsIgnoreCase("populate(")){
				fileType = strTokens[0].trim();
				cabbageIdentifiers.set(CabbageIDs::filetype, strTokens[0].trim());	
				if(strTokens.size()>1){
					workingDir = strTokens[1].trim();
					cabbageIdentifiers.set(CabbageIDs::workingdir, strTokens[1].trim());	
				}
			}

			else if(identArray[indx].equalsIgnoreCase("author(")){
				author = strTokens[0].trim();
				cabbageIdentifiers.set(CabbageIDs::author, strTokens[0].trim());	
			}

		
			else if(identArray[indx].equalsIgnoreCase("tabs(")){
				items.clear();
				var value;
				for(int i= 0;i<(int)strTokens.size();i++){
				items.add(strTokens[i]);	
				value.append(strTokens[i].trim());
				}
				cabbageIdentifiers.set(CabbageIDs::tabs, strTokens[0].trim());	
			}

			else if(identArray[indx].equalsIgnoreCase("tabpage(")){
				tabpage = strTokens[0].trim();  
				cabbageIdentifiers.set(CabbageIDs::tabpage, strTokens[0].trim());	
			} 

			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			//numeric paramters
			//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			
			else if(identArray[indx].equalsIgnoreCase("size(")){
				if(strTokens.size()<2){
					debugMessage ="WARNING: Not enough paramters passed to size(): usage pos(width, height\")";
				}
				else{
					sizeText = identArray.getReference(indx)+tstr+")";
					width = strTokens[0].trim().getFloatValue(); 
					height = strTokens[1].trim().getFloatValue(); 
					cabbageIdentifiers.set(CabbageIDs::width, strTokens[0].trim().getFloatValue());					
					cabbageIdentifiers.set(CabbageIDs::height, strTokens[1].trim().getFloatValue());
				}
            }
			//!!! This haven't been set for the generic data system
			else if(identArray[indx].equalsIgnoreCase("scale(")){
				if(strTokens.size()<2){
					debugMessage ="WARNING: Not enough paramters passed to scale(): usage scale(width, height\")";
				}
				else{
              scaleX = strTokens[0].trim().getFloatValue();  
			  scaleX = scaleX/width;
              scaleY = strTokens[1].trim().getFloatValue();  
			  scaleY = scaleY/height;
				}
            }
			else if(identArray[indx].equalsIgnoreCase("bounds(")){
				if(strTokens.size()<3){
					debugMessage ="WARNING: Not enough paramters passed to bounds(): usage pos(top, left width, height\")";
				}
				else{
				  left = strTokens[0].trim().getFloatValue();  
				  top = strTokens[1].trim().getFloatValue();  
				  width = strTokens[2].trim().getFloatValue();  
				  if(strTokens.size()==3){
					  height = width;
					  cabbageIdentifiers.set(CabbageIDs::height, width);	
				  }
				  else{
				  height = strTokens[3].trim().getFloatValue(); 
				  boundsText = identArray.getReference(indx)+tstr+")";
				  cabbageIdentifiers.set(CabbageIDs::left, strTokens[0].trim().getFloatValue());					
				  cabbageIdentifiers.set(CabbageIDs::top, strTokens[1].trim().getFloatValue());
				  cabbageIdentifiers.set(CabbageIDs::width, strTokens[2].trim().getFloatValue());					
				  cabbageIdentifiers.set(CabbageIDs::height, strTokens[3].trim().getFloatValue());			  
				  }
				  
				}
            }
			//!!! This haven't been set for the generic data system
            else if(identArray[indx].equalsIgnoreCase("config(")){
				vuConfig.clear();
				 for(int i= 0;i<(int)strTokens.size();i++){
					 vuConfig.add(strTokens[i].trim().getFloatValue());
				 }
				}

            else if(identArray[indx].equalsIgnoreCase("pos(")){
				if(strTokens.size()<2){
					debugMessage ="WARNING: Not enough paramters passed to pos(): usage pos(top, left\")";
				}
				else{
				  left = strTokens[0].trim().getFloatValue(); 
				  top = strTokens[1].trim().getFloatValue();
				  posText = identArray.getReference(indx)+tstr+")";
				  cabbageIdentifiers.set(CabbageIDs::left, strTokens[0].trim().getFloatValue());					
				  cabbageIdentifiers.set(CabbageIDs::top, strTokens[1].trim().getFloatValue());
				}
            }
			else if(identArray[indx].equalsIgnoreCase(" range(") ||
					identArray[indx].equalsIgnoreCase(",range(")){
				if(strTokens.size()<3){
					debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value, incr\")";
				}
				else{	
				StringArray tempArray;
				//showMessage(str);
				//showMessage(strTokens[0]);
				min = strTokens[0].trim().getDoubleValue();// getFloatValue(); 
				max = strTokens[1].trim().getDoubleValue();//.getFloatValue(); 
				cabbageIdentifiers.set(CabbageIDs::min, strTokens[0].trim().getDoubleValue());	
				cabbageIdentifiers.set(CabbageIDs::max, strTokens[1].trim().getDoubleValue());	
				tempArray.add(strTokens[0].trim());
				tempArray.add(strTokens[1].trim());
				if(strTokens.size()>2){
				value = strTokens[2].trim().getDoubleValue();//.getFloatValue(); 
				cabbageIdentifiers.set(CabbageIDs::value, strTokens[2].trim().getDoubleValue());	
				tempArray.add(strTokens[2].trim());
				}
				else value = 0;				

				if(strTokens.size()>3){
				sliderSkew = strTokens[3].trim().getFloatValue();//.getFloatValue(); 
				cabbageIdentifiers.set(CabbageIDs::sliderskew, strTokens[3].trim().getDoubleValue());
				tempArray.add(strTokens[3].trim());
				}
				
				if(strTokens.size()>4){  
				sliderIncr = strTokens[4].trim().getDoubleValue();
				tempArray.add(strTokens[4].trim());
				cabbageIdentifiers.set(CabbageIDs::sliderincr, strTokens[4].trim().getDoubleValue());
				}
				
				sliderRange = max-min;				
				decimalPlaces = getNumberOfDecimalPlaces(tempArray);
				if(decimalPlaces<1 && max<=1)
					decimalPlaces = 2;
				else
					decimalPlaces = getNumberOfDecimalPlaces(tempArray);
				cabbageIdentifiers.set(CabbageIDs::decimalplaces, decimalPlaces);
				cabbageIdentifiers.set(CabbageIDs::range, sliderRange);

				}

			}
			else if(identArray[indx].equalsIgnoreCase("rangex(")){
				if(strTokens.size()<3){
					debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")";
				}
				else{
				minX = strTokens[0].removeCharacters("()").trim().getFloatValue();  				
				maxX = strTokens[1].removeCharacters("()").trim().getFloatValue();  
				valueX = strTokens[2].removeCharacters("()").trim().getFloatValue(); 
				
				cabbageIdentifiers.set(CabbageIDs::minx, strTokens[0].trim().getDoubleValue());			
				cabbageIdentifiers.set(CabbageIDs::maxx, strTokens[1].getDoubleValue());
				cabbageIdentifiers.set(CabbageIDs::valuex, strTokens[2].getDoubleValue());
				cabbageIdentifiers.set(CabbageIDs::rangex, maxX-minX);
				
				if(strTokens.size()==4)
				decimalPlaces = strTokens[3].trim().getFloatValue();
				xypadRangeX = maxX-minX;
				cabbageIdentifiers.set(CabbageIDs::decimalplaces, strTokens[3].trim().getDoubleValue());
				}
			}
			else if(identArray[indx].equalsIgnoreCase("rangey(")){
				if(strTokens.size()<3){
					debugMessage ="WARNING: Not enough paramters passed to range(): usage range(minx, max, value\")";
				}
				else{
				minY = strTokens[0].trim().getFloatValue();  
				maxY = strTokens[1].trim().getFloatValue();  
				valueY = strTokens[2].trim().getFloatValue();
				

				cabbageIdentifiers.set(CabbageIDs::miny, strTokens[0].trim().getDoubleValue());	
				cabbageIdentifiers.set(CabbageIDs::maxy, strTokens[1].trim().getDoubleValue());
				cabbageIdentifiers.set(CabbageIDs::valuey, strTokens[2].trim().getDoubleValue());
				cabbageIdentifiers.set(CabbageIDs::rangey, maxY-minY);
				

				if(strTokens.size()==4)
				decimalPlaces = strTokens[3].trim().getFloatValue();
				xypadRangeY = maxY-minY;
				cabbageIdentifiers.set(CabbageIDs::decimalplaces, strTokens[3].trim().getDoubleValue());
				}
			}
			else if(identArray[indx].equalsIgnoreCase("min(")){
				min = strTokens[0].removeCharacters("()").trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::min, strTokens[0].trim().getFloatValue());	
			}
			else if(identArray[indx].equalsIgnoreCase("midictrl(")){
				if(strTokens.size()<2){
					debugMessage ="WARNING: Not enough paramters passed to midiCtrl(): usage midiCtrl(midiChan, midiCtrl\")";
				}
				else{
				midiChan = strTokens[0].trim().getFloatValue();  
				midiCtrl = strTokens[1].trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::midichan, strTokens[0].trim().getFloatValue());	
				cabbageIdentifiers.set(CabbageIDs::midictrl, strTokens[0].trim().getFloatValue());	
			}
			}
            else if(identArray[indx].equalsIgnoreCase("max(")){
				max = strTokens[0].trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::max, strTokens[0].trim().getFloatValue());	
			}


            else if(identArray[indx].equalsIgnoreCase("tab(")){
				tabbed = strTokens[0].trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::tabbed, strTokens[0].trim().getFloatValue());	
			}
            else if(identArray[indx].equalsIgnoreCase("latched(")){ 
				cabbageIdentifiers.set(CabbageIDs::latched, strTokens[0].trim().getFloatValue());	
			}
            else if(identArray[indx].equalsIgnoreCase("guirefresh(")){ 
				cabbageIdentifiers.set(CabbageIDs::guirefresh, strTokens[0].trim().getFloatValue());	
			}
            else if(identArray[indx].equalsIgnoreCase("textbox(")){
				textBox = strTokens[0].trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::textbox, strTokens[0].trim().getFloatValue());				
			}
			
            else if(identArray[indx].equalsIgnoreCase("readonly(")){
				cabbageIdentifiers.set(CabbageIDs::readonly, strTokens[0].trim().getFloatValue());		
			}			
			
            else if(identArray[indx].equalsIgnoreCase("tablenum(")||
			(identArray[indx].equalsIgnoreCase("tablenumber("))||
			identArray[indx].equalsIgnoreCase("tablenumbers(")||
			identArray[indx].equalsIgnoreCase("tablenumbs(")){
				tableNum = strTokens[0].trim().getFloatValue();  
				var value;
				value.append(tableNum);
				tableNumbers.add(tableNum);
				numTables++;
				if(strTokens.size()>1)
					for(int i=1;i<strTokens.size();i++){
						tableNumbers.add(strTokens[i].trim().getFloatValue());
						value.append(strTokens[i].trim().getFloatValue());
						numTables++;						
					}
				cabbageIdentifiers.set(CabbageIDs::tablenumber, value);
				//cabbageIdentifiers.set("tablenum", tableNum);
			}
            else if(identArray[indx].equalsIgnoreCase("popup(")){
				plantButton = strTokens[0].trim().getIntValue();
				cabbageIdentifiers.set(CabbageIDs::popup, strTokens[0].trim().getIntValue());
			}

            else if(identArray[indx].equalsIgnoreCase("drawmode(")){
				var value;
				if(strTokens.size()>0)
					for(int i=0;i<strTokens.size();i++){
						//add table modes to val array
						value.append(strTokens[i].trim().getFloatValue());					
					}
				cabbageIdentifiers.set(CabbageIDs::drawmode, value);
			}

            else if(identArray[indx].equalsIgnoreCase("resizemode(")){
				var value;
				if(strTokens.size()>0)
					for(int i=0;i<strTokens.size();i++){
						//add table modes to val array
						value.append(strTokens[i].trim().getFloatValue());					
					}
				cabbageIdentifiers.set(CabbageIDs::resizemode, value);
			}			
			
            else if(identArray[indx].equalsIgnoreCase("amprange(")){
				var value;
				if(strTokens.size()>0)
					for(int i=0;i<strTokens.size();i++){
						//add table modes to val array
						value.append(strTokens[i].trim().getFloatValue());					
					}
				cabbageIdentifiers.set(CabbageIDs::amprange, value);
			}			

			else if(identArray[indx].equalsIgnoreCase(",line(")||
					identArray[indx].equalsIgnoreCase(" line(")){
					cabbageIdentifiers.set("line", strTokens[0].trim().getFloatValue());	
					cabbageIdentifiers.set(CabbageIDs::line, strTokens[0].trim().getFloatValue());  
					}
					
            else if(identArray[indx].equalsIgnoreCase("value(")){
				value = strTokens[0].trim().getFloatValue();  
				cabbageIdentifiers.set(CabbageIDs::value, strTokens[0].trim().getFloatValue());	
			} 
		
			//!!! This haven't been set for the generic data system			
			else if(identArray[indx].equalsIgnoreCase("rctrls(")){
				if(strTokens[0].trim().getFloatValue()>5)
				rCtrls = 5;
				else
				rCtrls = strTokens[0].trim().getFloatValue();  
			}
			else {
			}
			strTokens.clear();
          }
          else{
 
          }
	}
	return 1;//must add error checking to this...

}
//=========================================================================
//retrieve numerical attributes
float CabbageGUIClass::getNumProp(Identifier prop)
{	
	return cabbageIdentifiers.getWithDefault(prop, -9999.f);
}

float CabbageGUIClass::getNumProp(Identifier prop, int index)
{
	//this method should never be called...
	jassert(1);	
}



//set numerical attributes
void CabbageGUIClass::setNumProp(Identifier prop, float val)
{
	cabbageIdentifiers.set(prop, val);
}


float CabbageGUIClass::getTableChannelValues(int index)
{
if(index<tableChannelValues.size())
	return tableChannelValues.getReference(index);
else
	return 0.f;	
}

void CabbageGUIClass::addTableChannelValues()
{
tableChannelValues.add(-1.f);
}

void CabbageGUIClass::setTableChannelValues(int index, float val)
{
if(index<tableChannelValues.size())
tableChannelValues.set(index, val);		
}


String CabbageGUIClass::getPropsString()
{
	return type << String(" bounds(") << String(left) << String(", ") << String(top) << String(", ") << String(width)
				<< String(", ") << String(height) << String("), channel(\"") << channel << String("), xyChannel(\"") << xyChannel << String("\"), value(")
				<< String(value) << String("), items(\"") << items[0].trim() << String("\", \"") << items[1].trim() << String("\")")
				<< String("), range(\"") << String(min) << String(", ") << String(max) << String(", ") << String(value) << String("\")");
}

String CabbageGUIClass::getStringProp(Identifier prop, int index)
{
	jassert(1);
/*
		if(prop.equalsIgnoreCase("channel"))
			return channels[index].trim();
		else if(prop.equalsIgnoreCase("snapshotData"))
			return snapshotData[index].trim();
		else*/	
		return "";
}

Rectangle<int> CabbageGUIClass::getComponentBounds()
{
Rectangle<int> rect;
rect.setBounds(getNumProp("left"), getNumProp("top"), getNumProp("width"), getNumProp("height"));
return rect;
}

String CabbageGUIClass::getStringProp(Identifier prop)
{
	var strings = cabbageIdentifiers.getWithDefault(prop, "");
	

	if(strings.size()>0){
		//Logger::writeToLog(prop+":"+strings[0].toString());
		return strings[0].toString();
	}
	else{
		//Logger::writeToLog(prop+":"+strings.toString());
		//Logger::writeToLog(prop+":"+strings.toString());
		return strings.toString();
	}
}



StringArray CabbageGUIClass::getStringArrayProp(Identifier prop)
{
	StringArray returnArray;
	var strings = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<strings.size();i++)
		returnArray.add(strings[i].toString());
	return returnArray;
}

String CabbageGUIClass::getStringArrayPropValue(Identifier prop, int index)
{
	StringArray returnArray;
	var strings = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<strings.size();i++)
		returnArray.add(strings[i].toString());
	if(isPositiveAndBelow(index,strings.size()))
	return returnArray[index];
	else
	return returnArray[strings.size()-1];
		
	
}

Array<int> CabbageGUIClass::getIntArrayProp(Identifier prop)
{
	Array<int> returnArray;
	var ints = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<ints.size();i++)
		returnArray.add(ints[i]);
	return returnArray;
}

int CabbageGUIClass::getIntArrayPropValue(Identifier prop, int index)
{
	Array<int> returnArray;
	var ints = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<ints.size();i++)
		returnArray.add(ints[i]);
	if(isPositiveAndBelow(index,ints.size()))
	return returnArray[index];
	else
	return returnArray[ints.size()-1];
}

Array<float> CabbageGUIClass::getFloatArrayProp(Identifier prop)
{
	Array<float> returnArray;
	var ints = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<ints.size();i++)
		returnArray.add(ints[i]);
	return returnArray;
}

float CabbageGUIClass::getFloatArrayPropValue(Identifier prop, int index)
{
	Array<float> returnArray;
	var ints = cabbageIdentifiers.getWithDefault(prop, "");
	for(int i=0;i<ints.size();i++)
		returnArray.add(ints[i]);
	if(isPositiveAndBelow(index,ints.size()))
	return returnArray[index];
	else
	return returnArray[ints.size()-1];
}

void CabbageGUIClass::setStringProp(Identifier prop, int index, String value)
{		
jassert(1)
}

void CabbageGUIClass::setStringProp(Identifier prop, String val)
{
	//if(prop=="reltoplant")
		if(val.length()>0){
		cabbageIdentifiers.set(prop, val);
		//Logger::writeToLog(prop+":"+val);
		}
}

String CabbageGUIClass::getColourProp(Identifier prop)
{
jassert(1)
}

String CabbageGUIClass::getStringForIdentifier(var propsArray, String identifier, String type)
{
	String str;
	if(type=="number"){
		if(propsArray.size()>0){
			for(int p=0;p<propsArray.size();p++)
			str += propsArray[p].toString()+",";						
			str= identifier+str.dropLastCharacters(1)+"), ";
			//Logger::writeToLog(str);
		}
		else
			str = identifier+propsArray.toString()+"), ";

	return str;
	}

	else if(type=="string"){
		if(propsArray.size()>0){
			for(int p=0;p<propsArray.size();p++)
			str += "\""+propsArray[p].toString()+"\",";						
			str = identifier+str.dropLastCharacters(1)+"), ";
			//Logger::writeToLog(str);			
		}
		else
			str = identifier+String("\"")+propsArray.toString()+"\"), ";		
		
	return str;
	}
	else
	return "";
}

String CabbageGUIClass::getCabbageCodeFromIdentifiers(NamedValueSet props)
{
	//Logger::writeToLog("::::getCabbageCodeFromIdentifiers::::");
	String line;
	var propsArray;
	String temp="";
	String type;
	String colour, fontcolour;
	String left, top, width, height, colR, colG, colB, colA, min, max, skew, drawmode, tablenumber,resizemode,
	incr, slidervalue, value, maxx, maxy, minx, miny, valuex, valuey, channel, xchannel, ychannel,
	amprange;
	var rgbVals;
	//run through the complete list of identifiers
	for(int i=0;i<props.size();i++){
		String identifier = props.getName(i).toString();
		propsArray = props.getValueAt(i);
		if(props.getValueAt(i).toString()!="")
			{
			if(props.getName(i).toString()=="top")
				top = props.getValueAt(i).toString();
			else if(props.getName(i).toString()=="left")
				left = props.getValueAt(i).toString();
			else if(props.getName(i).toString()=="width")
				width = props.getValueAt(i).toString();
			else if(props.getName(i).toString()=="height")
				height = props.getValueAt(i).toString();
			else if(identifier=="decimalplaces" ||
					identifier=="range" ||
					identifier=="name" ||
					identifier=="scalex" ||
					identifier=="scaley" ||
					identifier=="basetype" ||
					identifier=="kind" ||
					identifier=="comborange")
			{
				//none of these identifiers need to be seen...
			}
			else if(props.getName(i).toString()=="colour" || props.getName(i).toString()=="fontcolour"
					|| props.getName(i).toString()=="trackercolour" || props.getName(i).toString()=="fill" ||
					props.getName(i).toString()=="outline")
			{
				if(propsArray.size()>0){
				for(int y=0;y<propsArray.size();y++){
					rgbVals.append(propsArray[y]);
				}			
				colour = colour << props.getName(i).toString() <<"("<<rgbVals[0].toString() << ", " << rgbVals[1].toString() << ", " << rgbVals[2].toString() << ", " << rgbVals[3].toString() << "), ";
				}
				else{
					Colour col = Colour::fromString(props.getValueAt(i).toString());
					colour = colour << props.getName(i).toString() << "(" << (float)col.getRed() << ", " << (float)col.getGreen() << ", " << (float)col.getBlue() << ", " << (float)col.getAlpha() << "), ";
				}
				rgbVals.resize(0);
			}

			else if((identifier=="popup") && (int(props.getValueAt(i))==0)){
			//only show if not set to default		
			}
			else if((identifier=="midictrl") && (int(props.getValueAt(i))==-99)){
			//only show if not set to default				
			}
			else if((identifier=="midichan") && (int(props.getValueAt(i))==-99)){
			//only show if not set to default				
			}
			else if((identifier=="channel")){
				channel = getStringForIdentifier(propsArray, "channel(", "string");
			}
			
			else if(identifier=="drawmode"){
				drawmode = getStringForIdentifier(propsArray, "drawmode(", "number");
			}

			else if((identifier=="amprange")){
				amprange = getStringForIdentifier(propsArray, "amprange(", "number");
			}
		
			else if((identifier=="tablenumber")){
				tablenumber = getStringForIdentifier(propsArray, "tablenumber(", "number");
			}					
			
			else if((identifier=="resizemode")){
				resizemode = getStringForIdentifier(propsArray, "resizemode(", "number");
			}	
			
			else if((identifier=="xchannel")){
			xchannel = props.getValueAt(i).toString();		
			}
			else if((identifier=="ychannel")){
			ychannel = props.getValueAt(i).toString();		
			}					
			
			
			//grab slider settings..
			else if(identifier=="min")
					min = props.getValueAt(i).toString();
			else if(identifier=="max")
					max = props.getValueAt(i).toString();
			else if(identifier=="value"){
					slidervalue = props.getValueAt(i).toString();
					value = "value("+slidervalue+"), ";
			}
			else if(identifier=="sliderskew")
					skew = props.getValueAt(i).toString();
			else if(identifier=="sliderincr")
					incr = props.getValueAt(i).toString();

			//grab xypad x-ranges...
			else if(identifier=="minx")
					minx = props.getValueAt(i).toString();
			else if(identifier=="maxx")
					maxx = props.getValueAt(i).toString();	
			else if(identifier=="valuex")
					valuex = props.getValueAt(i).toString();	

			//grab xypad y-ranges...
			else if(identifier=="miny")
					miny = props.getValueAt(i).toString();
			else if(identifier=="maxy")
					maxy = props.getValueAt(i).toString();	
			else if(identifier=="valuey")
					valuey = props.getValueAt(i).toString();

			else if((identifier=="textbox") && (int(props.getValueAt(i))==0)){
			//only show if not set to default				
			}
			else if(identifier=="type")
				type = props.getValueAt(i).toString();
			else{
				//catch all other identifiers....
				line = line + identifier+String("(");
				
				if(propsArray.size()>0)
				for(int y=0;y<propsArray.size();y++){
					if(propsArray[y].isString())
					temp = temp+"\""+propsArray[y].toString()+"\", ";
					else
					temp = temp+propsArray[y].toString()+", ";
					}
				else	
						if(props.getValueAt(i).isString())							
						temp = temp+"\""+props.getValueAt(i).toString()+"\"";
						else
						temp = temp+props.getValueAt(i).toString();
				
				line = line + temp + "), ";
				temp = "";
				}
			}
	}

	String completeLine;
	String range = "range("+min+", "+max+", "+slidervalue+", "+skew+", "+incr+"), ";
	String rangex = "rangex("+minx+", "+maxx+", "+valuex+"), ";
	String rangey = "rangey("+miny+", "+maxy+", "+valuey+"), ";
	String xypadChannels = "channel(\""+xchannel+"\", \""+ychannel+"\"), ";
	if((type == "rslider") || (type=="hslider") || (type=="vslider"))
	completeLine = type+" bounds("+left+", "+top+", "+width+", "+height+"), "+channel+range+line.replace(", )", ")")+ " "+colour;
	else if(type=="xypad")
	completeLine = type+" bounds("+left+", "+top+", "+width+", "+height+"), "+xypadChannels+rangex+rangey+line.replace(", )", ")")+ " "+colour;
	else
	completeLine = type+" bounds("+left+", "+top+", "+width+", "+height+"), "+channel+line.replace(", )", ")")+ " "+tablenumber+drawmode+amprange+colour+resizemode+value;
	return completeLine;
	
}