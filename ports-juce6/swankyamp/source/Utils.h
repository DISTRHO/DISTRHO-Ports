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

#include <unordered_map>

#include <JuceHeader.h>

using SerializedState = std::unique_ptr<XmlElement>;

struct VersionNumber
{
  int major = -1;
  int minor = -1;
  int patch = -1;

  VersionNumber() {}

  VersionNumber(int major, int minor, int patch) :
      major(major), minor(minor), patch(patch)
  {
  }

  bool operator==(const VersionNumber& other) const
  {
    return this->major == other.major && this->minor == other.minor
        && this->patch == other.patch;
  }

  bool operator<(const VersionNumber& other) const
  {
    if (this->major < other.major) return true;
    if (this->major == other.major && this->minor < other.minor) return true;
    if (this->major == other.major && this->minor == other.minor
        && this->patch < other.patch)
      return true;
    return false;
  }

  bool operator>(const VersionNumber& other) const
  {
    return !(*this == other) && !(*this < other);
  }

  bool operator!=(const VersionNumber& other) const
  {
    return !(*this == other);
  }
};

VersionNumber parseVersionString(const String& versionString);

std::unordered_map<String, double>
mapParameterValues(const SerializedState& state);

void updateStateFromMap(
    SerializedState& state, const std::unordered_map<String, double>& map);

std::vector<String> buildParameterIds(const SerializedState& state);

float angleModulo(float angle);

Image buildImageNoise(int width, int height, Random& rng, float alpha);

/**
 * @brief Remap a float value to a positive and negative range.
 *
 * Remap the range [-1, 0) to [`to_low`, 0) and the range (0, 1] to
 * (0, `to_high`]. Note that the values below zero can be scaled differently
 * from those above zero.
 *
 * @param unit value to remap, usually in the unit scale [-1, +1]
 * @param to_low scale values above zero by this amount
 * @param to_high  scale values below zero by this amount
 * @return remapped value
 */
inline float remapSided(float unit, float to_low, float to_high)
{
  if (unit >= 0) { return unit * to_high; }
  else
  {
    return -unit * to_low;
  }
}

/**
 * @brief Remap a float value to a new range.
 *
 * Remap the range [-1, +1] to [`to_low`, `to_high`]. Unlike `remap_sided`, the
 * same scaling is applied to all input values. An input value of `0` will map
 * to the mean of `to_low` and `to_high`.
 *
 * @param unit value to remap, usually in the unit scale [-1, +1]
 * @param to_low value to reach when `unit` is at `-1`
 * @param to_high value to reach when `unit` is at `+1`
 * @return remapped value
 */
inline float remapRange(float unit, float to_low, float to_high)
{
  return (unit + 1.0f) / 2.0f * (to_high - to_low) + to_low;
}

/**
 * @brief Remap a given range of floats to a new range.
 *
 * Remap the range [`x1`, `x2`] to the range [`y1`, `y2`]. Clips the input `x`
 * to the range [`x1`, `x2`].
 *
 * @param x value to remap
 * @param x1 lower value in the range to map from
 * @param x2 upper value in the range to map from
 * @param y1 lower value in the range to map to
 * @param y2 upper value in the range to map to
 * @return remapped value
 */
inline float remapXY(float x, float x1, float x2, float y1, float y2)
{
  x = jmax(x1, jmin(x2, x));
  x = (x - x1) / (x2 - x1);
  float y = x * (y2 - y1) + y1;
  return y;
}

/**
 * @brief Remap a value from the range (-1, +1) to the same range, but with a
 * new metric such that dy/dx(x) follows a sinh curve.
 *
 * In other words, for an input x near the eges -1 or 1, a small change in x
 * will result in a bigger change in y. For input values near 0, a small
 * change in x will result in a smaller change in y.
 *
 * The calulating uses exponential values in the intermediate steps. This can
 * probably be fixed later, but for now scales should stay below 100.
 *
 * @param x the value to remap
 * @param x0 the point in the range (-1, 1) where y changes most slowly
 * @param scale larger values will create a bigger difference
 * @return remaped value
 */
inline float remapSinh(float x, float x0, float scale)
{
  // the value mapped by sinh
  const float mapped = sinh((x - x0) * scale);
  // y(x) at the bounds -1 and 1
  const float lower = sinh((-1.0f - x0) * scale);
  const float upper = sinh((+1.0f - x0) * scale);
  return (mapped - lower) / (upper - lower) * 2.0f - 1.0f;
}

inline float invertRemapSinh(float x, float x0, float scale)
{
  const float lower = sinh((-1.0f - x0) * scale);
  const float upper = sinh((+1.0f - x0) * scale);
  const float unscaled = (x + 1.0f) / 2.0f * (upper - lower) + lower;
  return asinh(unscaled) / scale + x0;
}

inline double transformUnitScale(
    double value,
    double lower,
    double upper,
    double lowerPost,
    double upperPost)
{
  const double post = 2.0 / (upperPost - lowerPost)
          * ((value + 1.0) / 2.0 * (upper - lower) + lower - lowerPost)
      - 1.0;
  return jmin(1.0, jmax(-1.0, post));
}

// commonly used in parameter groups that need to handle the size (e.g. to
// maintain an aspect ratio)
#define DISABLE_COMPONENT_RESIZE() \
  using Component::setSize; \
  using Component::setBounds; \
  using Component::setBoundsRelative; \
  using Component::setBoundsInset; \
  using Component::setBoundsToFit; \
  using Component::centreWithSize;
