/*---------------------------------------------------

SideChain (Compressor)
==========
A simple feed forward compressor for use in AIR plugin - this class contains side chain calculation

A simplified implementation of Martin Zuther's "Squeezer" with some modifications
https://github.com/mzuther/Squeezer

-----------------------------------------------------*/

#include "SideChain.h"

SideChain::SideChain(int nSampleRate)
{
	// Convert sample rate to double
	dSampleRate = (double) nSampleRate;

	// Initialize variables
	setThreshold(0.0);
	setRatio(1.0);

	// RMS smoothing for incoming values
	setDetectorRmsFilter(5.0);

	// Set sidechain speeds (ms)
	setAttackRate(1);
	setReleaseRate(60);

	reset();
}

SideChain::~SideChain() 
{
}

void SideChain::reset()
/*  Reset all relevant variables.
return value: none
*/
{
	dGainReduction = 0.0;
	dGainCompensation = 0.0;
	dDetectorOutputLevelSquared = 0.0;

	dCrestFactorAutoGain = 20.0;
}

double SideChain::getDetectorRmsFilter()
/* Get current detector RMS filter rate.

	return value (double): returns current detector RMS filter rate*/
{
	return dDetectorRateMs;
}

void SideChain::setDetectorRmsFilter(double dDetectorRateMsNew)
/* Set new detector RMS filter rate.
	dDetectorRateMsNew (double): new detector RMS filter rate; set to 0.0 to disable RMS

	return value: none*/
{
	if (dDetectorRateMsNew <= 0.0)
	{
		dDetectorRateMs = 0.0;
		dDetectorCoefficient = 0.0;
	}
	else
	{
		dDetectorRateMs = dDetectorRateMsNew;
		double dDetectorRateSeconds = dDetectorRateMs / 1000.0;

		// Apply logarithmic envelope to the coeffecient
		dDetectorCoefficient = exp(log(0.10) / (dDetectorRateSeconds * dSampleRate));
	}
}

double SideChain::getThreshold()
/* Get current threshold.

	return value (double): returns current threshold in dB*/
{
	return dThreshold;
}

void SideChain::setThreshold(double dThresholdNew)
/* Set new threshold.

	dThresholdNew (double): new threshold in dB

	return value: none
*/
{
	dThreshold = dThresholdNew;
	dGainCompensation = queryGainComputer(dCrestFactorAutoGain) / 2.0;
}

double SideChain::getRatio()
/* Get current compression ratio.
	return value (double): returns current compression ratio*/
{
	return 1.0 / (1.0 - dRatioInternal);
}

void SideChain::setRatio(double dRatioNew)
/* Set new compression ratio.

	dRatioNew (double): new compression ratio

	return value: none*/
{
	dRatioInternal = 1.0 - (1.0 / dRatioNew);
	dGainCompensation = queryGainComputer(dCrestFactorAutoGain) / 2.0;
}

int SideChain::getAttackRate()
/* Get attack rate.

	return value (integer): returns current attack rate (ms)*/
{
	return nAttackRate;
}

void SideChain::setAttackRate(int nAttackRateNew)
/* Set new attack rate.

	nAttackRateNew (integer): new attack rate in ms

	return value: none
	*/
{
	nAttackRate = nAttackRateNew;

	if (nAttackRate == 0)
	{
		dAttackCoefficient = 0.0;
	}
	else
	{
		double dAttackRateSeconds = nAttackRate / 1000.0;

		// Apply logarithmic envelope to the coeffecient
		dAttackCoefficient = exp(log(0.10) / (dAttackRateSeconds * dSampleRate));
	}
}

int SideChain::getReleaseRate()
/* Get release rate.

return value (integer): returns current release rate (ms)*/
{
	return nReleaseRate;
}

void SideChain::setReleaseRate(int nReleaseRateNew)
/* Set new release rate.

nReleaseRateNew (integer): new release rate in ms

return value: none
*/
{
	nReleaseRate = nReleaseRateNew;

	if (nReleaseRate == 0)
	{
		dReleaseCoefficient = 0.0;
	}
	else
	{
		double dReleaseRateSeconds = nReleaseRate / 1000.0;

		// Apply logarithmic envelope to the coeffecient
		dReleaseCoefficient = exp(log(0.10) / (dReleaseRateSeconds * dSampleRate));
	}
}

