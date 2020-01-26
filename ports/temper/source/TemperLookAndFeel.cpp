/*
  ==============================================================================

    TemperLookAndFeel.cpp
    Created: 1 Mar 2017 8:15:09pm
    Author:  Nick Thompson

  ==============================================================================
*/

#include "TemperLookAndFeel.h"

TemperLookAndFeel::TemperLookAndFeel()
{
    setColour(Slider::rotarySliderFillColourId, Colour::fromRGBA(226, 115, 0, 255));
}

Font TemperLookAndFeel::getBaseFont()
{
    return Font(Typeface::createSystemTypefaceFor(BinaryData::MontserratLight_otf,
                                                  BinaryData::MontserratLight_otfSize));
}


Font TemperLookAndFeel::getLabelFont(Label &)
{
    return getBaseFont().withPointHeight(10);
}

Font TemperLookAndFeel::getSliderReadoutFont()
{
    return getBaseFont().withPointHeight(14);
}

void TemperLookAndFeel::drawLabel(Graphics& g, Label& l)
{
    Colour labelColour = Colour::fromRGB(149, 89, 17);
    Font labelFont = getLabelFont(l);

    g.setColour(labelColour);
    g.setFont(labelFont);

    Rectangle<int> textArea (l.getBorderSize().subtractedFrom (l.getLocalBounds()));

    g.drawFittedText (l.getText(), textArea, l.getJustificationType(),
                      jmax (1, (int) (textArea.getHeight() / labelFont.getHeight())),
                      l.getMinimumHorizontalScale());
}

void TemperLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    const float radius = jmin (width / 2, height / 2) - 2.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    // Draw the readout
    Colour readoutColour = Colour::fromRGB(254, 173, 29).withAlpha(isMouseOver ? 1.0f : 0.9f);
    const double value = slider.getValue();
    String readoutValue = (value >= 1000.0 ? String(value / 1000.0, 1) + "k" : String(value, 1));
    String readout = readoutValue + slider.getTextValueSuffix();

    g.setColour(readoutColour);
    g.setFont(getSliderReadoutFont());
    g.drawText(readout, centreX - radius, centreY - 10.0f, rw, 24.0f, Justification::centred);

    // Draw the track
    g.setColour (slider.findColour (Slider::rotarySliderOutlineColourId));

    Path track;
    track.addArc(rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, true);
    g.strokePath(track, PathStrokeType(3.0f));

    // Draw the slider position
    Colour sliderFillStart = Colour::fromRGBA(245, 121, 35, 255).withAlpha(isMouseOver ? 1.0f : 0.9f);
    Colour sliderFillStop = Colour::fromRGBA(255, 184, 23, 255).withAlpha(isMouseOver ? 1.0f : 0.9f);
    ColourGradient sliderFill = ColourGradient(sliderFillStart,
                                               (float) x,
                                               (float) 0,
                                               sliderFillStop,
                                               (float) width,
                                               (float) 0,
                                               false);

    g.setGradientFill(sliderFill);

    Path filledArc;
    filledArc.addArc(rx, ry, rw, rw, rotaryStartAngle, angle, true);
    PathStrokeType(3.0f).createStrokedPath(filledArc, filledArc);
    g.fillPath(filledArc);
}
