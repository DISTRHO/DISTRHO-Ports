/*
 * Stutter Juice Plugin
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

#include "StutterJuiceUI.hpp"

using DGL::Point;

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

StutterJuiceUI::StutterJuiceUI()
    : UI(),
      fAboutWindow(this)
{


    // background
    fImgBackground = Image(StutterJuiceArtwork::backgroundData, StutterJuiceArtwork::backgroundWidth, StutterJuiceArtwork::backgroundHeight, GL_BGR);

    // about
    Image imageAbout(StutterJuiceArtwork::aboutData, StutterJuiceArtwork::aboutWidth, StutterJuiceArtwork::aboutHeight, GL_BGRA);
    fAboutWindow.setImage(imageAbout);

    // about button
    Image aboutImageNormal(StutterJuiceArtwork::aboutButtonNormalData, StutterJuiceArtwork::aboutButtonNormalWidth, StutterJuiceArtwork::aboutButtonNormalHeight);
    Image aboutImageHover(StutterJuiceArtwork::aboutButtonHoverData, StutterJuiceArtwork::aboutButtonHoverWidth, StutterJuiceArtwork::aboutButtonHoverHeight);
    fButtonAbout = new ImageButton(this, aboutImageNormal, aboutImageHover, aboutImageHover);
    fButtonAbout->setPos(358, 17);
    fButtonAbout->setCallback(this);


	//sliders
	// sliders
    Image sliderImage(StutterJuiceArtwork::sliderData, StutterJuiceArtwork::sliderWidth, StutterJuiceArtwork::sliderHeight);
    Point<int> sliderPosStart(293, 74);
    Point<int> sliderPosEnd(293+11, 74);

    // slider Rev
   

	int oX = 130;
	int oY = 93;
	int mX = 357 - oX;
	int mY = 120-oY;
	int mMY = 208 - oY;
	int w = 235 - oX - StutterJuiceArtwork::sliderWidth;
	
	for (int module=0; module<9; module++) {
		for (int param=0; param<3; param++) {
			sliderPosStart.setX(oX+(module%3)*mX);
			sliderPosStart.setY(oY+(param*mY)  +  (int) (floor(module/3)*mMY));
			printf("ypos: %i\n",  (int) floor(module/3)*mMY+oY);
			sliderPosEnd.setX(sliderPosStart.getX() + w);
			sliderPosEnd.setY(sliderPosStart.getY());
			
			fSliders[module][param]= new ImageSlider(this, sliderImage);
			fSliders[module][param]->setStartPos(sliderPosStart);
			fSliders[module][param]->setEndPos(sliderPosEnd);
			fSliders[module][param]->setRange(0.0f, 1.0f);
			fSliders[module][param]->setValue(0.0f);
			fSliders[module][param]->setCallback(this);
			
		}
	}
	 

}

StutterJuiceUI::~StutterJuiceUI()
{

}

// -----------------------------------------------------------------------
// DSP Callbacks

void StutterJuiceUI::d_parameterChanged(uint32_t index, float value)
{
}

void StutterJuiceUI::d_programChanged(uint32_t index)
{
	if (index != 0)
		return;
}

// -----------------------------------------------------------------------
// Widget Callbacks

void StutterJuiceUI::imageButtonClicked(ImageButton* button, int)
{
    if (button != fButtonAbout)
        return;

    fAboutWindow.exec();
}

void StutterJuiceUI::imageSliderDragStarted(ImageSlider* slider)
{

	for (int module=0; module<9; module++) {
		for (int param=0; param<3; param++) {
			if (slider == fSliders[module][param]) {
				d_editParameter(module*3+param, true);
			}
		}
	}
}

void StutterJuiceUI::imageSliderDragFinished(ImageSlider* slider)
{
	for (int module=0; module<9; module++) {
		for (int param=0; param<3; param++) {
			if (slider == fSliders[module][param]) {
				d_editParameter(module*3+param, false);
			}
		}
	}
}

void StutterJuiceUI::imageSliderValueChanged(ImageSlider* slider, float value)
{
	for (int module=0; module<9; module++) {
		for (int param=0; param<3; param++) {
			if (slider == fSliders[module][param]) {
				d_setParameterValue(module*3+param, value);
			}
		}
	}
}

void StutterJuiceUI::onDisplay()
{
    fImgBackground.draw();
    
}

// -----------------------------------------------------------------------

UI* createUI()
{
    return new StutterJuiceUI();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
