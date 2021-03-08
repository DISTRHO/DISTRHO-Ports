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

#include "../TooltipsData.h"
#include "../Utils.h"

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

/*
 * A group of parameters drawn in a box, with a label sitting atop the box.
 */
class ParameterGroup : public Component
{
public:
  ParameterGroup(const String& label);
  ParameterGroup() : ParameterGroup("") {}
  virtual ~ParameterGroup() {}

  virtual void paint(Graphics&) override;
  virtual void resized() override;

  virtual void attachVTS(AudioProcessorValueTreeState& vts) = 0;
  virtual void attachTooltips(const TooltipsData&) {}

  const juce::Rectangle<int>& getBorderBounds() const { return borderBounds; }

  void setLabel(const String& pLabel)
  {
    label.setText(pLabel, dontSendNotification);
  }
  void setFont(const Font& font);
  void setFont(float height);
  void setLineThickness(float thickness);
  void setSpacing(int spacing);
  void setBgNoiseAlpha(float alpha) { bgNoiseAlpha = alpha; }

  float getLineThickness() const { return lineThickness; }
  int getSpacing() const { return spacing; }

  enum ColourIds
  {
    borderColourId = 0x2000101,  // the colour of the border
    steelColourId = 0x2000102,  // the component background brushed steel colour
  };

protected:
  Label label;
  float lineThickness = 2.0f;
  int spacing = 12;
  juce::Rectangle<int> borderBounds;
  float bgNoiseAlpha = 0.04f;

  ColourGradient gradient;
  Image bgNoise;
  Random rng;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterGroup)
};
