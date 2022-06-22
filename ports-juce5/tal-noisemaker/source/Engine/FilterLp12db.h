/*
	==============================================================================
	This file is part of Tal-NoiseMaker by Patrick Kunz.

	Copyright(c) 2005-2010 Patrick Kunz, TAL
	Togu Audio Line, Inc.
	http://kunz.corrupt.ch

	This file may be licensed under the terms of of the
	GNU General Public License Version 2 (the ``GPL'').

	Software distributed under the License is distributed
	on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
	express or implied. See the GPL for the specific language
	governing rights and limitations.

	You should have received a copy of the GPL along with this
	program. If not, go to http://www.gnu.org/licenses/gpl.html
	or write to the Free Software Foundation, Inc.,  
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
	==============================================================================
 */

#ifndef __FilterLp12db_h_
#define __FilterLp12db_h_

#include "OscNoise.h"

class FilterLp12db 
{
public:
private:
	float pi;
	float v2, iv2;
	float ay1, ay2, ay3, ay4, amf;
	float az1, az2, az3, az4;
	float at1, at2, at3, at4;

	float kfc, kfcr, kacr, k2vg, k2vgNoisy;

	// temporary variables
	float tmp;
	float sampleRateFactor, cutoffInOld;
	
	float resonanceInOld;
	float resonanceCorrPre;
	float resonanceCorrPost;

    OscNoise *oscNoise;

public:
	FilterLp12db(float sampleRate) 
	{
		pi= 3.1415926535f;
		v2= 2.0f;   // twice the 'thermal voltage of a transistor'
		iv2= 1.0f/v2;
		cutoffInOld = -1.0f;
		resonanceInOld = -1.0f;

		sampleRateFactor= 44100.0f/sampleRate;
		if (sampleRateFactor > 1.0f) 
		{
			sampleRateFactor= 1.0f;
		}

		oscNoise = new OscNoise(sampleRate);
        reset();
	}

    ~FilterLp12db()
    {
        delete oscNoise;
    }

public:
    void reset()
    {
        az1= az2= az3= az4= ay1= ay2= ay3= ay4= amf= 0.4f;
		at1= at2= at3 = at4 = 0.0f;
    }

	inline void process(float *input, const float cutoffIn, const float resonance, const bool calcCeff) 
	{
		// Filter based on the text "Non linear digital implementation of the moog ladder filter" by Antti Houvilainen
		// Adopted from Csound code at http://www.kunstmusik.com/udo/cache/moogladder.udo

		if (resonanceInOld != resonance)  
		{
			resonanceInOld = resonance;
			float resonanceInverted = 1.0f - resonance;
			float invertedSquare = resonanceInverted * resonanceInverted;
			resonanceCorrPre = 1.0f + (1.0f - invertedSquare) * 1.0f;
			resonanceCorrPost = 1.0f + resonance * 1.1f;
		}

		*input *=  resonanceCorrPre;

		// Resonance [0..1]
		// Cutoff from 0 (0Hz) to 1 (nyquist) 
		if (calcCeff && cutoffIn != cutoffInOld)
		{ 
			cutoffInOld = cutoffIn;
			kfc  = cutoffIn * sampleRateFactor * 0.5f; // ~sr/2 + tanh approximation correction
 
			// Frequency & amplitude correction
			kfcr = 1.8730f*(kfc*kfc*kfc) + 0.4955f*(kfc*kfc) - 0.6490f*kfc + 0.9988f;
			kacr = 1.0f + 0.9f * cutoffIn;
 
			tmp = - 2.0f * pi * kfcr * kfc; // Filter Tuning
			k2vg = (1.0f-(1.0f+tmp+tmp*tmp*0.5f+tmp*tmp*tmp*0.16666667f+tmp*tmp*tmp*tmp*0.0416666667f+tmp*tmp*tmp*tmp*tmp*0.00833333333f));
		}

        float rnd1 = 0.001f * oscNoise->getNextSamplePositive() * (1.0f -cutoffIn);

        k2vgNoisy = k2vg + rnd1 * cutoffIn;

		float inWithRes = *input - 4.2f * resonance * amf * kacr; 

		ay1 = az1 + k2vgNoisy * (rnd1 + inWithRes - at1);
		at1 = ay1;

		ay2 = az2 + k2vgNoisy * (at1-at2);
		at2 = ay2; 

		ay3 = az3 + k2vgNoisy * (at2-at3);
		at3 = ay3;

		ay4 = az4 + k2vgNoisy * (at3-at4);
		at4 = ay4;

		az1 = ay1;
		az2 = ay2;
		az3 = ay3;
		az4 = ay4;

		// 1/2-sample delay for phase compensation
		amf = ay4; // * 0.625f + az4 * 0.375f;

		amf = tanhClipper(amf);

		if (amf > 0.0f)
		{
			amf *= 0.99f;
		}

        *input = tanhClipper(at2) * (resonanceCorrPost + cutoffIn * resonance * 1.5f);
	}

	inline float tanhApp(const float x) 
	{
		return x;
	}

	inline float tanhClipper(float x) 
	{
		// return tanh(x);
		x *= 2.0f;
		float a = fabs(x);
		float b = 6.0f+a*(3.0f+a);
		return (x*b)/(a*b+12.0f);
	}
 };
#endif