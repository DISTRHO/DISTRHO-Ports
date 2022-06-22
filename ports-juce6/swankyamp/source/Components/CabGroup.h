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
#include "ParameterGroup.h"
#include "RButton.h"
#include "RSliderLabel.h"

class CabGroup : public ParameterGroup
{
public:
  CabGroup();
  ~CabGroup() {}

  void attachVTS(AudioProcessorValueTreeState& vts) override;
  void attachTooltips(const TooltipsData& tooltips) override;

  void setHeight(int height) { setSize(0, height); }
  void resized() override;

private:
  RButton buttonCabOnOff;
  RSliderLabel sliderBright;
  RSliderLabel sliderDistance;
  RSliderLabel sliderDynamic;

  std::unique_ptr<ButtonAttachment> attCabOnOff;
  std::unique_ptr<SliderAttachment> attCabBrightness;
  std::unique_ptr<SliderAttachment> attCabDistance;
  std::unique_ptr<SliderAttachment> attCabDynamic;

  DISABLE_COMPONENT_RESIZE()
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabGroup)
};
