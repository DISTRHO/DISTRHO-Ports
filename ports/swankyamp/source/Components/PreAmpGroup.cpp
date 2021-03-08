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

#include "PreAmpGroup.h"

PreAmpGroup::PreAmpGroup() : ParameterGroup("PRE AMP")
{
  addAndMakeVisible(sliderDrive);
  sliderDrive.setLabel("DRIVE");
  sliderDrive.slider.setPosMapDownFmt("%4.1f");
  sliderDrive.slider.setOverValue(0.7f);

  addAndMakeVisible(sliderTight);
  sliderTight.setLabel("TIGHT");
  sliderTight.slider.setPosMapDownFmt("%4.1f");

  addAndMakeVisible(sliderGrit);
  sliderGrit.setLabel("GRIT");
  sliderGrit.slider.setPosMapDownFmt("%4.1f");
  sliderGrit.slider.setOverValue(0.7f);
}

void PreAmpGroup::attachVTS(AudioProcessorValueTreeState& vts)
{
  attDrive.reset(
      new SliderAttachment(vts, "idPreAmpDrive", sliderDrive.slider));
  attTight.reset(
      new SliderAttachment(vts, "idPreAmpTight", sliderTight.slider));
  attGrit.reset(new SliderAttachment(vts, "idPreAmpGrit", sliderGrit.slider));
}

void PreAmpGroup::attachTooltips(const TooltipsData& tooltips)
{
  sliderDrive.slider.setTooltip(tooltips.getForParam("idPreAmpDrive"));
  sliderTight.slider.setTooltip(tooltips.getForParam("idPreAmpTight"));
  sliderGrit.slider.setTooltip(tooltips.getForParam("idPreAmpGrit"));
}

void PreAmpGroup::resized()
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

  sliderGrit.setTopLeftPosition(corner);
  sliderGrit.slider.setMargin(0.15f * (float)innerHeight);
  sliderGrit.setHeight(innerHeight);

  corner = sliderGrit.getBounds().getTopRight() + Point<int>(spacing, 0);

  // can now determine the width and set it, this will re-call `resized` but
  // since the height is the same it won't re-do the calculation
  setSize(corner.getX() - getBounds().getX(), getHeight());
}
