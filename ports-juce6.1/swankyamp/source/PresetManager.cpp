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

#include <cmath>
#include <iomanip>
#include <sstream>

#include "unordered_map"

#include "Components/PresetGroup.h"
#include "PresetManager.h"

std::pair<int, String> extractNumAndName(const String& name)
{
  static const String digits = "09";
  static const String div = " ";

  int startIdx = -1;

  for (int i = 0; i < name.length(); i++)
  {
    const juce_wchar c = name[i];

    if (c >= digits[0] && c <= digits[1]) continue;

    const bool hasNumber = i > 0;
    const bool hasDivider = i + div.length() <= name.length()
        && name.substring(i, i + div.length()) == div;

    if (hasNumber && hasDivider) startIdx = i + div.length();

    break;
  }

  if (startIdx > 1)
  {
    int num = std::atoi(name.substring(0, startIdx - div.length()).toRawUTF8());
    String nameStripped = name.substring(startIdx);
    return std::pair<int, String>(num, nameStripped);
  }
  else
  {
    return std::pair<int, String>(-1, name);
  }
}

StateEntry::StateEntry(
    const String& name, File file, nonstd::optional<size_t> stateIdx) :
    name(name), file(file), stateIdx(stateIdx)
{
}

PresetManager::PresetManager(
    SwankyAmpAudioProcessor& processor,
    AudioProcessorValueTreeState& vts,
    ComboBox& comboBox,
    Button& buttonSave,
    Button& buttonRemove,
    Button& buttonNext,
    Button& buttonPrev,
    Button& buttonOpen) :
    processor(processor),
    vts(vts),
    comboBox(comboBox),
    buttonSave(buttonSave),
    buttonRemove(buttonRemove),
    buttonNext(buttonNext),
    buttonPrev(buttonPrev),
    buttonOpen(buttonOpen)
{
  presetDir = File::getSpecialLocation(
      File::SpecialLocationType::userApplicationDataDirectory);
#ifdef JUCE_MAC
  presetDir = presetDir.getChildFile("Audio").getChildFile("Presets");
#endif
  presetDir = presetDir.getChildFile(JucePlugin_Manufacturer)
                  .getChildFile(JucePlugin_Name);
  // silent if invalid, will only notify user when they try to save
  presetDir.createDirectory();

  parameterIds = buildParameterIds(SerializedState(vts.state.createXml()));

  addStateEntry("init", File(), nullptr);

  // cleanup:
  // go through xml files
  // any without correct tag are removed
  // any with resonantamp tag are converted to swankyamp

  if (!loadPresetsFromDir()) loadFactoryPresets();

  updatePresetDir();
  updateComboBox();

  comboBox.onChange = [&]() { comboBoxChanged(); };
  buttonSave.onClick = [&]() { buttonSaveClicked(); };
  buttonRemove.onClick = [&]() { buttonRemoveClicked(); };
  buttonNext.onClick = [&]() { buttonNextClicked(); };
  buttonPrev.onClick = [&]() { buttonPrevClicked(); };
  buttonOpen.onClick = [&]() { buttonOpenClicked(); };

  const String storedText = processor.getPresetText();
  setStateText(storedText);
}

PresetManager::~PresetManager()
{
  // UI elements can outlive the manager, ensure they don't try to use callbacks
  comboBox.onChange = []() {};
  buttonSave.onClick = []() {};
  buttonRemove.onClick = []() {};
  buttonNext.onClick = []() {};
  buttonPrev.onClick = []() {};
  buttonOpen.onClick = []() {};
}

void PresetManager::addStateEntry(
    const String& name, const File& file, SerializedState state)
{
  removeStateEntry(name);

  if (state != nullptr)
  {
    stateEntryIdx[name] = stateEntries.size();
    stateEntries.push_back(StateEntry(name, file, states.size()));
    states.push_back(std::move(state));
  }
  else
  {
    stateEntryIdx[name] = stateEntries.size();
    stateEntries.push_back(StateEntry(name, file, nonstd::nullopt));
  }
}

void PresetManager::removeStateEntry(const String& name)
{
  if (!containsName(name)) return;

  const size_t idx = stateEntryIdx[name];
  stateEntries.erase(stateEntries.begin() + idx);

  stateEntryIdx.clear();
  for (size_t i = 0; i < stateEntries.size(); i++)
    stateEntryIdx[stateEntries[i].name] = i;
}

