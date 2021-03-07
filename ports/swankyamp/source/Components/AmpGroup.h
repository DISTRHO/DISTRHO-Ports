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
#include "CabGroup.h"
#include "LevelsGroup.h"
#include "ParameterGroup.h"
#include "PowerAmpGroup.h"
#include "PreAmpGroup.h"
#include "StagingGroup.h"
#include "ToneStackGroup.h"

class AmpGroup : public ParameterGroup
{
public:
  AmpGroup();
  ~AmpGroup() {}

  void resized() override;
  void paint(Graphics& g) override;

  void attachVTS(AudioProcessorValueTreeState& vts) override;
  void attachTooltips(const TooltipsData& tooltips) override;

  void setGroupsHeight(int height);

  // TODO: for the time being keep public as need to access members, but this
  // should be cleaned up at some point
  LevelsGroup levelsGroup;
  PreAmpGroup preAmpGroup;
  PowerAmpGroup powerAmpGroup;
  StagingGroup stagingGroup;
  ToneStackGroup toneStackGroup;
  CabGroup cabGroup;

private:
  DISABLE_COMPONENT_RESIZE()
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpGroup)
};
