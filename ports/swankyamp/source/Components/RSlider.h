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

#pragma once

#include <JuceHeader.h>

struct RSliderDims
{
  // the radius of the knob
  float radius = 0.0f;
  // the amount of space between the knob edge and the graphic edge
  float margin = 0.0f;
  // the gap between the knob and the dots in the margin
  float gap = 0.0f;
  // the largest angle of the knob, where it cuts away
  float theta = 0.0f;
  // the bounds of the graphic
  juce::Rectangle<float> innerBounds;
  // the center of the knob
  Point<float> centre;
};

class RSlider : public Slider
{
public:
  RSlider();
  // NOTE: base class destructor is not virtual, so can't be using pointers to
  // an `RSlider` of type `Slider*` when deleting.
  ~RSlider() {}

  void resized() override;

  String fmtSliderPos(float sliderPos) const;

  // NOTE: this could be called on `resized` and cached, but it also depends on
  // the rotary parameters, so best to call on every paint
  RSliderDims calcDims() const;
  float calcWidthForHeight(float height) const;
  float calcHeightForWidth(float width) const;

  void setGap(float pGap) { gap = pGap; }
  void setMargin(float pMargin) { margin = pMargin; }
  void setOverValue(float value) { overValue = value; }
  void setPosMapLow(float value) { posMapLow = value; }
  void setPosMapHigh(float value) { posMapHigh = value; }
  void setPosMapFmt(const String& fmt) { posMapFmt = fmt; }
  void setPosMapDownFmt(const String& fmt) { posMapDownFmt = fmt; }

  const Image& getBgNoise() const { return bgNoise; }
  const float getOverValue() const { return overValue; }

  enum ColourIds
  {
    dialColourId = 0x2000201,
    dialOutlineColourId = 0x2000202,
    dialTextColourId = 0x2000203,
    dialDotColourId = 0x2000204,
  };

private:
  using Slider::setSliderStyle;
  using Slider::setTextBoxStyle;

  float posMapLow = 0.0f;
  float posMapHigh = 10.0f;
  String posMapFmt = "%02.0f";
  String posMapDownFmt = "";

  float gap = 2.0f;
  float margin = 0.0f;
  float overValue = -1.0f;

  Random rng;
  Image bgNoise;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RSlider)
};
