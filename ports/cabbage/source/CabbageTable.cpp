#include "CabbageTable.h"
#include "CabbageUtils.h"



/*
  ====================================================================================
	Cabbage Envelope Handle 
  ====================================================================================
*/
CabbageEnvelopeHandleComponent::CabbageEnvelopeHandleComponent(int _index, Colour colour, bool fixed)
:index(_index), x(0), y(0), colour(colour), fixed(fixed)
{
	getProperties().set(String("curveType"), LINEAR);
	this->setInterceptsMouseClicks(true, false);
}

CabbageEnvelopeHandleComponent::~CabbageEnvelopeHandleComponent()
{
}

void CabbageEnvelopeHandleComponent::paint (Graphics& g)
{
	g.setColour (colour);
	if(!fixed)
	g.fillEllipse (1, 1, getWidth()-2, getHeight()-2);
	else
	g.fillRect(1, 1, getWidth()-2, getHeight()-2);
	
}

Table* CabbageEnvelopeHandleComponent::getParentComponent()
{
	return (Table*)Component::getParentComponent();
}

void CabbageEnvelopeHandleComponent::removeThisHandle()
{
	getParentComponent()->removeHandle (this);	
}

void CabbageEnvelopeHandleComponent::mouseEnter (const MouseEvent& e)
{
	setMouseCursor (MouseCursor::DraggingHandCursor);
}

void CabbageEnvelopeHandleComponent::mouseUp (const MouseEvent& e)
{
changeMessage = "updateOnMouseUp";
sendChangeMessage();	
}

void CabbageEnvelopeHandleComponent::mouseDown (const MouseEvent& e)
{

	x = getX();
	y = getY();
		
	setMouseCursor (MouseCursor::DraggingHandCursor);

	dragger.startDraggingComponent (this, e);

	if ((e.mods.isShiftDown() == true) && (e.mods.isRightButtonDown() == true))
		removeThisHandle();

	PopupMenu pop, subm;
	pop.setLookAndFeel(&this->getTopLevelComponent()->getLookAndFeel());
	subm.setLookAndFeel(&this->getTopLevelComponent()->getLookAndFeel());
	if(e.mods.isRightButtonDown() == true)
	{
		//pop.addItem(1, "Linear");
		//subm.addItem(2, "Convex");
		//subm.addItem(3, "Concave");
		//pop.addSubMenu("Expon", subm);
		pop.addItem(4, "Delete");
		
		const int result = pop.show();
		if(result==1)
			getProperties().set(String("curveType"), LINEAR);
		else if(result==2)
			getProperties().set(String("curveType"), CONVEX);
		else if(result==3) 
			getProperties().set(String("curveType"), CONCAVE);
		else if(result==4){
			changeMessage = "removeHandle";
			sendChangeMessage();
		}
	}
}

CabbageEnvelopeHandleComponent* CabbageEnvelopeHandleComponent::getPreviousHandle()
{
	return getParentComponent()->getPreviousHandle(this);
}

CabbageEnvelopeHandleComponent* CabbageEnvelopeHandleComponent::getNextHandle()
{
	return getParentComponent()->getNextHandle(this);
}


void CabbageEnvelopeHandleComponent::mouseDrag (const MouseEvent& e)
{
	CabbageEnvelopeHandleComponent* previousHandle = getPreviousHandle();
	CabbageEnvelopeHandleComponent* nextHandle = getNextHandle();
	bool fixed = this->getProperties().getWithDefault("fixedPos", false);
		
		
	int leftLimit = previousHandle == 0 ? 0 : previousHandle->getX()+1;
	int rightLimit = nextHandle == 0 ? getParentWidth()-previousHandle->getHeight() : nextHandle->getX()-1;
	int topLimit = previousHandle == 0 ? 0 : previousHandle->getX()+1;
	int bottomLimit = nextHandle == 0 ? getParentWidth()-previousHandle->getHeight() : nextHandle->getX()-1;
	int dragX = x+e.getDistanceFromDragStartX();
	int dragY = y+e.getDistanceFromDragStartY();
		
	//dragger.dragComponent(this, e, &resizeLimits);
	if(dragX < leftLimit) 
		dragX = leftLimit;
	if(dragX > rightLimit) 
		dragX = rightLimit;
	if(dragY< 0) 
		dragY = 0;
	if(dragY > height-5) 
		dragY = height-5;
	if(fixed) 
		dragX = x;
		
	this->setTopLeftPosition(dragX, dragY);

	getParentComponent()->repaint();
}


