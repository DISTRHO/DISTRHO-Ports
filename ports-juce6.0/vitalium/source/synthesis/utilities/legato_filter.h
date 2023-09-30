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

#include "processor.h"

namespace vital {

  class LegatoFilter : public Processor {
    public:
      enum {
        kLegato,
        kTrigger,
        kNumInputs
      };

      enum {
        kRetrigger,
        kNumOutputs
      };

      LegatoFilter();

      virtual Processor* clone() const override {
        return new LegatoFilter(*this);
      }

      void process(int num_samples) override;

    private:
      poly_float last_value_;

      JUCE_LEAK_DETECTOR(LegatoFilter)
  };
} // namespace vital