void PresetManager::moveStateEntry(size_t idx, size_t newIdx)
{
  std::vector<StateEntry> moved;
  moved.reserve(stateEntries.size());

  if (newIdx < 1) newIdx = 1;

  if (newIdx == idx) return;

  if (newIdx > idx) newIdx += 1;

  for (size_t i = 0; i < stateEntries.size(); i++)
  {
    if (i == idx) continue;
    if (i == newIdx) moved.push_back(stateEntries[idx]);
    moved.push_back(stateEntries[i]);
  }

  if (newIdx >= stateEntries.size()) moved.push_back(stateEntries[idx]);

  stateEntries = moved;

  stateEntryIdx.clear();
  for (size_t i = 0; i < stateEntries.size(); i++)
    stateEntryIdx[stateEntries[i].name] = i;
}

void PresetManager::loadPreset(
    SerializedState state, File file, const String& name)
{
  if (state == nullptr) return;

  auto values = mapParameterValues(state);

  // range changes from 0.6 to 0.7
  if (state != nullptr && state->hasAttribute("pluginVersion")
      && parseVersionString(state->getStringAttribute("pluginVersion"))
          < VersionNumber(0, 7, 0))
  {
    if (values.find("idPowerAmpDrive") != values.end())
    {
      const double value = values["idPowerAmpDrive"];
      const double post =
          transformUnitScale(value, log(1.0), log(1e3), log(0.5), log(5e2));
      values["idPowerAmpDrive"] = post;
    }
    if (values.find("idPowerAmpSag") != values.end())
    {
      const double value = values["idPowerAmpSag"];
      const double post = transformUnitScale(value, 0.0, 1.0, 0.0, 0.5);
      values["idPowerAmpSag"] = post;
    }
  }

  // from 1.1 to 1.2
  if (state != nullptr && state->hasAttribute("pluginVersion")
      && parseVersionString(state->getStringAttribute("pluginVersion"))
          < VersionNumber(1, 2, 0))
  {
    if (values.find("idPreAmpDrive") != values.end())
    {
      const double value = values["idPreAmpDrive"];
      const float post = remapXY((float)value, -1.0f, 1.0f, -0.67f, 0.86f);
      values["idPreAmpDrive"] = (double)post;
    }
    if (values.find("idPowerAmpDrive") != values.end())
    {
      const double value = values["idPowerAmpDrive"];
      const float post = invertRemapSinh(
          remapXY((float)value, -1.0f, 1.0f, -0.85f, 1.14f), -0.2f, 1.0f);
      values["idPowerAmpDrive"] = (double)post;
    }
    if (values.find("idPowerAmpSag") != values.end())
    {
      const double value = values["idPowerAmpSag"];
      const float post = remapSinh((float)value, 0.0f, 1.0f);
      values["idPowerAmpSag"] = (double)post;
    }
  }

  // from 1.3.0 to 1.3.1
  if (state != nullptr && state->hasAttribute("pluginVersion")
      && parseVersionString(state->getStringAttribute("pluginVersion"))
          < VersionNumber(1, 3, 1))
  {
    if (values.find("idPreAmpDrive") != values.end())
    {
      const double value = values["idPreAmpDrive"];
      const float post = invertRemapSinh((float)value, 0.5f, 1.0f);
      values["idPreAmpDrive"] = (double)post;
    }
  }

  // from 1.3.1 to 1.4
  if (state != nullptr && state->hasAttribute("pluginVersion")
      && parseVersionString(state->getStringAttribute("pluginVersion"))
          < VersionNumber(1, 4, 0))
  {
    if (values.find("idLowCut") != values.end())
    {
      const double value = values["idLowCut"];
      const float post = remapXY(
          invertRemapSinh((float)value, 0.5f, 1.0f), 0.0f, 1.23f, -1.0f, +1.0f);
      values["idLowCut"] = (double)post;
    }
  }

  updateStateFromMap(state, values);

  addStateEntry(name, file, std::move(state));
}

void PresetManager::loadFactoryPresets()
{
  const std::unique_ptr<XmlElement> xml =
      XmlDocument::parse(BinaryData::presets_xml);
  if (xml == nullptr) return;

  bool writingSuccess = true;
  const Identifier& stateType = vts.state.getType();

  XmlElement* stateXml = xml->getFirstChildElement();
  while (stateXml != nullptr)
  {
    if (stateXml->hasTagName(stateType) && stateXml->hasAttribute("presetName"))
    {
      const String presetName = stateXml->getStringAttribute("presetName");
      stateXml->removeAttribute("presetName");
      stateXml->setAttribute("pluginVersion", JucePlugin_VersionString);

      File presetFile = presetDir.getChildFile(presetName + ".xml");
      loadPreset(
          std::make_unique<XmlElement>(*stateXml), presetFile, presetName);
      writingSuccess &= stateXml->writeTo(presetFile);
    }
    stateXml = stateXml->getNextElement();
  }

  if (!writingSuccess)
  {
    AlertWindow::showMessageBox(
        AlertWindow::AlertIconType::WarningIcon,
        "Factory preset failure",
        "Unalble to write factory presets to disk");
  }
}

