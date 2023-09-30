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

#include <JuceHeader.h>

#include "Utils.h"

float angleModulo(float angle)
{
  const float modulo = fmodf(angle, MathConstants<float>::twoPi);
  if (modulo > MathConstants<float>::pi)
    return modulo - MathConstants<float>::twoPi;
  else
    return modulo;
}

void fillImageNoise(Image& image, Random& rng, float alpha)
{
  if (image.getFormat() != Image::PixelFormat::ARGB)
    image.convertedToFormat(Image::PixelFormat::ARGB);
  for (int i = 0; i < image.getWidth(); i++)
    for (int j = 0; j < image.getHeight(); j++)
      image.setPixelAt(
          i, j, Colour::fromHSV(0.0f, 0.0f, 0.0f, rng.nextFloat() * alpha));
}

Image buildImageNoise(int width, int height, Random& rng, float alpha)
{
  Image noise(Image::PixelFormat::ARGB, jmax(1, width), jmax(1, height), false);
  fillImageNoise(noise, rng, alpha);
  return noise;
}

VersionNumber parseVersionString(const String& versionString)
{
  int section = 0;
  VersionNumber versionNumber;
  String buff;
  for (const auto c : versionString)
  {
    if (c == '.')
    {
      if (buff.isNotEmpty())
      {
        if (section == 0)
          versionNumber.major = buff.getIntValue();
        else if (section == 1)
          versionNumber.minor = buff.getIntValue();
        else if (section == 2)
          versionNumber.patch = buff.getIntValue();
      }
      section += 1;
      buff.clear();
      continue;
    }
    buff += c;
  }

  if (buff.isNotEmpty())
  {
    if (section == 0)
      versionNumber.major = buff.getIntValue();
    else if (section == 1)
      versionNumber.minor = buff.getIntValue();
    else if (section == 2)
      versionNumber.patch = buff.getIntValue();
  }

  return versionNumber;
}

/**
 * @brief Build map of serialized state parameter names to values.
 * @param state serialized state
 * @return map of parameter names to values
 */
std::unordered_map<String, double>
mapParameterValues(const SerializedState& state)
{
  std::unordered_map<String, double> values;

  XmlElement* element = state->getFirstChildElement();

  while (element != nullptr)
  {
    if (element->getTagName() == "PARAM" && element->hasAttribute("id")
        && element->hasAttribute("value"))
    {
      const String& id = element->getStringAttribute("id");
      const double value = element->getDoubleAttribute("value");
      values[id] = value;
    }

    element = element->getNextElement();
  }

  return values;
}

/**
 * @brief Set serialized state from map of parameter to value
 * @param state serialized state
 * @param map map of parameter names to values
 */
void updateStateFromMap(
    SerializedState& state, const std::unordered_map<String, double>& map)
{
  std::unordered_map<String, double> values;

  XmlElement* element = state->getFirstChildElement();

  while (element != nullptr)
  {
    if (element->getTagName() == "PARAM" && element->hasAttribute("id")
        && element->hasAttribute("value"))
    {
      const String& id = element->getStringAttribute("id");
      if (map.find(id) == map.end()) continue;
      element->setAttribute("value", map.at(id));
    }
    element = element->getNextElement();
  }
}

// TODO: could be useful for building parameter list in processor?
std::vector<String> buildParameterIds(const SerializedState& state)
{
  if (state == nullptr) return {};

  std::vector<String> parameterIds;

  XmlElement* element = state->getFirstChildElement();

  while (element != nullptr)
  {
    if (element->getTagName() == "PARAM" && element->hasAttribute("id"))
    {
      const String& id = element->getStringAttribute("id");
      parameterIds.push_back(id);
    }

    element = element->getNextElement();
  }

  return parameterIds;
}