/*
  ====================================================================================
	Table class
  ====================================================================================
*/
Table::Table (String chan, int tblNumber, int tableSize, bool fixedEnv, bool drawHorizontal, bool drawOriginal, Colour colour) : 
	tableSize(tableSize), 
	zoom(1.0),
	tableNumber(tblNumber), 
	channel(chan),
	activeColour(colour),
	draggingHandle (0), 
	scrubberPosition(0),
	isCurrentlyOnTop(false),
	drawOriginalTableData(drawOriginal),
	drawHorizontalSegments(drawHorizontal),
	fixedEnvelope(fixedEnv),
	handleWidth(8),
	drawFill(false),
	isResizing(true),
	useAmpRanges(false),
	editMode(false)
{

}

Table::~Table()
{
	handles.clear();
}
//====================================================
void Table::resized()
{
	//We need to make room for the h scrollbar, therefore table data
	//can't use the full height of the canvas
	tableTop = getHeight()*0.01;
	tableBottom = getHeight()*0.99;
	tableHeight = tableBottom - tableTop;
}

//====================================================
void Table::setOriginalWidth(int w)
{
	//Sets the original width from when the table was initialised
	origWidth = w; 
	maxZoomForOverview = tableSize / origWidth;
}

//====================================================
void Table::setGlobalAmpRange(float globalMaxAmp, float globalMinAmp, float globalAmpRange)
{
	minAmp = globalMinAmp;
	maxAmp = globalMaxAmp;
	ampRange = globalAmpRange;
}

//====================================================
void Table::createAmpOverviews (Array<double, CriticalSection> csndInputData)
{
	//This method creates smaller amp overview arrays using the
	//original table data...
	tableData.amps.clear();
	tableData.y.clear();
	overview.maxAmps.clear();
	overview.minAmps.clear();

	tableData.amps = csndInputData;
	
	//Logger::writeToLog(String(tableData.amps.size()));
	zeroAmpPosition = convertAmpToPixel(0);

	// Filling overview arrays. The original table data is broken into
	//separate blocks. The max and min values are then stored from each block.
	//Block size is dependent on the max zoom level we use for overviews.
	float incr = (float)tableSize / (getWidth()*maxZoomForOverview);
	float maxBlockValue, minBlockValue;
	for (float i=0; i<tableSize; i+=incr) { 
		maxBlockValue = minBlockValue = tableData.amps[(int)i+0.5];
		for (int j=0; j<(int)incr+0.5; j++) { //loop for current block
			if (tableData.amps[(int)i+j+0.5] > maxBlockValue) 
				maxBlockValue = tableData.amps[(int)i+j+0.5];
			if (tableData.amps[(int)i+j+0.5] < minBlockValue) 
				minBlockValue = tableData.amps[(int)i+j+0.5]; 
		}
		overview.maxAmps.add (maxBlockValue);
		overview.minAmps.add (minBlockValue);
	}
	setDataSource (zoom);
	//if(drawOriginalTableData==false)
	//createEnvPath();
	//makeTableEditable();
}

//====================================================
void Table::setDataSource (int zoomValue)
{
	//If current zoom <= max zoom for overview, then the 
	//overview arrays are used and converted to y coordinates.
	//Otherwise the initial table data is used.
	if (zoomValue <= maxZoomForOverview) { 
		int incrSize = maxZoomForOverview/zoomValue;
		float maxBlockValue, minBlockValue;
		
		overview.maxY.clear();
		overview.minY.clear();
		for (int i=0; i<overview.maxAmps.size(); i+=incrSize) {
			maxBlockValue = overview.maxAmps[i];
			minBlockValue = overview.minAmps[i];
			for (int j=0; j<incrSize; j++) { //loop for current block
				if (overview.maxAmps[i+j] > maxBlockValue) 
					maxBlockValue = overview.maxAmps[i+j];
				if (overview.minAmps[i+j]  < minBlockValue) 
					minBlockValue = overview.minAmps[i+j]; 
			}
		
			overview.maxY.add (convertAmpToPixel(maxBlockValue));
			overview.minY.add (convertAmpToPixel(minBlockValue));
			useOverview = true;
		}
	}
	// Else we use original table data for painting
	else {
		//numPixelsPerIndex = (zoomValue*origWidth) / (float)tableSize;
		numPixelsPerIndex = (float)getWidth() / (float)tableSize;
		useOverview = false;
	}
	
	repaint();
}

//====================================================
float Table::convertAmpToPixel (float ampValue)
{
float normalisedAmp = (ampValue-minAmp) / ampRange; //first change to normalised value

if(this->toggleMaxMin){
if(ampValue == maxAmp)
	return 0;
else if(ampValue==minAmp)
	return getHeight();
else{	
	return jmax(0.f, ((1-normalisedAmp) * tableHeight) + tableTop);
	}
}
else
	return jmax(0.f, ((1-normalisedAmp) * tableHeight) + tableTop);
}

