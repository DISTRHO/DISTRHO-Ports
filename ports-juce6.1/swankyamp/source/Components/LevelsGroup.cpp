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

#include "LevelsGroup.h"

LevelsGroup::LevelsGroup() : ParameterGroup("LEVELS")
{
  const auto inputTicks = MeterTicks{{-16.5f, "S"}, {-2.5f, "H"}};

  meterInL.setDbLow(-26.0f);
  meterInL.setDbHigh(+8.0f);
  meterInL.setTicks(inputTicks);

  meterInR.setDbLow(-26.0f);
  meterInR.setDbHigh(+8.0f);
  meterInR.setTicks(inputTicks);
  meterInR.setLabelWidth(0);

  const auto outputTicks = MeterTicks{
      {-5.0f, "-5"},
      {-10.0f, ""},
      {-15.0f, "-15"},
      {-20.0f, ""},
      {-25.0f, "-25"},
  };

  meterOutL.setDbLow(-30.0f);
  meterOutL.setDbHigh(0.0f);
  meterOutL.setTicks(outputTicks);
  meterOutL.setLabelWidth(0);

  meterOutR.setDbLow(-30.0f);
  meterOutR.setDbHigh(0.0f);
  meterOutR.setTicks(outputTicks);
  meterOutR.setLabelsOnRight(true);

  addAndMakeVisible(meterInL);
  addAndMakeVisible(meterInR);

  addAndMakeVisible(meterOutL);
  addAndMakeVisible(meterOutR);

  addAndMakeVisible(sliderInputLevel);
  sliderInputLevel.setLabel("INPUT");
  sliderInputLevel.slider.setPosMapHigh(35.0f);
  sliderInputLevel.slider.setPosMapLow(-35.0f);
  sliderInputLevel.slider.setPosMapFmt("%+.0f dB");
  sliderInputLevel.slider.setPosMapDownFmt("%+.1f");

  addAndMakeVisible(sliderOutputLevel);
  sliderOutputLevel.setLabel("OUTPUT");
  sliderOutputLevel.slider.setPosMapHigh(35.0f);
  sliderOutputLevel.slider.setPosMapLow(-35.0f);
  sliderOutputLevel.slider.setPosMapFmt("%+.0f dB");
  sliderOutputLevel.slider.setPosMapDownFmt("%+.1f");
}

void LevelsGroup::attachVTS(AudioProcessorValueTreeState& vts)
{
  attInputLevel.reset(
      new SliderAttachment(vts, "idInputLevel", sliderInputLevel.slider));
  attOutputLevel.reset(
      new SliderAttachment(vts, "idOutputLevel", sliderOutputLevel.slider));
}

void LevelsGroup::attachTooltips(const TooltipsData& tooltips)
{
  sliderInputLevel.slider.setTooltip(tooltips.getForParam("idInputLevel"));
  sliderOutputLevel.slider.setTooltip(tooltips.getForParam("idOutputLevel"));
}

void LevelsGroup::resized()
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

  meterInL.setTopLeftPosition(corner);
  meterInL.setBarHeight(innerHeight);

  corner = meterInL.getBounds().getTopRight() + Point<int>(spacing, 0);

  meterInR.setTopLeftPosition(corner);
  meterInR.setBarHeight(innerHeight);

  corner = meterInR.getBounds().getTopRight() + Point<int>(spacing, 0);

  sliderInputLevel.setTopLeftPosition(corner);
  sliderInputLevel.slider.setMargin(0.15f * (float)innerHeight);
  sliderInputLevel.setHeight(innerHeight);

  corner = sliderInputLevel.getBounds().getTopRight() + Point<int>(spacing, 0);

  sliderOutputLevel.setTopLeftPosition(corner);
  sliderOutputLevel.slider.setMargin(0.15f * (float)innerHeight);
  sliderOutputLevel.setHeight(innerHeight);

  corner = sliderOutputLevel.getBounds().getTopRight() + Point<int>(spacing, 0);

  meterOutL.setTopLeftPosition(corner);
  meterOutL.setBarHeight(innerHeight);

  corner = meterOutL.getBounds().getTopRight() + Point<int>(spacing, 0);

  meterOutR.setTopLeftPosition(corner);
  meterOutR.setBarHeight(innerHeight);

  corner = meterOutR.getBounds().getTopRight() + Point<int>(spacing, 0);

  // can now determine the width and set it, this will re-call `resized` but
  // since the height is the same it won't re-do the calculation
  setSize(corner.getX() - getBounds().getX(), getHeight());
}
