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
#include "LevelMeter.h"
#include "ParameterGroup.h"
#include "RSliderLabel.h"

class LevelsGroup : public ParameterGroup
{
public:
  LevelsGroup();
  ~LevelsGroup() {}

  void setHeight(int height) { setSize(0, height); }
  void resized() override;

  void attachVTS(AudioProcessorValueTreeState& vts) override;
  void attachTooltips(const TooltipsData& tooltips) override;

  LevelMeterListener* getLevelMeterListenerInL() { return &meterInL; }
  LevelMeterListener* getLevelMeterListenerInR() { return &meterInR; }
  LevelMeterListener* getLevelMeterListenerOutL() { return &meterOutL; }
  LevelMeterListener* getLevelMeterListenerOutR() { return &meterOutR; }

private:
  LevelMeter meterInL;
  LevelMeter meterInR;
  LevelMeter meterOutL;
  LevelMeter meterOutR;

  RSliderLabel sliderInputLevel;
  RSliderLabel sliderOutputLevel;

  std::unique_ptr<SliderAttachment> attInputLevel;
  std::unique_ptr<SliderAttachment> attOutputLevel;

  DISABLE_COMPONENT_RESIZE()
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelsGroup)
};
