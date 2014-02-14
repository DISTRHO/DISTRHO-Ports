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

#if JUCE_MAC || JUCE_IOS || DROWAUDIO_USE_FFTREAL



//============================================================================
namespace 
{
    inline float magnitude (const float real, 
                            const float imag, 
                            const float oneOverFFTSize, 
                            const float oneOverWindowFactor)
    {
        const float rawMagnitude = hypotf (real, imag);
        const float magnitudeForFFTSize = rawMagnitude * oneOverFFTSize;
        const float magnitudeForWindowFactor = magnitudeForFFTSize * oneOverWindowFactor;
        
        return magnitudeForWindowFactor;
    }
}

//============================================================================
FFTEngine::FFTEngine (int fftSizeLog2_)
    : fftOperation (fftSizeLog2_),
      windowProperties (getFFTProperties().fftSize),
      magnitutes (getFFTProperties().fftSizeHalved + 1)
{
}

FFTEngine::~FFTEngine()
{
}

void FFTEngine::performFFT (float* samples)
{	
	// first apply the current window
	windowProperties.applyWindow (samples, getFFTProperties().fftSize);
	fftOperation.performFFT (samples);
}

void FFTEngine::findMagnitudes (Buffer* bufferToFill)
{
	// local copies for speed
	float* magBuf = magnitutes.getData();
	if (bufferToFill != nullptr)
		magBuf = bufferToFill->getData();

	const SplitComplex &fftSplit = fftOperation.getFFTBuffer();
	const float oneOverFFTSize = (float) getFFTProperties().oneOverFFTSize;
	const int fftSizeHalved = getFFTProperties().fftSizeHalved;
	const float oneOverWindowFactor = windowProperties.getOneOverWindowFactor();
	
	// find magnitudes
	magBuf[0] = magnitude (fftSplit.realp[0], 0.0f, oneOverFFTSize, oneOverWindowFactor); // imag for DC is always zero 
	for (int i = 1; i < fftSizeHalved; i++)
	{		
		magBuf[i] = magnitude (fftSplit.realp[i], fftSplit.imagp[i], oneOverFFTSize, oneOverWindowFactor);
	}
	magBuf[fftSizeHalved] = magnitude (fftSplit.realp[0], 0.0f, oneOverFFTSize, oneOverWindowFactor); // imag for Nyquist is always zero 
	
	magnitutes.updateListeners();
}

void FFTEngine::updateMagnitudesIfBigger()
{
	// local copies for speed
	const SplitComplex &fftSplit = fftOperation.getFFTBuffer();
	float* magBuf = magnitutes.getData();
	const float oneOverFFTSize = (float) getFFTProperties().oneOverFFTSize;
	const int fftSizeHalved = getFFTProperties().fftSizeHalved;
	const float oneOverWindowFactor = windowProperties.getOneOverWindowFactor();
	
	// find magnitudes
	float newMag = magnitude (fftSplit.realp[0], 0.0f, oneOverFFTSize, oneOverWindowFactor); // imag for DC is always zero 
	if (newMag > magBuf[0])
		magBuf[0] = newMag;
    
	for (int i = 1; i < fftSizeHalved; i++)
	{		
		newMag = magnitude (fftSplit.realp[i], fftSplit.imagp[i], oneOverFFTSize, oneOverWindowFactor);

		if(newMag > magBuf[i])
			magBuf[i] = newMag;
	}

	newMag = magnitude (fftSplit.realp[0], 0.0f, oneOverFFTSize, oneOverWindowFactor); // imag for Nyquist is always zero 
	if (newMag > magBuf[fftSizeHalved])
		magBuf[fftSizeHalved] = newMag;
	
	magnitutes.updateListeners();
}

void FFTEngine::setWindowType (Window::WindowType type)
{
	windowProperties.setWindowType (type);
}



#endif // JUCE_MAC || DROWAUDIO_USE_FFTREAL
