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

class PresetGroup : public ParameterGroup
{
public:
  PresetGroup();
  ~PresetGroup() {}

  void attachVTS(AudioProcessorValueTreeState&) override {}
  void addPresetListener(ComboBox::Listener* listener)
  {
    presetSelector.addListener(listener);
  }

  void setHeight(int height) { setSize(0, height); }
  void paint(Graphics& g) override;
  void resized() override;

  enum ComboIds
  {
    PresetInit = 1,
    PresetOther = 2,
  };

  // public so the preset manager can modify it
  ComboBox presetSelector;
  DrawableButton buttonSave;
  DrawableButton buttonRemove;
  DrawableButton buttonNext;
  DrawableButton buttonPrev;
  DrawableButton buttonOpen;

private:
  DrawablePath saveIcon;
  DrawablePath saveIconHighlight;
  DrawablePath removeIcon;
  DrawablePath removeIconHighlight;
  DrawablePath nextIcon;
  DrawablePath nextIconHighlight;
  DrawablePath prevIcon;
  DrawablePath prevIconHighlight;
  DrawablePath openIcon;
  DrawablePath openIconHighlight;

  DISABLE_COMPONENT_RESIZE()
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetGroup)
};
