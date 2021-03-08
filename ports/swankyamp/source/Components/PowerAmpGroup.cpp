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

#include "PowerAmpGroup.h"

PowerAmpGroup::PowerAmpGroup() : ParameterGroup("POWER AMP")
{
  addAndMakeVisible(sliderDrive);
  sliderDrive.setLabel("DRIVE");
  sliderDrive.slider.setPosMapDownFmt("%4.1f");
  sliderDrive.slider.setOverValue(0.7f);

  addAndMakeVisible(sliderTight);
  sliderTight.setLabel("TIGHT");
  sliderTight.slider.setPosMapDownFmt("%4.1f");

  addAndMakeVisible(sliderSag);
  sliderSag.setLabel("SAG");
  sliderSag.slider.setPosMapDownFmt("%4.1f");
}

void PowerAmpGroup::attachVTS(AudioProcessorValueTreeState& vts)
{
  attDrive.reset(
      new SliderAttachment(vts, "idPowerAmpDrive", sliderDrive.slider));
  attTight.reset(
      new SliderAttachment(vts, "idPowerAmpTight", sliderTight.slider));
  attSag.reset(new SliderAttachment(vts, "idPowerAmpSag", sliderSag.slider));
}

void PowerAmpGroup::attachTooltips(const TooltipsData& tooltips)
{
  sliderDrive.slider.setTooltip(tooltips.getForParam("idPowerAmpDrive"));
  sliderTight.slider.setTooltip(tooltips.getForParam("idPowerAmpTight"));
  sliderSag.slider.setTooltip(tooltips.getForParam("idPowerAmpSag"));
}

void PowerAmpGroup::resized()
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

  sliderDrive.setTopLeftPosition(corner);
  sliderDrive.slider.setMargin(0.15f * (float)innerHeight);
  sliderDrive.setHeight(innerHeight);

  corner = sliderDrive.getBounds().getTopRight() + Point<int>(spacing, 0);

  sliderTight.setTopLeftPosition(corner);
  sliderTight.slider.setMargin(0.15f * (float)innerHeight);
  sliderTight.setHeight(innerHeight);

  corner = sliderTight.getBounds().getTopRight() + Point<int>(spacing, 0);

  sliderSag.setTopLeftPosition(corner);
  sliderSag.slider.setMargin(0.15f * (float)innerHeight);
  sliderSag.setHeight(innerHeight);

  corner = sliderSag.getBounds().getTopRight() + Point<int>(spacing, 0);

  // can now determine the width and set it, this will re-call `resized` but
  // since the height is the same it won't re-do the calculation
  setSize(corner.getX() - getBounds().getX(), getHeight());
}
