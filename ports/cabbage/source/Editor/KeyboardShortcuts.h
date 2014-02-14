#ifndef __KB_SHORTUCTS__
#define __KB_SHORTUCTS__

#include "../JuceHeader.h"

//============================================================
// classes for displaying keyboard shortcuts
//============================================================


class ShortcutsPanel : public PropertyPanel
{
public:
	ShortcutsPanel() : PropertyPanel()
	{
	setSize(200, 300);
	Array <PropertyComponent*> props;
	keyboardShortcutsXml = appProperties->getUserSettings()->getXmlValue("KeyboardShortcutXmlData");

		for(int i=0;i<keyboardShortcutsXml->getNumAttributes();i++)
		{
		textFields.add(Value(keyboardShortcutsXml->getAttributeValue(i)));
		attributeNames.add(keyboardShortcutsXml->getAttributeName(i));
	
		props.add(new TextPropertyComponent(textFields[i],
											keyboardShortcutsXml->getAttributeName(i),
											100, false));	
	
		props[i]->setPreferredHeight(20);
		props[i]->setColour(TextPropertyComponent::backgroundColourId, Colours::red);			
		}
		
	setSize(200, jmin(300, props.size()*props[0]->getPreferredHeight()));	
	addProperties(props);
	}
	
	void paint(Graphics &g){
		g.fillAll(Colours::lightgrey);
	} 		
	
	~ShortcutsPanel(){
		for(int i=0;i<keyboardShortcutsXml->getNumAttributes();i++)
		keyboardShortcutsXml->setAttribute(attributeNames[i], textFields[i].toString());
		appProperties->getUserSettings()->setValue("KeyboardShortcutXmlData", keyboardShortcutsXml);	
		Logger::writeToLog("update all variables before closing");
		keyboardShortcutsXml = nullptr;	
	}
	
	Array<Value> textFields;
	ScopedPointer<XmlElement> keyboardShortcutsXml;
	StringArray attributeNames;
	
};


#endif