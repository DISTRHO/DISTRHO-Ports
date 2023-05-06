/*---------------------------------------------------
 
 Compressor
 ==========
 A simple feed forward compressor for use in AIR plugin
 
 A simplified implementation of Martin Zuther's "Squeezer" with some modifications
 https://github.com/mzuther/Squeezer
 
 -----------------------------------------------------*/

#ifndef __COMPRESSOR_H__
#define __COMPRESSOR_H__

class SideChain;

#include "JuceHeader.h"
#include "SideChain.h"

//=============================================================
class Compressor
{
public:
	//=========================================================

	// Constructor
	Compressor(int channels, int sample_rate);
	~Compressor();

	// Declare functions for setting/getting comp parameters
	double getDetectorRmsFilter();
	void setDetectorRmsFilter(double dDetectorRateMsNew);

	double getThreshold();
	void setThreshold(double dThresholdNew);

	double getRatio();
	void setRatio(double dRatioNew);

	int getAttackRate();
	void setAttackRate(int nAttackRateNew);

	int getReleaseRate();
	void setReleaseRate(int nReleaseRateNew);

	double getMakeupGain();
	void setMakeupGain(double dMakeupGainNew);

	double getGainReduction(int nChannel);
	double getGainReductionPeak(int nChannel);
    
    double getSampleRate();
    void setSampleRate(double newSampleRate);
    
    void resetSideChain();

	double getTempGainReduction();

	// Declare function for processing audio
	void processBlock(AudioBuffer<float> &buffer);

private:
	// Declare de-normalization variables
    const float fDeNormal;
	const double dDeNormal;
	const double dBufferLength;

	// Variables for keeping track of channels and sample rate
	int nChannels;
	int nSampleRate;

	// Arrays for holding sample data
	OwnedArray<SideChain> p_arrSideChain;

	// Array<double> arrInputSamples;
	// Array<double> arrSidechainSamples;
	// Array<double> arrOutputSamples;

	AudioSampleBuffer bufInputSamples;
	AudioSampleBuffer bufSideChainSamples;
	AudioSampleBuffer bufOutputSamples;

	Array<double> arrGainReduction;
	Array<double> arrGainReductionPeak;

	AudioSampleBuffer bufGainReduction;
	AudioSampleBuffer bufGainReductionPeak;

	double dCrestFactor;

	double dReleaseCoefLinear;

	// Makeup gain variables
	double dMakeupGain;
	double dMakeupGainDb;

	// Might also need this
	double dGainNormalize;

	double tempGainReduction;
};

#endif // __COMRESSOR_H__
