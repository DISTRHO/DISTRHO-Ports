/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
enum
{
    width = 400,
    height = 300,
    titleHeight = 30,

    sliderWidth = 100,
    sliderY = 50,
    labelHeight = 20,
    labelY = 35,

    buttonX = 10,
    buttonY = 7,
    buttonWidth = 50,
    buttonHeight = 25,

    visualizerY = 160,
};


ChowAudioProcessorEditor::ChowAudioProcessorEditor (ChowAudioProcessor& p)
    : AudioProcessorEditor (&p),
    processor (p),
    visualizer (*p.vis.get())
{
    initSliders();
    initLabels();    
    initVisualizer();   
    initButtons();

    setSize (width, height);
}

ChowAudioProcessorEditor::~ChowAudioProcessorEditor()
{
    flipButton.setLookAndFeel (nullptr);
    rectButton.setLookAndFeel (nullptr);
    threshSlide.setLookAndFeel (nullptr);
    ratioSlide.setLookAndFeel (nullptr);
    inGainSlide.setLookAndFeel (nullptr);
    outGainSlide.setLookAndFeel (nullptr);
}

void ChowAudioProcessorEditor::initSliders()
{
    auto setupSlider = [this] (Slider& slide, AudioParameterFloat* param,
                               float skewMidPoint = NAN, String textSuffix = String (" dB"), float step = 0.1f)
    {
        slide.setName (param->name);
        slide.setRange (param->range.start, param->range.end, step);
        if (slide.getRange().contains (skewMidPoint))
            slide.setSkewFactorFromMidPoint ((double) skewMidPoint);

        slide.setLookAndFeel (&myLNF);
        slide.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        slide.setColour (Slider::rotarySliderFillColourId, Colours::darkorange);
        slide.setColour (Slider::rotarySliderOutlineColourId, Colours::darkgrey);
        slide.setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
        slide.setColour (Slider::thumbColourId, Colours::red);
        slide.setColour (Slider::textBoxTextColourId, Colours::darkorange);
        slide.setColour (Slider::textBoxHighlightColourId, Colours::grey);

        slide.setTextBoxStyle (Slider::TextBoxBelow, false, 80, 15);
        slide.setTextValueSuffix (textSuffix);

        slide.setValue (param->get());
        slide.addListener (this);
        addAndMakeVisible (slide);
    };

    setupSlider (threshSlide, processor.threshDB, -30.0f);
    setupSlider (ratioSlide, processor.ratio, 15.0f, String (" : 1"));
    setupSlider (inGainSlide, processor.inGainDB);
    setupSlider (outGainSlide, processor.outGainDB);
}

void ChowAudioProcessorEditor::initLabels()
{
    auto setupLabel = [this] (Label& label, AudioParameterFloat* param)
    {
        label.setText (param->name, dontSendNotification);
        label.setJustificationType (Justification::centred);
        label.setColour (Label::textColourId, Colours::red);
        label.setFont (17.0f);
        addAndMakeVisible (label);
    };

    setupLabel (threshLabel, processor.threshDB);
    setupLabel (ratioLabel, processor.ratio);
    setupLabel (inGainLabel, processor.inGainDB);
    setupLabel (outGainLabel, processor.outGainDB);
}

void ChowAudioProcessorEditor::initVisualizer()
{
    visualizer.setRepaintRate (1000);
    visualizer.setBufferSize (128);
    visualizer.setSamplesPerBlock (processor.getBlockSize());
    visualizer.setColours (Colours::black, Colours::darkorange);
    addAndMakeVisible (visualizer);
}