//====================================================
float Table::convertPixelToAmp(float pixelYValue)
{
//scale values between 0 and 1
double yAmp = (float)(this->getHeight()-(pixelYValue))/(float)this->getHeight();
//now bring them in range
yAmp = (yAmp*ampRange)+minAmp;
yAmp = (yAmp < minAmp ? minAmp : yAmp);
yAmp = (yAmp > maxAmp ? maxAmp : yAmp);	
return yAmp;	
}

//====================================================
void Table::cacheBackgroundImage()
{
	//This method creates the background image that the table data will be
	//painted on to.  It is then stored in cache.
	img = Image(Image::ARGB, origWidth, getHeight(), true);
	Graphics g (img);
			
	ImageCache::addImageToCache (img, 15);
}

//====================================================
void Table::paint (Graphics& g)
{

	//Image bg = ImageCache::getFromHashCode(15);
	//g.drawImage (bg, viewX, 0, origWidth, getHeight(), 
	//	0, 0, bg.getWidth(), bg.getHeight(), false);
	if (isCurrentlyOnTop)
		currColour = activeColour.withMultipliedSaturation(3);
	else
		currColour = activeColour;
	int startIndx, endIndx;

	// If using overview...
	if (useOverview && drawOriginalTableData) {
		startIndx = (viewX/getWidth()) * overview.maxY.size();
		endIndx = startIndx + viewWidth;
		float bottomYPixelValue, topYPixelValue;
		bottomYPixelValue = topYPixelValue = 0;

		// For loop which will draw a vertical line between the min and max value 
		//for each x pixel 
		int xPixelValue = 0; 
		for (int i=startIndx; i<endIndx; i++) {
			//We need to make sure that the current min value is not greater than the 
			//next max value. Otherwise there would be a gap in the wave form...
			if (overview.minY[i] < overview.maxY[i+1])
				bottomYPixelValue = overview.maxY[i+1];
			else
				bottomYPixelValue = overview.minY[i];
			if (overview.maxY[i] > overview.minY[i+1])
				topYPixelValue = overview.minY[i+1];
			else
				topYPixelValue = overview.maxY[i];
			 
			float minGap = 0.4;
			float diff = bottomYPixelValue - topYPixelValue;
			if (diff < minGap) {
				bottomYPixelValue += (minGap-diff)/2;
				topYPixelValue -= (minGap-diff)/2;
			}

			if (CabbageUtils::isNumber(topYPixelValue) && CabbageUtils::isNumber(bottomYPixelValue)) {
				g.setColour(currColour);
				topYPixelValue -= minWaveHeight/2;
				bottomYPixelValue += minWaveHeight/2;
				g.drawVerticalLine (xPixelValue+viewX, topYPixelValue, bottomYPixelValue);
				xPixelValue += 1;

				// Fill
				if (tableSize <= 4096) {
					g.setColour(currColour.withAlpha(0.1f));
					if (bottomYPixelValue < zeroAmpPosition)
						g.drawVerticalLine (xPixelValue+viewX, bottomYPixelValue, zeroAmpPosition);
					else if (bottomYPixelValue > zeroAmpPosition)
						g.drawVerticalLine (xPixelValue+viewX, zeroAmpPosition, bottomYPixelValue);

					if (isCurrentlyOnTop)
						currColour = activeColour.withMultipliedSaturation(3);
					else
						currColour = activeColour;
				}
			}
		}
	}
	
	//Else if using original array values for painting...
	else if (!useOverview && drawOriginalTableData) {
		g.setColour(currColour);
		startIndx = ((viewX/getWidth()) * tableData.amps.size()) + 0.5; //0.5 for rounding
		endIndx = (startIndx + (viewWidth/numPixelsPerIndex))+0.5; 
		float prevX = viewX;
		float prevY = convertAmpToPixel (tableData.amps[startIndx]);
		float currY;
		for (int i=startIndx+1; i<=endIndx; i++) {
			currY = convertAmpToPixel (tableData.amps[i]);
			if(drawHorizontalSegments==true){
			g.setColour(currColour);
			g.drawLine(prevX, prevY, prevX+numPixelsPerIndex, prevY, minWaveHeight);
			g.setColour(currColour.darker(.7));
			if(drawFill==true)
			g.fillRect(prevX, (prevY>convertAmpToPixel(0) ? prevY-minWaveHeight : prevY+minWaveHeight), numPixelsPerIndex, convertAmpToPixel(0)-prevY);	
			}
			else{
			g.drawLine(prevX, prevY, prevX+numPixelsPerIndex, currY, minWaveHeight);
			// For drawing index markers
			if (numPixelsPerIndex > 4)
				g.drawVerticalLine (prevX+numPixelsPerIndex, currY-3, currY+3);
			
			}
			prevX = prevX + numPixelsPerIndex;
			prevY = currY;
		}
	}

//	g.setColour(Colours::lightblue);
//	envPath.letoleToFit (0, tableTop, getWidth(), tableHeight, false);
//	g.strokePath (envPath, PathStrokeType(2.0f));
	//----- For handles....
	
	else if(!drawOriginalTableData){
	envPath.clear();
	
	for(int i = 0; i < handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked(i);
		int prevX, prevY;
		if((int)handle->getProperties().getWithDefault("curveType", 1)==0)
		{	
		//for linear envelopes
		if(i==0){
			envPath.startNewSubPath((handle->getX() + handle->getRight()) / 2,
									 (handle->getY() + handle->getBottom()) / 2);
		    if(toggleMaxMin)
			 g.setColour(currColour);
			else	
			 g.setColour(currColour.darker(.7f));
				 
			if(drawHorizontalSegments==true && fixedEnvelope == true)
			if(drawFill==true)
			g.fillRect(0, jmax(handle->getY(), 0),  jmax(handle->getWidth(), 0), jmax(0, getHeight()));	
			if(toggleMaxMin){
			g.setColour(Colours::black);	
			g.drawRect(0, jmax(handle->getY(), 0),  jmax(handle->getWidth(), 0), jmax(0, getHeight()), 1);
			g.setColour(currColour);	
			}
			prevX = handle->getX();
			prevY = handle->getY();
			}
		else 
			{
			if(drawHorizontalSegments==true && fixedEnvelope == false){
			if(drawFill==true)
			g.fillRect(jmax(handle->getX(), 0), jmax(handle->getY(), 0), jmax(handle->getWidth(), 0), jmax(0, getHeight()));
			if(toggleMaxMin){
			g.setColour(Colours::black);	
			g.drawRect(jmax(handle->getX(), 0), jmax(handle->getY(), 0), jmax(handle->getWidth(), 0), jmax(0, getHeight()), 1);
			g.setColour(currColour);	
			}
			envPath.lineTo((handle->getX() + handle->getRight()) / 2, (prevY+(handle->getHeight()/2)));				
			envPath.lineTo((handle->getX() + handle->getRight()) / 2, (handle->getY() + handle->getBottom()) / 2);
			
			}
			else if(drawHorizontalSegments==true && fixedEnvelope==true){
			if(drawFill==true)
			g.fillRect(jmax(handle->getX(), 0), jmax(handle->getY(), 0), jmax(handle->getWidth(), 0), jmax(0, getHeight()));
			if(toggleMaxMin){
			g.setColour(Colours::black);	
			g.drawRect(jmax(handle->getX(), 0), jmax(handle->getY(), 0), jmax(handle->getWidth(), 0), jmax(0, getHeight()), 1);
			g.setColour(currColour);	
			}
				
			envPath.lineTo((handle->getX()+1),
								(prevY+(handle->getHeight()/2)));	
			envPath.lineTo((handle->getX()+1),
								(handle->getY() + handle->getBottom()) / 2);				
				
			}
			else{
			envPath.lineTo((handle->getX() + handle->getRight()) / 2,
								(handle->getY() + handle->getBottom()) / 2);					
			}
			prevY = handle->getY();
			prevX = handle->getX();
		}
		}
		else
			{
			//for bezier envelopes
			if(i==0){
				envPath.startNewSubPath((handle->getX() + handle->getRight()) / 2,
									 (handle->getY() + handle->getBottom()) / 2);
			} 
			else 
				{
				int curX = handle->getX();
				int curY = handle->getY();
				int prevX = envPath.getCurrentPosition().getX();
				int prevY = envPath.getCurrentPosition().getY();
				//int controlPoint1
				if(curY>prevY){
					if((int)handle->getProperties().getWithDefault("curveType", 0)==CONVEX)
					envPath.quadraticTo(prevX+(curX/3), curY, 
									(curX + handle->getRight()) / 2,
									(curY + handle->getBottom()) / 2);
					else if((int)handle->getProperties().getWithDefault("curveType", 0)==CONCAVE)
					envPath.quadraticTo(curX-(curX/3), prevY, 
									(curX + handle->getRight()) / 2,
									(curY + handle->getBottom()) / 2);
				}
				else if(curY<prevY)
					if((int)handle->getProperties().getWithDefault("curveType", 0)==CONVEX)
					envPath.quadraticTo(curX-(curX/3.f), prevY,
								(curX + handle->getRight()) / 2,
								(curY + handle->getBottom()) / 2);
					else if((int)handle->getProperties().getWithDefault("curveType", 0)==CONCAVE)
					envPath.quadraticTo(prevX+(prevX/3.f), curY,
								(curX + handle->getRight()) / 2,
								(curY + handle->getBottom()) / 2);						
				}
			}
		
		//String coordinate;
		//coordinate = String(handle->getX()) + String(", ") + String(handle->getY());
		//g.setColour(Colours::lime);
		//g.drawFittedText(coordinate, handle->getX(), handle->getY(), 50, 20, Justification::centred, 1); 
		
		//envPath.lineTo(getWidth(), getHeight());
		//envPath.lineTo(0, getHeight());
		//envPath.closeSubPath();
		//g.setColour(currColour.darker(.7f));
		//if(toggleMaxMin)
		//	int test;
		//else if((drawHorizontalSegments==true && fixedEnvelope == false))
		//g.fillPath(envPath);
		//else
		if(!drawFill)
		g.strokePath (envPath, PathStrokeType(2.0f));
		}
	}
	
	//if current table is on top, place text on widget to indicate so..
	/*
	if(onTop){
	String text = "ftable:"+String(tableNumber);
	g.setColour(currColour);
	g.setFont(CabbageUtils::getComponentFont());
	int length = CabbageUtils::getComponentFont().getStringWidth(text)+10;
	g.drawText(text, getWidth()-length, 
						   2, 
						   length, 12, 
						   Justification::centred, 1);
	}*/
	
	//draw the crubber if needed
	if(scrubberPosition>0){
	g.setColour(currColour);
	scrubberPosition = scrubberPosition*getWidth();
	g.drawLine(scrubberPosition, 0, scrubberPosition, getHeight(), 2);
	}
}

