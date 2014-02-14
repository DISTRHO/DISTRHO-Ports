/*
  ==============================================================================

  This file is part of the dRowAudio JUCE module
  Copyright 2004-13 by dRowAudio.

  ------------------------------------------------------------------------------

  dRowAudio is provided under the terms of The MIT License (MIT):

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
  SOFTWARE.

  ==============================================================================
*/

#ifndef __DROWAUDIO_WINDOW_H__
#define __DROWAUDIO_WINDOW_H__

#include "../dRowAudio_Buffer.h"
#include "../../utility/dRowAudio_Constants.h"
#include "../../maths/dRowAudio_MathsUtilities.h"

//==============================================================================
class Window
{
public:
    //==============================================================================
	enum WindowType {
		Rectangular,
		Hann,
		Hamming,
		Cosine,
		Lanczos,
		ZeroEndTriangle,
		NonZeroEndTriangle,
		Gaussian,
		BartlettHann,
		Blackman,
		Nuttall,
		BlackmanHarris,
		BlackmanNuttall,
		FlatTop
	};
	
    //==============================================================================
	Window (int windowSize);

	~Window();

	void setWindowType (WindowType newType);
    
	WindowType getWindowType()              {	return windowType;          }
	
	float getWindowFactor()                 {	return windowFactor;        }
	
    float getOneOverWindowFactor()          {	return oneOverWindowFactor;	}
	
	void applyWindow (float *samples,  const int numSamples);
	
private:
    //==============================================================================
	void setUpWindowBuffer();
	
	void applyRectangularWindow (float *samples,  const int numSamples);
	void applyHannWindow (float *samples,  const int numSamples);
	void applyHammingWindow (float *samples,  const int numSamples);
	void applyCosineWindow (float *samples,  const int numSamples);
	void applyLanczosWindow (float *samples,  const int numSamples);
	void applyZeroEndTriangleWindow (float *samples,  const int numSamples);
	void applyNonZeroEndTriangleWindow (float *samples,  const int numSamples);
	void applyGaussianWindow (float *samples,  const int numSamples);
	void applyBartlettHannWindow (float *samples,  const int numSamples);
	void applyBlackmanWindow (float *samples,  const int numSamples);
	void applyNuttallWindow (float *samples,  const int numSamples);
	void applyBlackmanHarrisWindow (float *samples,  const int numSamples);
	void applyBlackmanNuttallWindow (float *samples,  const int numSamples);
	void applyFlatTopWindow (float *samples,  const int numSamples);
	
    //==============================================================================
	int windowSize;
	WindowType windowType;
	float windowFactor;
	float oneOverWindowFactor;
	
	Buffer windowBuffer;
    
    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Window);
};

#endif //__DROWAUDIO_WINDOW_H__