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

#ifndef __DROWAUDIO_FFTOPERATION__
#define __DROWAUDIO_FFTOPERATION__

#if (JUCE_MAC || JUCE_IOS) && ! DROWAUDIO_USE_FFTREAL
    typedef FFTSetup FFTConfig;
    typedef DSPSplitComplex SplitComplex;
#elif DROWAUDIO_USE_FFTREAL
    
    //#include "fftreal/FFTReal.h"
    
    typedef ScopedPointer< ffft::FFTReal<float> > FFTConfig;
    struct SplitComplex {
        float* realp;
        float* imagp;
    };
#endif

#if JUCE_MAC || JUCE_IOS || DROWAUDIO_USE_FFTREAL

//==============================================================================
class FFTProperties
{
public:
    //==============================================================================
	FFTProperties (int fftSizeLog2_)
        : fftSizeLog2 (fftSizeLog2_),
		  fftSize (1L << fftSizeLog2),
		  fftSizeMinus1 (fftSize - 1),
		  fftSizeHalved (fftSize >> 1),
		  oneOverFFTSizeMinus1 (1.0 / fftSizeMinus1),
		  oneOverFFTSize (1.0 / fftSize)
	{
	}
	
    //==============================================================================
	long fftSizeLog2;
	long fftSize;
	long fftSizeMinus1;
	long fftSizeHalved;
	
	double oneOverFFTSizeMinus1;
	double oneOverFFTSize;	
	
private:
    //==============================================================================
	friend class FFTOperation;
	
	FFTProperties();
	
	void setFFTSizeLog2 (int newFFTSizeLog2)
	{
		fftSizeLog2 = newFFTSizeLog2;
		fftSize = (1L << fftSizeLog2);
		fftSizeMinus1 = (fftSize - 1);
		fftSizeHalved = (fftSize >> 1);
		oneOverFFTSizeMinus1 = (1.0 / fftSizeMinus1);
		oneOverFFTSize = (1.0 / fftSize);
	}
	
    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTProperties);
};

//==============================================================================
/** Performs various FFT operations.
	This class wraps all the platform specific FFT operations.
 */
class FFTOperation
{
public:
    //==============================================================================
	FFTOperation (int fftSizeLog2);

	~FFTOperation();

	void setFFTSizeLog2 (int newFFTSize);
	
	const FFTProperties& getFFTProperties() {	return fftProperties;       }
	
	const SplitComplex& getFFTBuffer()      {	return fftBufferSplit;		}
	
	void performFFT (float* samples);
	
private:
    //==============================================================================
	FFTProperties fftProperties;
	HeapBlock<float> fftBuffer;
	
	FFTConfig fftConfig;
	SplitComplex fftBufferSplit;
	
	//==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FFTOperation);
};

#endif // JUCE_MAC || JUCE_IOS || DROWAUDIO_USE_FFTREAL
#endif //__DROWAUDIO_FFTOPERATION__