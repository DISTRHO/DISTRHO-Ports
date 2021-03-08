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

class RButton : public ToggleButton
{
public:
  RButton() {}
  ~RButton() {}

  void resized() override;

  float calcWidthForHeight(float height) const { return height / 2.0f; }
  float calcHeightForWidth(float width) const { return width * 2.0f; }

  const Image& getBgNoise() const { return bgNoise; }

  enum ColourIds
  {
    buttonColourId = 0x2000401,
    textColourId = 0x2000402,
  };

  String textOn = "ON";
  String textOff = "OFF";

private:
  Random rng;
  Image bgNoise;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RButton)
};
