#include "CabbageLookAndFeel.h"


CabbageLookAndFeel::CabbageLookAndFeel()
{
	setColour(AlertWindow::backgroundColourId, CabbageUtils::getDarkerBackgroundSkin());
	setColour(AlertWindow::textColourId, Colour(200, 200, 200));
	setColour(AlertWindow::outlineColourId, Colours::white);
	setColour(DirectoryContentsDisplayComponent::textColourId, Colours::whitesmoke);
	setColour(DirectoryContentsDisplayComponent::highlightColourId, Colours::red);
	
}

CabbageLookAndFeel::~CabbageLookAndFeel()
{
}

//==============================================================================

void CabbageLookAndFeel::drawLevelMeter(Graphics &g, int width, int	height,	float level)
{
    g.setColour (Colours::white.withAlpha (0.7f));
    g.fillRoundedRectangle (0.0f, 0.0f, (float) width, (float) height, 3.0f);
    g.setColour (Colours::black.withAlpha (0.2f));
    g.drawRoundedRectangle (1.0f, 1.0f, width - 2.0f, height - 2.0f, 3.0f, 1.0f);

    const int totalBlocks = 15;
    const int numBlocks = roundToInt (totalBlocks * level);
    const float w = (width - 6.0f) / (float) totalBlocks;

    for (int i = 0; i < totalBlocks; ++i)
    {
        if (i >= numBlocks)
            g.setColour (Colours::lightblue.withAlpha (0.6f));
        else
            g.setColour (i < totalBlocks - 1 ? Colours::cornflowerblue
                                             : Colours::red);

        g.fillRoundedRectangle (3.0f + i * w + w * 0.1f, 3.0f, w * 0.8f, height - 6.0f, w * 0.4f);
    }


} 	

//==============================================================================
void CabbageLookAndFeel::drawTreeviewPlusMinusBox (Graphics& g, int x, int y, int w, int h, bool isPlus, bool /*isMouseOver*/)
{
    const int boxSize = ((jmin (16, w, h) << 1) / 3) | 1;

    x += (w - boxSize) >> 1;
    y += (h - boxSize) >> 1;
    w = boxSize;
    h = boxSize;

    //g.setColour (Colour (0xe5ffffff));
    g.setColour(Colours::black);//background
    g.fillRect (x, y, w, h);

    //g.setColour (Colour (0x80000000));
    g.setColour(Colours::cornflowerblue);//outline
    g.drawRect (x, y, w, h);

	//g.setColour(Colours::lime);
	
    const float size = boxSize / 2 + 1.0f;
    const float centre = (float) (boxSize / 2);
    //plus sign
    g.setColour(Colours::green);

    g.fillRect (x + (w - size) * 0.5f, y + centre, size, 1.0f);
    
    if (isPlus){
        g.fillRect (x + centre, y + (h - size) * 0.5f, 1.0f, size);
	
	}
}

//draw file borwser rows
void CabbageLookAndFeel::drawFileBrowserRow (Graphics& g, int width, int height,
                                      const String& filename, Image* icon,
                                      const String& fileSizeDescription,
                                      const String& fileTimeDescription,
                                      const bool isDirectory,
                                      const bool isItemSelected,
                                      const int /*itemIndex*/,
                                      DirectoryContentsDisplayComponent&)
{
    if (isItemSelected)
        //g.fillAll (findColour (DirectoryContentsDisplayComponent::highlightColourId));
		g.fillAll(Colours::cornflowerblue.withAlpha(.2f));

    const int x = 32;
    g.setColour (Colours::black);

    if (icon != nullptr && icon->isValid())
    {
        g.drawImageWithin (*icon, 2, 2, x - 4, height - 4,
                           RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize,
                           false);
    }
    else
    {
		//g.fillAll(Colours::black);
		Path path;
		//draw custom folder icon, netter yet, just import a binary..
		path.addLineSegment(Line<float> (3, 3, 13, 3), 2);
		path.addLineSegment(Line<float> (13, 3, 15, 10), 2);
		path.addLineSegment(Line<float> (15, 10, 26, 10), 2);
		path.addLineSegment(Line<float> (26, 10, 26, 20), 2);
		path.addLineSegment(Line<float> (26, 20, 3, 20), 2);
		path.addLineSegment(Line<float> (3, 20, 3, 3), 2);
		g.setColour(Colours::cornflowerblue.withAlpha(.4f));
		PathStrokeType stroke(.01f);
		g.fillPath(path);
  }

    g.setColour (findColour (DirectoryContentsDisplayComponent::textColourId));
    g.setFont (height * 0.7f);

    if (width > 450 && ! isDirectory)
    {
        const int sizeX = roundToInt (width * 0.7f);
        const int dateX = roundToInt (width * 0.8f);

        g.drawFittedText (filename,
                          x, 0, sizeX - x, height,
                          Justification::centredLeft, 1);

        g.setFont (height * 0.5f);
        g.setColour (Colours::darkgrey);

        if (! isDirectory)
        {
            g.drawFittedText (fileSizeDescription,
                              sizeX, 0, dateX - sizeX - 8, height,
                              Justification::centredRight, 1);

            g.drawFittedText (fileTimeDescription,
                              dateX, 0, width - 8 - dateX, height,
                              Justification::centredRight, 1);
        }
    }
    else
    {
        g.drawFittedText (filename,
                          x, 0, width - x, height,
                          Justification::centredLeft, 1);

    }
}
//========= Rotary slider image ==============================================================================
Image CabbageLookAndFeel::drawRotaryImage(int diameter, const Colour sliderColour, const Colour trackerCol, float sliderPosProportional, 
																							float zeroPosProportional, 
																							bool useTrackerFill,
																							float markerOpacity)
{
	//A simpler slider is created if the diameter is 25 or less.
	bool useBigImage = true;
	if (diameter <=25)
		useBigImage = false;

	Image img = Image(Image::ARGB, diameter, diameter, true);
	Graphics g (img);

	// Calculating number of radians for 300 degrees.  This is because the slider goes from -150 to 150 degrees 
	// where 12 o'clock is zero.
	float numRadians = (300*3.14) / 180; 
	AffineTransform tnsForm = AffineTransform::identity; //this means no transform, or identical

	// Outer grey circle and green fill.  Only used if using big slider image.
	if (useBigImage) {
		g.setColour (Colour::fromRGBA(150, 165, 170, 150));
		g.drawEllipse (0.5, 0.5, diameter-1, diameter-1, 1);

		if (useTrackerFill) {
			Path path; 
			path.addArc (diameter*0.075, diameter*0.075, diameter*0.85, diameter*0.85, -2.6167 + (zeroPosProportional*numRadians), 
													(sliderPosProportional-0.5)*numRadians, true);
			PathStrokeType type (diameter*0.05);
			g.setColour(trackerCol);
			g.setOpacity (0.7);
			g.strokePath (path, type, tnsForm);
		}
	}
	
	//----------- Polygon -----------------------------------------
	g.setColour (Colour::fromRGBA(0, 0, 0, 150));
	g.fillEllipse (diameter*0.17, diameter*0.17, diameter*0.7, diameter*0.7); //for shadow

	Path newPolygon;
	Point<float> centre (diameter/2, diameter/2);
	
	if (diameter >= 40) { //If diameter is >= 40 then polygon has 12 steps
		newPolygon.addPolygon (centre, 12, diameter*0.35, 0);
		newPolygon.applyTransform (AffineTransform::rotation ((sliderPosProportional * numRadians), 
																		diameter/2, diameter/2));
	}
	else if ((diameter < 40) && (diameter > 25)) { //Polygon has 10 steps
		newPolygon.addPolygon (centre, 10, diameter*0.35, 0);
		newPolygon.applyTransform (AffineTransform::rotation ((sliderPosProportional * numRadians), 
																		diameter/2, diameter/2));
	}
	else //Else just use a circle. This is clearer than a polygon when very small.
		newPolygon.addEllipse (0, 0, diameter, diameter);

	ColourGradient cg = ColourGradient (Colours::white, 0, 0, Colours::black, diameter*0.8, diameter*0.8, false);
	g.setGradientFill (cg);
	g.fillPath (newPolygon, tnsForm);


	//-------- Inner circle -------------------------
	g.setColour (Colour::fromRGBA(0, 0, 0, 180)); //for inner shadow
	g.fillEllipse (diameter*0.185, diameter*0.185, diameter*0.65, diameter*0.65); 
	for (float i=0.09; i>0.0; i-=0.01) {
		g.setOpacity (i*2);
		g.fillEllipse (diameter * (0.19+i), diameter * (0.19+i), diameter*0.62, diameter*0.62);
	}

	// Using a colour gradient from white to the chosen colour gives the effect of a light source.  
	ColourGradient circleGrad = ColourGradient (Colours::white, diameter*-0.2, diameter*-0.2,
		sliderColour, diameter*0.5, diameter*0.5, false);
	g.setGradientFill (circleGrad);
	//filling inner circle
	if (diameter > 25)
		g.fillEllipse (diameter*0.19, diameter*0.19, diameter*0.62, diameter*0.62); 
	else
		g.fillEllipse (diameter*0.1, diameter*0.1, diameter*0.8, diameter*0.8);

	//------ Marker --------------  
	Path circleMarker;
	if (diameter > 25) //If diameter is greater than 25 use a rounded rectangle
		circleMarker.addRoundedRectangle (diameter*0.47, diameter*0.19, diameter*0.06, diameter*0.22, 
																		diameter*0.01, diameter*0.05);
	else //Otherwise use a normal rectangle
		circleMarker.addRectangle (diameter*0.47, diameter*0.1, diameter*0.06, diameter*0.3);

	circleMarker.applyTransform (AffineTransform::rotation (((sliderPosProportional-0.5) * numRadians), 
																		diameter/2, diameter/2));	
	g.setColour(sliderColour.contrasting(1.0f)); //will be contrasted against the slider bg
	g.setOpacity(markerOpacity);
	g.fillPath (circleMarker, tnsForm);
	

	return img;
}

