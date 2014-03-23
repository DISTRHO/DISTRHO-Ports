/*
*  ComponentLayoutEditor.cpp
*  
*  Original written by Haydxn
*  Modified by Jordan Hochenbaum on 10/25/10.
*  http://www.rawmaterialsoftware.com/viewtopic.php?f=6&t=2635
*

and then Rory made shit of it....

*/
#include "ComponentLayoutEditor.h"
//#include "./Plugin/CabbageStandaloneDialog.h"

//this get populated whenever we select multiple objects..
void SelectedComponents::itemSelected (ChildAlias* item){
		item->repaint ();
}
	
void SelectedComponents::itemDeselected (ChildAlias* item){
		item->repaint ();
}


ChildAlias::ChildAlias (Component* targetChild, String type, int ind)
:   target (targetChild), index(ind), type(type), dragX(0), dragY(0), lockMovement(false)
{   
   resizeContainer = new ComponentBoundsConstrainer();
   resizeContainer->setMinimumSize(jmax(target.getComponent()->getWidth()/10, 2), jmax(target.getComponent()->getHeight()/10, 2)); //set minimum size so objects cant be resized too small
   resizer = new ResizableBorderComponent (this,resizeContainer);
   addAndMakeVisible (resizer);
   addKeyListener(this);
   resizer->addMouseListener (this,false);
   constrainer = new ComponentBoundsConstrainer();  
   //Logger::writeToLog("Contructor:"+String(ind));
   getProperties().set("interest", "none");
   userAdjusting = false;
   updateFromTarget ();
   setRepaintsOnMouseActivity (true);
}

ChildAlias::~ChildAlias ()
{

}

void ChildAlias::resized ()
{
  resizer->setBounds (0,0,getWidth(),getHeight());
   
   if (resizer->isMouseButtonDown ())
   {
      applyToTarget ("");
   }
}

void ChildAlias::paint (Graphics& g)
{
	Colour c;
	String interest = getProperties().getWithDefault("interest", "");
	if (interest=="current")
	c = Colours::white;
	else if(interest=="selected")
	c = Colours::yellow.withAlpha(.5f);
	else
	c = Colours::white.withAlpha(.5f);	

	int borderWidth = (interest=="none" ? 1 : 3);
	g.setColour (Colours::white.withMultipliedAlpha (0.1f));
	g.fillAll ();
	g.setColour (c);
	g.drawRect (0,0,getWidth(),getHeight(), borderWidth);
	g.setColour (Colours::black);
	g.drawRect (1,1,getWidth()-1,getHeight()-1, borderWidth);
	g.setColour (c); 
	g.drawRect (2,2,getWidth()-2,getHeight()-2, borderWidth);
}

CabbageMainPanel* ChildAlias::getMainPanel()
{
	return (CabbageMainPanel*)target->getParentComponent();
}

const Component* ChildAlias::getTargetChild ()
{
   return target.getComponent ();
}

const Component* ChildAlias::getTarget()
{
   return target;
}

void ChildAlias::updateFromTarget ()
{
   if (target != NULL)
      //if (!target.hasBeenDeleted ())
   {
      setBounds ( target.getComponent ()->getBounds () );
   }
}

