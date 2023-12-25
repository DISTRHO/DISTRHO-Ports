/*
------------------------------------------------------------------------------

Simple waveshaper
==========
A simple harmonic waveshaper for use in Roth-AIR
By Daniel Rothmann

Provided under GNU General Public license:
http://www.gnu.org/licenses/

------------------------------------------------------------------------------
*/

#include "WaveShaper.h"

//====================================================

WaveShaper::WaveShaper(int channels)
{
	amount = 0;
	nChannels = channels;
}

double WaveShaper::getAmount()
{
	return amount;
}

void WaveShaper::setAmount(double newAmount)
{
	amount = 2 * newAmount / (1 - newAmount);
}

void WaveShaper::processBlock(AudioBuffer<float> &buffer)
{
	int nNumSamples = buffer.getNumSamples();

	// Loop through samples
	for (int nSample = 0; nSample < nNumSamples; ++nSample)
	{
		// Loop through channels
		for (int nChannel = 0; nChannel < nChannels; ++nChannel)
		{
			// Input sample (in)
			float in = buffer.getSample(nChannel, nSample);
			// Output sample (out)
			float out = 0.0;

			// Output sample becomes waveshaped in
			out = (1.0 + amount) * in / (1.0 + amount * abs(in));

			// Change sample value in buffer
			buffer.setSample(nChannel, nSample, out);
		}
	}
}
