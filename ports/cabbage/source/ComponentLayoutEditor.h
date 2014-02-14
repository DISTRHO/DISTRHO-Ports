/*
*  ComponentLayoutEditor.h
*  
*  Original written by Haydxn
*  Modified by Jordan Hochenbaum on 10/25/10.
*  http://www.rawmaterialsoftware.com/viewtopic.php?f=6&t=2635
*
*/

#ifndef _COMPONENTLAYOUTEDITOR_H_
#define _COMPONENTLAYOUTEDITOR_H_

#include "JuceHeader.h"
#include "CabbageUtils.h"
#include "CabbageMainPanel.h"
#include "CabbageLookAndFeel.h"

class ChildAlias;
#ifdef Cabbage_GUI_Editor
extern ApplicationProperties* appProperties;
#endif
//this get populated whenever we select multiple objects..
class SelectedComponents   : public SelectedItemSet<ChildAlias*>
{
public:
	void itemSelected (ChildAlias* item);
	void itemDeselected (ChildAlias* item);
};


//=============================================================================
class ComponentLayoutEditor   :   public Component,
									public LassoSource <ChildAlias*>,
									public ChangeBroadcaster
{
    public:
      
      enum ColourIds
      {
         aliasIdleColour,
         aliasHoverColour
      };
      
      ComponentLayoutEditor ();
      ~ComponentLayoutEditor ();
      
	void findLassoItemsInArea (Array <ChildAlias*>& results, const Rectangle<int>& area);
	Rectangle<int> getLassoRect(SelectedItemSet <ChildAlias*>);

    SelectedItemSet <ChildAlias*>& getLassoSelection();
	
      void resized ();
      void paint (Graphics& g);
      
      void setTargetComponent (Component* target);
      
      void bindWithTarget ();
      void updateFrames ();
	  
	  
	void selectDuplicatedComponents(Array<Rectangle <int> > controls);
	Point<int> currentMouseCoors;
	String currentEvent;
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp(const MouseEvent& e);
	LassoComponent <ChildAlias*> lassoComp;
	SelectedComponents selectedFilters;
	Array<Rectangle < int > > selectedCompsOrigCoordinates;
	Array<Rectangle < int > > selectedCompsNewCoordinates;

      void enablementChanged ();
      const Component* getTarget ();
	  void resetAllBorders();
      
    private:
      virtual ChildAlias* createAlias (Component* child, String type, int index);
      
      SafePointer<Component> target;
      OwnedArray<ChildAlias> frames;
	  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentLayoutEditor);
      
    };


//=============================================================================
class ChildAlias   :   public Component,
						public CabbageUtils,
						public KeyListener
    {
    public:
      ChildAlias (Component* targetChild, String type, int index);
      ~ChildAlias ();
      
      void resized ();
      void paint (Graphics& g);
      
      const Component* getTargetChild ();
	  const Component* getTarget();
	  CabbageMainPanel* getMainPanel();
	  ComponentLayoutEditor* getLayoutEditor(){
		ComponentLayoutEditor* c = dynamic_cast <ComponentLayoutEditor*> (getParentComponent());
		if(c)
		return c;
		else
			return nullptr;
	  }
      
	  bool keyPressed(const juce::KeyPress &key ,Component *);
      void updateFromTarget ();
      void applyToTarget (String triggeredFrom);
      
      virtual void userChangedBounds ();
      virtual void userStartedChangingBounds ();
      virtual void userStoppedChangingBounds ();
      
      bool boundsChangedSinceStart ();
      
	  void restrictBounds(int &x, int &y);
	  
      void mouseEnter (const MouseEvent& e);
      void mouseExit (const MouseEvent& e);
      void mouseDown (const MouseEvent& e);
      void mouseUp (const MouseEvent& e);
      void mouseDrag (const MouseEvent& e);
	  void mouseDoubleClick(const MouseEvent &event);		
		
      bool interest, lockMovement;
    private:
	  int index;
	  Array<Rectangle<int> > origBounds;
	  int dragX, dragY;
	  String type;
	  CriticalSection bounds;
      ScopedPointer<ComponentBoundsConstrainer>  constrainer;

      ComponentDragger dragger;
      SafePointer<Component> target;
      
      bool userAdjusting;
      Rectangle<int> startBounds;
      ScopedPointer<ComponentBoundsConstrainer> resizeContainer; //added resizeContainer to limit resizing sizes
      ScopedPointer<ResizableBorderComponent> resizer;
	  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChildAlias);
    };

#endif//_COMPONENTLAYOUTEDITOR_H_
