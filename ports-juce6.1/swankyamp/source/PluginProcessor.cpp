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

#include <algorithm>
#include <cmath>
#include <unordered_map>

#include <JuceHeader.h>

#if ! JUCE_AUDIOPROCESSOR_NO_GUI
 #include "PluginEditor.h"
#endif
#include "Utils.h"

#include "PluginProcessor.h"

// add parameter to the VTS with default range -1 to +1
#define MAKE_PARAMETER_UNIT(n) \
  std::make_unique<AudioParameterFloat>( \
      "id" #n, #n, NormalisableRange<float>(-1.0f, 1.0f, 2.0f / 1e3f), 0.0f)
// add parameter to the VTS with custom range
#define MAKE_PARAMETER(n, l, h, d) \
  std::make_unique<AudioParameterFloat>( \
      "id" #n, #n, NormalisableRange<float>(l, h, fabs(h - l) / 1e3f), d)
// assign a VTS parameter to an object member of the same name
#define ASSIGN_PARAMETER(n) par##n = parameters.getRawParameterValue("id" #n);

SwankyAmpAudioProcessor::SwankyAmpAudioProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                       .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                       .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
                       ),
#endif
    parameters(
        *this,
        nullptr,
        Identifier("APVTSSwankyAmp"),
        {
            MAKE_PARAMETER_UNIT(InputLevel),
            MAKE_PARAMETER_UNIT(OutputLevel),

            MAKE_PARAMETER_UNIT(TsLow),
            MAKE_PARAMETER_UNIT(TsMid),
            MAKE_PARAMETER_UNIT(TsHigh),
            MAKE_PARAMETER_UNIT(TsPresence),
            MAKE_PARAMETER(TsSelection, 0.0f, 2.0f, 0.0f),

            MAKE_PARAMETER(GainStages, 1.0f, 5.0f, 3.0f),
            MAKE_PARAMETER_UNIT(GainOverhead),
            MAKE_PARAMETER_UNIT(LowCut),

            std::make_unique<AudioParameterBool>(
                "idCabOnOff", "CabOnOff", true),
            MAKE_PARAMETER_UNIT(CabBrightness),
            MAKE_PARAMETER(CabDistance, 0.0f, 1.0f, 0.5f),
            MAKE_PARAMETER(CabDynamic, -1.0f, 1.0f, -0.3f),

            MAKE_PARAMETER(PreAmpDrive, -1.0f, 1.0f, -0.4f),
            MAKE_PARAMETER_UNIT(PreAmpTight),
            MAKE_PARAMETER_UNIT(PreAmpGrit),

            MAKE_PARAMETER(PowerAmpDrive, -1.0f, 1.0f, -0.2f),
            MAKE_PARAMETER_UNIT(PowerAmpTight),
            MAKE_PARAMETER_UNIT(PowerAmpGrit),

            MAKE_PARAMETER(PowerAmpSag, -1.0f, 1.0f, -0.6f),
            MAKE_PARAMETER_UNIT(PowerAmpSagRatio),
        })
{
  ASSIGN_PARAMETER(InputLevel)
  ASSIGN_PARAMETER(OutputLevel)

  ASSIGN_PARAMETER(TsLow)
  ASSIGN_PARAMETER(TsMid)
  ASSIGN_PARAMETER(TsHigh)
  ASSIGN_PARAMETER(TsPresence)
  ASSIGN_PARAMETER(TsSelection)

  ASSIGN_PARAMETER(GainStages)
  ASSIGN_PARAMETER(GainOverhead)
  ASSIGN_PARAMETER(LowCut)

  ASSIGN_PARAMETER(CabOnOff)
  ASSIGN_PARAMETER(CabBrightness)
  ASSIGN_PARAMETER(CabDistance)
  ASSIGN_PARAMETER(CabDynamic)

  ASSIGN_PARAMETER(PreAmpDrive)
  ASSIGN_PARAMETER(PreAmpTight)
  ASSIGN_PARAMETER(PreAmpGrit)

  ASSIGN_PARAMETER(PowerAmpDrive)
  ASSIGN_PARAMETER(PowerAmpTight)
  ASSIGN_PARAMETER(PowerAmpGrit)
  ASSIGN_PARAMETER(PowerAmpSag)

  ASSIGN_PARAMETER(PowerAmpSag)
  ASSIGN_PARAMETER(PowerAmpSagRatio)
}

#undef MAKE_PARAMETER_UNIT
#undef MAKE_PARAMETER
#undef ASSIGN_PARAMETER

SwankyAmpAudioProcessor::~SwankyAmpAudioProcessor() {}

// set the amp object user parameters from the VTS values
void SwankyAmpAudioProcessor::setAmpParameters()
{
  for (int i = 0; i < 2; i++)
  {
    const float preAmpDriveMap = remapSinh(*parPreAmpDrive, 0.5f, 1.0f);
    const float powerAmpDriveMap = remapSinh(*parPowerAmpDrive, -0.2f, 1.0f);
    const float powerAmpSagMap = remapSinh(*parPowerAmpSag, 0.0f, 1.0f);
    const float lowCutMap =
        remapSinh(remapXY(*parLowCut, -1.0f, 1.0f, 0.0f, 1.23f), 0.5f, 1.0f);

    amp_channel[i].set_input_level(*parInputLevel);
    amp_channel[i].set_output_level(
        *parOutputLevel
        + (10.0f + remapXY(preAmpDriveMap, 0.0f, 1.0f, 0.0f, -3.0f)) / 35.0f);
    amp_channel[i].set_triode_drive(preAmpDriveMap);
    amp_channel[i].set_tetrode_drive(powerAmpDriveMap);

    amp_channel[i].set_tonestack_bass(*parTsLow);
    amp_channel[i].set_tonestack_mids(*parTsMid);
    amp_channel[i].set_tonestack_treble(*parTsHigh);
    amp_channel[i].set_tonestack_presence(*parTsPresence);
    amp_channel[i].set_tonestack_selection(*parTsSelection);

    amp_channel[i].set_triode_num_stages(*parGainStages);
    amp_channel[i].set_triode_overhead(*parGainOverhead);

    amp_channel[i].set_cabinet_on((*parCabOnOff > 0.5f) ? true : false);
    amp_channel[i].set_cabinet_brightness(
        remapSided(*parCabBrightness, -0.6f, +0.6f));
    amp_channel[i].set_cabinet_distance(*parCabDistance);
    // full dynamic when knob is at 0.0
    amp_channel[i].set_cabinet_dynamic(
        remapXY(*parCabDynamic, -1.0f, 0.0f, -1.0f, +1.0f));
    // move the dynamic level down over the dynamic knob range
    amp_channel[i].set_cabinet_dynamic_level(-1.0f * *parCabDynamic);

    amp_channel[i].set_triode_hp_freq(remapSided(lowCutMap, -1.0f, +0.75f));
    // amp_channel[i].set_tetrode_hp_freq(remapSided(lowCutMap, -1.0f, +0.75f));

    const float minPreAmpTight =
        remapXY(preAmpDriveMap, -0.5f, +1.0f, -1.0f, 0.0f);
    const float adjPreAmpTight =
        remapRange(*parPreAmpTight, minPreAmpTight, +1.0f);

    amp_channel[i].set_triode_grid_tau(
        remapSided(adjPreAmpTight * -1.0f, -0.5f, +0.1f));
    amp_channel[i].set_triode_grid_ratio(
        remapSided(adjPreAmpTight * -1.0f, -1.0f, +0.1f));
    amp_channel[i].set_triode_plate_bias(
        remapSided(adjPreAmpTight, -1.0f, +0.5f));
    amp_channel[i].set_triode_plate_comp_ratio(
        remapSided(adjPreAmpTight, -1.0f, +0.0f));

    amp_channel[i].set_triode_grid_level(
        remapSided(*parPreAmpGrit * -1.0f, -0.2f, +3.0f));
    amp_channel[i].set_triode_grid_clip(
        remapSided(*parPreAmpGrit * -1.0f, -1.0f, +4.0f));
    amp_channel[i].set_triode_plate_comp_level(
        remapSided(*parPreAmpGrit * +1.0f, -0.0f, +1.0f));
    amp_channel[i].set_triode_plate_comp_offset(
        remapSided(*parPreAmpGrit * -1.0f, -0.0f, +5.0f));

    amp_channel[i].set_tetrode_grid_tau(
        remapSided(*parPowerAmpTight * -1.0f, -1.0f, +1.0f));
    amp_channel[i].set_tetrode_grid_ratio(
        remapSided(*parPowerAmpTight * -1.0f, -1.0f, +0.1f));
    amp_channel[i].set_tetrode_plate_comp_depth(
        remapSided(*parPowerAmpTight * -1.0f, -0.5f, +0.0f));
    amp_channel[i].set_tetrode_plate_sag_tau(
        remapSided(*parPowerAmpTight * -1.0f, -1.0f, +1.0f));

    amp_channel[i].set_tetrode_plate_sag_depth(
        powerAmpSagMap +
        // shift the depth higher at low drive to get some audible effect when
        // not much of signal is over clip, and lower at high drive to avoid
        // just hacking away the signal with a constant db offset
        remapXY(powerAmpDriveMap, -1.0f, 1.0f, 1.0f, -1.0f));
    amp_channel[i].set_tetrode_plate_sag_ratio(*parPowerAmpSagRatio);
    amp_channel[i].set_tetrode_plate_sag_onset(powerAmpSagMap);
    amp_channel[i].set_tetrode_plate_sag_factor(
        amp_channel[i].get_tetrode_drive());
    amp_channel[i].set_tetrode_plate_sag_toggle(
        powerAmpSagMap < -0.99f ? -1.0f : 1.0f);
  }
}

const String SwankyAmpAudioProcessor::getName() const
{
  return JucePlugin_Name;
}

bool SwankyAmpAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool SwankyAmpAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool SwankyAmpAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
  return true;
#else
  return false;
#endif
}

double SwankyAmpAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int SwankyAmpAudioProcessor::getNumPrograms() { return 1; }

int SwankyAmpAudioProcessor::getCurrentProgram() { return 0; }

void SwankyAmpAudioProcessor::setCurrentProgram(int index)
{
  ignoreUnused(index);
}

const String SwankyAmpAudioProcessor::getProgramName(int index)
{
  ignoreUnused(index);
  return {};
}

void SwankyAmpAudioProcessor::changeProgramName(
    int index, const String& newName)
{
  ignoreUnused(index, newName);
}

void SwankyAmpAudioProcessor::prepareToPlay(
    double sampleRate, int samplesPerBlock)
{
  ignoreUnused(samplesPerBlock);

  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  for (int i = 0; i < 2; i++) amp_channel[i].prepare(jmax(1, (int)sampleRate));
}

void SwankyAmpAudioProcessor::releaseResources()
{
  // When playback stops, you can use this as an opportunity to free up any
  // spare memory, etc.
  for (int i = 0; i < 2; i++) amp_channel[i].reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SwankyAmpAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
  ignoreUnused(layouts);
  return true;

#else
  // outputs must be mono or stereo
  if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
      && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
    return false;
  // inputs must be mono or stereo
  if (layouts.getMainInputChannelSet() != AudioChannelSet::mono()
      && layouts.getMainInputChannelSet() != AudioChannelSet::stereo())
    return false;
  // if input is stereo, output must be stereo
  if (layouts.getMainInputChannelSet() == AudioChannelSet::stereo()
      && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
    return false;

    // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void SwankyAmpAudioProcessor::processBlock(
    AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
  ignoreUnused(midiMessages);

  if (buffer.getNumSamples() <= 0) return;

  ScopedNoDenormals noDenormals;
  auto totalNumInputChannels = getTotalNumInputChannels();
  auto totalNumOutputChannels = getTotalNumOutputChannels();

  // copy plugin parameter values into the amp object
  setAmpParameters();

  // In case we have more outputs than inputs, this code clears any output
  // channels that didn't contain input data, (because these aren't
  // guaranteed to be empty - they may contain garbage).
  // This is here to avoid people getting screaming feedback
  // when they first compile a plugin, but obviously you don't need to keep
  // this code if your algorithm always overwrites all the output channels.
  for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    buffer.clear(i, 0, buffer.getNumSamples());

  const auto numSamples = buffer.getNumSamples();

  for (int ichannel = 0; ichannel < jmin(totalNumInputChannels, 2); ichannel++)
  {
    auto inLevel = buffer.getMagnitude(ichannel, 0, numSamples);
    // convert to decibels and add the input level which ranges from -35 to +35
    inLevel = 20 * log10f(inLevel) + (*parInputLevel * 35);
    if (meterListenersIn[ichannel] != nullptr)
      meterListenersIn[ichannel]->update(inLevel);
    if (ichannel == 0 && totalNumInputChannels < 2
        && meterListenersIn[1] != nullptr)
      meterListenersIn[1]->update(inLevel);
  }

  // mono to mono: run the amp once
  if (totalNumInputChannels == 1 && totalNumOutputChannels == 1)
  {
    float* amp_buffer = buffer.getWritePointer(0);
    amp_channel[0].process(buffer.getNumSamples(), &amp_buffer);
  }
  // mono to stereo: run the amp once, copy the result
  else if (totalNumInputChannels == 1 && totalNumOutputChannels == 2)
  {
    float* amp_buffer = buffer.getWritePointer(0);
    amp_channel[0].process(buffer.getNumSamples(), &amp_buffer);
    float* amp_buffer_other = buffer.getWritePointer(1);
    std::memcpy(
        (void*)amp_buffer_other, (void*)amp_buffer, numSamples * sizeof(float));
  }
  // stereo to stereo: run the amp twice
  else if (totalNumInputChannels == 2 && totalNumOutputChannels == 2)
  {
    for (int i = 0; i < 2; i++)
    {
      float* amp_buffer = buffer.getWritePointer(i);
      amp_channel[i].process(buffer.getNumSamples(), &amp_buffer);
    }
  }

  for (int ichannel = 0; ichannel < jmin(totalNumOutputChannels, 2); ichannel++)
  {
    auto outLevel = buffer.getMagnitude(ichannel, 0, numSamples);
    // note: the output level parameter is already factored into the buffer
    outLevel = 20 * log10f(outLevel);
    if (meterListenersOut[ichannel] != nullptr)
      meterListenersOut[ichannel]->update(outLevel);
    if (ichannel == 0 && totalNumOutputChannels < 2
        && meterListenersOut[1] != nullptr)
      meterListenersOut[1]->update(outLevel);
  }

  if (numBurnIn > 0)
  {
    const int numBurnInUsed = jmin(numBurnIn.load(), buffer.getNumSamples());
    buffer.applyGain(0, numBurnInUsed, 0.0f);
    numBurnIn -= numBurnInUsed;
  }
}

#if ! JUCE_AUDIOPROCESSOR_NO_GUI
bool SwankyAmpAudioProcessor::hasEditor() const
{
  return true;  // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SwankyAmpAudioProcessor::createEditor()
{
  return new SwankyAmpAudioProcessorEditor(*this, parameters);
}
#endif

void SwankyAmpAudioProcessor::setPresetText(const String& text)
{
  storedPresetText = text;
}

const String& SwankyAmpAudioProcessor::getPresetText() const
{
  return storedPresetText;
}

void SwankyAmpAudioProcessor::getStateInformation(MemoryBlock& destData)
{
  auto state = parameters.copyState();
  std::unique_ptr<XmlElement> xml(state.createXml());
  const String presetText = getPresetText();
  if (presetText.isNotEmpty()) { xml->setAttribute("presetName", presetText); }
  copyXmlToBinary(*xml, destData);
}

void SwankyAmpAudioProcessor::setStateInformation(
    const void* data, int sizeInBytes)
{
  std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
  if (xmlState.get() != nullptr)
  {
    if (xmlState->hasTagName(parameters.state.getType()))
    {
      if (xmlState->hasAttribute("presetName"))
      {
        setStateInformation(
            xmlState, xmlState->getStringAttribute("presetName"), true);
      }
      else
      {
        setStateInformation(xmlState, "", true);
      }
    }
  }
}

void SwankyAmpAudioProcessor::setStateInformation(
    const std::unique_ptr<XmlElement>& state,
    const String& presetText,
    bool useAll)
{
  setPresetText(presetText);

  std::unordered_map<String, double> values;
  if (state != nullptr) values = mapParameterValues(state);

  for (const auto& id : parameterIds)
  {
    if (!useAll && (id == "idInputLevel" || id == "idCabOnOff")) continue;

    auto parameter = parameters.getParameter(id);
    if (parameter == nullptr) continue;

    if (values.find(id) == values.end())
      parameter->setValueNotifyingHost(parameter->getDefaultValue());
    else
      parameter->setValueNotifyingHost(
          parameter->convertTo0to1((float)values[id]));
  }

  // clear the amp state so that buffered values don't decay too slowly with new
  // parameters
  for (int i = 0; i < 2; i++) amp_channel[i].reset();

  notifyStateChanged = true;
  numBurnIn = burnInLength;
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new SwankyAmpAudioProcessor();
}
