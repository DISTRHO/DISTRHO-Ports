/*---------------------------------------------------

Compressor
==========
A simple feed forward compressor for use in AIR plugin
 
 A simplified implementation of Martin Zuther's "Squeezer" with some modifications
https://github.com/mzuther/Squeezer

-----------------------------------------------------*/

#include <float.h>
#include "Compressor.h"

//====================================================

Compressor::Compressor(int channels, int sample_rate) :
	// Set de-normalization params to float and double absolute minimum
	fDeNormal(FLT_MIN),
	dDeNormal(DBL_MIN),
	dBufferLength(0.1),
	nChannels(channels),
	nSampleRate(sample_rate)
{
	jassert((channels == 1) || (channels == 2));

	bufInputSamples.clear();
	bufSideChainSamples.clear();
	bufOutputSamples.clear();

	bufInputSamples.setSize(nChannels, 1);
	bufSideChainSamples.setSize(nChannels, 1);
	bufOutputSamples.setSize(nChannels, 1);
    
    dCrestFactor = 20.0;
    dReleaseCoefLinear = 26 * dBufferLength / 3.0;
    
    setMakeupGain(0.0);
    
    // Clear the sidechain array in case of junk
    p_arrSideChain.clear();

	// Add sidechain objects and add an entry for each channel into the sample buffers
	for (int nChannel = 0; nChannel < 2; ++nChannel)
	{
        // Make a new sidechain element for each channel
		p_arrSideChain.add(new SideChain(nSampleRate));
	}
}

Compressor::~Compressor() {

}

double Compressor::getDetectorRmsFilter()
/* Get current detector RMS rate.

	return value (double): returns current detector RMS rate*/
{
	return p_arrSideChain[0]->getDetectorRmsFilter();
}

void Compressor::setDetectorRmsFilter(double dDetectorRateMsNew)
/* Set new detector RMS rate.

	dDetectorRateMsNew (double): new detector RMS filter rate

	return value: none*/
{
	for (int nChannel = 0; nChannel < nChannels; ++nChannel)
	{
		p_arrSideChain[nChannel]->setDetectorRmsFilter(dDetectorRateMsNew);
	}
}

double Compressor::getThreshold()
/* Get current threshold

	return value (double): current threshold in dB*/
{
	return p_arrSideChain[0]->getThreshold();
}

void Compressor::setThreshold(double dThresholdNew)
/* Set new threshold.

	dThresholdNew (double): new threshold in dB

	return value: none*/
{
	for (int nChannel = 0; nChannel < nChannels; ++nChannel)
	{
		p_arrSideChain[nChannel]->setThreshold(dThresholdNew);
	}
}

double Compressor::getRatio()
/* Get current comp ratio.

	return value (double): returns current ratio*/
{
	double dRatioNew = p_arrSideChain[0]->getRatio();

	return dRatioNew;
}

void Compressor::setRatio(double dRatioNew)
/* Set new ratio.

	dRatioNew (double): new ratio

	return value: none*/
{
	for (int nChannel = 0; nChannel < nChannels; ++nChannel)
	{
		p_arrSideChain[nChannel]->setRatio(dRatioNew);
	}
}

int Compressor::getAttackRate()
/* Get current attack rate.

	return value (int): returns the current attack rate in ms*/
{
	return p_arrSideChain[0]->getAttackRate();
}

void Compressor::setAttackRate(int nAttackRateNew)
/* Set new attack rate.

	nAttackRateNew (int): new attack rate in ms

	return value: none*/
{
	for (int nChannel = 0; nChannel < nChannels; ++nChannel)
	{
		p_arrSideChain[nChannel]->setAttackRate(nAttackRateNew);
	}
}

int Compressor::getReleaseRate()
/* Get current release rate.

return value (int): returns the current release rate in ms*/
{
	return p_arrSideChain[0]->getReleaseRate();
}

void Compressor::setReleaseRate(int nReleaseRateNew)
/* Set new release rate.

nReleaseRateNew (int): new release rate in ms

return value: none*/
{
	for (int nChannel = 0; nChannel < nChannels; ++nChannel)
	{
		p_arrSideChain[nChannel]->setReleaseRate(nReleaseRateNew);
	}
}

double Compressor::getMakeupGain()
/* Get current make-up gain.

	return value (double): current make-up gain in dB*/
{
	return dMakeupGainDb;
}