//====================================================
void Table::mouseDown (const MouseEvent& e)
{
	/* With shift button down a new envelope handle is added. No zoom. */
	if(editMode==true)
	if(fixedEnvelope==false)
	if(e.mods.isShiftDown() == true)
		draggingHandle = addHandle (e.x, e.y, false, handleWidth, activeColour.contrasting(.5));

}

//====================================================
void Table::applyZoom (int zoomInput)
{
	zoom = zoomInput;
	setDataSource(zoom);
	this->repaint();
}

//====================================================
void Table::setToEnabled(bool isEnabled)
{
	isCurrentlyOnTop = isEnabled;

	if (isCurrentlyOnTop) //if on top
		minWaveHeight = 3;
	else 
		minWaveHeight = 1.5;
}

//====================================================
void Table::setViewStart(float x)
{
	viewX = x;
}

//====================================================
void Table::setViewWidth(float width)
{
	viewWidth = width;
}

//====================================================
void Table::modifyHandlePos (float j)
{
	//This function changes the x position of each envelope handle
	//after a zoom in or out. j will either be 2 or 0.5.
	for (int i=0; i<handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked (i);
		handle->setBounds (handle->getX() * j, handle->getY(), HANDLESIZE, HANDLESIZE);
	}
}

//====================================================
void Table::createHandlesFromTable(int points)
{
	handles.clear();
	Colour col;
	editMode=true;
	int x;
	float end = getWidth()*.99;
	float scaleX = .99;

	if(points>tableSize){
		points=tableSize;
		fixedEnvelope = true;
	}

	float segmentIncr = (float)tableSize/(float)points;
	if(fixedEnvelope && drawHorizontalSegments){
		handleWidth = getWidth()/tableSize;
		col = activeColour;
		scaleX = 1;
		}
	else
		col = activeColour.contrasting(.5);

	addHandle(-4, convertAmpToPixel(tableData.amps[0]), true, handleWidth, col); 
	
	for(int i=segmentIncr;i<tableSize;i+=segmentIncr){
			int x = int((float(i)/(float)tableSize)*getWidth()*scaleX);
			addHandle(x, convertAmpToPixel(tableData.amps[i]), (i==0 || i==tableSize, fixedEnvelope), handleWidth, col);
		}
	addHandle(end, convertAmpToPixel(tableData.amps[0]), true, handleWidth, col);
}
//====================================================
void Table::toggleMinMaxAmp(int x)
{
for(int i=0;i<handles.size();i++)
		if((x> handles[i]->getX()) && (x<handles[i]->getRight())){
			if(handles[i]->getPosition().getY()>1)
			handles[i]->setTopLeftPosition(handles[i]->getX(), 0);
			else
			handles[i]->setTopLeftPosition(handles[i]->getX(), getHeight());
			
			repaint();
		}
}
//====================================================
void Table::setYValueOfHandle(int x, int y)
{
for(int i=0;i<handles.size();i++)
		if((x>= handles[i]->getX()) && (x<=handles[i]->getRight())){
			handles[i]->setTopLeftPosition(handles[i]->getX(), y);
			repaint();
		}
}

