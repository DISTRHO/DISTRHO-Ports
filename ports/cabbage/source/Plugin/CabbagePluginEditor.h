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

#ifndef __PLUGINEDITOR_H_F4EBBBA1__
#define __PLUGINEDITOR_H_F4EBBBA1__

#include  "../CabbageCustomWidgets.h"
#include "../JuceHeader.h"
#include "CabbagePluginProcessor.h"
//#include "../CabbageLookAndFeel.h"
#include "../CabbagePropertiesDialog.h"
#include "../CabbageUtils.h"

extern CabbageLookAndFeel* lookAndFeel;
extern CabbageLookAndFeelBasic* lookAndFeelBasic;

class CabbageMainPanel;
class ComponentLayoutEditor;
class CabbageCornerResizer;
//==============================================================================
class CabbagePlantWindow  : public DocumentWindow
{

public:
        CabbagePlantWindow(const String& title, const Colour& backgroundColour)
                : DocumentWindow (title, backgroundColour, DocumentWindow::closeButton){};
        ~CabbagePlantWindow(){
        };

        void closeButtonPressed(){
        setVisible(false);
		setAlwaysOnTop(false);
        };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePlantWindow);
};

//==============================================================================
// pointData class for holding info about each segment of a line
class PointData 
{
public:
Point <int> point;
int curveType;	
PointData(Point<int> point, int curveType):point(point),curveType(curveType)
{}
~PointData(){}				
};

//==============================================================================
// main GUI editor window
//==============================================================================
class CabbagePluginAudioProcessorEditor  : public AudioProcessorEditor,
                                                                public CabbageUtils,
                                                                public SliderListener,
                                                                public ComboBoxListener, 
                                                                public ButtonListener,
                                                                public KeyListener,
                                                                public ChangeBroadcaster,
                                                                public ChangeListener,
                                                                public ActionListener, 
																public Timer
																
