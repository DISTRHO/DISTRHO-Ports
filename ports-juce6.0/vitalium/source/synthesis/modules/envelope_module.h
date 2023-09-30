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

#include "synth_module.h"

namespace vital {
  class Envelope;

  class EnvelopeModule : public SynthModule {
    public:
      enum {
        kTrigger,
        kNumInputs
      };
    
      enum {
        kValue,
        kPhase,
        kNumOutputs
      };

      EnvelopeModule(const std::string& prefix, bool force_audio_rate = false);
      virtual ~EnvelopeModule() { }

      void init() override;
      virtual Processor* clone() const override { return new EnvelopeModule(*this); }

      void setControlRate(bool control_rate) override { 
        if (!force_audio_rate_)
          envelope_->setControlRate(control_rate);
      }

    protected:
      std::string prefix_;
      Envelope* envelope_;
      bool force_audio_rate_;
    
      JUCE_LEAK_DETECTOR(EnvelopeModule)
  };
} // namespace vital

