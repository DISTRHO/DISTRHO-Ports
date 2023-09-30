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

#include "PluginProcessor.h"
#include <JuceHeader.h>

#include "Components/AmpGroup.h"
#include "Components/PresetGroup.h"
#include "PresetManager.h"
#include "SwankyAmpLAF.h"
#include "TooltipsData.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

class StateChangeMonitor : public Timer
{
public:
  StateChangeMonitor(int refreshRateHz) { startTimerHz(refreshRateHz); }
  virtual ~StateChangeMonitor() = default;
  virtual void stateChangeCallback() = 0;

private:
  void timerCallback() override { stateChangeCallback(); }
};

class SwankyAmpAudioProcessorEditor :
    public AudioProcessorEditor,
    public StateChangeMonitor
{
public:
  SwankyAmpAudioProcessorEditor(
      SwankyAmpAudioProcessor&, AudioProcessorValueTreeState&);
  ~SwankyAmpAudioProcessorEditor();

  void paint(Graphics&) override;
  void resized() override;

  String getPresetName() const { return presetGroup.presetSelector.getText(); }
  void setPresetTextDontNotify(const String& text);

private:
  // TODO: consdier setting and calling resized, bur for now these are const
  const int padding = 64;
  const int spacing = 32;
  const int groupHeight = 128;
  const int headerHeight = 24;
  const int headerPadding = 16;

  SwankyAmpLAF laf;

  // This reference is provided as a quick way for your editor to
  // access the processor object that created it.
  SwankyAmpAudioProcessor& processor;
  AudioProcessorValueTreeState& valueTreeState;

  std::vector<String> managerListenIds;

  Label versionLabel;
  AmpGroup ampGroup;
  PresetGroup presetGroup;

  // NOTE: must be declared *AFTER* valueTreeState AND presetGroup
  PresetManager presetManager;

  TooltipsData tooltipsData;
  TooltipWindow tooltipWindow;

  std::unique_ptr<Drawable> logoSvg;

  Image bgNoise;
  Path bgPattern;
  Random rng;

  void buildBgPattern();
  void stateChangeCallback() override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwankyAmpAudioProcessorEditor)
};

#undef INSERT_PARAMETER
