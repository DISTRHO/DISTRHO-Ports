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

#include "LevelMeter.h"

void LevelMeter::setBarWidth(int width)
{
  barWidth = width;
  setSize(labelWidth + labelGap + barWidth, getHeight());
}

void LevelMeter::setBarHeight(int height)
{
  setSize(labelWidth + labelGap + barWidth, height);
}

void LevelMeter::setLabelWidth(int width)
{
  labelWidth = width;
  setSize(labelWidth + labelGap + barWidth, getHeight());
}

void LevelMeter::setLabelGap(int width)
{
  labelGap = width;
  setSize(labelWidth + labelGap + barWidth, getHeight());
}

float LevelMeter::dbToLevel(float db) const
{
  return (db - dbLow) / (dbHigh - dbLow);
}

void LevelMeter::paint(Graphics& g)
{
  g.fillAll(Colours::transparentBlack);

  const auto bounds = g.getClipBounds();

  auto fullBar = Rectangle<float>(bounds.toFloat());
  if (!labelsOnRight)
    fullBar.setLeft(fullBar.getX() + labelWidth + labelGap);
  else
    fullBar.setRight(fullBar.getRight() - labelWidth - labelGap);

  const float lw = 2.0f;
  const float corner = 2.0f;
  const Font& font =
      SwankyAmpLAF::getDefaultFontNarrow().withHeight(labelHeight);

  const auto clippedLevel = jmax(0.0f, jmin(1.0f, level));

  auto filledBarPath = Path();
  filledBarPath.addRoundedRectangle(
      fullBar.getX(),
      fullBar.getY() + fullBar.getHeight() * (1.0f - clippedLevel),
      fullBar.getWidth(),
      fullBar.getHeight() * clippedLevel,
      corner,
      corner,
      false,
      false,
      true,
      true);

  g.setColour(findColour(backgroundColourId));
  g.fillRoundedRectangle(fullBar, corner);

  g.setColour(findColour(meterColourId));
  g.fillPath(filledBarPath);

  g.setFont(font);
  for (const auto& tick : ticks)
  {
    const float y =
        (1.0f - (tick.first - dbLow) / (dbHigh - dbLow)) * fullBar.getHeight();

    if (labelWidth > 0)
    {
      auto labelBox = Rectangle<float>();
      if (!labelsOnRight)
        labelBox.setX((float)bounds.getX());
      else
        labelBox.setX((float)fullBar.getRight() + labelGap);
      labelBox.setY(y - labelHeight / 2.0f);
      labelBox.setSize((float)labelWidth, labelHeight);

      g.setColour(findColour(textColourId));
      g.drawText(tick.second, labelBox, Justification::centredRight, false);
    }

    auto tickLineBox = Rectangle<float>(fullBar);
    tickLineBox.setY(y);
    tickLineBox.setHeight(lw);

    g.setColour(findColour(backgroundColourId));
    g.fillRect(tickLineBox);
  }

  g.setColour(findColour(outlineColourId));
  g.drawRoundedRectangle(
      BorderSize<float>(lw / 2.0f).subtractedFrom(fullBar), corner, lw);
}

void LevelMeter::update(float db)
{
  // The processor will call this from the audio thread, which simply stores
  // the maximum until the timer paints it to the UI
  maxLevel = jmax(maxLevel.load(), dbToLevel(db));
}

void LevelMeter::timerCallback()
{
  // The editor owns this component, and some time thread (?) will make calls to
  // this method to repaint the meter.

  auto callbackLevel = maxLevel.exchange(0.0);

  const float refreshRateHz = 1.0f / getTimerInterval() * 1e3f;
  const float decayFactor = powf(0.5f, 1.0f / (decayTau * refreshRateHz));

  if (callbackLevel > level)
    level = callbackLevel;
  else if (level > 1e-3)
    level *= decayFactor;
  else
    level = 0;

  repaint();
}