//===== Linear slider bg image =========================================================================
Image CabbageLookAndFeel::drawLinearBgImage (float width, float height, float sliderPosProportional, 
																					float zeroPosProportional,
																					bool useTrackerFill,
																					bool isVertical,
																					const Colour trackerFill)
{
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);
	
	//----- For horizontal sliders ---------------------------------------------------
	if (isVertical == false) {
		// Drawing the small ticks
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.3);
		float markerGap = width/10; //gap between ticks
		for (int i=1; i<5; i++)
			g.drawLine ((i*markerGap), height*0.3, (i*markerGap), height*0.7, .7);
		for (int i=6; i<10; i++)
			g.drawLine ((i*markerGap), height*0.3, (i*markerGap), height*0.7, .7);
		//Drawing the centre tick, this will be longer and thicker
		g.setOpacity (0.6);
		g.drawLine ((width/2), height*0.25, (width/2), height*0.75, 1.5);

		// Main Rectangle. Creating the illusion of lighting by painting an almost transparent rectangle first.
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.1);
		g.fillRoundedRectangle (0, height*0.44, width, height*0.15, height*0.05); //for light effect
		g.setColour (Colour::fromRGBA(5, 5, 5, 255));
		g.fillRoundedRectangle (0, height*0.425, width*0.99, height*0.15, height*0.05); //main rectangle

		// Tracker fill. 
		if (useTrackerFill) {
			ColourGradient fill;
			if (zeroPosProportional != 0) {
				fill = ColourGradient (trackerFill, 0, 0, trackerFill, width, 0, false);
				fill.addColour(zeroPosProportional, Colours::transparentBlack);
				fill.addColour(zeroPosProportional+0.05, trackerFill);
				fill.addColour(zeroPosProportional-0.05, trackerFill);
			}
			else
				fill = ColourGradient (Colours::transparentBlack, 0, 0, trackerFill, width*0.1, 0, false);

			g.setGradientFill (fill);
			g.setOpacity(0.9);
			g.drawLine (zeroPosProportional*width, height*0.5, sliderPosProportional*width, height*0.5, height*0.05);
		}
	}
	//----- For vertical sliders ---------------------------------------------------
	if (isVertical == true) {
		// ticks
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.3);
		float markerGap = height/10; //gap between ticks
		for (int i=1; i<5; i++)
			g.drawLine (width*0.3, (i*markerGap), width*0.7, (i*markerGap), .7);
		for (int i=6; i<10; i++)
			g.drawLine (width*0.3, (i*markerGap), width*0.7, (i*markerGap), .7);
		//Drawing the centre tick, this will be longer and thicker
		g.setOpacity (0.6);
		g.drawLine (width*0.25, (height/2), width*0.75, (height/2), 1.5);

		// Main Rectangle. Creating the illusion of lighting by painting an almost transparent rectangle first.
		g.setColour (Colours::whitesmoke);
		g.setOpacity (0.1);
		g.fillRoundedRectangle (width*0.44, 0, width*0.15, height, width*0.05); //for light effect
		g.setColour (Colour::fromRGBA(5, 5, 5, 255));
		g.fillRoundedRectangle (width*0.425, 0, width*0.15, height*0.99, width*0.05); //main rectangle

		// Tracker fill.  
		if (useTrackerFill == true) {
			ColourGradient fill;
			if (zeroPosProportional != 0) {
				fill = ColourGradient (trackerFill, 0, height, trackerFill, 0, 0, false);
				fill.addColour(zeroPosProportional, Colours::transparentBlack);
				fill.addColour(zeroPosProportional+0.05, trackerFill);
				fill.addColour(zeroPosProportional-0.05,trackerFill);
			}
			else
				fill = ColourGradient (Colours::transparentBlack, 0, height, trackerFill, 0, height*0.9, false);

			g.setGradientFill (fill);
			g.setOpacity(0.9);
			sliderPosProportional = 1 - sliderPosProportional; //inverting y axis
			zeroPosProportional = 1 - zeroPosProportional;
			g.drawLine (width*0.5, sliderPosProportional*height, width*0.5, zeroPosProportional*height, width*0.05);
		}
	}

	return img;
}

//========= Linear slider thumb image ====================================================================
Image CabbageLookAndFeel::drawLinearThumbImage (float width, float height, const Colour thumbFill, 
																					bool isVertical)
{
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);

	// Colour
	float brightness = thumbFill.getBrightness();
	if (brightness > 0.9)
		brightness = 0.9;
	ColourGradient thumb = ColourGradient (Colours::white, 0, 0, 
			thumbFill.withBrightness(brightness), width/2, height/2, false);

	//----- For horizontal sliders ------------------------------------------
	if (isVertical == false) {
		//For shadow effect
		g.setColour (Colours::black);
		g.setOpacity (0.8);
		g.fillEllipse ((width*0.2)+1, (height*0.1)+1, width*0.6, height*0.8);
		g.setOpacity (0.4);
		g.fillEllipse ((width*0.2)+3, (height*0.1)+3, width*0.6, height*0.8);
		//Colouring in the thumb
		g.setGradientFill (thumb);
		g.fillEllipse (width*0.2, height*0.1, width*0.6, height*0.8);
	}

	//----- For vertical sliders ------------------------------------------
	else if (isVertical == true) {
		//For shadow effect
		g.setColour (Colours::black);
		g.setOpacity (0.8);
		g.fillEllipse ((width*0.1)+1, (height*0.2)+1, width*0.8, height*0.6);
		g.setOpacity (0.4);
		g.fillEllipse ((width*0.1)+3, (height*0.2)+3, width*0.8, height*0.6);
		//Colouring in the thumb
		g.setGradientFill (thumb);
		g.fillEllipse (width*0.1, height*0.2, width*0.8, height*0.6);
	}

	return img;
}

//========= Toggle Button image ========================================================
Image CabbageLookAndFeel::drawToggleImage (float width, float height, bool isToggleOn, Colour colour, bool isRect)
{
    Image img = Image(Image::ARGB, width, height, true);
    Graphics g (img);
	float opacity = 0;
   
	if (isRect) { //if rectangular toggle
        g.setColour (Colour::fromRGBA (10, 10, 10, 255));
        g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);
      
        if (isToggleOn == true) {
            g.setColour (colour);
            g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);
            opacity = 0.4;
        }
        else { //off
            // Shadow
            for (float i=0.01; i<0.05; i+=0.01) {
                g.setColour (Colour::fromRGBA (0, 0, 0, 255/(i*100)));
                g.fillRoundedRectangle (width*i, height*i, 
                        width*0.95, height*0.95, height*0.1);
            }
            // Filling in the button
            Colour bg1 = Colour::fromRGBA (25, 25, 28, 255);
            Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
            ColourGradient cg = ColourGradient (bg1, 0, 0, bg2, width*0.5, height*0.5, false);
            g.setGradientFill (cg);
            g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);
            opacity = 0.2;
        }

        // For emphasising the top and left edges to give the illusion that light is shining on them
        ColourGradient edgeHighlight = ColourGradient (Colours::whitesmoke, 0, 0,
                Colours::transparentWhite, 0, height*0.1, false);
        g.setGradientFill (edgeHighlight);
        g.setOpacity (opacity);
        g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

        ColourGradient edgeHighlight2 = ColourGradient (Colours::whitesmoke, 0, 0,
                Colours::transparentWhite, height*0.1, 0, false);
        g.setGradientFill (edgeHighlight2);
		g.setOpacity (opacity);
        g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);
	}
	else { //else if round toggle
		//base
		ColourGradient base = ColourGradient (Colours::white, width*-0.3, height*-0.3, Colours::black,
			width*0.8, height*0.8, false);
		g.setGradientFill(base);
		g.fillEllipse (0, 0, width, height);
    
		g.setColour(Colour::fromRGB(70, 70, 70));
		g.fillEllipse(width*0.04, height*0.04, width*0.92, height*0.92);

		if (isToggleOn) { //on
			ColourGradient cg = ColourGradient(colour.withSaturation(0.2), width*0.4, height*0.4, colour,
				width*0.8, height*0.8, true);        
			g.setGradientFill (cg);
			g.fillEllipse(width*0.09, height*0.09, width*0.82, height*0.82);
		}
		else { //off
			g.setColour(Colours::black);
			g.fillEllipse(width*0.09, height*0.09, width*0.82, height*0.82);
    
			ColourGradient cg = ColourGradient (Colours::white, width*0.4, height*0.4, colour.darker(0.9), width*0.3, height*0.3, true);
			g.setGradientFill (cg);
			g.setOpacity(0.4);
			g.fillEllipse(width*0.1, height*0.1, width*0.8, height*0.8);
		}
	}
    return img;	
}

//========= Text button image ========================================================
Image CabbageLookAndFeel::drawTextButtonImage (float width, float height, bool isButtonDown, Colour colour)
{
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);
	float opacity;

	//----- Outline
	g.setColour (Colour::fromRGBA (10, 10, 10, 255));
	g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

	//----- If "off"
	if (isButtonDown == false) {
		//----- Shadow
		for (float i=0.01; i<0.05; i+=0.01) {
			g.setColour (Colour::fromRGBA (0, 0, 0, 255/(i*100)));
			g.fillRoundedRectangle (width*i, height*i, 
				width*0.95, height*0.95, height*0.1);
			opacity = 0.3;
		}
	}
	else
		opacity = 0.1;

	//----- Filling in the button
	//Colour bg1 = Colour::fromRGBA (25, 25, 28, 255);
	//Colour bg2 = Colour::fromRGBA (15, 15, 18, 255);
	Colour bg1 = colour;
	Colour bg2 = colour.darker();
	
	ColourGradient cg = ColourGradient (bg1, 0, 0, bg2, width*0.5, height*0.5, false);
	g.setGradientFill (cg);
	g.fillRoundedRectangle (width*0.01, height*0.01, width*0.93, height*0.93, height*0.1);

	//----- For emphasising the top and left edges to give the illusion that light is shining on them
	ColourGradient edgeHighlight = ColourGradient (Colours::whitesmoke, 0, 0,
	Colours::transparentWhite, 0, height*0.1, false);
	g.setGradientFill (edgeHighlight);
	g.setOpacity (opacity);
	g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);

	ColourGradient edgeHighlight2 = ColourGradient (Colours::whitesmoke, 0, 0,
		Colours::transparentWhite, height*0.1, 0, false);
	g.setGradientFill (edgeHighlight2);
	g.setOpacity (opacity);
	g.fillRoundedRectangle (0, 0, width*0.95, height*0.95, height*0.1);
	
	return img;
}

/*
	------------------------------------------------------------------------------------------------------
	
	Start of Look and Feel methods

	------------------------------------------------------------------------------------------------------
*/