void Compressor::setMakeupGain(double dMakeupGainNew)
/* Set new make-up gain.

	nMakeupGainNew (double): new makeup gain in dB
	
	return value: none*/
{
	dMakeupGainDb = dMakeupGainNew;
	dMakeupGain = SideChain::dbtolvl(dMakeupGainDb);
}

double Compressor::getGainReduction(int nChannel)
/* Get current gain reduction.

	nChannel (int): queried audio channel
	
	return value (double): returns current gain reduction in dB*/
{
	jassert(nChannel >= 0);
	jassert(nChannel < nChannels);

	return arrGainReduction[nChannel];
}

double Compressor::getGainReductionPeak(int nChannel)
/* Get current peak gain reduction

	nChannel (int): queried audio channel
	
	return value (double): returns current peak gain reduction in dB*/
{
	jassert(nChannel >= 0);
	jassert(nChannel < nChannels);

	return arrGainReductionPeak[nChannel];
}

double Compressor::getSampleRate()
{
    return p_arrSideChain[0]->getSampleRate();
}

void Compressor::setSampleRate(double newSampleRate)
{
	if (newSampleRate != p_arrSideChain[0]->getSampleRate()) 
	{
		for (int nChannel = 0; nChannel < nChannels; ++nChannel)
		{
			p_arrSideChain[nChannel]->setSampleRate(newSampleRate);
		}

		nSampleRate = (int)newSampleRate;
	}
}

void Compressor::resetSideChain() {
    for (int nChannel = 0; nChannel < 2; ++nChannel)
    {
        p_arrSideChain[nChannel]->reset();
    }
}

double Compressor::getTempGainReduction()
{
	return tempGainReduction;
}

void Compressor::processBlock(AudioBuffer<float> &buffer)
{
	int nNumSamples = buffer.getNumSamples();

	// Loop through samples
	for (int nSample = 0; nSample < nNumSamples; ++nSample)
	{
		// Get input samples, de-normalize and store in buffer
		for (int nChannel = 0; nChannel < nChannels; ++nChannel)
		{
			// Get current input sample (both as float and as double)
			float fInputSample = buffer.getSample(nChannel, nSample);
			double dInputSample = (double) fInputSample;

			// Remove denormal numbers input samples
			fInputSample += fDeNormal;
			dInputSample += dDeNormal;
			fInputSample -= fDeNormal;
			dInputSample -= dDeNormal;

			// jassert(arrInputSamples.size() == 2 && arrSidechainSamples.size() == 2 && arrOutputSamples.size() == 2);

			// Store de-normalized input sample
			// arrInputSamples.set(nChannel, dInputSample);
			bufInputSamples.setSample(nChannel, 0, dInputSample);

			// Process each channel instead of stereo linking (for channel compability)
			// arrSidechainSamples.set(nChannel, arrInputSamples[nChannel]);
			bufSideChainSamples.setSample(nChannel, 0, bufInputSamples.getSample(nChannel, 0));

			// Calculate level of sidechain sample
			double dSideChainInputLevel = SideChain::lvltodb(fabs(bufSideChainSamples.getSample(nChannel, 0)));

			// Apply crest factor
			dSideChainInputLevel += dCrestFactor;

			// Send current input sample to gain reduction
			p_arrSideChain[nChannel]->processSample(dSideChainInputLevel);

			// Store gain reduction (might only be for metering)
			// arrGainReduction.set(nChannel, p_arrSideChain[nChannel]->getGainReduction());

			// Apply gain reduction to current input sample
			double dGainReduction = p_arrSideChain[nChannel]->getGainReduction();
			tempGainReduction = dGainReduction;

			// arrOutputSamples.set(nChannel, arrInputSamples[nChannel] / SideChain::dbtolvl(dGainReduction));
			bufOutputSamples.setSample(nChannel, 0, bufInputSamples.getSample(nChannel, 0) / SideChain::dbtolvl(dGainReduction));

			// arrOutputSamples.set(nChannel, arrOutputSamples[nChannel] * dMakeupGain);
			bufOutputSamples.setSample(nChannel, 0, bufOutputSamples.getSample(nChannel, 0) * dMakeupGain);

			// Set sample to output buffer
			// float fOutput = arrOutputSamples[nChannel];
			float fOutput = bufOutputSamples.getSample(nChannel, 0);
			buffer.setSample(nChannel, nSample, fOutput);
		}
	}
}