double SideChain::getGainReduction()
/* Get current gain reduction.

	return value (double): returns current gain reduction in dB*/
{
	// Return gain reduction - gain compensation (edited script)
	return dGainReduction - dGainCompensation;
}

double SideChain::queryGainComputer(double dInputLevel)
/* Calculate gain reduction and envs from input lvl

	dInputLevel (double): current input level in dB

	return value: calculated gain reduction in dB*/
{
	// Calculate how far input is above threshold
	double dAboveThreshold = dInputLevel - dThreshold;

	// If level is under threshold return 0, otherwise return amt above threshold times the ratio
	if (dInputLevel <= dThreshold)
	{
		return 0.0;
	}
	else
	{
		return dAboveThreshold * dRatioInternal;
	}
}

void SideChain::processSample(double dInputLevel)
/* Process a single sample value

	dInputLevel (double): current audio sample value in dB

	return value: current gain rection in dB*/
{
	// Send input level to gain computer
	dGainReductionIdeal = queryGainComputer(dInputLevel);

	// Filter calculated gain reductino through level detection filter
	double dGainReductionNew = applyLevelDetectionFilter(dGainReductionIdeal);

	// Send input from gain computer to level detector (smooth branching hard coded)
	applyDetectorSmoothDecoupled(dGainReductionNew);
}

double SideChain::applyLevelDetectionFilter(double dDetectorInputLevel)
{
	// Bypass RMS sensing if rate is <= 0
	if (dDetectorRateMs <= 0.0)
	{
		return dDetectorInputLevel;
	}
	else
	{
		// Do some root mean square magic
		double dDetectorInputLevelSquared = dDetectorInputLevel * dDetectorInputLevel;
		double dDetectorOutputLevelSquaredOld = dDetectorOutputLevelSquared;

		dDetectorOutputLevelSquared = (dDetectorCoefficient * dDetectorOutputLevelSquaredOld) + (1.0 - dDetectorCoefficient) * dDetectorInputLevelSquared;

		double dDetectorOutputLevel = sqrt(dDetectorOutputLevelSquared);
		return dDetectorOutputLevel;
	}
}

void SideChain::applyDetectorSmoothDecoupled(double dGainReductionNew)
/* Calculate smooth decoupled detector (s-curve)

	dGainReductionNew (double): calculated gain reduction in dB

	return value: none*/
{
	// apply release envelope
	if (dReleaseCoefficient == 0.0)
	{
		dGainReductionIntermediate = dGainReductionNew;
	}
	else
	{
		double dGainReductionIntermediateOld = dGainReductionIntermediate;
		dGainReductionIntermediate = (dReleaseCoefficient * dGainReductionIntermediateOld) + (1.0 - dReleaseCoefficient) * dGainReductionNew;

		// Maximally fast peak detection
		if (dGainReductionNew > dGainReductionIntermediate)
		{
			dGainReductionIntermediate = dGainReductionNew;
		}
	}

	// apply attack envelope
	if (dAttackCoefficient == 0.0)
	{
		dGainReduction = dGainReductionIntermediate;
	}
	else
	{
		double dGainReductionOld = dGainReduction;
		dGainReduction = (dAttackCoefficient * dGainReductionOld) + (1.0 - dAttackCoefficient) * dGainReductionIntermediate;
	}
}

double SideChain::lvltodb(double dLevel)
/* Convert level from linear scale to dB

	dLevel (double): audio level

	return value (double): given level in dB*/
{
	double dDecibels = 20.0 * log10(dLevel);

	return dDecibels;
}

double SideChain::dbtolvl(double dDecibels)
/* Convert level from dB to linear scale

	dDecibels (double): audio level in dB

	return value (double): given level in linear scale*/
{
	double dLevel = pow(10.0, dDecibels / 20.0);
	return dLevel;
}

double SideChain::getSampleRate()
{
    return dSampleRate;
}

void SideChain::setSampleRate(double newSampleRate)
{
    dSampleRate = newSampleRate;
}