//=========== Rotary Sliders ==============================================================================
void CabbageLookAndFeel::drawRotarySlider(Graphics& g, int /*x*/, int /*y*/, int /*width*/, int /*height*/,
																	float sliderPosProportional,
																	float /*startAngle*/,
																	float /*endAngle*/,
																	Slider& slider)
{
	// Font colour
	String fontColour = slider.getProperties().getWithDefault("fontcolour", "").toString();
	Colour fontcolour;
	if(fontColour.length()>1)
		fontcolour = Colour::fromString(fontColour);
	else
		fontcolour = CabbageUtils::getComponentFontColour();

	// Slider colour
	Colour sliderColour = slider.findColour(0x1001200, false);


	// Value font colour
	String valueFontColour = slider.getProperties().getWithDefault(String("valueFontColour"), ""); //using same colour as tracker
	if (valueFontColour.length()<2)
		valueFontColour = sliderColour.contrasting(1.0f).toString();

	// Name
	String name(slider.getName());
	Font nameFont = CabbageUtils::getComponentFont();
	float nameWidth = nameFont.getStringWidth(name);
	name = CabbageUtils::cabbageString (name, nameFont, slider.getWidth());

	// Image dimensions etc..
	float destX, destY, destHeight, destWidth, sliderBottom;
	destX = destY = 0;
	sliderBottom = slider.getHeight();
	
	// Setting up textbox variables
	Font valueFont(CabbageUtils::getValueFont());
	// Setting up the format of the string....
	int numDec = slider.getProperties().getWithDefault("decimalPlaces", 0);
	String format;
	format << "%." << numDec << "f";
	String sliderValue = CabbageUtils::cabbageString(String::formatted(format, slider.getValue()), valueFont, slider.getWidth());	
	float strWidth = valueFont.getStringWidth(sliderValue);

	// If no textbox
	if (slider.getTextBoxPosition() == Slider::NoTextBox) {
		slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);

		//Name label goes at bottom
		if (slider.getName().length() > 0) {
			g.setColour (fontcolour);
			g.setFont (nameFont);
			g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), slider.getHeight() - nameFont.getHeight(), 
				(int)nameWidth, nameFont.getHeight(), Justification::centred, false);
			sliderBottom -= nameFont.getHeight(); //gap for name label
		}
	}
	// Else if textbox
	else {
		slider.setTextBoxStyle (Slider::TextBoxBelow, true, strWidth, 15);
		sliderBottom -= slider.getTextBoxHeight();
		//Name label goes at top...
		if (slider.getName().length() > 0) {
			g.setColour (fontcolour);
			g.setFont (nameFont);
			g.drawText (name, (slider.getWidth()/2) - (nameWidth/2), 0, (int)nameWidth, nameFont.getHeight(), 
				Justification::centred, false);
			destY += nameFont.getHeight();
		}
	}
	
	destHeight = sliderBottom - destY;
	destWidth = destHeight;							//because rotary slider images have the same width and height....
	destX = ((slider.getWidth()-destWidth) / 2) + 0.5;	//starting x position, rounding up

	// Getting the zero position in proportion to the range.  The tracker fill starts at 0, not the minimum...
	float zeroPosProportional = 0;
	if (slider.getMinimum() < 0)
		zeroPosProportional = (slider.getMinimum()*-1) / (slider.getMaximum() - slider.getMinimum());

	// If using a tracker fill
	bool useTracker = true;
	String trackColour = slider.getProperties().getWithDefault(String("tracker"), "");
	if(trackColour.length()<2){
		trackColour = Colours::lime.toString();
		useTracker = false;
	}

	float markerOpacity = 1;
	if (slider.isMouseOverOrDragging())
		markerOpacity = 0.4;

	// Creating slider image
	Image newSlider = drawRotaryImage(jmax(1.f, destWidth), sliderColour, Colour::fromString(trackColour), sliderPosProportional, 
		zeroPosProportional, useTracker, markerOpacity);
	g.drawImage (newSlider, destX, destY, destWidth, destHeight, 0, 0, newSlider.getWidth(), newSlider.getHeight(), false);

	// If NO textbox and mouse is hovering or dragging, then draw the value across the slider.  This has to be done
	// after the images as it must go on top of them. 
	int valueAlwaysOn = slider.getProperties().getWithDefault(String("valueAlwaysOn"), 0); //using same colour as tracker
	//String valueFontColour = slider.getProperties().getWithDefault(String("valueFontColour"), "");
	
	if ((slider.getTextBoxPosition() == Slider::NoTextBox) && (slider.isMouseOverOrDragging() == true) ||
			valueAlwaysOn) {

		// Background box
		g.setColour(sliderColour);
		g.fillRoundedRectangle ((slider.getWidth()/2) - (strWidth/2), destHeight/2 - valueFont.getHeight()/2, 
			strWidth, valueFont.getHeight(), valueFont.getHeight()/5);	

		g.setColour(Colour::fromString(valueFontColour));
		g.setFont (valueFont);
		g.drawText (sliderValue, (slider.getWidth()/2) - (strWidth/2), destHeight/2 - valueFont.getHeight()/2, 
			(int)strWidth, valueFont.getHeight(), Justification::centred, false);
	}
}

				
//=========== Linear Slider Background ===========================================================================
void CabbageLookAndFeel::drawLinearSliderBackground (Graphics &g, int x, int /*y*/, int width, int height, float sliderPos, 
                                                                                                float /*minSliderPos*/, 
                                                                                                float /*maxSliderPos*/, 
                                                                                                const Slider::SliderStyle style, 
                                                                                                Slider &slider)
{
	// Colour
    String fontColour = slider.getProperties().getWithDefault("fontcolour", "").toString();
	Colour fontcolour;
	if(fontColour.length()>1)
		fontcolour = Colour::fromString(fontColour);
	else
		fontcolour = CabbageUtils::getComponentFontColour();
    
    // Getting the zero position.  The tracker fill starts at 0, not the minimum...
    float zeroPosProportional = 0; 
    if (slider.getMinimum() < 0)
		zeroPosProportional = slider.valueToProportionOfLength(0); //takes into account skew factor

	// Current position of slider in proportion to the value
	float sliderPosProp = slider.valueToProportionOfLength(slider.getValue()); //takes into account the skew factor

    Image newBackground;
    float destX, destY, sliderEndPosition;
    float destHeight = slider.getHeight(); 
    float destWidth = slider.getWidth();
    
    // Name label
	Font nameLabelFont = CabbageUtils::getComponentFont();
    bool showNameLabel = false;
	String nameLabel(slider.getName());
    float nameLabelWidth;
    if (slider.getName().length() > 0) 
        showNameLabel = true; 
    

	// Tracker fill
	bool useTracker = true;
	String trackColour = slider.getProperties().getWithDefault(String("tracker"), "");
	if(trackColour.length()<2){
		trackColour = Colours::lime.toString();
		useTracker = false;
	}

	// Setting up textbox variables
	Font valueFont(CabbageUtils::getValueFont());
	// Setting up the format of the string....
	int numDec = slider.getProperties().getWithDefault("decimalPlaces", 0);
	String format;
	format << "%." << numDec << "f";
	String sliderValue = CabbageUtils::cabbageString(String::formatted(format, slider.getValue()), valueFont, slider.getWidth());	
	float strWidth = valueFont.getStringWidth(sliderValue);

    //===================== If Horizontal Slider =================================================
    if (style == Slider::LinearHorizontal) {
		// Making sure that the specified size is ok...
		if (slider.getHeight() > (slider.getWidth()*0.5))
			slider.setBounds(slider.getX(), slider.getY(), slider.getWidth(), slider.getWidth()*0.5);

		// Name alignment
		String alignment = slider.getProperties().getWithDefault("align", "").toString();
		Justification just(1);
		if (alignment.length()>1) {
			if (alignment.compare("left") == 0)
				just.left;
			else if (alignment.compare("right") == 0)
				just.right;
			else if (alignment.compare("centre") == 0)
				just.centred;
		}

        nameLabelWidth = slider.getWidth() * 0.2;   //available space for the name label


        // Start and end of slider image, height*0.25 is to make room for the edge of the slider thumb when at 
        // a maximum or minimum
        destX = height*0.25; 
		destY = 0;
        sliderEndPosition = slider.getWidth() - destX; 
        
        // If there is a name label it will be shown at the start
        if (showNameLabel) {
            nameLabel = CabbageUtils::cabbageString (nameLabel, CabbageUtils::getComponentFont(), nameLabelWidth);
			g.setFont(nameLabelFont);
			g.setColour(fontcolour);
			g.drawText (nameLabel, 0, (height/2) - (CabbageUtils::getComponentFont().getHeight()/2), 
				(int)nameLabelWidth, CabbageUtils::getComponentFont().getHeight(), just, false);
            destX += nameLabelWidth; 
        }

        // Textbox 
        if (slider.getTextBoxPosition() == Slider::NoTextBox) 
            slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
        else { 
            slider.setTextBoxStyle (Slider::TextBoxRight, false, strWidth, 15);
            sliderEndPosition -= strWidth;
        }

        destWidth = sliderEndPosition - destX; 
	
		// Slider is enabled and value changed only if mouse click is within the actual slider image...
        if (slider.isMouseButtonDown() == true) {
            Point<int> mousePos = slider.getMouseXYRelative();
			slider.setEnabled(true);
            if ((mousePos.getX() >= (destX-5)) && (mousePos.getX() <= (sliderEndPosition+5))) {
				sliderPosProp = (mousePos.getX()-destX)/destWidth;
				
				slider.setValue(slider.proportionOfLengthToValue(sliderPosProp)); //takes into account the skew factor
			}
        }
        slider.setEnabled (false); // disabling slider

		// Final slider position in proportion to length...
        sliderPosProp = slider.valueToProportionOfLength(slider.getValue());
		
		// Getting image
		newBackground = drawLinearBgImage (destWidth, destHeight, sliderPosProp, zeroPosProportional, 
			useTracker, false, Colour::fromString(trackColour));	
    }

    //=========================== Else if Vertical Slider =======================================
    else if (style == Slider::LinearVertical) {
		// Making sure that the specified size is ok...
		if (slider.getWidth() > (slider.getHeight()*0.5))
			slider.setBounds(slider.getX(), slider.getY(), slider.getHeight()*0.5, slider.getHeight());
		
		nameLabel = CabbageUtils::cabbageString(nameLabel, nameLabelFont, width);
		nameLabelWidth = nameLabelFont.getStringWidth(nameLabel);
        destX = ((width - destWidth) / 2);    
        // Starting y position, width*0.25 is to make room for the edge of the thumb when at a 
        // maximum or minimum
		float thumbHeight = width/2;
        destY = thumbHeight/2; 
        sliderEndPosition = slider.getHeight() - destY;

		g.setFont(nameLabelFont);
		g.setColour(fontcolour);

        // If no textbox.....Name label goes at top, value goes at bottom.
        if (slider.getTextBoxPosition() == Slider::NoTextBox) {
            slider.setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
            if (showNameLabel) { 
				g.drawText (nameLabel, (width/2) - (nameLabelWidth/2), slider.getHeight() - nameLabelFont.getHeight(), 
					(int)nameLabelWidth, nameLabelFont.getHeight(), Justification::centred, false);
                sliderEndPosition -= nameLabelFont.getHeight();
            }
			destY += CabbageUtils::getValueFont().getHeight(); //value at top 
        }

        // Else if textbox......Name label will go at top.
        else {
            String str;
            str << slider.getMaximum() << slider.getInterval();
			slider.setTextBoxStyle (Slider::TextBoxBelow, false, CabbageUtils::getValueFont().getStringWidth(str), 15);
			sliderEndPosition -= slider.getTextBoxHeight();
            if (showNameLabel) { 
				g.drawText (nameLabel, (width/2) - (nameLabelWidth/2), 0, nameLabelFont.getStringWidth(nameLabel), 
					nameLabelFont.getHeight(), Justification::centred, false);
				destY += nameLabelFont.getHeight(); 
                //sliderEndPosition -= 0; Textbox automatically adjusts height.
            }
        }

        destHeight = sliderEndPosition - destY;

        // Slider is enabled and value changed only if mouse click is within the actual slider image...
        if (slider.isMouseButtonDown() == true) {
            Point<int> mousePos = slider.getMouseXYRelative();
            slider.setEnabled (true);
            if ((mousePos.getY() >= (destY-5)) && (mousePos.getY() <= (sliderEndPosition+5))) {
				sliderPosProp = 1 - ((mousePos.getY()-destY)/destHeight); //inverting because of y axis
				slider.setValue(slider.proportionOfLengthToValue(sliderPosProp), sendNotification);
            }
        }
        slider.setEnabled (false); //disabling

		// Final slider position in proportion to length
		sliderPosProp = slider.valueToProportionOfLength(slider.getValue());

        // Getting image
		newBackground = drawLinearBgImage (destWidth, destHeight, sliderPosProp, zeroPosProportional, 
			useTracker, true, Colour::fromString(trackColour));
    }
    
    // Drawing Image. 
    g.setOpacity (1);
    g.drawImage(newBackground, destX, destY, destWidth, destHeight, 0, 0, destWidth, destHeight, false);
}