bool PresetManager::loadPresetsFromDir()
{
  std::vector<String> filePaths;
  auto files =
      presetDir.findChildFiles(File::TypesOfFileToFind::findFiles, false);

  if (files.isEmpty()) return false;

  std::sort(files.begin(), files.end());

  const Identifier& stateType = vts.state.getType();

  for (int i = 0; i < files.size(); i++)
  {
    const File& presetFile = files[i];
    if (presetFile.getFileExtension() != ".xml"
        && presetFile.getFileExtension() != ".XML")
      continue;

    const auto numAndName =
        extractNumAndName(presetFile.getFileNameWithoutExtension());

    SerializedState stateXml = XmlDocument::parse(presetFile);

    // allow using preseets from version before name change
    if (stateXml->hasTagName("APVTSResonantAmp"))
      stateXml->setTagName("APVTSSwankyAmp");

    if (stateXml->hasTagName(stateType))
    {
      const String& presetName = numAndName.second;
      loadPreset(
          std::make_unique<XmlElement>(*stateXml), presetFile, presetName);
    }
  }

  return true;
}

void PresetManager::updateComboBox()
{
  // don't re-trigger comboBoxChanged
  comboBox.clear(NotificationType::dontSendNotification);
  const int width = (int)(log10f((float)stateEntries.size())) + 1;

  for (size_t idx = 0; idx < stateEntries.size(); idx++)
  {
    const auto& entry = stateEntries[idx];
    const int id = (int)(idx + 1);

    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(width);
    ss << idx << " " << entry.name;

    const String text = idx > 0 ? ss.str() : entry.name;
    comboBox.addItem(text, id);

    if (entry.name == currentName)
      // don't re-trigger comboBoxChanged
      comboBox.setText(text, NotificationType::dontSendNotification);
  }
}

void PresetManager::updatePresetDir()
{
  const int width = (int)(log10f((float)stateEntries.size())) + 1;

  size_t idx = 0;
  for (auto& entry : stateEntries)
  {
    if (!entry.file.existsAsFile()) continue;

    idx++;

    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(width);
    ss << idx << " " << entry.name << ".xml";

    const String targetPath =
        presetDir.getChildFile(ss.str()).getFullPathName();
    if (entry.file == targetPath) continue;

    if (entry.file.getParentDirectory() == presetDir)
    {
      if (entry.file.moveFileTo(targetPath)) entry.file = File(targetPath);
    }
    else
    {
      if (entry.file.copyFileTo(targetPath)) entry.file = File(targetPath);
    }
  }
}

void PresetManager::updateButtonState()
{
  if (currentName == "init")
  {
    buttonSave.setEnabled(false);
    buttonRemove.setEnabled(false);
    return;
  }

  buttonSave.setEnabled(true);
  if (containsName(currentName)) buttonRemove.setEnabled(true);
}

void PresetManager::comboBoxChanged()
{
  const auto numAndName = extractNumAndName(comboBox.getText());
  const String& name = numAndName.second;
  const int ord = numAndName.first;

  if (name == "") return;

  if (!containsName(name))
  {
    const bool makeNew = currentName == "init" || !containsName(currentName)
        || AlertWindow::showOkCancelBox(
                             AlertWindow::AlertIconType::QuestionIcon,
                             "New preset name",
                             "Create new preset \"" + name + "\" or rename \""
                                 + currentName + "\"?",
                             "new",
                             "rename");

    if (makeNew)
    {
      // new entry
      addStateEntry(name, File(), SerializedState(vts.state.createXml()));
      currentName = name;
      buttonSaveClicked();
    }
    // can only happen if not init and currentName exists
    else
    {
      const size_t idx = stateEntryIdx[currentName];
      StateEntry& currentEntry = stateEntries[idx];
      currentEntry.name = name;
      stateEntryIdx[name] = idx;
      stateEntryIdx.erase(currentName);
      currentName = name;
    }

    // possible a new order was defined, update if so
    if (ord > 0 && stateEntryIdx.find(name) != stateEntryIdx.end()
        && ord != stateEntryIdx[name])
      moveStateEntry(stateEntryIdx[name], (size_t)ord);

    updatePresetDir();
    updateComboBox();
    updateButtonState();

    // after making a new preset, leave enabled to help user with feedback
    if (makeNew) buttonSave.setEnabled(true);
  }
  else
  {
    currentName = name;
    // cn only happen if name exists
    const StateEntry& currentEntry = stateEntries[stateEntryIdx[name]];

    if (currentEntry.stateIdx != nonstd::nullopt)
    {
      // load the state for this entry
      setState(states[*currentEntry.stateIdx]);

      if (ord > 0 && ord != (int)(stateEntryIdx[name]))
      {
        moveStateEntry(stateEntryIdx[name], (size_t)ord);
        updatePresetDir();
        updateComboBox();
      }

      updateButtonState();
      // just loaded a preset
      buttonSave.setEnabled(false);
    }
    else
    {
      // the init preset
      setState(nullptr);
      buttonRemove.setEnabled(false);
      buttonSave.setEnabled(false);
    }
  }
}

