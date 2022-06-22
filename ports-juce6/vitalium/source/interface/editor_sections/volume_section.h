/* Copyright 2013-2019 Matt Tytel
 *
 * vital is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vital is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vital.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "JuceHeader.h"
#include "synth_section.h"

class PeakMeterViewer;
class VolumeSlider;

class VolumeSection : public SynthSection {
  public:
    VolumeSection(String name);
    ~VolumeSection();

    int getMeterHeight();
    int getBuffer();
    void resized() override;
    void paintBackground(Graphics& g) override;

  private:
    std::unique_ptr<VolumeSlider> volume_;
    std::unique_ptr<PeakMeterViewer> peak_meter_left_;
    std::unique_ptr<PeakMeterViewer> peak_meter_right_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VolumeSection)
};