//========== Linear Slider Thumb =========================================================================
void CabbageLookAndFeel::drawLinearSliderThumb (Graphics &g, int x, int /*y*/, int width, int height, float sliderPos, 
																								float /*minSliderPos*/, 
																								float /*maxSliderPos*/, 
																								const Slider::SliderStyle style, 
																								Slider &slider)
{
	// Value font colour
	String valueFontColour = slider.getProperties().getWithDefault(String("tracker"), ""); //using same colour as tracker
	if (valueFontColour.length()<2)
		valueFontColour = Colour::fromRGB(200, 200, 200).toString();

	float destX, destY, thumbWidth, thumbHeight, sliderStart, sliderEndPosition;
	Colour thumbFill = slider.findColour(Slider::thumbColourId, false);
	Image newThumb;

	float sliderPosProp = slider.valueToProportionOfLength(slider.getValue());

	// The following determines if slider value should be displayed, in the event of no textbox.  It 
	// also calculates the string sliderValue and its width etc..
	String sliderValue;
	float sliderValueWidth;
	Font valueFont (CabbageUtils::getValueFont());
	if (slider.getTextBoxPosition() == Slider::NoTextBox) {
		String interval(slider.getInterval()); //number of decimal places
		int numDec = interval.length()-2; //taking away decimal point and number before
		if (numDec < 0)
			numDec = 0;

		// Setting up format string....
		numDec = slider.getProperties().getWithDefault("decimalPlaces", 0);
		String format;
		format << "%." << numDec << "f";
		sliderValue = String::formatted(format, slider.getValue());
		sliderValueWidth = valueFont.getStringWidth(sliderValue);
	}

	//===================== If Horizontal ================================================
	if (style == Slider::LinearHorizontal) {
		String nameLabel(slider.getName());
		Font nameFont = CabbageUtils::getComponentFont();
		g.setFont (nameFont);
		float nameLabelWidth = slider.getWidth() * 0.2; //fixed width for name label

		// Making sure that the specified size is ok...
		if (slider.getHeight() > (slider.getWidth()*0.5))
			height = slider.getWidth()*0.5;

		thumbWidth = thumbHeight = height * 0.5;

		// Initialising the start and end coordinates of the slider.  A gap of thumbWidth/2 is left so 
		// that the edge of the thumb can be seen when at a minimum or maximum value. 
		sliderStart = thumbWidth/2; 
		sliderEndPosition = slider.getWidth() - (thumbWidth/2); 
		
		if (slider.getName().length() > 0)  //If name label is to be shown...
			sliderStart += nameLabelWidth;
		
		if (slider.getTextBoxPosition() != slider.NoTextBox)  //If a textbox IS to be displayed
			sliderEndPosition -= slider.getTextBoxWidth(); 

		// Thumb position 
		float availableWidth = sliderEndPosition - sliderStart;
		destY = (height/2) - (thumbHeight/2);
		destX = (sliderPosProp*availableWidth) + sliderStart - (thumbWidth/2);//subtracting (thumbWidth/2) centres the thumb over slider position		

		// Drawing the slider value above the thumb if there is no textbox, and mouse is hovering or dragging
		if ((slider.getTextBoxPosition() == Slider::NoTextBox) && (slider.isMouseOverOrDragging() == true)) {
			float boxWidth = sliderValueWidth + 2;
			float valuePos = (sliderPosProp * ((availableWidth+(thumbWidth/2))-boxWidth)) + sliderStart-(thumbWidth/4);
			g.setColour (CabbageUtils::getDarkerBackgroundSkin());
			g.fillRoundedRectangle (valuePos, 0, boxWidth, valueFont.getHeight(), valueFont.getHeight()/5);

			g.setColour(Colours::whitesmoke);
			g.setFont(valueFont);
			g.drawText(sliderValue, valuePos+1, 0, (int)sliderValueWidth, valueFont.getHeight(), Justification::centred, false);
		}		

		newThumb = drawLinearThumbImage (thumbWidth, thumbHeight, thumbFill, false); //creating image
	}

	//=========================== Else if vertical ===================================================
	else if (style == Slider::LinearVertical) {
		// Making sure that the specified size is ok...
		if (slider.getWidth() > (slider.getHeight()*0.5))
			width = slider.getHeight()*0.5;

		thumbWidth = thumbHeight = width * 0.5;

		sliderStart = thumbHeight/2; //leaving room for thumb
		sliderEndPosition = slider.getHeight() - (thumbHeight/2);
		
		// If No Text Box is to be displayed.....Value displayed above thumb
		if (slider.getTextBoxPosition() == Slider::NoTextBox) {
			sliderStart += valueFont.getHeight();
			if (slider.getName().length() > 0) //name label goes at bottom
				sliderEndPosition -= CabbageUtils::getComponentFont().getHeight();
		}

		// If textbox....name label at top
		else {
			if (slider.getName().length() > 0) 
				sliderStart += CabbageUtils::getComponentFont().getHeight();
			sliderEndPosition -= slider.getTextBoxHeight();
		}

		// Thumb position 
		float availableHeight = sliderEndPosition - sliderStart;
		float newSliderPos = ((1-sliderPosProp) * availableHeight) + sliderStart;
		destX = (width/2) - (thumbWidth/2);
		destY = newSliderPos - (thumbHeight / 2);

		// Drawing the slider value above the thumb if there is no textbox, and mouse is hovering or dragging
		if ((slider.getTextBoxPosition() == Slider::NoTextBox) && (slider.isMouseOverOrDragging() == true)) {
			float boxWidth = sliderValueWidth+2;
			g.setColour (CabbageUtils::getDarkerBackgroundSkin());
			g.fillRoundedRectangle ((width/2) - (boxWidth/2), newSliderPos-(valueFont.getHeight()+(thumbHeight/2)), 
				boxWidth, valueFont.getHeight(), 2);
			g.setColour (Colours::whitesmoke);
			g.setFont (valueFont);
			g.drawText (sliderValue, (width/2) - (sliderValueWidth/2), newSliderPos-(valueFont.getHeight()+(thumbHeight/2)), 
				(int)sliderValueWidth, valueFont.getHeight(), Justification::centred, false);
		}
		newThumb = drawLinearThumbImage (thumbWidth, thumbHeight, thumbFill, true); //creating image
	}
	
	g.drawImage (newThumb, destX, destY, thumbWidth, thumbHeight, 0, 0, newThumb.getWidth(), newThumb.getHeight(), false);
}

//======= Toggle Buttons ========================================================================
void CabbageLookAndFeel::drawToggleButton (Graphics &g, ToggleButton &button, bool /*isMouseOverButton*/, bool /*isButtonDown*/)
{
	int destWidth, destHeight;
	destHeight = button.getHeight();
	if (button.getButtonText().length() > 0)
		destWidth = destHeight;
	else
		destWidth = button.getWidth();

	int destX = 0;
	int destY = 0;
	bool isToggleOn;

	if (button.getToggleState() == true)
		isToggleOn = true;
	else
		isToggleOn = false;

	//----- Getting the colours...
	Colour col, fcol;
	if (button.getProperties().getWithDefault("colour", "").toString().length()<1){
        for (float i=0.01; i<0.03; i+=0.01)
			col = Colour::fromRGBA (0, 0, 0, 255/(i*100));
    }
    else
		col = Colour::fromString(button.getProperties().getWithDefault("colour", "lime").toString());
	
	if (button.getProperties().getWithDefault("fontcolour", "").toString().length()<2){
		fcol = CabbageUtils::getComponentFontColour();
    }
    else
		fcol = Colour::fromString(button.getProperties().getWithDefault("fontcolour", "lime").toString());

	bool isRECT = button.getProperties().getWithDefault("isRect", 0);

	//----- Creating the image
	Image newButton = drawToggleImage (destWidth, destHeight, isToggleOn, col, isRECT);

	//----- Drawing image
	g.drawImage (newButton, destX, destY, destWidth, destHeight, 0, 0, destWidth, destHeight, false);

	//----- Text
	if (button.getButtonText().length() > 0) {
		Justification just (1); //left
		g.setFont (CabbageUtils::getComponentFont());
		g.setColour (fcol);

		String name;
		name << button.getButtonText();
		name = CabbageUtils::cabbageString (name, CabbageUtils::getComponentFont(), (button.getWidth()-(destWidth+5))); //shortening string if too long

		g.drawText (name, destWidth+5, destY, button.getWidth(), button.getHeight(), just, false);
	}
}

//========= Text Button Background ======================================================================
void CabbageLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& /*backgroundColour*/, 
																			bool /*isButtonOver*/, 
																			bool isButtonDown)
{
	float width = button.getWidth();
	float height = button.getHeight();
	String colour = button.getProperties().getWithDefault("colour", "");
	Image newButton = drawTextButtonImage (width, height, isButtonDown, Colour::fromString(colour));
	g.drawImage (newButton, 0, 0, width, height, 0, 0, width, height, false);
}

//======= Text Button text =========================================================================
void CabbageLookAndFeel::drawButtonText (Graphics &g, TextButton &button, bool isMouseOverButton, 
																				bool isButtonDown)
{
	float width = button.getWidth();
	float height = button.getHeight();
	float destX;
	float destWidth = width*0.8;
	Font font = CabbageUtils::getComponentFont();
	String text;
	text << button.getButtonText();
	text = CabbageUtils::cabbageString (text, font, destWidth);
	Justification just (36); //centered
	float destY = (height*0.5) - (font.getHeight()*0.5);
	
	Colour textColour;
	String col = button.getProperties().getWithDefault("fontcolour", "");
	if(col.length()>2)
		textColour = Colour::fromString(col);
	else
		textColour = CabbageUtils::getComponentFontColour();

	//----- The text colour and start position change if the button is down or up....
	if (isButtonDown == true) {
		g.setColour (textColour.brighter(0.3));
		destX = width*0.095;
	}
	else {
		g.setColour (textColour);
		destX = width*0.1;
	}

	g.setFont (font);
	g.drawText (text, destX, destY, destWidth, font.getHeight(),
		just, false);
}

//=========== ComboBoxes ============================================================================
void CabbageLookAndFeel::drawComboBox(Graphics& g, int width, int height, bool /*isButtonDown*/,
																	int /*buttonX*/,
																	int /*buttonY*/,
																	int /*buttonW*/,
																	int /*buttonH*/,
																	ComboBox& box)
{
	// Colour
	String bgColour = box.getProperties().getWithDefault(String("colour"), "");
	String fontColour = box.getProperties().getWithDefault(String("fontcolour"), "");
	Colour bgCol, fCol;
	if (bgColour.length()>2)
		bgCol = Colour::fromString(bgColour);
	else
		bgCol = CabbageUtils::getDarkerBackgroundSkin();

	if (fontColour.length()>2)
		fCol = Colour::fromString(fontColour);
	else
		fCol = CabbageUtils::getComponentFontColour();

	// The box that contains the arrow
	g.setColour(CabbageUtils::getComponentFontColour());
	float arrowBoxWidth;
	if (width >= 40)
		arrowBoxWidth = 20;
	else
		arrowBoxWidth = width/2;

	// Main bg
	g.setColour(bgCol);
	g.fillRoundedRectangle (0, 0, width, height, height*0.1);

	// Border outline
	g.setColour(CabbageUtils::getBorderColour());
	float borderWidth = CabbageUtils::getBorderWidth();
	g.drawRoundedRectangle (borderWidth/2, borderWidth/2, width-borderWidth, 
		height-borderWidth, height*0.1, borderWidth);

	// Arrow
	g.setColour(fCol);
	const Line<float> line(width-(arrowBoxWidth/2), height*0.3, width-(arrowBoxWidth/2), height*0.7); 
	g.drawArrow(line, 0, arrowBoxWidth*0.4, height*0.4);	
}


//============= TextEditor Outline ======================================================================
void CabbageLookAndFeel::drawTextEditorOutline (Graphics &g, int width, int height, TextEditor &/*textEditor*/)
{
	g.setColour (Colours::transparentBlack);
	g.drawRoundedRectangle (0, 0, width, height, height/3, 1);
}