//====================================================
// delete handles if user selects delete from handle popup menu
//====================================================
void Table::changeListenerCallback(juce::ChangeBroadcaster *source)
{
CabbageEnvelopeHandleComponent* hand = dynamic_cast <CabbageEnvelopeHandleComponent*> (source);
if(hand){
	//PathFlatteningIterator flatpath(envPath);
	Logger::writeToLog("Table Length in Pixels:"+String(getWidth()));
	Logger::writeToLog("Table Length in samples:"+String(tableSize));
	//while(flatpath.next())
		//Logger::writeToLog("Next pos:"+String(flatpath.x2));
	
	
	if(hand->changeMessage=="removeHandle"){
		removeHandle(hand);
		sendChangeMessage();
		}
		
	else if(hand->changeMessage=="updateOnMouseUp"){
		sendChangeMessage();
		}
	}
	//change messages being sent from this component
	else
	sendChangeMessage();		
}



CabbageEnvelopeHandleComponent* Table::addHandle(int x, int y, bool fixedPos, int width, Colour col)
{
	int i;
	for (i=0; i<handles.size(); i++) {
		CabbageEnvelopeHandleComponent* handle = handles.getUnchecked (i);
		if (handle->getX() > x)
			break;
	}
	
	CabbageEnvelopeHandleComponent* handle = new CabbageEnvelopeHandleComponent(handles.size(), col, fixedEnvelope);
	handle->addChangeListener(this);
	handle->getProperties().set("fixedPos", fixedPos);
	handle->height = getHeight();
	handle->width = getWidth();
	//if(this->toggleMaxMin)
	//handle->setBounds (x, y, width, 3);
	//else
	handle->setBounds (x, y, width, HANDLESIZE);
	addAndMakeVisible (handle);
	handles.insert (i, handle);
	repaint();
	return handle;
}

