#ifndef __CABBAGETABLE_H_
#define __CABBAGETABLE_H_

#include "JuceHeader.h"
#include "CabbageUtils.h"

#define HANDLESIZE 8
#define LINEAR 0
#define CONVEX 1
#define CONCAVE 2

/*
  ====================================================================================

	Cabbage Envelope Handle
	Each envelope handle is a component.

  ====================================================================================
*/
class CabbageEnvelopeHandleComponent : public Component,
	public ChangeBroadcaster
{
public:
	CabbageEnvelopeHandleComponent(int index, Colour colour, bool fixed);
	~CabbageEnvelopeHandleComponent();

	class Table* getParentComponent();
	void paint (Graphics& g);
	void removeThisHandle();
	void mouseEnter (const MouseEvent& e);
	void mouseDown (const MouseEvent& e);
	void mouseDrag (const MouseEvent& e);
	void mouseUp (const MouseEvent& e);
	int index;
	int height, width;
	int x,y;

	CabbageEnvelopeHandleComponent* getPreviousHandle();
	CabbageEnvelopeHandleComponent* getNextHandle();

	String changeMessage;

private:
	Colour colour;
	bool fixed;
	ComponentDragger dragger;
	int lastX, lastY;
	int offsetX, offsetY;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageEnvelopeHandleComponent);
};


/*
  ====================================================================================

	Data Structures to hold table data

  ====================================================================================
*/
class TableData
{
public:
	Array<double, CriticalSection> amps, x, y;
};

class OverviewData
{
public:
	Array<double> minAmps, maxAmps, minY, maxY;
};

/*
  ====================================================================================

	Table

  ====================================================================================
*/
class Table : public Component,
	public ChangeBroadcaster,
	public ChangeListener
{
public:
	Table (String chan, int tblNumber, int tableSize, bool fixedEnv, bool drawHorizontal, bool drawOriginal, Colour colour);
	~Table();

	void resized();
	void setOriginalWidth(int w);
	void setGlobalAmpRange (float globalMax, float globalMin, float globalRange);
	void createAmpOverviews (Array<double, CriticalSection> csndInputData);
	void setDataSource (int zoomValue);
	float convertAmpToPixel (float ampValue);
	float convertPixelToAmp(float pixelYValue);
	void cacheBackgroundImage();
	void paint (Graphics& g);
	void mouseDown (const MouseEvent& e);
	void applyZoom(int newZoom);
	void setToEnabled(bool isEnabled);
	void setViewWidth(float width);
	void setViewStart(float x);
	CabbageEnvelopeHandleComponent* addHandle(int x, int y, bool fixedPos, int width, Colour col);
	CabbageEnvelopeHandleComponent* getPreviousHandle(CabbageEnvelopeHandleComponent* thisHandle);
	CabbageEnvelopeHandleComponent* getNextHandle(CabbageEnvelopeHandleComponent* thisHandle);
	int getHandleIndex(CabbageEnvelopeHandleComponent* thisHandle);
	void removeHandle (CabbageEnvelopeHandleComponent* thisHandle);
	void modifyHandlePos (float j);
	void makeTableEditable();
	void createHandlesFromTable(int points);
	void changeListenerCallback(juce::ChangeBroadcaster *source);
	TableData tableData;
	float scrubberPosition;
	String channel;
	int tableNumber, tableSize;
	Path envPath;
	bool drawOriginalTableData, isCurrentlyOnTop, drawFill,
	     drawHorizontalSegments, fixedEnvelope, toggleMaxMin;
	OwnedArray<CabbageEnvelopeHandleComponent, CriticalSection> handles;
	void setYValueOfHandle(int x, int y);
	void toggleMinMaxAmp(int x);
	float minAmp, maxAmp;
	String changeMessage;
	String currentfStatement;
	float ampRange, zeroAmpPosition;
	Colour currColour;
	bool isResizing;
	bool useAmpRanges;
	Point<float> minMaxAmps;
	bool editMode;
	
private:
	Viewport* viewport;

	Image img;
	int gen;
	int handleWidth;
	float tableTop, tableBottom, tableLeft, tableHeight;
	float zoom;
	float viewX, viewWidth;

	OverviewData overview;
	Colour activeColour;
	int origWidth;
	bool useOverview;
	float maxZoomForOverview, numPixelsPerIndex;
	float minWaveHeight;

	CabbageEnvelopeHandleComponent* draggingHandle;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Table);
};

/*
  ====================================================================================

	Table Manager

	This class superimposes tables on top of one another, turning them into one
	component

  ====================================================================================
*/
class CabbageTableManager : public Component
{
public:
	CabbageTableManager(int tableSize);
	~CabbageTableManager();
	void resized();
	void setOriginalWidth(float width);
	void paint(Graphics& g);
	float convertAmpToPixel (float ampValue);
	void addTable (String name, String channel, int tableNumber,
	               int tableSize,
	               bool fixedEnv,
	               bool drawHoriz,
	               bool drawOrig,
	               bool toggleMaxMin,
	               bool drawFill,
				   int resizeMode,
				   Point<float> maxMin,
	               Colour colour,
				   bool readOnly,
	               ChangeListener* listener);
	void fillTable (int tableID, Array<double, CriticalSection> csndInputData);
	void tableToTop (int tableOnTop);
	void mouseDown (const MouseEvent& e);
	void mouseDrag(const MouseEvent& e);
	Table* getTable(int index);
	
	int getNumberOfTables(){
	return tables.size();	
	}

private:
	OwnedArray<Table, CriticalSection> tables;
	float originalWidth;
	float alpha;
	bool readOnly;
	int zoom, maxZoom;
	float globalMaxAmp, globalMinAmp, globalAmpRange;
	float maxNumPixelsPerIndex;
	int currentTableIndex;
	int tableSize;
	float tableTop, tableBottom, tableHeight;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTableManager);
};


/*
  ====================================================================================

	Table Viewer

	This class provides a viewport which allows for scrolling. It views one
	CabbageTableManager object.

  ====================================================================================
*/
class CabbageTableViewer : public Viewport
	//public Component
{
public:
	CabbageTableViewer();
	~CabbageTableViewer();
	void resized();
	void addTable (String name, int tableSize, Colour colour, float alpha);
	void fillTable (int tableIndex, Array<float> csndInputData);
	void tableToTop (int tableIndex);
	void setScrubberPosition(int tableIndex, float position);

private:
	ScopedPointer<CabbageTableManager> tableManager;
	//OwnedArray<Table> tables;
	//float tableOpacity;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageTableViewer);
};


#endif //__CABBAGETABLE_H_