//============= TextEditor Background ==============================================================
void CabbageLookAndFeel::fillTextEditorBackground (Graphics &g, int width, int height, TextEditor &textEditor)
{
	textEditor.setColour (0x1000200, Colours::transparentBlack); //this gets rid of the square rectangle background
	textEditor.setColour(0x1000201, Colours::white);
	textEditor.setColour(0x1000203, Colours::white);
	//g.setColour (CabbageUtils::getBackgroundSkin());
	//g.fillRoundedRectangle (0, 0, width, height, height/3);
}


//=========== Labels, slider textboxes are also labels =============================================
void CabbageLookAndFeel::drawLabel (Graphics &g, Label &label)
{	
	// Getting parent component of label
	Component* comp = label.getParentComponent();
	
	// If slider
	if (dynamic_cast<Slider*>(comp)) { 
		g.setColour(CabbageUtils::getDarkerBackgroundSkin());
		g.setOpacity(0.7);
		g.fillRoundedRectangle (0, 0, label.getWidth(), label.getHeight(), label.getHeight()/5);
		g.setColour(Colours::whitesmoke);
		g.setFont(CabbageUtils::getValueFont());
		g.drawText (label.getText(), 0, 0, label.getWidth(), label.getHeight(), Justification::centred, false);
		// Border
		g.setColour(CabbageUtils::getBorderColour());
		float borderWidth = CabbageUtils::getBorderWidth();
		g.drawRoundedRectangle(borderWidth/2, borderWidth/2, label.getWidth()-borderWidth, label.getHeight()-borderWidth,
			label.getHeight()/5, borderWidth);
	}
	// Else If not a slider
	else { 
		g.setColour (Colours::transparentBlack);
		g.fillRoundedRectangle (0, 0, label.getWidth(), label.getHeight(), label.getHeight()/5);

		// For the text
		g.setFont (CabbageUtils::getComponentFont());
		String col = label.getProperties().getWithDefault("textColour", "");
		if (col.length()>0)
			g.setColour(Colour::fromString(col));
		else
			g.setColour (CabbageUtils::getComponentFontColour());

		g.drawText (label.getText(), 0, 0, label.getWidth(), label.getHeight(), Justification::centred, false);
	}
}


//======== Group Components ======================================================================
void CabbageLookAndFeel::drawGroupComponentOutline (Graphics &g, int w, int h, const String &text, 
																		const Justification &position, 
																		GroupComponent &group)
{
	//----- Background
	Colour col;
	String bgColour = group.getProperties().getWithDefault("colour", "").toString();
	String fontColour = group.getProperties().getWithDefault("fontcolour", "").toString();
	//Logger::writeToLog(bgColour);
	//Logger::writeToLog(fontColour);

	if (bgColour.length()>1){
		col = Colour::fromString(bgColour);
		g.setColour (col);
	}
	else
		g.setColour (CabbageUtils::getComponentSkin());

	g.fillRoundedRectangle (0, 0, w, h, 5);

	//----- Outline
	g.setColour (CabbageUtils::getComponentFontColour());
	g.setOpacity (0.1);
	g.drawRoundedRectangle (0.5, 0.5, w-1, h-1, 5, 1);
	
	//----- Text
	String name = group.getText();
	Font font = CabbageUtils::getTitleFont();
#ifndef MACOSX
	font.setFallbackFontName("Verdana"); 
#endif	
g.setFont (font);

	if (fontColour.length()>1){
		col = Colour::fromString(fontColour);
		g.setColour (col);
	}
	else 
		g.setColour (CabbageUtils::getComponentFontColour());

	name = CabbageUtils::cabbageString (name, font, group.getWidth());
	g.drawText (name, 0, 5, w, font.getHeight(), 36, false);
	if(!group.getProperties().getWithDefault("groupLine", 0).equals(var(0))){
	g.drawLine (10, 20, w-10, 20, 0.2);
	}
	//----- Corner holes
	g.setColour (CabbageUtils::getBackgroundSkin());
	g.setOpacity(0.7);
	g.fillEllipse (2, 2, 4, 4); //top-left
	g.fillEllipse (2, h-6, 4, 4); //bottom-left
	g.fillEllipse (w-6, 2, 4, 4); //top-right
	g.fillEllipse (w-6, h-6, 4, 4); //bottom-right
	
}

//======== Scrollbars ==============================================================================
void CabbageLookAndFeel::drawScrollbar (Graphics &g, ScrollBar &scrollbar, int x, int y, int width, 
																						int height, 
																						bool isScrollbarVertical, 
																						int thumbStartPosition, 
																						int thumbSize, 
																						bool isMouseOver, 
																						bool isMouseDown)
{
	g.setColour (Colours::transparentBlack);
	g.fillAll();

	g.setColour (CabbageUtils::getComponentSkin());
	g.drawRoundedRectangle (x, y, width, height, 5, 1);

	if (isScrollbarVertical == false) //horizontal
		g.fillRoundedRectangle (thumbStartPosition+3, 3, thumbSize-6, height-6, 5);
	else //vertical
		g.fillRoundedRectangle (3, thumbStartPosition+3, width-6, thumbSize-6, 5);

}

//======= get deafult Scrollbar buttons =======================================================================
int CabbageLookAndFeel::getDefaultScrollbarWidth()
{
return 18;	
}
//======= Scrollbar buttons =======================================================================
void CabbageLookAndFeel::drawScrollbarButton (Graphics &g, ScrollBar &scrollbar, int width, int height, 
																					int buttonDirection, 
																					bool isScrollbarVertical, 
																					bool isMouseOverButton, 
																					bool isButtonDown)
{
	g.setColour (Colours::transparentBlack);
	g.fillAll();

	if (isButtonDown == true)
		g.setColour (CabbageUtils::getComponentSkin().brighter(0.5f));
	else
		g.setColour (CabbageUtils::getComponentSkin());

	if (buttonDirection == 3) { //left arrow
		Line<float> left (width*0.8, height/2, width*0.2, height/2);
		g.drawArrow (left, 0, 10, 10); 
	}
	else if (buttonDirection == 1) { //right arrow
		Line<float> right (width*0.2, height/2, width*0.8, height/2);
		g.drawArrow (right, 0, 10, 10); 
	}
	else if (buttonDirection == 0) { //up arrow
		Line<float> up (width/2, height*0.8, width/2, height*0.2);
		g.drawArrow (up, 0, 10, 10); 
	}
	else if (buttonDirection == 2) { //down arrow
		Line<float> down (width/2, height*0.2, width/2, height*0.8);
		g.drawArrow (down, 0, 10, 10); 
	}
	

}

//======== Popup Menu background ======================================================================
void CabbageLookAndFeel::drawPopupMenuBackground(Graphics &g, int width, int height)
{
	g.setColour (CabbageUtils::getDarkerBackgroundSkin());
    g.fillAll();

	g.setColour (CabbageUtils::getComponentSkin());
	g.drawRect (0, -5, width, height+5, 1); //dont want to see top line
}

//====== Returns image of a check mark ==============================================
Image CabbageLookAndFeel::drawCheckMark()
{
	Image img = Image(Image::ARGB, 10, 10, true);
	Graphics g(img);

	Path path;
	path.startNewSubPath(3, 7);
	path.lineTo(5, 10);
	path.lineTo(10, 0);
	g.setColour (Colours::cornflowerblue);
	g.strokePath(path, PathStrokeType(2.0f));

	return img;
}

//======== Popup Menu Items ===========================================================================
void CabbageLookAndFeel::drawPopupMenuItem (Graphics &g, int width, int height, bool isSeparator, bool /*isActive*/, 
																								bool isHighlighted, 
																								bool isTicked, 
																								bool hasSubMenu, 
																								const String &text, 
																								const String &shortcutKeyText, 
																								Image */*image*/, 
																								const Colour* textColourToUse)
{
	Colour textColour;		
	if ((isHighlighted == true) && (isSeparator == false)) {
		g.setColour (CabbageUtils::getComponentSkin());
		g.fillAll();
		g.setColour (Colours::cornflowerblue);
	}
	else{
		if (textColourToUse != nullptr){
			textColour = *textColourToUse;
			g.setColour(textColour);
		}
		else
			g.setColour (CabbageUtils::getComponentFontColour());
	}
	
	
	g.setFont (CabbageUtils::getComponentFont());
	g.drawText (CabbageUtils::cabbageString(text, CabbageUtils::getComponentFont(), width*0.8), 20, 0, width*0.8, height, 1, false);

	if (isSeparator == true) {
        g.setColour(CabbageUtils::getComponentSkin());
        g.drawLine(0, height/2, width, 3);
    }

	if (isTicked) {
		Image checkMark = drawCheckMark();
		g.setColour(Colours::cornflowerblue);
		g.drawImage(checkMark, 5, (height/2)-5, 10, 10, 0, 0, 10, 10, false);	
	}
	if (hasSubMenu) {
		g.setColour(Colours::cornflowerblue);
		const Line<float> line(width-15, height*.5, width-5, height*.5);
		g.drawArrow(line, 0, height*.3, height*.3);
	}
        if (shortcutKeyText.isNotEmpty())
        {
			const int leftBorder = (height * 5) / 4;
			const int rightBorder = 4;

            g.drawText (shortcutKeyText,
                        leftBorder, 0, width - (leftBorder + rightBorder + 4), height,
                        Justification::centredRight,
                        true);
        }
 
}

//======== Menubar background ======================================================================
void CabbageLookAndFeel::drawMenuBarBackground(Graphics &g, int width, int height, bool isMouseOverBar, MenuBarComponent &menuBar)
{
	g.setColour (CabbageUtils::getBackgroundSkin());
	g.fillAll();
}

//======== Menubar item background ======================================================================
void CabbageLookAndFeel::drawMenuBarItem(Graphics & g, int width, int height, int itemIndex,
                                                                              const String &itemText, 
																			  bool isMouseOverItem,
                                                                              bool isMenuOpen, 
																			  bool isMouseOverBar,
                                                                              MenuBarComponent &menuBar)
{
	if ((isMouseOverItem == true) || (isMenuOpen == true)) {
		g.setColour (CabbageUtils::getBackgroundSkin().darker (0.2));
		g.fillRect (0, 0, width, height);
	}

	g.setColour (CabbageUtils::getComponentFontColour());
	g.setFont (CabbageUtils::getComponentFont());
    g.drawFittedText(CabbageUtils::cabbageString(itemText, CabbageUtils::getComponentFont(), width*0.9), 5, 0, width, height, 36, 1);
}

//======== Document Window title bar ===================================================================
void CabbageLookAndFeel::drawDocumentWindowTitleBar (DocumentWindow &window, Graphics &g, int w, int h, 
																						int /*titleSpaceX*/, 
																						int titleSpaceW, 
																						const Image */*icon*/, 
																						bool /*drawTitleTextOnLeft*/)
{
	window.setUsingNativeTitleBar(false);

	g.setColour (CabbageUtils::getDarkerBackgroundSkin());
	g.fillAll();

	g.setColour (CabbageUtils::getComponentFontColour());
	Font font = CabbageUtils::getTitleFont();
#ifndef MACOSX
	font.setFallbackFontName("Verdana"); 
#endif
	font.setHeight(16);
	g.setFont (font);
	g.drawText (CabbageUtils::cabbageString(window.getName(), font, titleSpaceW), (w/2)-(titleSpaceW/2), 
									(h/2)-(font.getHeight()/2), titleSpaceW, font.getHeight(), 36, false);
}

