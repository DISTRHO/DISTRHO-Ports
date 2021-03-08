/*
 *  Swanky Amp tube amplifier simulation
 *  Copyright (C) 2020  Garrin McGoldrick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <JuceHeader.h>

#include "RSliderLabel.h"

RSliderLabel::RSliderLabel()
{
  addAndMakeVisible(slider);
  addAndMakeVisible(label);
  label.setJustificationType(Justification::centredTop);
  label.setFont(16.0f);
  slider.setMouseDragSensitivity(400);
}

void RSliderLabel::setWidth(int width)
{
  setDimension = SetDimension::SetFromWidth;
  setSize(width, 0);
}

void RSliderLabel::setHeight(int height)
{
  setDimension = SetDimension::SetFromHeight;
  setSize(0, height);
}

void RSliderLabel::setSliderMargin(float margin)
{
  slider.setMargin(margin);
  // since the slider aspect ratio changes, must re-calculate the sizes
  resized();
}

void RSliderLabel::resized()
{
  if (setDimension == SetDimension::NoDimension)
  {
    slider.setSize(0, 0);
    label.setSize(0, 0);
    return;
  }

  const int labelHeight = (int)(label.getFont().getHeight() + 0.5f);

  if (setDimension == SetDimension::SetFromHeight)
  {
    const int sliderHeight = jmax(0, getHeight() - labelHeight);
    const int width =
        (int)(slider.calcWidthForHeight((float)sliderHeight) + 0.5f);
    // note: this does re-trigger `resized`, but it won't recurse because it
    // proceeds only if bounds changed
    setSize(width, getHeight());
  }
  else if (setDimension == SetDimension::SetFromWidth)
  {
    const int sliderHeight =
        (int)(slider.calcHeightForWidth((float)getWidth()) + 0.5f);
    setSize(getWidth(), sliderHeight + labelHeight);
  }

  slider.setSize(getWidth(), getHeight() - labelHeight);
  label.setSize(getWidth(), labelHeight);
  label.setTopLeftPosition(slider.getLocalBounds().getBottomLeft());
}
