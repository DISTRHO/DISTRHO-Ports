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

#include "ToneStackGroup.h"

ToneStackGroup::ToneStackGroup() : ParameterGroup("TONE")
{
  addAndMakeVisible(sliderLow);
  sliderLow.setLabel("LOW");
  sliderLow.slider.setPosMapDownFmt("%4.1f");

  addAndMakeVisible(sliderMid);
  sliderMid.setLabel("MID");
  sliderMid.slider.setPosMapDownFmt("%4.1f");

  addAndMakeVisible(sliderHigh);
  sliderHigh.setLabel("HIGH");
  sliderHigh.slider.setPosMapDownFmt("%4.1f");

  addAndMakeVisible(sliderPresence);
  sliderPresence.setLabel("PRESENCE");
  sliderPresence.slider.setPosMapDownFmt("%4.1f");
}

void ToneStackGroup::attachVTS(AudioProcessorValueTreeState& vts)
{
  attLow.reset(new SliderAttachment(vts, "idTsLow", sliderLow.slider));
  attMid.reset(new SliderAttachment(vts, "idTsMid", sliderMid.slider));
  attHigh.reset(new SliderAttachment(vts, "idTsHigh", sliderHigh.slider));
  attPresence.reset(
      new SliderAttachment(vts, "idTsPresence", sliderPresence.slider));
}

void ToneStackGroup::attachTooltips(const TooltipsData& tooltips)
{
  sliderLow.slider.setTooltip(tooltips.getForParam("idTsLow"));
  sliderMid.slider.setTooltip(tooltips.getForParam("idTsMid"));
  sliderHigh.slider.setTooltip(tooltips.getForParam("idTsHigh"));
  sliderPresence.slider.setTooltip(tooltips.getForParam("idTsPresence"));
}

void ToneStackGroup::resized()
{
  const int prevInnerHeight = getBorderBounds().getHeight() - 2 * spacing;
  const Point<int> prevCorner =
      getBorderBounds().getTopLeft() + Point<int>(spacing, spacing);

  ParameterGroup::resized();

  const int innerHeight = getBorderBounds().getHeight() - 2 * spacing;
  Point<int> corner =
      getBorderBounds().getTopLeft() + Point<int>(spacing, spacing);

  // only re-set the positions when the height or position changes
  if (prevInnerHeight == innerHeight && prevCorner == corner) return;

  sliderLow.setTopLeftPosition(corner);
  sliderLow.slider.setMargin(0.15f * (float)innerHeight);
  sliderLow.setHeight(innerHeight);

  corner = sliderLow.getBounds().getTopRight() + Point<int>(spacing, 0);

  sliderMid.setTopLeftPosition(corner);
  sliderMid.slider.setMargin(0.15f * (float)innerHeight);
  sliderMid.setHeight(innerHeight);

  corner = sliderMid.getBounds().getTopRight() + Point<int>(spacing, 0);

  sliderHigh.setTopLeftPosition(corner);
  sliderHigh.slider.setMargin(0.15f * (float)innerHeight);
  sliderHigh.setHeight(innerHeight);

  corner = sliderHigh.getBounds().getTopRight() + Point<int>(spacing, 0);

  sliderPresence.setTopLeftPosition(corner);
  sliderPresence.slider.setMargin(0.15f * (float)innerHeight);
  sliderPresence.setHeight(innerHeight);

  corner = sliderPresence.getBounds().getTopRight() + Point<int>(spacing, 0);

  // can now determine the width and set it, this will re-call `resized` but
  // since the height is the same it won't re-do the calculation
  setSize(corner.getX() - getBounds().getX(), getHeight());
}
