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

#include "../SwankyAmpLAF.h"

#include "AmpGroup.h"

#define GROUP_DROP_SHADOW(n) \
  auto n##ShadowPath = Path(); \
  n##ShadowPath.addRoundedRectangle( \
      n##Group.getBorderBounds(), 2.0f * n##Group.getLineThickness()); \
  n##ShadowPath.applyTransform(AffineTransform::translation( \
      (float)n##Group.getBounds().getTopLeft().getX(), \
      (float)n##Group.getBounds().getTopLeft().getY())); \
  auto n##Shadow = SwankyAmpLAF::getDropShadow(); \
  n##Shadow.drawForPath(g, n##ShadowPath);

AmpGroup::AmpGroup()
{
  spacing = 32;
  lineThickness = 0;
  label.setFont(0.0f);

  addAndMakeVisible(levelsGroup);
  addAndMakeVisible(levelsGroup);
  addAndMakeVisible(preAmpGroup);
  addAndMakeVisible(powerAmpGroup);
  addAndMakeVisible(stagingGroup);
  addAndMakeVisible(toneStackGroup);
  addAndMakeVisible(cabGroup);
}

void AmpGroup::setGroupsHeight(int height)
{
  levelsGroup.setHeight(height);
  preAmpGroup.setHeight(height);
  powerAmpGroup.setHeight(height);
  stagingGroup.setHeight(height);
  toneStackGroup.setHeight(height);
  cabGroup.setHeight(height);
  resized();
}

void AmpGroup::attachVTS(AudioProcessorValueTreeState& vts)
{
  levelsGroup.attachVTS(vts);
  preAmpGroup.attachVTS(vts);
  powerAmpGroup.attachVTS(vts);
  stagingGroup.attachVTS(vts);
  toneStackGroup.attachVTS(vts);
  cabGroup.attachVTS(vts);
}

void AmpGroup::attachTooltips(const TooltipsData& tooltips)
{
  levelsGroup.attachTooltips(tooltips);
  preAmpGroup.attachTooltips(tooltips);
  powerAmpGroup.attachTooltips(tooltips);
  stagingGroup.attachTooltips(tooltips);
  toneStackGroup.attachTooltips(tooltips);
  cabGroup.attachTooltips(tooltips);
}

void AmpGroup::paint(Graphics& g)
{
  // skip  painting the group background and borders
  Component::paint(g);
  GROUP_DROP_SHADOW(levels)
  GROUP_DROP_SHADOW(preAmp)
  GROUP_DROP_SHADOW(powerAmp)
  GROUP_DROP_SHADOW(staging)
  GROUP_DROP_SHADOW(toneStack)
  GROUP_DROP_SHADOW(cab)
}

void AmpGroup::resized()
{
  ParameterGroup::resized();

  levelsGroup.setTopLeftPosition(spacing, spacing);
  preAmpGroup.setTopLeftPosition(spacing, spacing + levelsGroup.getBottom());
  powerAmpGroup.setTopLeftPosition(spacing, spacing + preAmpGroup.getBottom());
  stagingGroup.setTopLeftPosition(
      spacing + preAmpGroup.getRight(), spacing + levelsGroup.getBottom());
  toneStackGroup.setTopLeftPosition(
      spacing + powerAmpGroup.getRight(), spacing + preAmpGroup.getBottom());
  cabGroup.setTopLeftPosition(spacing + levelsGroup.getRight(), spacing);

  setSize(
      toneStackGroup.getRight() + spacing,
      toneStackGroup.getBottom() + spacing);
}
