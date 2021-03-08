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

#include "../Utils.h"

#include "ParameterGroup.h"

ParameterGroup::ParameterGroup(const String& pLabel)
{
  setLabel(pLabel);
  label.setJustificationType(Justification::topLeft);
  label.setFont(24.0f);

  gradient.addColour(0.0f, Colours::white);
  gradient.addColour(0.5f, Colours::white);
  gradient.addColour(1.0f, Colours::white);

  addAndMakeVisible(label);
}

void ParameterGroup::setFont(const Font& font)
{
  label.setFont(font);
  // note that if the font size changes, the box will need to change
  resized();
}

void ParameterGroup::setFont(float height)
{
  label.setFont(height);
  resized();
}

void ParameterGroup::setLineThickness(float thickness)
{
  lineThickness = thickness;
  // note: border bounds depend on line thickness affects paint
  resized();
}

void ParameterGroup::setSpacing(int pSpacing)
{
  spacing = pSpacing;
  resized();
}

void ParameterGroup::paint(Graphics& g)
{
  gradient.setColour(0, findColour(steelColourId));
  gradient.setColour(1, Colours::white);
  gradient.setColour(2, findColour(steelColourId));

  g.setGradientFill(gradient);
  g.fillRoundedRectangle(getBorderBounds().toFloat(), 2.0f * lineThickness);

  g.drawImage(
      bgNoise, getBorderBounds().toFloat(), RectanglePlacement::stretchToFit);

  g.setColour(findColour(borderColourId));
  g.drawRoundedRectangle(
      getBorderBounds().toFloat(), 2.0f * lineThickness, lineThickness);

  g.setColour(Colour::fromHSV(0.0f, 0.0f, 1.0f, 0.5f));
  g.drawRoundedRectangle(
      getBorderBounds().toFloat().translated(
          -lineThickness / 4.0f, -lineThickness / 4.0f),
      2.0f * lineThickness,
      lineThickness / 2.0f);
}

void ParameterGroup::resized()
{
  label.setBounds(
      getLocalBounds().withHeight((int)(label.getFont().getHeight() + 0.5f)));

  Rectangle<float> bounds = getLocalBounds().toFloat();
  bounds.setTop(label.getFont().getHeight());
  BorderSize<float>(lineThickness / 2.0f).subtractFrom(bounds);
  borderBounds = bounds.toNearestInt();

  bgNoise = buildImageNoise(
      // noise is not as wide, and gets stretched to create brushed metal
      jmax(1, getBorderBounds().getWidth() / 20),
      getBorderBounds().getHeight(),
      rng,
      bgNoiseAlpha);

  gradient.point1 = getBorderBounds().getCentre().toFloat().translated(
      -0.5f * (float)getHeight(), 0.0f);
  gradient.point2 = getBorderBounds().getCentre().toFloat().translated(
      +0.5f * (float)getHeight(), 0.0f);
}
