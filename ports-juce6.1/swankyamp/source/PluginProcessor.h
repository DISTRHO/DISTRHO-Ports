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

#include "Components/LevelMeter.h"
#include "dsp/PushPullAmp.h"

class SwankyAmpAudioProcessor : public AudioProcessor
{
public:
  SwankyAmpAudioProcessor();
  ~SwankyAmpAudioProcessor();

  // The amplifier DSP objects (contains DSP state and the process function)
  // one for each possible channel (even if unused)
  PushPullAmp amp_channel[2];

  // Objects with an `update` method for updating the value of input meters.
  LevelMeterListener* meterListenersIn[2] = {nullptr, nullptr};
  LevelMeterListener* meterListenersOut[2] = {nullptr, nullptr};

  AudioProcessorValueTreeState parameters;

  std::atomic<float>* parInputLevel = nullptr;
  std::atomic<float>* parOutputLevel = nullptr;

  std::atomic<float>* parTsLow = nullptr;
  std::atomic<float>* parTsMid = nullptr;
  std::atomic<float>* parTsHigh = nullptr;
  std::atomic<float>* parTsPresence = nullptr;
  std::atomic<float>* parTsSelection = nullptr;

  std::atomic<float>* parGainStages = nullptr;
  std::atomic<float>* parGainOverhead = nullptr;

  std::atomic<float>* parCabOnOff = nullptr;
  std::atomic<float>* parCabBrightness = nullptr;
  std::atomic<float>* parCabDistance = nullptr;
  std::atomic<float>* parCabDynamic = nullptr;

  std::atomic<float>* parPreAmpDrive = nullptr;
  std::atomic<float>* parPreAmpTight = nullptr;
  std::atomic<float>* parPreAmpGrit = nullptr;
  std::atomic<float>* parLowCut = nullptr;

  std::atomic<float>* parPowerAmpDrive = nullptr;
  std::atomic<float>* parPowerAmpTight = nullptr;
  std::atomic<float>* parPowerAmpGrit = nullptr;

  std::atomic<float>* parPowerAmpSag = nullptr;
  std::atomic<float>* parPowerAmpSagRatio = nullptr;

  const std::vector<String> parameterIds = {
      "idInputLevel",    "idOutputLevel",
      "idTsLow",         "idTsMid",
      "idTsHigh",        "idTsPresence",
      "idTsSelection",   "idGainStages",
      "idGainOverhead",  "idCabOnOff",
      "idCabBrightness", "idCabDistance",
      "idCabDynamic",    "idPreAmpDrive",
      "idPreAmpTight",   "idPreAmpGrit",
      "idLowCut",        "idPowerAmpDrive",
      "idPowerAmpTight", "idPowerAmpGrit",
      "idPowerAmpSag",   "idPowerAmpSagRatio",
  };

  std::atomic<bool> notifyStateChanged { false };
  const int burnInLength = 1024;
  std::atomic<int> numBurnIn { burnInLength };

  void setAmpParameters();

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

#if ! JUCE_AUDIOPROCESSOR_NO_GUI
  AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;
#endif

  const String getName() const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool isMidiEffect() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int index) override;
  const String getProgramName(int index) override;
  void changeProgramName(int index, const String& newName) override;

  void getStateInformation(MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;
  void setStateInformation(
      const std::unique_ptr<XmlElement>& state,
      const String& presetName,
      bool useAll = true);

  void setPresetText(const String& text);
  const String& getPresetText() const;

private:
  CriticalSection setStateMutex;
  String storedPresetText;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SwankyAmpAudioProcessor)
};