void ChildAlias::applyToTarget (String triggeredFrom)
{
   if (target != NULL)
      //!target.hasBeenDeleted ())
   {
        Component* c = (Component*) target.getComponent ();
		((CabbageMainPanel*)(getTarget()->getParentComponent()))->childBounds.clear();
		((CabbageMainPanel*)(getTarget()->getParentComponent()))->origChildBounds.clear();

		if(type.containsIgnoreCase("CabbageGroupbox")||
			type.containsIgnoreCase("CabbageImage"))
			c->toBack();
			
		else 
		c->toFront(true);

		//if just resizing is taking place we need to resize child components..
		if(startBounds.getTopLeft()==c->getBounds().getTopLeft())
		for(int i=0;i<c->getNumChildComponents();i++){
			float x = ((float)c->getWidth()/(float)startBounds.getWidth());
			float y = ((float)c->getHeight()/(float)startBounds.getHeight());
			
			c->getChildComponent(i)->setBounds(origBounds[i].getX()*x, 
												origBounds[i].getY()*y,
												origBounds[i].getWidth()*x,
												origBounds[i].getHeight()*y);
			((CabbageMainPanel*)(getTarget()->getParentComponent()))->childBounds.add(c->getChildComponent(i)->getBounds());
			((CabbageMainPanel*)(getTarget()->getParentComponent()))->origChildBounds.add(origBounds[i]);
		}
		//Logger::writeToLog("ComponentEditor:"+String(((CabbageMainPanel*)(getTarget()->getParentComponent()))->childBounds.size()));
		//Logger::writeToLog("Number of children: "+String(((CabbageMainPanel*)(getTarget()->getParentComponent()))->childBounds.size()));
	  ((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendActionMessage("Message sent from CabbageMainPanel");
	  c->setBounds (getBounds ());
      userChangedBounds ();
   }
}

void ChildAlias::userChangedBounds ()
{
   //update minimum onscreen amounts so that object can't be resized past the screen area
   resizeContainer->setMinimumOnscreenAmounts(getHeight()+target.getComponent()->getHeight(), getWidth()+target.getComponent()->getWidth(), 
                                    getHeight()+target.getComponent()->getHeight(), getWidth()+target.getComponent()->getWidth());
   
}

void ChildAlias::userStartedChangingBounds ()
{
}

void ChildAlias::userStoppedChangingBounds ()
{
}

bool ChildAlias::boundsChangedSinceStart ()
{
   return startBounds != getBounds ();
}

bool ChildAlias::keyPressed(const juce::KeyPress &key ,Component *)
{
Logger::writeToLog("ChildAlias:"+key.getTextDescription());	
return false;
}

void ChildAlias::mouseDown (const MouseEvent& e)
{		
	//reset drag values..
	dragX = 0;
	dragY = 0;

		
int numSelected = getLayoutEditor()->getLassoSelection().getNumSelected();
bool partOfSelection=false;

if(e.mods.isCommandDown()){
		getLayoutEditor()->selectedFilters.addToSelection(this);
		getProperties().set("interest", "selected");
		repaint();
	}
else{
	for(int i=0;i<numSelected;i++){
	if(getLayoutEditor()->selectedFilters.getSelectedItem(i)->getBounds()==this->getBounds())
	partOfSelection=true;	
	}	
	if(partOfSelection==false){
	getLayoutEditor()->selectedFilters.deselectAll();	
	getLayoutEditor()->resetAllBorders();
	numSelected = 0;
	}
}

	if(numSelected<1){	
		ComponentLayoutEditor* parent = findParentComponentOfClass <ComponentLayoutEditor>();
		if(parent){
		parent->resetAllBorders();
		parent->currentEvent = "mouseDownChildAlias";
		parent->sendChangeMessage();
		}
		getLayoutEditor()->selectedCompsOrigCoordinates.clear();
		getLayoutEditor()->selectedCompsOrigCoordinates.add(this->getBounds());
		//Logger::writeToLog("ChildAlias MouseDown SingleSel:\n"+CabbageUtils::getBoundsString(getBounds()));
		toFront (true);
		if(!e.mods.isCommandDown())
		getProperties().set("interest", "current");
		repaint ();
		parent = nullptr;
		}
	else{
		getLayoutEditor()->selectedCompsOrigCoordinates.clear();	
		for(int i=0;i<numSelected;i++){
			//add original position of selected filters to vector
			//Logger::writeToLog("ChildAlias MouseDown MultiSel:\n"+CabbageUtils::getBoundsString(getLayoutEditor()->getLassoSelection().getSelectedItem(i)->getBounds()));
			getLayoutEditor()->selectedCompsOrigCoordinates.add(
			getLayoutEditor()->getLassoSelection().getSelectedItem(i)->getBounds());	
			}
	}
	

	
	numSelected = getLayoutEditor()->getLassoSelection().getNumSelected();
	Logger::writeToLog(String(numSelected));


		
	if (e.eventComponent == resizer)
	   {
	   }
	else
	   {
		  //added a constrainer so that components can't be dragged off-screen
		  constrainer->setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
		  //dragger.startDraggingComponent (this,e);
	   }

   userAdjusting = true;
   startBounds = getBounds ();
   userStartedChangingBounds ();

   //get the bounds of each of the child components if we are dealing with a plant
   Component* c = (Component*) target.getComponent ();
   origBounds.clear();
   for(int i=0;i<c->getNumChildComponents();i++){
   origBounds.add(c->getChildComponent(i)->getBounds());
   }

#ifdef Cabbage_Build_Standalone
	if(e.mods.isRightButtonDown()){
		PopupMenu m;
		m.setLookAndFeel(&getParentComponent()->getLookAndFeel());
		m.addItem(2, "Delete");
		m.addItem(3, "Duplicate");
		m.addItem(4, "Create plant");
		m.addItem(5, "Break up plant");
		m.addItem(1, "Add to repository");
		int choice = m.show();
		if(choice==1){
			this->getTopLevelComponent()->setAlwaysOnTop(false);
			AlertWindow alert("Add to Repository", "Enter a name and hit 'escape'", AlertWindow::NoIcon, this->getTopLevelComponent()); 
			CabbageLookAndFeel basicLookAndFeel;
			alert.setLookAndFeel(&basicLookAndFeel);
			alert.setColour(TextEditor::textColourId, Colours::white);
			//alert.addTextBlock("Enter a name and hit 'escape'(The following symbols not premitted in names:"" $ % ^ & * ( ) - + )");
			alert.addTextEditor("textEditor", "name", "");
			alert.runModalLoop();
			this->getTopLevelComponent()->setAlwaysOnTop(true);
				bool clashingNames=false;
				int result; 
				
				String plantDir = appProperties->getUserSettings()->getValue("PlantFileDir", "");	
				//Logger::writeToLog(plantDir);
				Array<File> tempfiles;
				StringArray plants;   
				addFilesToPopupMenu(m, tempfiles, plantDir, "*.plant", 100);
				
				for(int i=0;i<tempfiles.size();i++){
				Logger::outputDebugString(tempfiles[i].getFullPathName());
				plants.add(tempfiles[i].getFileNameWithoutExtension());
				}
				
				for(int i=0;i<plants.size();i++)				
				if(plants[i]==alert.getTextEditorContents("textEditor"))
					clashingNames = true;

			ComponentLayoutEditor* parent = findParentComponentOfClass <ComponentLayoutEditor>();
			if(parent){
				parent->currentEvent = "addPlantToRepo:"+alert.getTextEditorContents("textEditor");
			

				if(clashingNames==true){
					result = CabbageUtils::showYesNoMessage("Do you wish to overwrite the existing plant?", &getLookAndFeel());	
					if(result == 0)
					parent->sendChangeMessage();	
					else
						showMessage("Nothing written to repository", &getLookAndFeel());
				}
				else{
					parent->sendChangeMessage();
				}
			}
		}
		else if(choice==2){
			//notify host to delete component/s
			ComponentLayoutEditor* parent = findParentComponentOfClass <ComponentLayoutEditor>();
			if(parent){
			parent->currentEvent = "deleteComponents";
			parent->sendChangeMessage();
			}
		}

		else if(choice==3){
			//notify host to delete component/s
			ComponentLayoutEditor* parent = findParentComponentOfClass <ComponentLayoutEditor>();
			if(parent){
			parent->currentEvent = "duplicateComponents";
			parent->sendChangeMessage();
			}	
		}
		
		else if(choice==4){
			//notify host to delete component/s
			ComponentLayoutEditor* parent = findParentComponentOfClass <ComponentLayoutEditor>();
			if(parent){
			parent->currentEvent = "convertToPlant";
			parent->sendChangeMessage();
			}	
		}		

		else if(choice==5){
			//notify host to delete component/s
			ComponentLayoutEditor* parent = findParentComponentOfClass <ComponentLayoutEditor>();
			if(parent){
			parent->currentEvent = "breakUpPlant";
			parent->sendChangeMessage();
			}	
		}
		
	}
#endif

}

void ChildAlias::mouseUp (const MouseEvent& e)
{ 
	int numSelected = getLayoutEditor()->getLassoSelection().getNumSelected();

	getLayoutEditor()->selectedCompsNewCoordinates.clear();
	if(numSelected>0){
		for(int i=0;i<numSelected;i++){
			//add original position of selected filters to vector
			getLayoutEditor()->selectedCompsNewCoordinates.add(
			getLayoutEditor()->getLassoSelection().getSelectedItem(i)->getBounds());	
			}
		}
	else getLayoutEditor()->selectedCompsNewCoordinates.add(this->getBounds());	
	
	
	if (e.eventComponent == resizer)
	{
	}
	else
	{
	  //add this to reset MainComponent to have keyboard focus so that keyboard shortcuts (eg. lock/unlock) still work / intercept the messages
	  getTopLevelComponent()->getChildComponent(0)->grabKeyboardFocus(); 
	}
	if (userAdjusting) userStoppedChangingBounds ();
	userAdjusting = false;   


	applyToTarget("");

	if(type.containsIgnoreCase("CabbageGroupbox")||
	   type.containsIgnoreCase("CabbageImage"))
	   toBack();
	else 
	   toFront(true);
	   
	//notify host of mouseUp event
	ComponentLayoutEditor* parent = findParentComponentOfClass <ComponentLayoutEditor>();
	if(parent){
	parent->currentEvent = "mouseUpChildAlias";
	parent->sendChangeMessage();
	}	   

	this->grabKeyboardFocus();

	parent = nullptr;
}

void ChildAlias::mouseDoubleClick(const MouseEvent &event)
{
((CabbageMainPanel*)(getTarget()->getParentComponent()))->sendActionMessage("Message sent from CabbageMainPanel:DoubleClick");
}



void ChildAlias::mouseDrag (const MouseEvent& e)
{
int numSelected = getLayoutEditor()->getLassoSelection().getNumSelected();	
if (e.mods.isLeftButtonDown()){
   if (e.eventComponent == resizer)
   {
   }
   else
   {
      if (!e.mouseWasClicked ())
      {
		 constrainer->setMinimumOnscreenAmounts(getHeight(), getWidth(), getHeight(), getWidth());
         
		 //if a child is selected, and it's part of a group, move all commponents...
		 if(numSelected>0)
		 for(int i=0;i<numSelected;i++){
			 ChildAlias* c = getLayoutEditor()->getLassoSelection().getSelectedItem(i);
		 	
			 
			//if(e.getDistanceFromDragStartX()%10==0)
				dragX = e.getDistanceFromDragStartX();
			//if(e.getDistanceFromDragStartY()%10==0)
				dragY = e.getDistanceFromDragStartY();
			 
			//snap to grid....
			int gridSize = 2;	

	        Rectangle<int> bounds = getLayoutEditor()->getLassoRect(getLayoutEditor()->getLassoSelection());

			int selectedCompsPosX = getLayoutEditor()->selectedCompsOrigCoordinates[i].getX();
			int selectedCompsPosY = getLayoutEditor()->selectedCompsOrigCoordinates[i].getY();
			selectedCompsPosY = selectedCompsPosY+dragY;			
			selectedCompsPosY = selectedCompsPosY/gridSize*gridSize;				
			selectedCompsPosX = selectedCompsPosX+dragX;	
			selectedCompsPosX = selectedCompsPosX/gridSize*gridSize;

			restrictBounds(selectedCompsPosX, selectedCompsPosY);				
			c->setTopLeftPosition(selectedCompsPosX, selectedCompsPosY);
			c->applyToTarget(""); 
		 }
		 else{

				dragX = e.getDistanceFromDragStartX();
				dragY = e.getDistanceFromDragStartY();	
			int selectedCompsPosX = startBounds.getX()+dragX;	
			int selectedCompsPosY = startBounds.getY()+dragY;
			//snap to grid....
			int gridSize = 2;
			selectedCompsPosX = selectedCompsPosX/gridSize*gridSize;
			selectedCompsPosY = selectedCompsPosY/gridSize*gridSize;
			
			restrictBounds(selectedCompsPosX, selectedCompsPosY);
	
				setTopLeftPosition(selectedCompsPosX,selectedCompsPosY);
		 }
		 applyToTarget ("");
		 if(type.containsIgnoreCase("juce::GroupComponent")||
					type.containsIgnoreCase("CabbageImage"))
			toBack();
      }
   }
   if(type.containsIgnoreCase("juce::GroupComponent")||
	   type.containsIgnoreCase("CabbageImage"))
	   toBack();
   else 
	   toFront(true);
}//end of left click check
}
//=====================================
//restrict bounds
//=====================================

void ChildAlias::restrictBounds(int &x, int &y)
{
if(x>getParentComponent()->getWidth()-this->getWidth())
		x = getParentComponent()->getWidth()-this->getWidth();
if(x<0)
		x = 0;
if(y>getParentComponent()->getHeight()-this->getHeight())
		y = getParentComponent()->getHeight()-this->getHeight();
if(y<0)
		y = 0;		
	
	
}

void ChildAlias::mouseEnter (const MouseEvent& e)
{
int numSelected = getLayoutEditor()->getLassoSelection().getNumSelected();	
if(numSelected>1)
	resizer->setVisible(false);
else resizer->setVisible(true);
//  interest = true;
//repaint ();
}

void ChildAlias::mouseExit (const MouseEvent& e)
{
//interest = false;
//repaint ();
}

//=============================================================================
ComponentLayoutEditor::ComponentLayoutEditor ()
:   target (0)
{
   setColour (ComponentLayoutEditor::aliasIdleColour,Colours::lightgrey.withAlpha(0.2f));
   setColour (ComponentLayoutEditor::aliasHoverColour,Colours::white.withAlpha(0.5f));
   // setInterceptsMouseClicks (false, true);   
}

ComponentLayoutEditor::~ComponentLayoutEditor ()
{
   //if (target != getTopLevelComponent()->getChildComponent(0) ){deleteAndZero(target);} //added this to make sure we dont remove our background component
   //if (target) { deleteAndZero (target); } //original
}

void ComponentLayoutEditor::resized ()
{
   for (int i=0; i<frames.size(); i++)
   {
      frames.getUnchecked(i)->updateFromTarget ();
   }
}

void ComponentLayoutEditor::paint (Graphics& g)
{
}

SelectedItemSet <ChildAlias*>& ComponentLayoutEditor::getLassoSelection()
{
   return selectedFilters;
}


Rectangle<int> ComponentLayoutEditor::getLassoRect(SelectedItemSet <ChildAlias*> children)
{
Rectangle<int> bounds(9999, 9999, -9999, -9999);
for(int i=0;i<children.getItemArray().size();i++)
	bounds.setX(children.getSelectedItem(i)->getX()<bounds.getX() ? children.getSelectedItem(i)->getX() : bounds.getX());
for(int i=0;i<children.getItemArray().size();i++)
	bounds.setY(children.getSelectedItem(i)->getY()<bounds.getY() ? children.getSelectedItem(i)->getY() : bounds.getY());
for(int i=0;i<children.getItemArray().size();i++)
	bounds.setY(children.getSelectedItem(i)->getY()<bounds.getY() ? children.getSelectedItem(i)->getY() : bounds.getY());
for(int i=0;i<children.getItemArray().size();i++)
	if(children.getSelectedItem(i)->getWidth()+children.getSelectedItem(i)->getX()>bounds.getX()+bounds.getWidth())
	bounds.setWidth(children.getSelectedItem(i)->getWidth()+children.getSelectedItem(i)->getX()-bounds.getX());
for(int i=0;i<children.getItemArray().size();i++)
	if(children.getSelectedItem(i)->getHeight()+children.getSelectedItem(i)->getY()>bounds.getY()+bounds.getHeight())
	bounds.setHeight(children.getSelectedItem(i)->getHeight()+children.getSelectedItem(i)->getY()-bounds.getY());

return bounds;	
}

void ComponentLayoutEditor::setTargetComponent (Component* targetComp)
{
   jassert (targetComp);
   jassert (targetComp->getParentComponent() == getParentComponent());
   
   if (target)
   {
      if (target.getComponent() == targetComp) return;
      deleteAndZero (target);
   }
   
   target = targetComp;
   bindWithTarget ();
}

void ComponentLayoutEditor::bindWithTarget ()
{
   if (target != NULL)
      //if (target && !target->hasBeenDeleted ())
   {
      Component* t = (Component*) target.getComponent ();
      Component* p = t->getParentComponent ();      
      p->addAndMakeVisible (this);
      setBounds (t->getBounds ());
      
      updateFrames ();
   }
}

void ComponentLayoutEditor::updateFrames ()
{
   int compIndex = 0;
   frames.clear ();
   
   if (target != NULL)
      //if (target && !target->hasBeenDeleted ())
   {
      Component* t = (Component*) target.getComponent ();

      int n = t->getNumChildComponents ();
      for (int i=0; i<n; i++)
      {
         Component* c = t->getChildComponent (i);
		 String type(typeid(*c).name());

		 if (c)
         {
                ChildAlias* alias = createAlias (c, type, compIndex++);
				//pass on relative X and Y's to alias components so they are plant aware...
				alias->getProperties().set("plantX", var(c->getProperties().getWithDefault(var::identifier("plantX"), 0)));
                alias->getProperties().set("plantY", var(c->getProperties().getWithDefault(var::identifier("plantY"), 0)));				
				if (alias)
                {
                frames.add (alias);
                addAndMakeVisible (alias);
                }
         }
      }
   }
}

void ComponentLayoutEditor::resetAllBorders()
{
for(int i=0;i<frames.size();i++){
	frames[i]->getProperties().set("interest", "none");
}
repaint();
}

void ComponentLayoutEditor::enablementChanged ()
{
   if (isEnabled ())
   {
      setVisible (true);
   }
   else
   {
      setVisible (false);
   }
}

void ComponentLayoutEditor::mouseUp(const MouseEvent& e)
{
//Logger::writeToLog("Number selected: "+String(selectedFilters.getNumSelected()));
for(int i=0;i<selectedFilters.getNumSelected();i++){
//Logger::writeToLog(getChildComponent(i)->getName());
	selectedFilters.getSelectedItem(i)->getProperties().set("interest", "selected");
	selectedFilters.getSelectedItem(i)->repaint();
	selectedCompsOrigCoordinates.add(selectedFilters.getSelectedItem(i)->getBounds());
}
lassoComp.endLasso();
removeChildComponent (&lassoComp);	
}

void ComponentLayoutEditor::mouseDrag (const MouseEvent& e)
{
	selectedFilters.deselectAll();
	lassoComp.toFront (false);
    lassoComp.dragLasso (e);
	currentEvent = "mouseDragLayoutEditor";
	sendChangeMessage();
}

void ComponentLayoutEditor::mouseDown (const MouseEvent& e)
{
//deselect all grouped filters and set alpha to normal
selectedFilters.deselectAll();
for(int i=0;i<getNumChildComponents();i++){
	getChildComponent(i)->getProperties().set("interest", "none");
	getChildComponent(i)->repaint();
	}
//clear vector containing child positions before movement
selectedCompsOrigCoordinates.clear();	
	
if(e.mods.isPopupMenu()){	
	currentMouseCoors = e.getPosition();
	//call change method in CabbagePluginEditor..
	currentEvent = "triggerPopupMenu";
	sendChangeMessage();
	}
else{
	addChildComponent (&lassoComp);
    lassoComp.beginLasso (e, this);
	}
	//Logger::writeToLog(currentEvent);
}


void ComponentLayoutEditor::selectDuplicatedComponents(Array<Rectangle <int> > controls)
{
	selectedFilters.deselectAll();
	int num = getNumChildComponents();
		for(int u=0;u<controls.size();u++){
			for(int i=0;i<getNumChildComponents();i++){
			//Logger::writeToLog("Duped Control:"+String(controls[u].getX()));
			//Logger::writeToLog("Child Control:"+String(getChildComponent(i)->getBounds().getX()));
		
			if(controls[u]==getChildComponent(i)->getBounds()){
				getChildComponent(i)->getProperties().set("interest", "selected");
				getChildComponent(i)->repaint();
				ChildAlias* c = (ChildAlias*)getChildComponent(i);
				selectedFilters.addToSelection(c);			
			}
		}
	}	
}


void ComponentLayoutEditor::findLassoItemsInArea (Array <ChildAlias*>& results, const Rectangle<int>& area)
{
    const Rectangle<int> lasso (area);

	for (int i = 0; i < getNumChildComponents()-1; i++)
    {
		ChildAlias* c = (ChildAlias*)getChildComponent(i);
		if (c->getBounds().intersects (lasso)){
		results.addIfNotAlreadyThere(c);
		selectedFilters.addToSelection(c);
		//Logger::writeToLog(c->getName());
		}
		else
			selectedFilters.deselect(c);
    }
}


const Component* ComponentLayoutEditor::getTarget ()
{
   if (target) return target.getComponent ();
   return 0;
}

ChildAlias* ComponentLayoutEditor::createAlias (Component* child, String type, int index)
{
   return new ChildAlias (child, type, index);
}

