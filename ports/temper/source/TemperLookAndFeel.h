/*
  ==============================================================================

    TemperLookAndFeel.h
    Created: 1 Mar 2017 8:15:08pm
    Author:  Nick Thompson

  ==============================================================================
*/

#ifndef TEMPERLOOKANDFEEL_H_INCLUDED
#define TEMPERLOOKANDFEEL_H_INCLUDED

#include "JuceHeader.h"

class TemperLookAndFeel : public LookAndFeel_V2
{
public:
    TemperLookAndFeel();

    Font getBaseFont ();
    Font getLabelFont (Label&) override;
    Font getSliderReadoutFont ();

    void drawLabel (Graphics&, Label&) override;
    void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           Slider&) override;
};

#endif  // TEMPERLOOKANDFEEL_H_INCLUDED