int Table::getHandleIndex(CabbageEnvelopeHandleComponent* thisHandle)
{
	return handles.indexOf(thisHandle);
}

CabbageEnvelopeHandleComponent* Table::getPreviousHandle(CabbageEnvelopeHandleComponent* thisHandle)
{
	int thisHandleIndex = handles.indexOf(thisHandle);
	
	if(thisHandleIndex <= 0) 
		return 0;
	else 
		return handles.getUnchecked(thisHandleIndex-1);
}

CabbageEnvelopeHandleComponent* Table::getNextHandle(CabbageEnvelopeHandleComponent* thisHandle)
{
	int thisHandleIndex = handles.indexOf(thisHandle);
	
	if(thisHandleIndex == -1 || thisHandleIndex >= handles.size()-1) 
		return 0;
	else
		return handles.getUnchecked(thisHandleIndex+1);
}

void Table::removeHandle (CabbageEnvelopeHandleComponent* thisHandle)
{
	if (handles.size() > 0) {
		handles.removeObject(thisHandle, true);
		repaint();
	}
}


/*
  ====================================================================================

	Table Manager class

  ====================================================================================
*/
CabbageTableManager::CabbageTableManager(int tableSize)
	: alpha(1.0f), zoom(1), maxZoom(1), maxNumPixelsPerIndex(1),
	globalMaxAmp(0.f), globalMinAmp(0.f), tableSize(tableSize), readOnly(false)
{
}

CabbageTableManager::~CabbageTableManager()
{
	tables.clear();
}

void CabbageTableManager::resized()
{
	//We need to make room for the h scrollbar, therefore table data
	//can't use the full height of the canvas
	tableTop = getHeight()*0.15;
	tableBottom = getHeight()*0.85;
	tableHeight = tableBottom - tableTop;
}

void CabbageTableManager::setOriginalWidth(float width)
{
	originalWidth = width;
}

Table* CabbageTableManager::getTable(int index)
{
	if(index > 0 && index < tables.size());
	return tables[index];
}


