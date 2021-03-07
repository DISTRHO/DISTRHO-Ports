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

#include "RSlider.h"

RSlider::RSlider()
{
  setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
  setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
}

// from:
// https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
// I don't even recognize C++ anymore, this looks like rust to me
template<typename... Args>
std::string stringFormat(const std::string& format, Args... args)
{
  size_t size = snprintf(nullptr, 0, format.c_str(), args...)
      + (size_t)1;  // Extra space for '\0'
  if (size <= 0) throw std::runtime_error("Error during formatting.");
  std::unique_ptr<char[]> buf(new char[size]);
  snprintf(buf.get(), size, format.c_str(), args...);
  return std::string(
      buf.get(),
      buf.get() + size - 1);  // We don't want the '\0' inside
}

String RSlider::fmtSliderPos(float sliderPos) const
{
  const float pos = sliderPos * (posMapHigh - posMapLow) + posMapLow;
  if (isMouseButtonDown() && posMapDownFmt.length() > 0)
    return String(stringFormat(posMapDownFmt.toStdString(), pos));
  else
    return String(stringFormat(posMapFmt.toStdString(), pos));
}

RSliderDims RSlider::calcDims() const
{
  RSliderDims dims;

  dims.gap = gap;
  dims.margin = margin;

  const RotaryParameters rotPars = getRotaryParameters();
  dims.theta = jmax(
      fabsf(angleModulo(rotPars.startAngleRadians)),
      fabsf(angleModulo(rotPars.endAngleRadians)));

  const float radiusForWidth =
      (float)getBounds().getWidth() / 2.0f - dims.margin;
  const float radiusForHeight = ((float)getBounds().getHeight() - dims.margin)
      / (1.0f - cosf(dims.theta));
  dims.radius = jmin(radiusForWidth, radiusForHeight);

  // the amount clipped from the bottom due to limited angle
  const float clipped = jmin(
      // remove the bottom margin, and some amount of the knob
      (1.0f + cosf(dims.theta)) * dims.radius + dims.margin,
      // always leave at least the top margin
      2.0f * dims.radius + dims.margin);

  dims.innerBounds = getBounds().toFloat();
  dims.innerBounds.setSize(
      (dims.radius + dims.margin) * 2.0f,
      (dims.radius + dims.margin) * 2.0f - clipped);
  dims.innerBounds.translate(
      (getBounds().getWidth() - dims.innerBounds.getWidth()) / 2.0f,
      (getBounds().getHeight() - dims.innerBounds.getHeight()) / 2.0f);

  dims.centre.setXY(
      dims.innerBounds.getX() + dims.margin + dims.radius,
      dims.innerBounds.getY() + dims.margin + dims.radius);

  return dims;
}

float RSlider::calcWidthForHeight(float height) const
{
  const RotaryParameters rotPars = getRotaryParameters();
  const float theta = jmax(
      fabsf(angleModulo(rotPars.startAngleRadians)),
      fabsf(angleModulo(rotPars.endAngleRadians)));

  const float radiusForHeight = (height - margin) / (1.0f - cosf(theta));

  return (radiusForHeight + margin) * 2.0f;
}

float RSlider::calcHeightForWidth(float width) const
{
  const float radiusForWidth = width / 2.0f - margin;
  return (radiusForWidth + margin) * 2.0f;
}

void RSlider::resized()
{
  Slider::resized();
  bgNoise = buildImageNoise(getWidth(), getHeight(), rng, 0.1f);
}
