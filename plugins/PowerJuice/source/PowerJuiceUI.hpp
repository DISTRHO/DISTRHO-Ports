/*
 * Power Juice Plugin
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

#ifndef POWERJUICEUI_HPP_INCLUDED
#define POWERJUICEUI_HPP_INCLUDED

#include "DistrhoUI.hpp"

#include "ImageAboutWindow.hpp"
#include "ImageButton.hpp"
#include "ImageKnob.hpp"
#include "ImageSlider.hpp"

#include "PowerJuiceArtwork.hpp"
#include "PowerJuicePlugin.hpp"
#include <deque>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <cstdlib>

using DGL::Image;
using DGL::ImageAboutWindow;
using DGL::ImageButton;
using DGL::ImageKnob;

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class PowerJuiceUI : public UI,
                      public ImageButton::Callback,
                      public ImageKnob::Callback
{
public:
    PowerJuiceUI();
    ~PowerJuiceUI() override;

protected:
    // -------------------------------------------------------------------
    // Information

    unsigned int d_getWidth() const noexcept override
    {
        return PowerJuiceArtwork::backgroundWidth;
    }

    unsigned int d_getHeight() const noexcept override
    {
        return PowerJuiceArtwork::backgroundHeight;
    }

    // -------------------------------------------------------------------
    // DSP Callbacks

    void d_parameterChanged(uint32_t index, float value) override;
    void d_programChanged(uint32_t index) override;

    // -------------------------------------------------------------------
    // Widget Callbacks

    void imageButtonClicked(ImageButton* button, int) override;
    void imageKnobDragStarted(ImageKnob* knob) override;
    void imageKnobDragFinished(ImageKnob* knob) override;
    void imageKnobValueChanged(ImageKnob* knob, float value) override;

    void onDisplay() override;

private:
    Image fImgBackground;
    ImageAboutWindow fAboutWindow;

    ImageKnob* fKnobAttack;
    ImageKnob* fKnobRelease;
    ImageKnob* fKnobThreshold;
    ImageKnob* fKnobRatio;
    ImageKnob* fKnobMakeup;
    ImageKnob* fKnobMix;
    ImageButton* fButtonAbout;

	std::deque<float> input, output, gainReduction;


	boost::interprocess::shared_memory_object* shm_obj;
	boost::interprocess::mapped_region* region;


};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif // POWERJUICEUI_HPP_INCLUDED