//====== Draw Window Button Normal Image =================================================================
Image CabbageLookAndFeel::drawWindowButtonNormal(int buttonType)
{
	int width, height;
	width = height = 20;
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);

	String str;

	if (buttonType == 1) 
		str << "_";
	else if (buttonType == 2)
		str << "+";
	else if (buttonType == 4) 
		str << "x";

	//----- Background
	g.setColour (Colours::transparentBlack);
	g.fillRoundedRectangle(1, 1, width-2, height-2, 2);
	
	//----- Text symbol
	Font font = CabbageUtils::getComponentFont();
	g.setFont (font);
	g.setColour (CabbageUtils::getComponentFontColour());
	g.drawText(str, (width/2) - (font.getHeight()/2), (width/2) - (font.getHeight()/2), 
		font.getHeight(), font.getHeight(), 36, false);

	return img;
}

//====== Draw Window Button Image Is Over ==============================================================
Image CabbageLookAndFeel::drawWindowButtonIsOver(int buttonType)
{
	int width, height;
	width = height = 20;
	Image img = Image(Image::ARGB, width, height, true);
	Graphics g (img);

	String str;

	if (buttonType == 1) 
		str << "_";
	else if (buttonType == 2)
		str << "+";
	else if (buttonType == 4) 
		str << "x";

	//---- Background
	g.setColour (Colours::transparentBlack);
	g.fillRoundedRectangle(1, 1, width-2, height-2, 2);

	//---- Text symbol
	Font font = CabbageUtils::getComponentFont();
	g.setFont (font);
	g.setColour (Colours::whitesmoke);
	g.drawText(str, (width/2) - (font.getHeight()/2), (width/2) - (font.getHeight()/2), 
		font.getHeight(), font.getHeight(), 36, false);

	//---- V lines
	ColourGradient cg = ColourGradient(Colours::transparentBlack, 0, 0, Colours::transparentBlack,
		0, height, false);
	cg.addColour (0.5, CabbageUtils::getComponentFontColour());
	g.setGradientFill (cg);
	g.drawLine (0, 0, 0, height, 1);
	g.drawLine (width, 0, width, height, 1);

	return img;
}

//========= Document Window Buttons =======================================================================
Button* CabbageLookAndFeel::createDocumentWindowButton (int buttonType)
{
	ImageButton* button;
	button = new ImageButton("Button");
	Image normalImage, isOverImage;

	normalImage = drawWindowButtonNormal(buttonType);
	isOverImage = drawWindowButtonIsOver(buttonType);

	button->setImages(true, false, true, normalImage, 1, Colours::transparentBlack, isOverImage,
		1, Colours::transparentBlack, isOverImage, 1, Colours::transparentBlack, 0);

	
	return button;
}

//============= alert windows methods ===================================
AlertWindow* CabbageLookAndFeel::createAlertWindow (const String& title,
                                             const String& message,
                                             const String& button1,
                                             const String& button2,
                                             const String& button3,
                                             AlertWindow::AlertIconType iconType,
                                             int numButtons,
                                             Component* associatedComponent)
{
    AlertWindow* aw = new AlertWindow (title, message, iconType, associatedComponent);

    if (numButtons == 1)
    {
        aw->addButton (button1, 0,
                       KeyPress (KeyPress::escapeKey),
                       KeyPress (KeyPress::returnKey));
    }
    else
    {
        const KeyPress button1ShortCut ((int) CharacterFunctions::toLowerCase (button1[0]), 0, 0);
        KeyPress button2ShortCut ((int) CharacterFunctions::toLowerCase (button2[0]), 0, 0);
        if (button1ShortCut == button2ShortCut)
            button2ShortCut = KeyPress();

        if (numButtons == 2)
        {
            aw->addButton (button1, 1, KeyPress (KeyPress::returnKey), button1ShortCut);
            aw->addButton (button2, 0, KeyPress (KeyPress::escapeKey), button2ShortCut);
        }
        else if (numButtons == 3)
        {
            aw->addButton (button1, 1, button1ShortCut);
            aw->addButton (button2, 2, button2ShortCut);
            aw->addButton (button3, 0, KeyPress (KeyPress::escapeKey));
        }
    }

    return aw;
}

void CabbageLookAndFeel::drawAlertBox (Graphics& g,
                                AlertWindow& alert,
                                const Rectangle<int>& textArea,
                                TextLayout& textLayout)
{
	g.fillAll (alert.findColour (AlertWindow::backgroundColourId));
	g.fillAll (Colours::black);
    int iconSpaceUsed = 0;

    const int iconWidth = 80;
    int iconSize = jmin (iconWidth + 50, alert.getHeight() + 20);

    if (alert.containsAnyExtraComponents() || alert.getNumButtons() > 2)
        iconSize = jmin (iconSize, textArea.getHeight() + 50);

    const Rectangle<int> iconRect (iconSize / -10, iconSize / -10,
                                   iconSize, iconSize);

    if (alert.getAlertType() != AlertWindow::NoIcon)
    {
        Path icon;
        uint32 colour;
        char character;

        if (alert.getAlertType() == AlertWindow::WarningIcon)
        {
				Image logo = ImageCache::getFromMemory (BinaryData::logo_cabbage_Black_png, BinaryData::logo_cabbage_Black_pngSize);
				g.setOpacity(.2f);
				g.drawImage(logo, -300, -100, 600, 500, 0, 0, logo.getWidth(), logo.getHeight());
				
        }
        else
        {
            colour    = alert.getAlertType() == AlertWindow::InfoIcon ? (uint32) 0x605555ff : (uint32) 0x40b69900;
            character = alert.getAlertType() == AlertWindow::InfoIcon ? 'i' : '?';

            icon.addEllipse ((float) iconRect.getX(), (float) iconRect.getY(),
                             (float) iconRect.getWidth(), (float) iconRect.getHeight());

        }

//        GlyphArrangement ga;
//        ga.addFittedText (Font (iconRect.getHeight() * 0.9f, Font::bold),
//                          String::charToString ((juce_wchar) (uint8) character),
//                          (float) iconRect.getX(), (float) iconRect.getY(),
//                          (float) iconRect.getWidth(), (float) iconRect.getHeight(),
//                          Justification::centred, false);
//        ga.createPath (icon);
//
//        icon.setUsingNonZeroWinding (false);
//        g.setColour (Colour (colour));
//        g.fillPath (icon);

        iconSpaceUsed = iconWidth;
    }

	g.setColour (Colours::pink);//alert.findColour (AlertWindow::textColourId));

    textLayout.draw (g, Rectangle<int> (textArea.getX() + iconSpaceUsed,
                                        textArea.getY(),
                                        textArea.getWidth() - iconSpaceUsed,
                                        textArea.getHeight()).toFloat());

    g.setColour (alert.findColour (AlertWindow::outlineColourId));
    g.drawRect (0, 0, alert.getWidth(), alert.getHeight());
}

int CabbageLookAndFeel::getAlertBoxWindowFlags()
{
    return ComponentPeer::windowAppearsOnTaskbar
            | ComponentPeer::windowHasDropShadow;
}

int CabbageLookAndFeel::getAlertWindowButtonHeight()
{
    return 28;
}

Font CabbageLookAndFeel::getAlertWindowMessageFont()
{
    return Font (15.0f);
}

Font CabbageLookAndFeel::getAlertWindowFont()
{
    return Font (12.0f);
}

//tabbed component methods
//==============================================================================
int CabbageLookAndFeel::getTabButtonOverlap (int tabDepth)
{
    return 1 + tabDepth / 3;
}

int CabbageLookAndFeel::getTabButtonSpaceAroundImage()
{
    return 4;
}

int CabbageLookAndFeel::getTabButtonBestWidth (TabBarButton& button, int tabDepth)
{
    int width = Font (tabDepth * 0.6f).getStringWidth (button.getButtonText().trim())
                  + getTabButtonOverlap (tabDepth) * 2;

    Component* const extraComponent = button.getExtraComponent();

    if (extraComponent != nullptr)
        width += button.getTabbedButtonBar().isVertical() ? extraComponent->getHeight()
                                                          : extraComponent->getWidth();

    return jlimit (tabDepth * 2, tabDepth * 8, width);
}

Rectangle<int> CabbageLookAndFeel::getTabButtonExtraComponentBounds (const TabBarButton& button, Rectangle<int>& textArea, Component& comp)
{
    Rectangle<int> extraComp;

    const TabbedButtonBar::Orientation orientation = button.getTabbedButtonBar().getOrientation();

    if (button.getExtraComponentPlacement() == TabBarButton::beforeText)
    {
        switch (orientation)
        {
            case TabbedButtonBar::TabsAtBottom:
            case TabbedButtonBar::TabsAtTop:     extraComp = textArea.removeFromLeft   (comp.getWidth()); break;
            case TabbedButtonBar::TabsAtLeft:    extraComp = textArea.removeFromBottom (comp.getHeight()); break;
            case TabbedButtonBar::TabsAtRight:   extraComp = textArea.removeFromTop    (comp.getHeight()); break;
            default:                             jassertfalse; break;
        }
    }
    else
    {
        switch (orientation)
        {
            case TabbedButtonBar::TabsAtBottom:
            case TabbedButtonBar::TabsAtTop:     extraComp = textArea.removeFromRight  (comp.getWidth()); break;
            case TabbedButtonBar::TabsAtLeft:    extraComp = textArea.removeFromTop    (comp.getHeight()); break;
            case TabbedButtonBar::TabsAtRight:   extraComp = textArea.removeFromBottom (comp.getHeight()); break;
            default:                             jassertfalse; break;
        }
    }

    return extraComp;
}

void CabbageLookAndFeel::createTabButtonShape (TabBarButton& button, Path& p, bool /*isMouseOver*/, bool /*isMouseDown*/)
{
    const Rectangle<int> activeArea (button.getActiveArea());
    const float w = (float) activeArea.getWidth();
    const float h = (float) activeArea.getHeight();

    float length = w;
    float depth = h;

    if (button.getTabbedButtonBar().isVertical())
        std::swap (length, depth);

    const float indent = (float) getTabButtonOverlap ((int) depth);
    const float overhang = 4.0f;

    switch (button.getTabbedButtonBar().getOrientation())
    {
        case TabbedButtonBar::TabsAtLeft:
            p.startNewSubPath (w, 0.0f);
            p.lineTo (0.0f, indent);
            p.lineTo (0.0f, h - indent);
            p.lineTo (w, h);
            p.lineTo (w + overhang, h + overhang);
            p.lineTo (w + overhang, -overhang);
            break;

        case TabbedButtonBar::TabsAtRight:
            p.startNewSubPath (0.0f, 0.0f);
            p.lineTo (w, indent);
            p.lineTo (w, h - indent);
            p.lineTo (0.0f, h);
            p.lineTo (-overhang, h + overhang);
            p.lineTo (-overhang, -overhang);
            break;

        case TabbedButtonBar::TabsAtBottom:
            p.startNewSubPath (0.0f, 0.0f);
            p.lineTo (indent, h);
            p.lineTo (w - indent, h);
            p.lineTo (w, 0.0f);
            p.lineTo (w + overhang, -overhang);
            p.lineTo (-overhang, -overhang);
            break;

        default:
            p.startNewSubPath (0.0f, h);
            p.lineTo (indent, 0.0f);
            p.lineTo (w - indent, 0.0f);
            p.lineTo (w, h);
            p.lineTo (w + overhang, h + overhang);
            p.lineTo (-overhang, h + overhang);
            break;
    }

    p.closeSubPath();

    p = p.createPathWithRoundedCorners (3.0f);
}