void ChowAudioProcessorEditor::initButtons()
{
    auto setupButton = [this] (TextButton& button, AudioParameterBool* param)
    {
        button.setName (param->name);
        button.setButtonText (button.getName());

        button.setLookAndFeel (&myLNF);
        button.setColour (TextButton::buttonOnColourId, Colours::darkorange);
        button.setColour (TextButton::textColourOnId, Colours::black);
        button.setColour (TextButton::buttonColourId, Colours::black);
        button.setColour (TextButton::textColourOffId, Colours::darkorange);

        button.setToggleState (param->get(), dontSendNotification);
        button.setClickingTogglesState (true);

        addAndMakeVisible (button);
    };

    setupButton (flipButton, processor.flip);
    flipButton.onClick = [this] { *processor.flip = flipButton.getToggleState(); };

    setupButton (rectButton, processor.rect);
    rectButton.onStateChange = [this]
    { 
        bool newRect = rectButton.getToggleState();
        ratioSlide.setEnabled (! newRect);
    };
    rectButton.onClick  = [this]
    {
        *processor.rect = rectButton.getToggleState();
    };
}

//==============================================================================
void ChowAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);

    g.setColour (Colours::darkorange);
    g.setFont (Font ((float) titleHeight, Font::bold));
    g.drawFittedText ("CHOW", 0, 0, width, titleHeight, Justification::centred, 1);

    g.drawLine (0.0f, (float) visualizerY, (float) width, (float) visualizerY, 5.0f);
}

void ChowAudioProcessorEditor::resized()
{
    rectButton.setBounds (buttonX, buttonY, buttonWidth, buttonHeight);
    flipButton.setBounds (width - buttonWidth - buttonX, buttonY, buttonWidth, buttonHeight);

    inGainLabel.setBounds (0, labelY, sliderWidth, labelHeight);
    threshLabel.setBounds (inGainLabel.getRight(), labelY, sliderWidth, labelHeight);
    ratioLabel.setBounds (threshLabel.getRight(), labelY, sliderWidth, labelHeight);
    outGainLabel.setBounds (ratioLabel.getRight(), labelY, sliderWidth, labelHeight);


    inGainSlide.setBounds (0, sliderY, sliderWidth, sliderWidth);
    threshSlide.setBounds (inGainSlide.getRight(), sliderY, sliderWidth, sliderWidth);
    ratioSlide.setBounds (threshSlide.getRight(), sliderY, sliderWidth, sliderWidth);
    outGainSlide.setBounds (ratioSlide.getRight(), sliderY, sliderWidth, sliderWidth);

    auto bounds = getLocalBounds();
    bounds.removeFromTop (visualizerY);
    visualizer.setBounds (bounds);
}

AudioParameterFloat* ChowAudioProcessorEditor::getParamForSlider (Slider* slider)
{
    if (processor.threshDB->name == slider->getName())
        return processor.threshDB;
    else if (processor.ratio->name == slider->getName())
        return processor.ratio;
    else if (processor.inGainDB->name == slider->getName())
        return processor.inGainDB;
    else if (processor.outGainDB->name == slider->getName())
        return processor.outGainDB;
    else
        return nullptr;
}

Slider* ChowAudioProcessorEditor::getSliderForParam (AudioParameterFloat* param)
{
    if (processor.threshDB->name == param->name)
        return &threshSlide;
    else if (processor.ratio->name == param->name)
        return &ratioSlide;
    else if (processor.inGainDB->name == param->name)
        return &inGainSlide;
    else if (processor.outGainDB->name == param->name)
        return &outGainSlide;
    else
        return nullptr;
}

Button* ChowAudioProcessorEditor::getButtonForParam (AudioParameterBool* param)
{
    if (processor.rect->name == param->name)
        return &rectButton;
    else if (processor.flip->name == param->name)
        return &flipButton;
    else
        return nullptr;
}

void ChowAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (AudioParameterFloat* param = getParamForSlider(slider)){
        *param = (float) slider->getValue();
    }
}

void ChowAudioProcessorEditor::sliderDragStarted(Slider* slider)
{
    if (AudioParameterFloat* param = getParamForSlider(slider))
        param->beginChangeGesture();
}

void ChowAudioProcessorEditor::sliderDragEnded(Slider* slider)
{
    if (AudioParameterFloat* param = getParamForSlider(slider))
        param->endChangeGesture();

}
