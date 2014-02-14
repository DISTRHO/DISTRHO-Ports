// ==================================================================================
// Copyright (c) 2012 HiFi-LoFi
//
// This is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ==================================================================================

#ifndef _CONVOLVER_H
#define _CONVOLVER_H

#include "JuceHeader.h"

#include "FFTConvolver/TwoStageFFTConvolver.h"


class Convolver : public fftconvolver::TwoStageFFTConvolver
{
public:
  Convolver();
  virtual ~Convolver();
  
protected:
  virtual void startBackgroundProcessing();
  virtual void waitForBackgroundProcessing();
  
private:
  friend class ConvolverBackgroundThread;
  
  juce::ScopedPointer<juce::Thread> _thread;
  juce::Atomic<uint32> _backgroundProcessingFinished;
  juce::WaitableEvent _backgroundProcessingFinishedEvent;
};


#endif // Header guard
