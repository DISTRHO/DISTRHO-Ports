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
#include <nonstd/optional.hpp>
#include <unordered_map>

#include "PluginProcessor.h"

struct StateEntry
{
  StateEntry(const String& name, File file, nonstd::optional<size_t> stateIdx);
  StateEntry() {}

  String name;
  File file;
  nonstd::optional<size_t> stateIdx = nonstd::nullopt;
};

/** Connects a value tree state to a combo box and preset directory. */
class PresetManager : public AudioProcessorValueTreeState::Listener
{
public:
  PresetManager(
      SwankyAmpAudioProcessor& processor,
      AudioProcessorValueTreeState& vts,
      ComboBox& comboBox,
      Button& bntSave,
      Button& bntRemove,
      Button& bntNext,
      Button& bntPrev,
      Button& btnOpen);
  ~PresetManager();

  void comboBoxChanged();
  void buttonSaveClicked();
  void buttonRemoveClicked();
  void buttonNextClicked();
  void buttonPrevClicked();
  void buttonOpenClicked();
  void parameterChanged(const String& id, float newValue);

  const std::vector<String>& getParameterIds() const { return parameterIds; }

  void setState(const SerializedState& state);
  void setStateText(const String& text);

private:
  void loadPreset(SerializedState state, File file, const String& name);
  void loadFactoryPresets();
  bool loadPresetsFromDir();

  void updateComboBox();
  void updatePresetDir();
  void updateButtonState();

  bool containsName(const String& name) const
  {
    return stateEntryIdx.find(name) != stateEntryIdx.end();
  }

  void
  addStateEntry(const String& name, const File& file, SerializedState state);
  void removeStateEntry(const String& name);
  void moveStateEntry(size_t idx, size_t newIdx);

  SwankyAmpAudioProcessor& processor;
  AudioProcessorValueTreeState& vts;
  ComboBox& comboBox;
  Button& buttonSave;
  Button& buttonRemove;
  Button& buttonNext;
  Button& buttonPrev;
  Button& buttonOpen;
  File presetDir;

  std::vector<String> parameterIds;

  String currentName = "init";
  std::vector<StateEntry> stateEntries;
  std::unordered_map<String, size_t> stateEntryIdx;
  std::vector<SerializedState> states;
};
