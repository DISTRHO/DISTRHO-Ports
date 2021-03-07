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

#include "../Utils.h"
#include "RSlider.h"

class RSliderLabel : public Component
{
public:
  RSliderLabel();
  ~RSliderLabel() {}

  void resized() override;

  void setLabel(const String& text)
  {
    label.setText(text, NotificationType::dontSendNotification);
  }

  void setHeight(int size);
  void setWidth(int size);
  void setSliderMargin(float size);

  // TODO: publich for parameter attachment, but maybe RSliderLabel should
  // just inherent from RSlider and not compose the elements?
  RSlider slider;

private:
  Label label;

  enum class SetDimension
  {
    SetFromHeight,
    SetFromWidth,
    NoDimension
  };
  SetDimension setDimension = SetDimension::NoDimension;

  DISABLE_COMPONENT_RESIZE()
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RSliderLabel)
};
