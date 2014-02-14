#ifndef __CABBAGELOOKANDFEEL_H_
#define __CABBAGELOOKANDFEEL_H_

/*
  ====================================================================================

	Cabbage Look And Feel Class

  ====================================================================================
*/

#include "JuceHeader.h"
#include "CabbageUtils.h"


class CabbageLookAndFeel	:	public LookAndFeel_V2
{
public:
	CabbageLookAndFeel();
	~CabbageLookAndFeel();

	
	Image drawRotaryImage(int diameter, const Colour circleFill, const Colour trackerCol, float sliderPosProportional, float zeroPosProportional,
																								bool useTrackerFill,
																								float markerOpacity);
	Image drawLinearBgImage (float width, float height, float sliderPosProportional, float zeroPosProportional, 
																								bool useTrackerFill,
																								bool isVertical,
																								const Colour trackerCol);
																								 
	void drawTableHeaderColumn (Graphics& g, const String& columnName, int /*columnId*/,
                                         int width, int height,
                                         bool isMouseOver, bool isMouseDown,
                                         int columnFlags);
	void drawTableHeaderBackground (Graphics& g, TableHeaderComponent& header);
	Image drawLinearThumbImage (float width, float height, const Colour thumbFill, bool isVertical);
	static Image drawToggleImage (float width, float height, bool isToggleOn, Colour colour, bool isRect);
	Image drawTextButtonImage (float width, float height, bool isButtonDown, Colour colour);
	virtual void drawRotarySlider (Graphics &g, int x, int y, int width, int height, float sliderPosProportional, 
																					float rotaryStartAngle, 
																					float rotaryEndAngle, 
																					Slider &slider);
	virtual void drawLinearSliderBackground (Graphics &g, int x, int y, int width, int height, 
																					float sliderPos, 
																					float minSliderPos, 
																					float maxSliderPos, 
																					const Slider::SliderStyle style, 
																					Slider &slider);
	virtual void drawLinearSliderThumb (Graphics &g, int x, int y, int width, int height, float sliderPos, 
																						float minSliderPos, 
																						float maxSliderPos, 
																						const Slider::SliderStyle style, 
																						Slider &slider);
	virtual void drawButtonBackground (Graphics&, Button&, const Colour&, bool, bool);
	virtual void drawButtonText (Graphics &g, TextButton &button, bool isMouseOverButton, bool isButtonDown);
									  
	virtual void drawLabel (Graphics &g, Label &label);
	virtual void drawComboBox (Graphics&, int, int, bool, int, int, int, int, ComboBox&);
	virtual void drawToggleButton (Graphics &g, ToggleButton &button, bool isMouseOverButton, bool isButtonDown);
	virtual void drawTextEditorOutline (Graphics &g, int width, int height, TextEditor &textEditor);
	virtual void fillTextEditorBackground (Graphics &g, int width, int height, TextEditor &textEditor);
	virtual void drawGroupComponentOutline (Graphics &g, int w, int h, const String &text, const Justification &position, 
																									GroupComponent &group);
	virtual int getDefaultScrollbarWidth();
	virtual void drawScrollbar (Graphics &g, ScrollBar &scrollbar, int x, int y, int width, int height, 
																							bool isScrollbarVertical, 
																							int thumbStartPosition, 
																							int thumbSize, 
																							bool isMouseOver, 
																							bool isMouseDown);
	virtual void drawScrollbarButton (Graphics &g, ScrollBar &scrollbar, int width, int height, int buttonDirection, 
																								bool isScrollbarVertical, 
																								bool isMouseOverButton, 
																								bool isButtonDown);

	void setDefaultSansSerifTypefaceName (const String &newName);
	virtual void drawMenuBarBackground(Graphics &g, int width, int height, bool isMouseOverBar, MenuBarComponent &menuBar);   
    virtual void drawMenuBarItem(Graphics & g, int width, int height, int itemIndex,
                                      const String &itemText, bool isMouseOverItem,
                                      bool isMenuOpen, bool isMouseOverBar,
                                      MenuBarComponent &menuBar);

	void drawPopupMenuBackground(Graphics &g, int width, int height);
	Image drawCheckMark(); 
	virtual void drawPopupMenuItem (Graphics &g, int width, int height, bool isSeparator, bool isActive, bool isHighlighted, 
																											bool isTicked, 
																											bool hasSubMenu, 
																											const String &text, 
																											const String &shortcutKeyText, 
																											Image *image, 
																											const Colour *const textColour);
	virtual void drawDocumentWindowTitleBar (DocumentWindow &window, Graphics &g, int w, int h, int titleSpaceX, int titleSpaceW, 
																											const Image *icon, 
																											bool drawTitleTextOnLeft);
	Image drawWindowButtonNormal(int buttonType);
	Image drawWindowButtonIsOver(int buttonType);
	virtual Button* createDocumentWindowButton (int buttonType);