void CabbageLookAndFeel::fillTabButtonShape (TabBarButton& button, Graphics& g, const Path& path,  bool /*isMouseOver*/, bool /*isMouseDown*/)
{
    const Colour tabBackground (button.getTabBackgroundColour());
	//const Colour tabBackground (CabbageUtils::getBackgroundSkin());
    const bool isFrontTab = button.isFrontTab();

    g.setColour (isFrontTab ? tabBackground
                            : tabBackground.withMultipliedAlpha (0.9f));
	//g.setColour(CabbageUtils::getBackgroundSkin());
    g.fillPath (path);

    g.setColour (button.findColour (isFrontTab ? TabbedButtonBar::frontOutlineColourId
                                               : TabbedButtonBar::tabOutlineColourId, false)
                    .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    g.strokePath (path, PathStrokeType (isFrontTab ? 1.0f : 0.5f));
}

void CabbageLookAndFeel::drawTabButtonText (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
{
    const Rectangle<float> area (button.getTextArea().toFloat());

    float length = area.getWidth();
    float depth  = area.getHeight();

    if (button.getTabbedButtonBar().isVertical())
        std::swap (length, depth);

    Font font (depth * 0.6f);
    font.setUnderline (button.hasKeyboardFocus (false));

    GlyphArrangement textLayout;
    textLayout.addFittedText (font, button.getButtonText().trim(),
                              0.0f, 0.0f, (float) length, (float) depth,
                              Justification::centred,
                              jmax (1, ((int) depth) / 12));

    AffineTransform t;

    switch (button.getTabbedButtonBar().getOrientation())
    {
        case TabbedButtonBar::TabsAtLeft:   t = t.rotated (float_Pi * -0.5f).translated (area.getX(), area.getBottom()); break;
        case TabbedButtonBar::TabsAtRight:  t = t.rotated (float_Pi *  0.5f).translated (area.getRight(), area.getY()); break;
        case TabbedButtonBar::TabsAtTop:
        case TabbedButtonBar::TabsAtBottom: t = t.translated (area.getX(), area.getY()); break;
        default:                            jassertfalse; break;
    }

    Colour col;

    if (button.isFrontTab() && (button.isColourSpecified (TabbedButtonBar::frontTextColourId)
                                    || isColourSpecified (TabbedButtonBar::frontTextColourId)))
        col = findColour (TabbedButtonBar::frontTextColourId);
    else if (button.isColourSpecified (TabbedButtonBar::tabTextColourId)
                 || isColourSpecified (TabbedButtonBar::tabTextColourId))
        col = findColour (TabbedButtonBar::tabTextColourId);
    else
        col = button.getTabBackgroundColour().contrasting();

	//hardcode text for tab bubtton
	//col = Colours::cornflowerblue;
    const float alpha = button.isEnabled() ? ((isMouseOver || isMouseDown) ? 1.0f : 0.8f) : 0.3f;

    g.setColour (col.withMultipliedAlpha (alpha));
    textLayout.draw (g, t);
}

void CabbageLookAndFeel::drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown)
{
    Path tabShape;
    createTabButtonShape (button, tabShape, isMouseOver, isMouseDown);

    const Rectangle<int> activeArea (button.getActiveArea());
    tabShape.applyTransform (AffineTransform::translation ((float) activeArea.getX(),
                                                           (float) activeArea.getY()));

    DropShadow (Colours::black.withAlpha (0.5f), 2, Point<int> (0, 1)).drawForPath (g, tabShape);

    fillTabButtonShape (button, g, tabShape, isMouseOver, isMouseDown);
    drawTabButtonText (button, g, isMouseOver, isMouseDown);
}

void CabbageLookAndFeel::drawTabAreaBehindFrontButton (TabbedButtonBar& bar, Graphics& g, const int w, const int h)
{
	const float shadowSize = 0.2f;

    Rectangle<int> shadowRect, line;
    ColourGradient gradient (Colours::black.withAlpha (bar.isEnabled() ? 0.3f : 0.15f), 0, 0,
                             Colours::transparentBlack, 0, 0, false);

    switch (bar.getOrientation())
    {
        case TabbedButtonBar::TabsAtLeft:
            gradient.point1.x = (float) w;
            gradient.point2.x = w * (1.0f - shadowSize);
            shadowRect.setBounds ((int) gradient.point2.x, 0, w - (int) gradient.point2.x, h);
            line.setBounds (w - 1, 0, 1, h);
            break;

        case TabbedButtonBar::TabsAtRight:
            gradient.point2.x = w * shadowSize;
            shadowRect.setBounds (0, 0, (int) gradient.point2.x, h);
            line.setBounds (0, 0, 1, h);
            break;

        case TabbedButtonBar::TabsAtTop:
            gradient.point1.y = (float) h;
            gradient.point2.y = h * (1.0f - shadowSize);
            shadowRect.setBounds (0, (int) gradient.point2.y, w, h - (int) gradient.point2.y);
            line.setBounds (0, h - 1, w, 1);
            break;

        case TabbedButtonBar::TabsAtBottom:
            gradient.point2.y = h * shadowSize;
            shadowRect.setBounds (0, 0, w, (int) gradient.point2.y);
            line.setBounds (0, 0, w, 1);
            break;

        default: break;
    }

    g.setGradientFill (gradient);
    g.fillRect (shadowRect.expanded (2, 2));

    g.setColour (Colour (0x80000000));
    g.fillRect (line);
}

Button* CabbageLookAndFeel::createTabBarExtrasButton()
{
    const float thickness = 7.0f;
    const float indent = 22.0f;

    Path p;
    p.addEllipse (-10.0f, -10.0f, 120.0f, 120.0f);

    DrawablePath ellipse;
    ellipse.setPath (p);
    ellipse.setFill (Colours::red);

    p.clear();
    p.addEllipse (0.0f, 0.0f, 100.0f, 100.0f);
    p.addRectangle (indent, 50.0f - thickness, 100.0f - indent * 2.0f, thickness * 2.0f);
    p.addRectangle (50.0f - thickness, indent, thickness * 2.0f, 50.0f - indent - thickness);
    p.addRectangle (50.0f - thickness, 50.0f + thickness, thickness * 2.0f, 50.0f - indent - thickness);
    p.setUsingNonZeroWinding (false);

    DrawablePath dp;
    dp.setPath (p);
    dp.setFill (Colour (0x59000000));

    DrawableComposite normalImage;
    normalImage.addAndMakeVisible (ellipse.createCopy());
    normalImage.addAndMakeVisible (dp.createCopy());

    dp.setFill (Colour (0xcc000000));

    DrawableComposite overImage;
    overImage.addAndMakeVisible (ellipse.createCopy());
    overImage.addAndMakeVisible (dp.createCopy());

    DrawableButton* db = new DrawableButton ("tabs", DrawableButton::ImageFitted);
    db->setImages (&normalImage, &overImage, nullptr);
    return db;
}

//table header 
void CabbageLookAndFeel::drawTableHeaderBackground (Graphics& g, TableHeaderComponent& header)
{
    g.fillAll (Colours::black);

    const int w = header.getWidth();
    const int h = header.getHeight();

    g.setGradientFill (ColourGradient (CabbageUtils::getBackgroundSkin(), 0.0f, h * 0.5f,
                                       Colours::black, 0.0f, h - 1.0f,
                                       false));
    g.fillRect (0, h / 2, w, h);

    g.setColour (Colour (0x33000000));
    g.fillRect (0, h - 1, w, 1);

    for (int i = header.getNumColumns (true); --i >= 0;)
        g.fillRect (header.getColumnPosition (i).getRight() - 1, 0, 1, h - 1);
}

void CabbageLookAndFeel::drawTableHeaderColumn (Graphics& g, const String& columnName, int /*columnId*/,
                                         int width, int height,
                                         bool isMouseOver, bool isMouseDown,
                                         int columnFlags)
{
    if (isMouseDown)
        g.fillAll (Colour (0x8899aadd));
    else if (isMouseOver)
        g.fillAll (Colour (0x5599aadd));

    int rightOfText = width - 4;

    if ((columnFlags & (TableHeaderComponent::sortedForwards | TableHeaderComponent::sortedBackwards)) != 0)
    {
        const float top = height * ((columnFlags & TableHeaderComponent::sortedForwards) != 0 ? 0.35f : (1.0f - 0.35f));
        const float bottom = height - top;

        const float w = height * 0.5f;
        const float x = rightOfText - (w * 1.25f);
        rightOfText = (int) x;

        Path sortArrow;
        sortArrow.addTriangle (x, bottom, x + w * 0.5f, top, x + w, bottom);

        g.setColour (Colour (0x99000000));
        g.fillPath (sortArrow);
    }

	//overriding colour
    g.setColour (Colours::cornflowerblue);
    g.setFont (Font (height * 0.5f, Font::bold));
    const int textX = 4;
    g.drawFittedText (columnName, textX, 0, rightOfText - textX, height, Justification::centredLeft, 1);
}

/*
  =========================================================================================================
	
	Alternative Look and Feel Class
	
  =========================================================================================================
*/
CabbageLookAndFeelBasic::CabbageLookAndFeelBasic()
{

}

CabbageLookAndFeelBasic::~CabbageLookAndFeelBasic()
{
}

//=========== Linear Slider Background ====================================================================
void CabbageLookAndFeelBasic::drawLinearSliderBackground (Graphics &g, int /*x*/, int /*y*/, int /*width*/, int /*height*/, 
																					float sliderPos, 
																					float /*minSliderPos*/, 
																					float /*maxSliderPos*/, 
																					const Slider::SliderStyle /*style*/, 
																					Slider &slider)
{
	//h slider
	slider.setTextBoxStyle (Slider::TextBoxRight, true, slider.getWidth()*0.25, 15); 
	float availableWidth = slider.getWidth() * 0.65f;
	g.setColour(Colour::fromRGB(20, 20, 20));
	g.fillRoundedRectangle (0, slider.getHeight()*0.3, availableWidth, slider.getHeight()*0.4, slider.getHeight() / 20.0f);
	g.setColour(Colour::fromRGB(10, 10, 10));
	g.drawRoundedRectangle (0.25f, slider.getHeight()*0.3 + 0.25f, availableWidth - 0.5f, slider.getHeight()*0.4 - 0.5f, slider.getHeight() / 20.0f, 0.5f);

	// Slider is enabled and value changed only if mouse click is within the actual slider area...
	float sliderPosProportional;
    if (slider.isMouseButtonDown()) {
        Point<int> mousePos = slider.getMouseXYRelative();
		slider.setEnabled(true);
        if ((mousePos.getX() >= 0) && (mousePos.getX() <= availableWidth)) {
			sliderPosProportional = mousePos.getX() / availableWidth;
				
			slider.setValue(slider.proportionOfLengthToValue(sliderPosProportional)); //takes into account the skew factor
		}
    }
    slider.setEnabled (false); // disabling slider	
    sliderPosProportional = slider.valueToProportionOfLength(slider.getValue()); // Final slider position in proportion to length...

	//For the fill
	//float div = (slider.getValue()-slider.getMinimum()) / (slider.getMaximum()-slider.getMinimum());
	sliderPos = sliderPosProportional * availableWidth;  //div * availableWidth;
	Colour fillColour = Colours::cornflowerblue.withMultipliedAlpha(0.8f);
	if (slider.isMouseButtonDown())
		fillColour = Colours::cornflowerblue.withMultipliedBrightness(2.0f);
	g.setColour(fillColour);
	//g.fillRoundedRectangle (0, slider.getHeight()*0.4, sliderPos, slider.getHeight()*0.2, slider.getHeight() / 20.0f);

	//Fill border
	g.setColour(Colours::black);
	g.drawRoundedRectangle(0.5f, slider.getHeight()*0.3 + 0.5f, sliderPos - 1.0f, slider.getHeight()*0.4 - 1.0f, slider.getHeight() / 20.0f, 1.0f);
}

//=========== Linear Thumb =================================================================================
void CabbageLookAndFeelBasic::drawLinearSliderThumb (Graphics &g, int /*x*/, int /*y*/, int /*width*/, int /*height*/, 
																						float sliderPos, 
																						float /*minSliderPos*/, 
																						float /*maxSliderPos*/, 
																						const Slider::SliderStyle /*style*/, 
																						Slider &slider)
{
	//h sliders
	float thumbWidth = slider.getHeight() * 0.7f;
	float div = (slider.getValue()-slider.getMinimum()) / (slider.getMaximum()-slider.getMinimum());
	float availableWidth = slider.getWidth() * 0.65f;
	sliderPos = div * availableWidth;
	float sliderPosProportional = sliderPos / availableWidth;
	
	float destX = (sliderPos - (sliderPosProportional * thumbWidth));
	float destY = ((slider.getHeight() / 2) - (thumbWidth / 2));

	//thumb fill	 
	ColourGradient thumbColour = ColourGradient(findColour (Slider::thumbColourId), destX, destY, 
		Colour::fromRGB(0, 0, 0), destX+thumbWidth, thumbWidth, false);

	g.setGradientFill(thumbColour);
	g.fillRoundedRectangle(destX, destY, thumbWidth, thumbWidth, slider.getHeight() / 20.0f);

	//thumb border
	g.setColour(Colours::black);
	g.drawRoundedRectangle(destX + 0.5f, destY + 0.5f, thumbWidth - 1.0f, thumbWidth - 1.0f, slider.getHeight() / 20.0f, 1.0f);
}

//=========================================================================================================
void CabbageLookAndFeelBasic::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour, 
																							bool isMouseOverButton, 
																							bool isButtonDown)
{
    const int w = button.getWidth();
    const int h = button.getHeight();

    const float indent = 2.0f;
    const int cornerSize = jmin (roundToInt (w * 0.1f),
                                 roundToInt (h * 0.3f));

	g.drawRoundedRectangle(0, 0, w, h, 5, 1.f);

	g.setColour(CabbageUtils::getComponentSkin());	
	g.fillRoundedRectangle(0, 0, w, h, 5);

	g.setColour (CabbageUtils::getComponentFontColour());
	g.setFont (CabbageUtils::getComponentFont());
	g.drawFittedText (button.getName(),
					  0 + 4, 0 + 2, w - 8, h - 4,
					  Justification::centred, 2);

	g.setOpacity(0.2);
	g.drawRoundedRectangle(0.5, 0.5, w-1, h-1, 5, 1.0f);
/*
	Colour bc (backgroundColour);

    Path p;
    p.addRoundedRectangle (indent, indent,
                           width - indent * 2.0f,
                           height - indent * 2.0f,
                           (float) cornerSize);
    g.setColour (bc.withSaturation(0.f));
    g.strokePath (p, PathStrokeType (2.0f));


    

    if (isMouseOverButton)
    {
        if (isButtonDown)
            bc = bc.brighter();
        else if (bc.getBrightness() > 0.5f)
            bc = bc.darker (0.3f);
        else
            bc = bc.brighter (0.3f);
    }
	//Logger::writeToLog(backgroundColour.toDisplayString(false));
    g.setColour (bc);
    g.fillPath (p);



    g.setColour (bc.withSaturation ((isMouseOverButton) ? 0.1f : 0.0f));
    g.strokePath (p, PathStrokeType ((isMouseOverButton) ? 2.0f : 2.0f));
	 */
}