{
public:
    CabbagePluginAudioProcessorEditor (CabbagePluginAudioProcessor* ownerFilter);
    ~CabbagePluginAudioProcessorEditor();

    void paint (Graphics& g);
    void resized();
	void setEditMode(bool on);
	void InsertGUIControls(CabbageGUIClass cAttr);
	void ksmpsYieldCallback();
	void updateSize();
	ScopedPointer<CabbagePropertiesDialog> propsWindow;
	//main GUI controls vectors..
	OwnedArray<Component> comps;
	OwnedArray<Component> layoutComps;
	void updateLayoutEditorFrames();
	ScopedPointer<CabbageTable> cabTable;
	ScopedPointer<CabbageCornerResizer> resizer;
private:
        
		void createfTableData(Table* table);
        bool keyPressed(const juce::KeyPress &,Component *);
		void updateSizesAndPositionsOfComponents(int newLine = 0);
		void deleteComponents();
		void duplicateComponents();
		void populateLineNumberArray(StringArray csdArray);
		void addToRepository(String entryName);
		void convertIntoPlant();
		void breakUpPlant();
        void InsertGroupBox(CabbageGUIClass &cAttr);
        void comboBoxChanged (ComboBox* combo);
        void InsertComboBox(CabbageGUIClass &cAttr);
		void InsertSoundfiler(CabbageGUIClass &cAttr);
		void InsertDirectoryList(CabbageGUIClass &cAttr);
        void SetupWindow(CabbageGUIClass &cAttr);
        void InsertSlider(CabbageGUIClass &cAttr);
		void sliderValueChanged (Slider*);
        void InsertButton(CabbageGUIClass &cAttr);
        void InsertSourceButton(CabbageGUIClass &cAttr);
        void InsertVUMeter(CabbageGUIClass &cAttr);
        void InsertCheckBox(CabbageGUIClass &cAttr);
        void InsertCsoundOutput(CabbageGUIClass &cAttr);
        void InsertMIDIKeyboard(CabbageGUIClass &cAttr);
        void InsertXYPad(CabbageGUIClass &cAttr);
		void InsertFileButton(CabbageGUIClass &cAttr);
        void InsertImage(CabbageGUIClass &cAttr);
        void InsertLabel(CabbageGUIClass &cAttr);
        void InsertTable(CabbageGUIClass &cAttr);
		void InsertMultiTab(CabbageGUIClass &cAttr);
        void InsertInfoButton(CabbageGUIClass &cAttr);
        void InsertLineSeparator(CabbageGUIClass &cAttr);
        void InsertPatternMatrix(CabbageGUIClass &cAttr);
        void InsertSnapshot(CabbageGUIClass &cAttr);
        void InsertPVSViewer(CabbageGUIClass &cAttr);
		void InsertTransport(CabbageGUIClass &cAttr);
        void buttonClicked(Button*);
		void buttonStateChanged(Button*);
        void showInsertControlsMenu(int x, int y);
		void actionListenerCallbackForWidgets(const String message);
		void insertScoreStatementText(Table *table, bool overwrite);
		void restoreParametersFromPresets(XmlElement* xmlData);
		void savePresetsFromParameters(File selectedFile, String mode);
		void refreshDiskReadingGUIControls(String typeOfControl);
		void updateGUIControls();

		void timerCallback();
        bool LOCKED;
        void insertComponentsFromCabbageText(StringArray text, bool useOffset);
//		void insertComponentsFromCabbageTextArray(StringArray text, bool plant);
        Array<int> lineNumbers;
		Array<int> plantLineNumbers;
		int currentLineNumber;
        File SnapShotFile;
        String presetFileText;
		String tempPlantText;
        void actionListenerCallback (const String& message);
        int zero_dbfs;
		StringArray tempArray;
		StringArray pastEvents;
		Array<Rectangle <int> > boundsForDuplicatedCtrls; 


        //CabbagePluginAudioProcessor* filter;
        CabbagePluginAudioProcessor* getFilter() const
			{
				return static_cast <CabbagePluginAudioProcessor*> (getAudioProcessor());
			}

	    bool keyStateChanged(bool onoff){
	    keyIsPressed = onoff;
		return false;
	    }
		bool keyIsPressed;
		bool isMouseDown;
		void positionComponentWithinPlant(String type, int idx, float left, float top, float width, float height, Component *layout, Component *control);
        //ScopedPointer<CabbagePlantWindow> subPatch;
        OwnedArray<CabbageButton> plantButton;
        OwnedArray<CabbagePlantWindow> subPatch;
        ScopedPointer<InfoWindow> infoWindow;
#ifdef Cabbage_GUI_Editor
        ScopedPointer<CabbageMainPanel> componentPanel;
        ScopedPointer<ComponentLayoutEditor> layoutEditor;
#else
        ScopedPointer<Component> componentPanel;
#endif
        ScopedPointer<MidiKeyboardComponent> midiKeyboard;
        ScopedPointer<LookAndFeel> feely;
		Array<float> incomingValues;
		ComponentBoundsConstrainer resizeLimits;

        AudioPlayHead::CurrentPositionInfo hostInfo;
		void changeListenerCallback (ChangeBroadcaster *source);
        Colour formColour, fontColour;
        String authorText;
        String formPic;
        float inValue;
		int xyPadIndex;
        ScopedPointer<CabbageLookAndFeel> lookAndFeel;
		ScopedPointer<CabbageLookAndFeelBasic> basicLookAndFeel;
        ScopedPointer<Label> debugLabel;
		StringArray scoreEvents;



        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbagePluginAudioProcessorEditor);

};


//==============================================================================
class CabbageCornerResizer : public ResizableCornerComponent
{
	CabbagePluginAudioProcessorEditor* editor;
public:
	CabbageCornerResizer(CabbagePluginAudioProcessorEditor* parent, Component* comp, ComponentBoundsConstrainer *constrainer):
						ResizableCornerComponent(comp, constrainer), editor(parent){};
	~CabbageCornerResizer(){};

	void mouseUp(const MouseEvent &e){
		editor->updateSize();
		}
						
	
};

#endif  // __PLUGINEDITOR_H_F4EBBBA1__