void CabbageTableManager::paint(Graphics& g)
{
	g.setColour(CabbageUtils::getDarkerBackgroundSkin());
	g.fillAll();

	// Amp horizontal markers...
	if(getHeight()>40){
	g.setColour (Colour::fromRGBA (220, 220, 240, 255));
	g.drawLine (0, convertAmpToPixel(globalMaxAmp), getWidth(), convertAmpToPixel(globalMaxAmp), 0.1);
	g.drawLine (0, convertAmpToPixel(globalMinAmp), getWidth(), convertAmpToPixel(globalMinAmp), 0.1);
	g.drawLine (0, convertAmpToPixel(globalMaxAmp*0.5), getWidth(), convertAmpToPixel(globalMaxAmp*0.5), 0.1);
	g.drawLine (0, convertAmpToPixel(globalMinAmp*0.5), getWidth(), convertAmpToPixel(globalMinAmp*0.5), 0.1);
	if (globalMinAmp < 0) 
		g.drawLine (0, convertAmpToPixel(0), getWidth(), convertAmpToPixel(0), 0.3);
	}
	else{
	float incr = (float)getWidth()/(float)tableSize;
	g.setColour (Colour::fromRGBA (220, 220, 240, 255));
	for(int i=0;i<getWidth();i+=(incr))	
			g.drawLine(i, 0, i, getHeight()-.1, 0.1);
	g.drawLine (0, getHeight()-.1, getWidth(), getHeight()-.1, 0.1);
	g.drawLine (0, 0, getWidth(), 0, 0.1);		
		
	}
	// update tables
	for (int i=0; i<tables.size(); ++i) 
		tables[i]->setViewStart(0);
	
	
}

float CabbageTableManager::convertAmpToPixel (float ampValue)
{
	// This method converts amps to y pixel values
	float normalisedAmp = (ampValue-globalMinAmp) / globalAmpRange; //first change to normalised value
	return jmax(0.f, ((1-normalisedAmp) * tableHeight) + tableTop);
}

void CabbageTableManager::addTable (String name, 
									String channel, 
									int tableNumber, 
									int tableSize, 
									bool fixedEnv, 
									bool drawHoriz, 
									bool drawOrig, 
									bool toggleMaxMin, 
									bool drawFill,
									int resizeMode,
									Point<float> minMax,
									Colour colour, 
									bool readonly,
									ChangeListener* listener)
{
	int i = tables.size();
	tables.add (new Table(channel, tableNumber, tableSize, fixedEnv, drawHoriz, drawOrig, colour));
	tables[i]->setBounds (0, 0, getWidth(), getHeight());
	tables[i]->addChangeListener(listener);
	tables[i]->setOriginalWidth (getWidth());
	tables[i]->setViewWidth(getWidth());
	tables[i]->toggleMaxMin = toggleMaxMin;
	tables[i]->drawFill = drawFill;
	readOnly = readonly;
	addAndMakeVisible (tables[i]);
	tables[i]->addMouseListener(this, false); //"this" will now also get mouse events for tables[i]
	tableToTop(i); //setting this table to the top
	
	
	Logger::writeToLog("MinRange"+String(minMax.getX()));
	Logger::writeToLog("MaxRange"+String(minMax.getY()));
	
	if(resizeMode==1)
		tables[i]->isResizing = true;
	
	if(minMax.getX()!=minMax.getY())
		tables[i]->useAmpRanges=true;
		
	tables[i]->minMaxAmps = minMax;
	
	//the max possible zoom is based on the table with the most indices. It will have a max zoom
	//of 50 pixels for every index.
	if ((tableSize*50) > maxNumPixelsPerIndex){
		maxNumPixelsPerIndex = (tableSize*50);
		maxZoom = sqrt(maxNumPixelsPerIndex);
	}
}

void CabbageTableManager::tableToTop (int tableIndex)
{
	jassert(tables[tableIndex])
	tables[tableIndex]->toFront(true);
	
	Logger::writeToLog("Table on top is"+String(tables[tableIndex]->tableNumber));
	
	for(int i=0;i<tables.size();i++){
		tables[i]->setToEnabled(false); //disabling the previously active table
		tables[i]->setAlpha(0.7);		
	}
	
	if (tables.size() > 1) {
		tables.swap(tableIndex, tables.size()-1); //swapping currently active table to end of array
	}

	tables.getLast()->setToEnabled(true);
	tables.getLast()->setAlpha(1.0);

}

