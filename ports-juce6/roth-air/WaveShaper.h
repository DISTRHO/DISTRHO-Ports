/*
------------------------------------------------------------------------------

Simple waveshaper
================
A simple harmonic waveshaper for use in Roth-AIR
By Daniel Rothmann

Provided under GNU General Public license:
http://www.gnu.org/licenses/

------------------------------------------------------------------------------
*/

#ifndef WAVESHAPER_H_INCLUDED
#define WAVESHAPER_H_INCLUDED

#include "JuceHeader.h"

class WaveShaper
{
public:
	WaveShaper(int channels);

	double getAmount();
	void setAmount(double newAmount);

	void processBlock(AudioBuffer<float> &buffer);

private:
	double amount;
	int nChannels;
};



#endif  // WAVESHAPER_H_INCLUDED
