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
#include "BlepData.h"
class SawOsc 
{
	DelayLine* del1;
	float *buffer1;
	const int hsam;
	const int n;
	int bP1;
public:
	SawOsc() : hsam(Samples)
		, n(Samples*2)
	{
		bP1=0;
		del1 = new DelayLine(hsam);
		buffer1= new float[n];
		for(int i = 0 ; i < n ; i++)
			buffer1[i]=0;
	}
	~SawOsc()
	{
		delete del1;
		delete buffer1;
	}
	inline float aliasReduction()
	{
		return -getNextBlep(buffer1,bP1);
	}
	inline void processMaster(float x,float delta,bool& hardSyncReset,float& hardSyncFrac)
	{
		if(x >= 1.0f)
		{
			x-=1.0f;
			hardSyncFrac = x/delta;
			mixInImpulseCenter(buffer1,bP1,x/delta, 1);
			hardSyncReset = true;
		}
	}
	inline float getValue(float x)
	{
		del1->feedDelay(x - 0.5);
		return del1->getDelayedSample();
	}
	inline float getValueFast(float x)
	{
		return x - 0.5;
	}
	inline void processSlave(float x , float delta,bool hardSyncReset,float hardSyncFrac)
	{
		if(x >= 1.0f)
		{
			x -= 1.0f;
			if(((!hardSyncReset)||(x/delta > hardSyncFrac)))//de morgan processed equation
			{
				mixInImpulseCenter(buffer1,bP1,x/delta, 1);
			}
			else
			{
				//if transition do not ocurred 
				x+=1;
			}
		}
		if(hardSyncReset)
		{
			float fracMaster = (delta * hardSyncFrac);
			float trans = (x-fracMaster);
			mixInImpulseCenter(buffer1,bP1,hardSyncFrac,trans);
		}
	}
	inline void mixInImpulseCenter(float * buf,int& bpos,float offset, float scale) 
	{
		int lpIn =(int)(B_OVERSAMPLING*(offset));
		float frac = offset * B_OVERSAMPLING - lpIn;
		for(int i = 0 ; i <n;i++)
		{
			float mixvalue = 0;
			mixvalue = (blep[lpIn]*(1-frac)+blep[lpIn+1]*(frac));
			if(i>=Samples)
				mixvalue-=1;
			buf[(bpos+i)&(n-1)]  += mixvalue*scale;
			lpIn += B_OVERSAMPLING;
		}
	}
	inline float getDelayedSample(float* buf,int& dpos)
	{
		int idx;
		idx = dpos-(hsam);
		if(idx <0)
			idx+=hsam;
		return buf[idx];
	}
	inline void feedDelay(float* buf,int& dpos,float sm)
	{
		buf[dpos] = sm;
		dpos++;
		if(dpos >= (hsam))
			dpos-=(hsam);
	}
	inline float getNextBlep(float* buf,int& bpos) 
	{
		buf[bpos]= 0.0f;
		bpos++;

		// Wrap pos
		if (bpos>=n) 
		{
			bpos -= n;
		}
		return buf[bpos];
	}
};