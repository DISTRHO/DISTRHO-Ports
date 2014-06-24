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
#include "SynthEngine.h"
class Lfo
{
private:
	float phase;
	float s, sq, sh;
	float s1;
    Random rg;
	float SampleRate;
	float SampleRateInv;
public:
	float Frequency;
	int waveForm;
	Lfo()
	{
		s1=0;
		Frequency=1;
		phase=0;
		s=sq=sh=0;
		rg=Random();
	}
	inline float getVal()
	{
		 float Res = 0;
            if((waveForm &1) !=0 )
                Res+=s;
            if((waveForm&2)!=0)
                Res+=sq;
            if((waveForm&4)!=0)
                Res+=sh;
			return tptlpupw(s1, Res,700,SampleRateInv);
	}
	void setSamlpeRate(float sr)
	{
		SampleRate=sr;
		SampleRateInv = 1 / SampleRate;
	}
	inline void update()
	{
		phase+=((Frequency * SampleRateInv * float_Pi*2));
		sq = (phase>0?1:-1);
		s = sin(phase);
		if(phase > float_Pi)
		{
			phase-=2*float_Pi;
			sh = rg.nextFloat()*2-1;
		}

	}
};