	int getAlertBoxWindowFlags();
	int getAlertWindowButtonHeight();
	Font getAlertWindowMessageFont();
	Font getAlertWindowFont();
	void drawAlertBox (Graphics& g, AlertWindow& alert, const Rectangle<int>& textArea, TextLayout& textLayout);

	AlertWindow* createAlertWindow (const String& title, const String& message, const String& button1, const String& button2,
                                             const String& button3, AlertWindow::AlertIconType iconType, int numButtons,
                                             Component* associatedComponent);
	
	int getTabButtonSpaceAroundImage();
    int getTabButtonOverlap (int tabDepth);
    int getTabButtonBestWidth (TabBarButton&, int tabDepth);
    Rectangle<int> getTabButtonExtraComponentBounds (const TabBarButton&, Rectangle<int>& textArea, Component& extraComp);

    void drawTabButton (TabBarButton&, Graphics& g, bool isMouseOver, bool isMouseDown);
    void drawTabButtonText (TabBarButton&, Graphics& g, bool isMouseOver, bool isMouseDown);
    void drawTabAreaBehindFrontButton (TabbedButtonBar&, Graphics& g, int w, int h);

    void createTabButtonShape (TabBarButton&, Path& path,  bool isMouseOver, bool isMouseDown);
    void fillTabButtonShape (TabBarButton&, Graphics& g, const Path& path, bool isMouseOver, bool isMouseDown);

    void drawFileBrowserRow (Graphics& g, int width, int height,
                                     const String& filename, Image* icon,
                                     const String& fileSizeDescription,
                                     const String& fileTimeDescription,
                                     bool isDirectory,
                                     bool isItemSelected,
                                     int itemIndex,
                                     DirectoryContentsDisplayComponent& component);


	void drawTreeviewPlusMinusBox (Graphics& g, int x, int y, int w, int h, bool isPlus, bool /*isMouseOver*/);
    void drawLevelMeter(Graphics &g, int width, int	height,	float level);
	Button* createTabBarExtrasButton();
	
	juce_UseDebuggingNewOperator


private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLookAndFeel);
	
	
};


/*
  ====================================================================================

	Alternative Look And Feel Class

  ====================================================================================
*/
class CabbageLookAndFeelBasic	:	public LookAndFeel_V2
{
public:
	CabbageLookAndFeelBasic();
	~CabbageLookAndFeelBasic();
	
	void drawLabel(Graphics&, Label&);
	virtual void drawLinearSliderBackground (Graphics &g, int x, int y, int width, int height, 
																					float sliderPos, 
																					float minSliderPos, 
																					float maxSliderPos, 
																					const Slider::SliderStyle style, 
																					Slider &slider);
	virtual void drawLinearSliderThumb (Graphics &g, int x, int y, int width, int height, float sliderPos, 
																						float minSliderPos, 
																						float maxSliderPos, 
																						const Slider::SliderStyle style, 
																						Slider &slider);
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown);

	virtual void drawScrollbar (Graphics &g, ScrollBar &scrollbar, int x, int y, int width, int height, 
																							bool isScrollbarVertical, 
																							int thumbStartPosition, 
																							int thumbSize, 
																							bool isMouseOver, 
																							bool isMouseDown);
	void drawDocumentWindowTitleBar (DocumentWindow &window, Graphics &g, int w, int h, 
																						int /*titleSpaceX*/, 
																						int titleSpaceW, 
																						const Image */*icon*/, 
		
																				bool /*drawTitleTextOnLeft*/);

	void drawResizableWindowBorder (Graphics &g, int w, int h, const BorderSize< int > &border, ResizableWindow &window);
	void drawPopupMenuBackground(Graphics &g, int width, int height);
	void drawMenuBarBackground(Graphics &g, int width, int height, bool isMouseOverBar, MenuBarComponent &menuBar);
	Image drawCheckMark();
	void drawMenuBarItem(Graphics & g, int width, int height, int itemIndex,
                                                                              const String &itemText, 
																			  bool isMouseOverItem,
                                                                              bool isMenuOpen, 
																			  bool isMouseOverBar,
                                                                              MenuBarComponent &menuBar);
	
	void drawPopupMenuItem (Graphics &g, int width, int height, bool isSeparator, bool /*isActive*/, 
																					bool isHighlighted, 
																					bool isTicked, 
																					bool hasSubMenu, 
																					const String &text, 
																					const String &shortcutKeyText, 
																					Image */*image*/, 
																					const Colour */*const textColour*/);

	
	juce_UseDebuggingNewOperator


private:

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageLookAndFeelBasic);
	
};


#endif //__CABBAGELOOKANDFEEL_H_
