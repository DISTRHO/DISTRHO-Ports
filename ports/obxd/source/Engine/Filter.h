/*
	==============================================================================
	This file is part of Obxd synthesizer.

	Copyright © 2013-2014 Filatov Vadim
	
	Contact author via email :
	justdat_@_e1.ru

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
#pragma once
#include "ObxdVoice.h"
#include <math.h>
class Filter
{
private:
	float s1,s2,s3,s4;
	float R;
	float R24;
	float rcor,rcorInv;
	float rcor24,rcor24Inv;
	//24 db multimode
	float mmt;
	int mmch;
public:
	float SampleRate;
	float sampleRateInv;
	bool bandPassSw;
	float mm;
	Filter()
	{
		bandPassSw = false;
		mm=0;
		s1=s2=s3=s4=0;
		SampleRate = 44000;
		sampleRateInv = 1 / SampleRate;
		rcor =560.0 / 44000;
		rcorInv = 1 / rcor;
		rcor24 = 970.0 / 44000;
		rcor24Inv = 1 / rcor24;
		R=1;
		R24=0;
	}
	void setMultimode(float m)
	{
		mm = m;
		mmch = (int)(mm * 3);
		mmt = mm*3-mmch;
	}
	inline void setSampleRate(float sr)
	{
		SampleRate = sr;
		sampleRateInv = 1/SampleRate;
		float rcrate =sqrt((44000/SampleRate));
		rcor = (480.0 / 44000)*rcrate;
		rcor24 = (970.0 / 44000)*rcrate;
		rcorInv = 1 / rcor;
		rcor24Inv = 1 / rcor24;
	}
	inline void setResonance(float res)
	{
		R = 1-res;
		R24 =( 3.5 * res);
	}
	inline float NR(float sample, float g)
	{ 
		float y = ((sample- R * s1*2 - g*s1  - s2)/(1+ g*(2*R + g))) + dc;
		return y;
	}
	inline float NR24(float sample,float g,float lpc)
	{
		float ml = 1 / (1+g);
		float S = (lpc*(lpc*(lpc*s1 + s2) + s3) +s4)*ml;
		float G = lpc*lpc*lpc*lpc;
		float y = (sample - R24 * S) / (1 + R24*G);
		return y + 1e-8;
	}
	inline float Apply4Pole(float sample,float g)
	{
			float g1 = (float)tan(g *sampleRateInv * juce::float_Pi);
			g = g1;
			
			float lpc = g / (1 + g);
			float y0 = NR24(sample,g,lpc);
			//first low pass in cascade
			double v = (y0 - s1) * lpc;
			double res = v + s1;
			s1 = res + v;
			//damping
			s1 =atan(s1*rcor24)*rcor24Inv;

			float y1= res;
			float y2 = tptpc(s2,y1,g);
			float y3 = tptpc(s3,y2,g);
			float y4 = tptpc(s4,y3,g);
			float mc = 0.0f;
			switch(mmch)
			{
			case 0:
				mc = ((1 - mmt) * y4 + (mmt) * y3);
				break;
			case 1:
				mc = ((1 - mmt) * y3 + (mmt) * y2);
				break;
			case 2:
				mc = ((1 - mmt) * y2 + (mmt) * y1);
				break;
			case 3:
				mc = y1;
				break;
			}
			//half volume comp
			return mc * (1 + R24 * 0.45);
	}
	inline float Apply(float sample,float g)
        {
			float g1 = (float)tan(g *sampleRateInv * juce::float_Pi);
			g = g1;
            //float v = ((sample- R * s1*2 - g2*s1 - s2)/(1+ R*g1*2 + g1*g2));
			float v = NR(sample,g);
            float y1 = v*g + s1;
            s1 = v*g+y1;
			//damping
			s1 = atan(s1 * rcor) * rcorInv;

			float y2 = y1*g + s2;
			s2 = y2 + y1*g;
            double mc;
			if(!bandPassSw)
            mc = (1-mm)*y2 + (mm)*v;
			else
			{

				mc =2 * ( mm < 0.5 ? 
					((0.5 - mm) * y2 + (mm) * y1):
					((1-mm) * y1 + (mm-0.5) * v)
						);
			}

			return mc;
        }
};