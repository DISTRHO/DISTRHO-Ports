/*
 * Trigger Juice Plugin
 * Copyright (C) 2014 Andre Sklenar <andre.sklenar@gmail.com>, www.juicelab.cz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#include "TriggerJuiceUI.hpp"

using DGL::Point;

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

TriggerJuiceUI::TriggerJuiceUI()
    : UI(),
      fAboutWindow(this)
{


    // background
    fImgBackground = Image(TriggerJuiceArtwork::backgroundData, TriggerJuiceArtwork::backgroundWidth, TriggerJuiceArtwork::backgroundHeight, GL_BGRA);

    // about
    Image imageAbout(TriggerJuiceArtwork::aboutData, TriggerJuiceArtwork::aboutWidth, TriggerJuiceArtwork::aboutHeight, GL_BGRA);
    fAboutWindow.setImage(imageAbout);

    // about button
    Image aboutImageNormal(TriggerJuiceArtwork::aboutButtonNormalData, TriggerJuiceArtwork::aboutButtonNormalWidth, TriggerJuiceArtwork::aboutButtonNormalHeight);
    Image aboutImageHover(TriggerJuiceArtwork::aboutButtonHoverData, TriggerJuiceArtwork::aboutButtonHoverWidth, TriggerJuiceArtwork::aboutButtonHoverHeight);
    fButtonAbout = new ImageButton(this, aboutImageNormal, aboutImageHover, aboutImageHover);
    fButtonAbout->setPos(358, 17);
    fButtonAbout->setCallback(this);


	// knobs
    Image knobImage(TriggerJuiceArtwork::knobData, TriggerJuiceArtwork::knobWidth, TriggerJuiceArtwork::knobHeight);

	// knob KnobAttack
    fKnobAttack = new ImageKnob(this, knobImage);
    fKnobAttack->setPos(42, 77);
    fKnobAttack->setRange(0.0f, 1.0f);
    fKnobAttack->setValue(0.0f);
    fKnobAttack->setRotationAngle(270);
    fKnobAttack->setCallback(this);

	// knob KnobRelease
    fKnobRelease = new ImageKnob(this, knobImage);
    fKnobRelease->setPos(139, 77);
    fKnobRelease->setRange(0.0f, 1.0f);
    fKnobRelease->setValue(0.0f);
    fKnobRelease->setRotationAngle(270);
    fKnobRelease->setCallback(this);


	//sliders
	// sliders
    Image sliderImage(TriggerJuiceArtwork::sliderData, TriggerJuiceArtwork::sliderWidth, TriggerJuiceArtwork::sliderHeight);
    Point<int> sliderPosStart(293, 74);
    Point<int> sliderPosEnd(293+11, 74);

    // slider Rev
    fSliderRev = new ImageSlider(this, sliderImage);
    fSliderRev->setStartPos(sliderPosStart);
    fSliderRev->setEndPos(sliderPosEnd);
    fSliderRev->setRange(0.0f, 1.0f);
    fSliderRev->setValue(0.0f);
    fSliderRev->setCallback(this);

    // slider Split
    sliderPosStart.setY(104);
    sliderPosEnd.setY(104);
	fSliderSplit = new ImageSlider(this, sliderImage);
    fSliderSplit->setStartPos(sliderPosStart);
    fSliderSplit->setEndPos(sliderPosEnd);
    fSliderSplit->setRange(0.0f, 1.0f);
    fSliderSplit->setValue(0.0f);
    fSliderSplit->setCallback(this);

	// slider MS
    sliderPosStart.setY(134);
	sliderPosEnd.setY(134);
	fSliderMS = new ImageSlider(this, sliderImage);
    fSliderMS->setStartPos(sliderPosStart);
    fSliderMS->setEndPos(sliderPosEnd);
    fSliderMS->setRange(0.0f, 1.0f);
    fSliderMS->setValue(0.0f);
    fSliderMS->setCallback(this);

}

TriggerJuiceUI::~TriggerJuiceUI()
{
    delete fButtonAbout;

    //knobs
    delete fKnobAttack;
    delete fKnobRelease;

	//sliders
	delete fSliderRev;
	delete fSliderSplit;
	delete fSliderMS;
}

// -----------------------------------------------------------------------
// DSP Callbacks

void TriggerJuiceUI::d_parameterChanged(uint32_t index, float value)
{
    switch (index)
    {
    case TriggerJuicePlugin::paramAttack:
        fKnobAttack->setValue(value);
        break;
    case TriggerJuicePlugin::paramRelease:
        fKnobRelease->setValue(value);
        break;
	case TriggerJuicePlugin::paramRev:
        fSliderRev->setValue(value);
        break;
	case TriggerJuicePlugin::paramSplit:
        fSliderSplit->setValue(value);
        break;
	case TriggerJuicePlugin::paramMS:
        fSliderMS->setValue(value);
        break;
    }
}

void TriggerJuiceUI::d_programChanged(uint32_t index)
{
    if (index != 0)
        return;

    // Default values
    fKnobAttack->setValue(0.0f);
    fKnobRelease->setValue(0.0f);
	fSliderRev->setValue(0.0f);
	fSliderSplit->setValue(0.0f);
	fSliderMS->setValue(0.0f);
}

// -----------------------------------------------------------------------
// Widget Callbacks

void TriggerJuiceUI::imageButtonClicked(ImageButton* button, int)
{
    if (button != fButtonAbout)
        return;

    fAboutWindow.exec();
}

void TriggerJuiceUI::imageKnobDragStarted(ImageKnob* knob)
{
    if (knob == fKnobAttack)
        d_editParameter(TriggerJuicePlugin::paramAttack, true);
    else if (knob == fKnobRelease)
        d_editParameter(TriggerJuicePlugin::paramRelease, true);
}

void TriggerJuiceUI::imageKnobDragFinished(ImageKnob* knob)
{
    if (knob == fKnobAttack)
        d_editParameter(TriggerJuicePlugin::paramAttack, false);
    else if (knob == fKnobRelease)
        d_editParameter(TriggerJuicePlugin::paramRelease, false);
}

void TriggerJuiceUI::imageKnobValueChanged(ImageKnob* knob, float value)
{
    if (knob == fKnobAttack)
        d_setParameterValue(TriggerJuicePlugin::paramAttack, value);
    else if (knob == fKnobRelease)
        d_setParameterValue(TriggerJuicePlugin::paramRelease, value);
}

void TriggerJuiceUI::imageSliderDragStarted(ImageSlider* slider)
{
    if (slider == fSliderRev)
        d_editParameter(TriggerJuicePlugin::paramRev, true);
    else if (slider == fSliderSplit)
        d_editParameter(TriggerJuicePlugin::paramSplit, true);
    else if (slider == fSliderMS)
        d_editParameter(TriggerJuicePlugin::paramMS, true);
}

void TriggerJuiceUI::imageSliderDragFinished(ImageSlider* slider)
{
    if (slider == fSliderRev)
        d_editParameter(TriggerJuicePlugin::paramRev, false);
    else if (slider == fSliderSplit)
        d_editParameter(TriggerJuicePlugin::paramSplit, false);
    else if (slider == fSliderMS)
        d_editParameter(TriggerJuicePlugin::paramMS, false);
}

void TriggerJuiceUI::imageSliderValueChanged(ImageSlider* slider, float value)
{
    if (slider == fSliderRev)
        d_setParameterValue(TriggerJuicePlugin::paramRev, value);
    else if (slider == fSliderSplit)
        d_setParameterValue(TriggerJuicePlugin::paramSplit, value);
    else if (slider == fSliderMS)
        d_setParameterValue(TriggerJuicePlugin::paramMS, value);
}

void TriggerJuiceUI::onDisplay()
{
    fImgBackground.draw();
}

// -----------------------------------------------------------------------

UI* createUI()
{
    return new TriggerJuiceUI();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
