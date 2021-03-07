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

typedef std::pair<float, String> MeterTick;
typedef std::vector<MeterTick> MeterTicks;

struct LevelMeterListener
{
  virtual ~LevelMeterListener() {}
  virtual void update(float) = 0;
};

class LevelMeter : public Component, public LevelMeterListener, private Timer
{
public:
  LevelMeter() { setRefreshRate(30); }

  void paint(Graphics& g) override;
  void update(float db) override;

  // NOTE: startTime just adds this to a list of timers a singleton time thread
  // so it shoudld be inexpensive to create a timer per meter
  void setRefreshRate(int refreshRateHz) { startTimer(refreshRateHz); }
  void setDbLow(float db) { dbLow = db; }
  void setDbHigh(float db) { dbHigh = db; }
  void setDecayTau(float tau) { decayTau = tau; }
  void setTicks(const MeterTicks& pTicks) { ticks = pTicks; }

  void setBarWidth(int width);
  void setBarHeight(int height);
  void setLabelWidth(int width);
  void setLabelHeight(float height) { labelHeight = height; }
  void setLabelGap(int width);
  void setLabelsOnRight(bool choice) { labelsOnRight = choice; }

  float getLabelHeight() const { return labelHeight; }
  int getLabelWidth() const { return labelWidth; }

  enum ColourIds
  {
    outlineColourId = 0x2000301,
    backgroundColourId = 0x2000302,
    meterColourId = 0x2000303,
    textColourId = 0x2000304,
  };

private:
  void timerCallback() override;
  float dbToLevel(float db) const;

  std::atomic<float> maxLevel = {0.0f};
  float level = 0.0f;
  float dbLow = -30.0f;
  float dbHigh = 0.0f;
  float decayTau = 0.5f;
  MeterTicks ticks;

  int barWidth = 16;
  int labelWidth = 16;
  float labelHeight = 16.0f;
  int labelGap = 2;
  bool labelsOnRight = false;

  DISABLE_COMPONENT_RESIZE()
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
