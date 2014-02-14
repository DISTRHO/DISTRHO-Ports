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

#ifndef __DROWAUDIO_FFTENGINE__
#define __DROWAUDIO_FFTENGINE__

#if JUCE_MAC || JUCE_IOS || DROWAUDIO_USE_FFTREAL

#include "dRowAudio_Window.h"
#include "dRowAudio_FFTOperation.h"
#include "../../utility/dRowAudio_Constants.h"

//==============================================================================
class FFTEngine
{
public:
    //==============================================================================
	FFTEngine (int fftSizeLog2);
	
	~FFTEngine();

	/** Performs an FFT operation.
		The number of samples must be equal to the fftSize.
	 */
	void performFFT (float* samples);
	
	/**	This will fill the buffer with the magnitudes of the last performed FFT.
		You can then get this buffer using getMagnitudesBuffer(). Remember that
		the size of the buffer is the fftSizeHalved+1 to incorporate the Nyquist.
		Optionally you can pass in a Buffer to be filled. Remember this must be
		the correct size.
	 */
	void findMagnitudes (Buffer* bufferToFill = nullptr);

	/**	This will fill the buffer with the magnitudes of the last performed FFT if they are bigger.
		You can then get this buffer using getMagnitudesBuffer(). Remember that
		the size of the buffer is the fftSizeHalved + 1 to incorporate the Nyquist.
	 */
	void updateMagnitudesIfBigger();

	void setWindowType (Window::WindowType type);
	
	int getFFTSize()                        {	return getFFTProperties().fftSize;      }
	Buffer& getMagnitudesBuffer()           {	return magnitutes;                     }
	Window& getWindow()                     {	return windowProperties;               }
			
	const FFTProperties& getFFTProperties()	{	return fftOperation.getFFTProperties();	}

private:
    //==============================================================================
	FFTOperation fftOperation;
	Window windowProperties;
	Buffer magnitutes;
	
    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTEngine);
};

#endif //JUCE_MAC || DROWAUDIO_USE_FFTREAL

#endif //__DROWAUDIO_FFTENGINE__