void PresetManager::buttonSaveClicked()
{
  if (currentName == "init") return;
  if (!containsName(currentName))
  {
    // not currently a valid selection, create one
    comboBoxChanged();
    return;
  }

  // can only happen if currentName exists
  StateEntry& currentEntry = stateEntries[stateEntryIdx[currentName]];
  if (currentEntry.name == "") return;

  SerializedState state = vts.state.createXml();
  if (state == nullptr) return;

  if (currentEntry.file.getFullPathName() != ""
      && !AlertWindow::showOkCancelBox(
          AlertWindow::AlertIconType::QuestionIcon,
          "Confirm save",
          "Save preset: " + currentName + "?"))
    return;

  state->setAttribute("pluginVersion", JucePlugin_VersionString);

  // TODO: sanitize preset names for valid file paths... could be tricky with
  // multiple os support
  if (currentEntry.file.getFullPathName() == "")
    currentEntry.file = presetDir.getChildFile(currentEntry.name + ".xml");

  if (!state->writeTo(currentEntry.file))
  {
    AlertWindow::showMessageBox(
        AlertWindow::AlertIconType::WarningIcon,
        "Failed to save",
        "Failed to save preset file.");
    return;
  }

  states.push_back(std::move(state));
  currentEntry.stateIdx = {states.size() - 1};

  // TODO: properly extract save functionaliy from button callback, this is
  // button specific, not save-specific
  updatePresetDir();
}

void PresetManager::buttonRemoveClicked()
{
  if (currentName == "init") return;
  if (!containsName(currentName)) return;

  // can only happen if currentName exists
  StateEntry& currentEntry = stateEntries[stateEntryIdx[currentName]];

  if (currentEntry.file.getFullPathName() != "")
  {
    currentEntry.file.deleteFile();
    currentEntry.file = File();
  }

  removeStateEntry(currentEntry.name);
  updatePresetDir();
  updateComboBox();
  setStateText("init");
}

void PresetManager::buttonNextClicked()
{
  if (!containsName(currentName))
  {
    comboBox.setSelectedId(1, NotificationType::sendNotificationSync);
    return;
  }

  size_t currentIdx = stateEntryIdx[currentName];

  if (currentIdx < stateEntries.size() - 1)
  {
    currentIdx += 1;
    currentName = stateEntries[currentIdx].name;
    const int id = (int)(currentIdx + 1);
    comboBox.setSelectedId(id, NotificationType::sendNotificationSync);
  }
}

void PresetManager::buttonPrevClicked()
{
  if (!containsName(currentName))
  {
    comboBox.setSelectedId(1, NotificationType::sendNotificationSync);
    return;
  }

  size_t currentIdx = stateEntryIdx[currentName];

  if (currentIdx > 1)
  {
    currentIdx -= 1;
    currentName = stateEntries[currentIdx].name;
    const int id = (int)(currentIdx + 1);
    comboBox.setSelectedId(id, NotificationType::sendNotificationSync);
  }
}

void PresetManager::buttonOpenClicked() { presetDir.startAsProcess(); }

void PresetManager::parameterChanged(const String& id, float)
{
  if (currentName != "init" && id != "idInputLevel" && id != "idCabOnOff")
    buttonSave.setEnabled(true);
}

void PresetManager::setState(const SerializedState& state)
{
  processor.setStateInformation(state, comboBox.getText(), false);
}

void PresetManager::setStateText(const String& text)
{
  const auto name = extractNumAndName(text).second;
  if (name.isEmpty())
  {
    currentName = "init";
    comboBox.setText("init", NotificationType::dontSendNotification);
  }
  else
  {
    currentName = name;
    comboBox.setText(text, NotificationType::dontSendNotification);
  }
  updateButtonState();
}