//======== Scrollbars ==============================================================================
void CabbageLookAndFeelBasic::drawScrollbar (Graphics &g, ScrollBar &scrollbar, int x, int y, int width, 
																						int height, 
																						bool isScrollbarVertical, 
																						int thumbStartPosition, 
																						int thumbSize, 
																						bool isMouseOver, 
																						bool isMouseDown)
{
	g.setColour (Colours::transparentBlack);
	g.fillAll();

	g.setColour (CabbageUtils::getComponentSkin());
	g.drawRoundedRectangle (x, y, width, height, 5, 1);

	if (isScrollbarVertical == false) //horizontal
		g.fillRoundedRectangle (thumbStartPosition+3, 3, thumbSize-6, height-6, 5);
	else //vertical
		g.fillRoundedRectangle (3, thumbStartPosition+3, width-6, thumbSize-6, 5);

}

//=========== Labels, slider textboxes are also labels =============================================
void CabbageLookAndFeelBasic::drawLabel (Graphics &g, Label &label)
{	
	// Getting parent component of label
	Component* comp = label.getParentComponent();
	
	// If slider
	if (dynamic_cast<Slider*>(comp)) { 
		//g.setColour(CabbageUtils::getDarkerBackgroundSkin());
		//g.fillRoundedRectangle (0, 0, label.getWidth(), label.getHeight(), label.getHeight() / 10.0f);
		g.setColour(CabbageUtils::getComponentFontColour());
		g.setFont(CabbageUtils::getValueFont());
		g.drawText (label.getText(), 0, 0, label.getWidth(), label.getHeight(), Justification::left, false);
		/* Border
		g.setColour(CabbageUtils::getBorderColour());
		float borderWidth = CabbageUtils::getBorderWidth();
		g.drawRoundedRectangle(borderWidth/2, borderWidth/2, label.getWidth()-borderWidth, label.getHeight()-borderWidth,
			label.getHeight()/5, borderWidth);*/
	}
	// Else If not a slider
	else { 
		g.setColour (Colours::transparentBlack);
		g.fillRoundedRectangle (0, 0, label.getWidth(), label.getHeight(), label.getHeight()/5);

		// For the text
		g.setFont (CabbageUtils::getComponentFont());
		String col = label.getProperties().getWithDefault("textColour", "");
		if (col.length()>0)
			g.setColour(Colour::fromString(col));
		else
			g.setColour (CabbageUtils::getComponentFontColour());

		g.drawText (label.getText(), 0, 0, label.getWidth(), label.getHeight(), Justification::centred, false);
	}
}

//======== Document Window title bar ===================================================================
void CabbageLookAndFeelBasic::drawDocumentWindowTitleBar (DocumentWindow &window, Graphics &g, int w, int h, 
																						int /*titleSpaceX*/, 
																						int titleSpaceW, 
																						const Image */*icon*/, 
																						bool /*drawTitleTextOnLeft*/)
{
	window.setUsingNativeTitleBar(false);
	window.setOpaque(true);
	g.setColour (CabbageUtils::getComponentSkin().brighter());
	g.fillRoundedRectangle(0, 0, w, h+20, 10);

	g.setColour (CabbageUtils::getComponentFontColour());
	Font font = CabbageUtils::getTitleFont();
#ifndef MACOSX
	font.setFallbackFontName("Verdana"); 
#endif
	font.setHeight(16);
	g.setFont (font);
	g.drawText (CabbageUtils::cabbageString(window.getName(), font, titleSpaceW), (w/2)-(titleSpaceW/2), 
									(h/2)-(font.getHeight()/2), titleSpaceW, font.getHeight(), 36, false);
}

//======== Popup Menu background ======================================================================
void CabbageLookAndFeelBasic::drawPopupMenuBackground(Graphics &g, int width, int height)
{
	g.setColour (CabbageUtils::getDarkerBackgroundSkin());
    g.fillAll();

	g.setColour (CabbageUtils::getComponentSkin());
	g.drawRect (0, -5, width, height+5, 1); //dont want to see top line
}

//======== Menubar background ======================================================================
void CabbageLookAndFeelBasic::drawMenuBarBackground(Graphics &g, int width, int height, bool isMouseOverBar, MenuBarComponent &menuBar)
{
	g.setColour (CabbageUtils::getBackgroundSkin());
	g.fillAll();
}

//======== Menubar item background ======================================================================
void CabbageLookAndFeelBasic::drawMenuBarItem(Graphics & g, int width, int height, int itemIndex,
                                                                              const String &itemText, 
																			  bool isMouseOverItem,
                                                                              bool isMenuOpen, 
																			  bool isMouseOverBar,
                                                                              MenuBarComponent &menuBar)
{
	if ((isMouseOverItem == true) || (isMenuOpen == true)) {
		g.setColour (CabbageUtils::getBackgroundSkin().darker (0.2));
		g.fillRect (0, 0, width, height);
	}

	g.setColour (CabbageUtils::getComponentFontColour());
	g.setFont (CabbageUtils::getComponentFont());
    g.drawFittedText(CabbageUtils::cabbageString(itemText, CabbageUtils::getComponentFont(), width*0.9), 5, 0, width, height, 36, 1);
}

//====== Returns image of a check mark ==============================================
Image CabbageLookAndFeelBasic::drawCheckMark()
{
	Image img = Image(Image::ARGB, 10, 10, true);
	Graphics g(img);

	Path path;
	path.startNewSubPath(3, 7);
	path.lineTo(5, 10);
	path.lineTo(10, 0);
	g.setColour (Colours::cornflowerblue);
	g.strokePath(path, PathStrokeType(2.0f));

	return img;
}

void CabbageLookAndFeelBasic::drawResizableWindowBorder (Graphics &g, int w, int h, const 
																BorderSize< int > &border, 
																ResizableWindow &window)
{

}

//======== Popup Menu Items ===========================================================================
void CabbageLookAndFeelBasic::drawPopupMenuItem (Graphics &g, int width, int height, bool isSeparator, bool /*isActive*/, 
																								bool isHighlighted, 
																								bool isTicked, 
																								bool hasSubMenu, 
																								const String &text, 
																								const String &shortcutKeyText, 
																								Image */*image*/, 
																								const Colour */*const textColour*/)
{
	if ((isHighlighted == true) && (isSeparator == false)) {
		g.setColour (CabbageUtils::getComponentSkin());
		g.fillAll();
		g.setColour (Colours::cornflowerblue);
	}
	else
		g.setColour (CabbageUtils::getComponentFontColour());

	g.setFont (CabbageUtils::getComponentFont());
	g.drawText (CabbageUtils::cabbageString(text, CabbageUtils::getComponentFont(), width*0.8), 20, 0, width*0.8, height, 1, false);

	if (isSeparator == true) {
        g.setColour(CabbageUtils::getComponentSkin());
        g.drawLine(0, height/2, width, 3);
    }

	if (isTicked) {
		Image checkMark = drawCheckMark();
		g.setColour(Colours::cornflowerblue);
		g.drawImage(checkMark, 5, (height/2)-5, 10, 10, 0, 0, 10, 10, false);	
	}
	if (hasSubMenu) {
		g.setColour(Colours::cornflowerblue);
		const Line<float> line(width-15, height*.5, width-5, height*.5);
		g.drawArrow(line, 0, height*.3, height*.3);
	}
        if (shortcutKeyText.isNotEmpty())
        {
			const int leftBorder = (height * 5) / 4;
			const int rightBorder = 4;

            g.drawText (shortcutKeyText,
                        leftBorder, 0, width - (leftBorder + rightBorder + 4), height,
                        Justification::centredRight,
                        true);
        }
}
