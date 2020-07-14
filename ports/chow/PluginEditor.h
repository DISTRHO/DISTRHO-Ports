/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "MyLNF.h"
#include "ChowSlider.h"

//==============================================================================
/**
*/
class ChowAudioProcessorEditor  : public AudioProcessorEditor,
                                  public Slider::Listener
{
public:
    ChowAudioProcessorEditor (ChowAudioProcessor&);
    ~ChowAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    AudioParameterFloat* getParamForSlider (Slider* slider);
    Slider* getSliderForParam (AudioParameterFloat* param);
    Button* getButtonForParam (AudioParameterBool* param);
    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    void sliderDragEnded (Slider* slider) override;

private:
    void initSliders();
    void initLabels();
    void initVisualizer();
    void initButtons();

    ChowAudioProcessor& processor;

    Slider threshSlide;
    Label threshLabel;

    Slider ratioSlide;
    Label ratioLabel;

    ChowSlider inGainSlide;
    Label inGainLabel;

    ChowSlider outGainSlide;
    Label outGainLabel;

    TextButton flipButton;
    TextButton rectButton;

    MyLNF myLNF;

    AudioVisualiserComponent& visualizer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChowAudioProcessorEditor)
};
