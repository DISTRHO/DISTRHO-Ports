/*
  ==============================================================================

    ScaleComponent.h
    Created: 27 Aug 2021 1:26:08pm
    Author:  discoDSP

  ==============================================================================
*/

#pragma once

//==============================================================================
#include <JuceHeader.h>
class ObxdAudioProcessor;




//==============================================================================
class ScalableComponent
{
public:
    ~ScalableComponent();
    int getScaleInt();
    void setScaleFactor(float newScaleFactor, bool newIsHighResolutionDisplay);
    float getScaleImage();
    float getScaleFactor() const;
	bool getIsHighResolutionDisplay() const;

    virtual void scaleFactorChanged();

protected:
    ScalableComponent(ObxdAudioProcessor* owner_);

    Image getScaledImageFromCache(const String& imageName, float scaleFactor, bool isHighResolutionDisplay);

private:
    ObxdAudioProcessor* processor;
    float scaleFactor;
	bool isHighResolutionDisplay;
    
};


//==============================================================================
class CustomLookAndFeel : public LookAndFeel_V4,
                             public ScalableComponent
{
public:
    CustomLookAndFeel(ObxdAudioProcessor* owner_):LookAndFeel_V4(), ScalableComponent(owner_) {
        this->setColour(PopupMenu::backgroundColourId, Colour(20, 20, 20));
        this->setColour(PopupMenu::textColourId, Colour(245, 245, 245));
        this->setColour(PopupMenu::highlightedBackgroundColourId, Colour(60, 60, 60));
        this->setColour(Label::textColourId, Colour(245, 245, 245));

    };
    

    PopupMenu::Options getOptionsForComboBoxPopupMenu (ComboBox& box, Label& label) override
    {
        PopupMenu::Options option = LookAndFeel_V4::getOptionsForComboBoxPopupMenu(box, label);
        return option.withStandardItemHeight (label.getHeight()/ getScaleFactor());
    };
    Font getPopupMenuFont () override
    {

        float scaleFactor = getScaleFactor();
        DBG("getPopupMenuFont::scaleFactor " << scaleFactor);
        if (scaleFactor > 1.0) scaleFactor *= 0.85;

        
        #ifdef JUCE_MAC
            return Font("Helvetica Neue", 18.0 * scaleFactor, Font::plain);
        #endif
            
        #ifdef JUCE_WINDOWS
            return Font("Arial", 18.0 * scaleFactor, Font::plain);
        #endif

        #ifdef JUCE_LINUX
            return Font("DejaVu Sans", 18.0 * scaleFactor, Font::plain);
        #endif
    }
 
};
