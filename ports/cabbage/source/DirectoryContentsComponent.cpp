/*
  Copyright (C) 2009 Rory Walsh

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

#include "DirectoryContentsComponent.h"

//==============================================================================
DirectoryContentsComponent::DirectoryContentsComponent(String dir, String files):
fileTypes(files),
rootDir(dir),
thread ("audio file preview"), 
fileTreeComp (0),
editor(0),
ftableCounter(1),
lookAndFeel(0),
//filter("*.wav;*.ogg;*.aiff", "*.*", "Audio Files"),
filter(files, "*.*", ""),
directoryList(&filter, thread)
{
    ;
	//directoryList.addChangeListener()
	//create and set Cabbage look and feel
    lookAndFeel=new CabbageLookAndFeel();
	basicLookAndFeel = new CabbageLookAndFeelBasic();
	setLookAndFeel(lookAndFeel);
	//directory list
	fileTreeComp = new FileTreeComponent(directoryList);
	fileTreeComp->addListener(this);
	//if file dir is valid set it, otherwise root is base 

	//custom FunctionTable class
	tablesList.add(new FunctionTableList());
	tablesList[0]->addActionListener(this);
	//table list class, uses above object as it's model
	tables.add(new TableListBox("list", tablesList[0])); 
	tables[0]->setLookAndFeel(lookAndFeel);
	tables[0]->setMultipleSelectionEnabled(true);
	tables[0]->setColour(TableListBox::backgroundColourId, Colours::black);
	tables[0]->setColour(TableListBox::outlineColourId, Colours::black);
	
	tables[0]->setOutlineThickness (1);	
	//addAndMakeVisible(tables[0]);	
	functionRowData.add(new StringArray());
	
	tabComp = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	tabComp->setLookAndFeel(basicLookAndFeel);
	tabComp->setTabBarDepth(25);
	//tabComp->setColour(TabbedButtonBar::tabTextColourId, Colours::white);
	tabComp->setColour(TabbedButtonBar::tabOutlineColourId, Colours::black);
	tabComp->addTab("Bank 1", Colours::white, tables[0], false);
	tabComp->setOutline(0);

	tabComp->repaint();
	addAndMakeVisible(tabComp);
	
	tables[0]->getHeader().addColumn ("Function Tables", 1, 400, 35, 400, TableHeaderComponent::defaultFlags);
	tables[0]->getHeader().setSize(tables[0]->getWidth(), 25);

	//file directory setups....
	if(File(rootDir).isDirectory())
	directoryList.setDirectory(File(rootDir), true, true);
	else
    directoryList.setDirectory (File::getSpecialLocation (File::userHomeDirectory), true, true);
	
    thread.startThread (3);    
    fileTreeComp->setColour(0x1000501, Colours::green);//linesColourId
    fileTreeComp->setColour(DirectoryContentsDisplayComponent::textColourId, Colours::white);
	fileTreeComp->setColour(ListBox::outlineColourId, Colours::red);
    fileTreeComp->addListener (this);     
	
	addBankButton = new TextButton("Add Bank");
	addBankButton->addListener(this);
	addBankButton->setLookAndFeel(basicLookAndFeel);
	addBankButton->setColour(TextButton::buttonColourId, Colours::cornflowerblue.withAlpha(.4f));
	addBankButton->setColour(TextButton::textColourOffId, Colours::white);
	addBankButton->setColour(TextButton::textColourOnId, Colours::white);
		
	updateTablesButton = new TextButton("Update Tables");
	updateTablesButton->addListener(this);
	updateTablesButton->setLookAndFeel(basicLookAndFeel);
	updateTablesButton->setColour(TextButton::buttonColourId, Colours::cornflowerblue.withAlpha(.4f));
	updateTablesButton->setColour(TextButton::textColourOffId, Colours::white);
	updateTablesButton->setColour(TextButton::textColourOnId, Colours::white);		
	
	autoUpdate = new ToggleButton("Auto-update");
	autoUpdate->getProperties().set("isRect", true);
	autoUpdate->getProperties().set("colour", Colours::lime.toString());
	autoUpdate->getProperties().set("fontcolour", Colours::white.toString());
	
	
 	//add everything to screen
	addAndMakeVisible(autoUpdate);
    addAndMakeVisible (fileTreeComp);	
	addAndMakeVisible(updateTablesButton);
	addAndMakeVisible(addBankButton);
	
	
	setSize (600, 500);
}

//====================================================================================
DirectoryContentsComponent::~DirectoryContentsComponent()
{
	thread.stopThread(10);
}

//====================================================================================
void DirectoryContentsComponent::paint (Graphics& g)
{
	g.setColour(CabbageUtils::getBackgroundSkin());
	g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 7);

}

//====================================================================================
void DirectoryContentsComponent::resized()
{
	//resize evcerything
	fileTreeComp->setBounds (5, 5, (getWidth()/2)-5, getHeight()-45);
	tabComp->setBounds (getWidth()/2+5, 5, (getWidth()/2)-10, getHeight()-45);
	addBankButton->setBounds(5, getHeight()-35, 90, 25);
	updateTablesButton->setBounds(100, getHeight()-35, 110, 25);
	autoUpdate->setBounds(220, getHeight()-35, 150, 25);
}

//====================================================================================
//populate table rows with new strings and add new items to listable
void DirectoryContentsComponent::fileClicked (const File& file, const MouseEvent& e)
{
	
}

//====================================================================================
//this will listen for messages from our FunctionTableList object, and behave accordingly. 
void DirectoryContentsComponent::actionListenerCallback(const juce::String& string){
	if(string=="delete selected"){
		if(tables[tabComp->getCurrentTabIndex()]->getNumSelectedRows()>0){
		SparseSet<int> sparseSet = tables[tabComp->getCurrentTabIndex()]->getSelectedRows();
		for(int i=sparseSet.size();i>=0;i--){
			functionRowData[tabComp->getCurrentTabIndex()]->remove(sparseSet[i]);
		}
		tables[tabComp->getCurrentTabIndex()]->deselectAllRows();
		tablesList[tabComp->getCurrentTabIndex()]->addOrModifyRows(tabComp->getCurrentTabIndex(), *functionRowData[tabComp->getCurrentTabIndex()]);
		tables[tabComp->getCurrentTabIndex()]->updateContent();
		tables[tabComp->getCurrentTabIndex()]->repaint();
		}
	}
	else if(string=="deselect")
		tables[tabComp->getCurrentTabIndex()]->deselectAllRows();
}

//====================================================================================
//button clicks
void DirectoryContentsComponent::buttonClicked(Button* button)
{
	//each time the add bank button is clicked add a new object to tablesList, and tables arrays, then show them on the 
	//tabbed component.. 
	if(button->getName()=="Add Bank"){
	functionRowData.add(new StringArray());
	tablesList.add(new FunctionTableList());
	tablesList[tablesList.size()-1]->addActionListener(this);
	tables.add(new TableListBox("list", tablesList[tablesList.size()-1])); 
	tables[tables.size()-1]->setLookAndFeel(lookAndFeel);
	tables[tables.size()-1]->setMultipleSelectionEnabled(true);
	tables[tables.size()-1]->setColour(TableListBox::textColourId, Colours::white);
	tables[tables.size()-1]->setColour(TableListBox::backgroundColourId, Colours::black);
	tables[tables.size()-1]->getHeader().addColumn ("Function Tables", 1, 400, 50, 400, TableHeaderComponent::defaultFlags);
	tabComp->addTab("Bank "+String(tabComp->getNumTabs()+1), Colours::white, tables[tables.size()-1], false);
	tabComp->setCurrentTabIndex(tables.size()-1);	
	}
	
	if(button->getName()=="Update Tables"){
	sendActionMessage("updatingDirectoryTables");
	}
}

//====================================================================================
void DirectoryContentsComponent::selectionChanged()
{	
		if(fileTreeComp->getSelectedFile().existsAsFile()){
		updateSelection(fileTreeComp->getSelectedFile());
		if(autoUpdate->getToggleState())
		sendActionMessage("updatingTables");
		}
}

//====================================================================================
const StringArray DirectoryContentsComponent::getFunctionTables()
{
StringArray tables;
//only retrieve the current selected bank
for(int y=0;y<functionRowData[tabComp->getCurrentTabIndex()]->size();y++){
	tables.add("f "+String((tabComp->getCurrentTabIndex()+1)*50+y)+" 0 0 1 \""+functionRowData[tabComp->getCurrentTabIndex()]->getReference(y)+"\" 0 4 1");
}

return tables;		
}

//====================================================================================
void DirectoryContentsComponent::updateSelection(const File& file){
		//update the number of rows in our listbox passing
		if(tables[tabComp->getCurrentTabIndex()]->getNumSelectedRows()>0){
			SparseSet<int> sparseSet = tables[tabComp->getCurrentTabIndex()]->getSelectedRows();
			for(int i=0;i<sparseSet.size();i++){
				functionRowData[tabComp->getCurrentTabIndex()]->getReference(sparseSet[i])= file.getFullPathName();
				tablesList[tabComp->getCurrentTabIndex()]->addOrModifyRows(tabComp->getCurrentTabIndex(), *functionRowData[tabComp->getCurrentTabIndex()]);
				tables[tabComp->getCurrentTabIndex()]->repaintRow(sparseSet[i]);
				tables[tabComp->getCurrentTabIndex()]->updateContent();
				tables[tabComp->getCurrentTabIndex()]->repaint();
			}
		}
		else{
		functionRowData[tabComp->getCurrentTabIndex()]->add(file.getFullPathName());	
		
		tablesList[tabComp->getCurrentTabIndex()]->addOrModifyRows(tabComp->getCurrentTabIndex(), *functionRowData[tabComp->getCurrentTabIndex()]);
		tables[tabComp->getCurrentTabIndex()]->updateContent();
		}	
}
	
//====================================================================================