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

#include "CabbagePropertiesDialog.h"

ControlProperty::ControlProperty(String name, var _value): 
PropertyComponent(name),
name(name),
value(_value)
{
	
	if((name=="text" || name=="channel"  || name=="items" 
					 || name=="drawmode" || name=="tablenumber")
					 || name=="tablecolour" || name=="amprange" 
					 || name=="resizemode"){
		addAndMakeVisible(textComboField = new TextComboField(name, value));
		//textComboField->setLookAndFeel(lookAndFeel);
		textComboField->addActionListener(this);
	}
	else if(name.contains("colour")){
		addAndMakeVisible(colourField = new ColourField(name, value.toString()));	
		colourField->addActionListener(this);
		colourField->repaint();
	}
	else if(name.contains("file") || name.contains("workingdir")){
		addAndMakeVisible(fileBrowserField = new FileBrowserField(name, value.toString()));	
		fileBrowserField->addActionListener(this);		
	}
	else{
		addAndMakeVisible(textField = new TextField(name));
		textField->addActionListener(this);
		if(value.size()>0)
		textField->setText(value[0]);
		else
		textField->setText(value);	
		setName(name);	
	}

}

void ControlProperty::actionListenerCallback (const String& message)
{
//when a user clicks escape from a propery component this method is called.
//everytime this method is called we update the parameters
if(message=="ColourField")
		value = colourField->value;
else if(message=="TextField")
	value = textField->value;
else if(message=="TextComboField")
		value = textComboField->value;	
else if(message=="FileBrowserField")
		value = fileBrowserField->value;	
else if(message=="UpdateAll")
	{
	CabbagePropertiesDialog* parentWindow = findParentComponentOfClass <CabbagePropertiesDialog>();
	if(parentWindow)
		parentWindow->updateIdentifiers();
	}
}

void ControlProperty::resized()
{
	if(textField)textField->setBounds(getWidth()/2, 2.5, getWidth()/2-5, getHeight()-5);	
	if(textComboField)textComboField->setBounds(getWidth()/2, 2.5, getWidth()/2-5, getHeight()-5);
	if(colourField)colourField->setBounds(getWidth()/2, 2.5, getWidth()/2-5, getHeight()-5);	
	if(fileBrowserField)fileBrowserField->setBounds(getWidth()/2, 2.5, getWidth()/2-5, getHeight()-5);	
}


void ControlProperty::paint(Graphics &g){
	g.setColour (Colour(10, 10, 10));
	g.fillAll();

	g.setColour (Colours::orange);
	g.setFont (CabbageUtils::getComponentFont());

	const Rectangle<int> r (5, 0, getWidth()/2, getHeight()-2);

	g.drawFittedText(name,
					  r,
					  Justification::centred, 2);
	g.setColour (Colours::black);
	g.drawRect(0, 0, getWidth(), getHeight(), 1);	
}