void CabbageTableManager::fillTable (int tableIndex, Array<double, CriticalSection> csndInputData)
{
	if(isPositiveAndBelow(tableIndex, tables.size()))
	{
	float currTableMaxAmp, currTableMinAmp;
	if(tables[tableIndex]->isResizing==0)
	globalMaxAmp = globalMinAmp = 0;
	
	// Getting the min and max amplitude values....
	currTableMinAmp = currTableMaxAmp = csndInputData.getFirst();
	for (int i=0; i<csndInputData.size(); ++i) {
		//Logger::writeToLog(String(csndInputData[i]));
		if (csndInputData[i] > currTableMaxAmp)
			currTableMaxAmp = csndInputData[i];
		if (csndInputData[i] < currTableMinAmp)
			currTableMinAmp = csndInputData[i];
	}


	//if min and max amps are the same value....
	if (currTableMinAmp == currTableMaxAmp) {
		if (currTableMinAmp > 0)
			currTableMinAmp = 0;
		else if (currTableMinAmp < 0) {
			currTableMaxAmp = 0;
		}
		else { //else if both are 0
			currTableMinAmp = -1;
			currTableMaxAmp = 1;
		}
	}

	if (currTableMaxAmp > globalMaxAmp)
		globalMaxAmp = currTableMaxAmp;
	if (currTableMinAmp < globalMinAmp)
		globalMinAmp = currTableMinAmp;

	if(tables[tableIndex]->useAmpRanges){
			globalMinAmp = tables[tableIndex]->minMaxAmps.getX();
			globalMaxAmp = tables[tableIndex]->minMaxAmps.getY();
	}

	globalAmpRange = globalMaxAmp - globalMinAmp; 
	
	for (int i=0; i<tables.size(); ++i)
		tables[i]->setGlobalAmpRange(globalMaxAmp, globalMinAmp, globalAmpRange);

	tables[tableIndex]->createAmpOverviews(csndInputData);
	
	if(tables[tableIndex]->drawHorizontalSegments &&
		tables[tableIndex]->fixedEnvelope &&
		tables[tableIndex]->toggleMaxMin){
		tables[tableIndex]->createHandlesFromTable(32);
		tables[tableIndex]->setInterceptsMouseClicks(false,false);
	}	
	
	}
	//Logger::writeToLog("MaxAmp="+String(globalMaxAmp));
	//Logger::writeToLog("MinAmp="+String(globalMinAmp));
	
}

void CabbageTableManager::mouseDrag(const MouseEvent& e)
{
	
}

void CabbageTableManager::mouseDown (const MouseEvent& e)
{	
	if(e.mods.isRightButtonDown())
		{
		
		PopupMenu pop, subMenu1, subMenu2;
		pop.setLookAndFeel(&getTopLevelComponent()->getLookAndFeel());
		subMenu1.setLookAndFeel(&getTopLevelComponent()->getLookAndFeel());
		subMenu2.setLookAndFeel(&getTopLevelComponent()->getLookAndFeel());

		subMenu1.addItem(101, "1 segment");
		subMenu1.addItem(102, "2 segments");
		subMenu1.addItem(104, "4 segments");
		subMenu1.addItem(108, "8 segments");
		subMenu1.addItem(112, "12 segments");
		subMenu1.addItem(116, "16 segments");
		subMenu1.addItem(120, "20 segments");
		subMenu1.addItem(124, "24 segments");
		subMenu1.addItem(128, "28 segments");
		subMenu1.addItem(132, "32 segments");


		
		for(int i=0;i<tables.size();i++){
		subMenu2.addColouredItem(200+i, "fTable:"+String(tables[i]->tableNumber), tables[i]->currColour);
		
		if(!readOnly){
			pop.addSubMenu(TRANS("Edit..."), subMenu1);			
			pop.addSubMenu(TRANS("Table to front..."), subMenu2);
			pop.addItem(300, "Replace existing table");
			pop.addItem(301, "Add table to score");
			}
		}
		
		int choice = pop.show();
		if((choice>=100) && (choice<200)){
			for(int i=0;i<tables.size();i++){
				if(tables[i]->isCurrentlyOnTop){
					//Logger::writeToLog("currentOnTop:"+String(tables[i]->tableNumber));
					tables[i]->createHandlesFromTable(choice-100);
					tables[i]->drawOriginalTableData = false;
				}
			}
		}
		else if(choice>=200 && choice<300){
			this->tableToTop(choice-200);						
		}
		else if(choice==300)
			{
			for(int i=0;i<tables.size();i++)
				{
				if(tables[i]->isCurrentlyOnTop)
					{
					tables[i]->changeMessage = "overwriteFunctionTable";
					tables[i]->sendChangeMessage();
					}			
				}
			}
		else if(choice==301)
			{
			for(int i=0;i<tables.size();i++)
				{
				if(tables[i]->isCurrentlyOnTop)
					{
					tables[i]->changeMessage = "writeNewFunctionTable";
					tables[i]->sendChangeMessage();
					}			
				}
			}
	}
		//if left button is pressed..
		else{
				for(int i=0;i<tables.size();i++)
					{
					if(tables[i]->isCurrentlyOnTop)
						{
						if(tables[i]->toggleMaxMin)
							{
							tables[i]->toggleMinMaxAmp(e.getMouseDownX());
							tables[i]->changeMessage = "updateFunctionTable";
							tables[i]->sendChangeMessage();
							}
						//tables[i]->setYValueOfHandle(e.getMouseDownX(), e.getMouseDownY());
						}
					}
			
			}
				
}


	








