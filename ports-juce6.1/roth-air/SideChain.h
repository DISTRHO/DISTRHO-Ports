/*---------------------------------------------------
 
 SideChain (Compressor)
 ==========
 A simple feed forward compressor for use in AIR plugin - this class contains side chain calculation
 
 A simplified implementation of Martin Zuther's "Squeezer" with some modifications
 https://github.com/mzuther/Squeezer
 
 -----------------------------------------------------*/

#ifndef __SIDECHAIN_H__
#define __SIDECHAIN_H__

#include "JuceHeader.h"

class SideChain
{
public:
	SideChain(int nSampleRate);
	~SideChain();

	void reset();

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

	double getGainReduction();

	void processSample(double dSampleValue);

	static double lvltodb(double dLevel);
	static double dbtolvl(double dDecibels);
    
    double getSampleRate();
    void setSampleRate(double newSampleRate);

private:
	// JUCE_LEAK_DETECTOR(SideChain);

	double dSampleRate;
	double dCrestFactorAutoGain;
	double dGainReduction;
	double dGainReductionIdeal = 0.0;
	double dGainReductionIntermediate = 0.0;
	double dGainCompensation;

	double dDetectorCoefficient;
	double dDetectorOutputLevelSquared;
	double dDetectorRateMs;
	int nDetectorType;
	
	double dThreshold;
	double dRatioInternal;

	int nAttackRate;
	double dAttackCoefficient = 0.0;

	int nReleaseRate;
	double dReleaseCoefficient = 0.0;

	double queryGainComputer(double dInputLevel);
	double applyLevelDetectionFilter(double dDetectorInputLevel);
	// void applyDetectorLinear(double dGainRectionNew);
	void applyDetectorSmoothDecoupled(double dGainReductionNew);
	// void applyDetectorSmoothBranching(double dGainReductionNew);
};

#endif //__SIDECHAIN_H__
