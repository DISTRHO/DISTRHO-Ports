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
class DelayLine
{
private:
	float* dl;
	int iidx;
	int maxc;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayLine)
public:
	DelayLine() 
	{
		dl  = NULL;
		iidx = 0;
		maxc=0;
	}
	DelayLine(int count)
	{
		iidx=0;
		maxc=count;
		dl = new float[maxc];
		for(int i = 0 ; i < maxc;i++)
			dl[i]=0;
	}
	~DelayLine()
	{
		delete dl;
	}
	inline void feedDelay(float sm)
	{
		dl[iidx] = sm;
		iidx++;
		if(iidx >= (maxc))
			iidx-=(maxc);
	}
	inline float getDelayedSample()
	{
		int idx;
		idx = iidx-(maxc);
		if(idx <0)
			idx+=maxc;
		return dl[idx];
	}
};
class DelayLineBoolean
{
private:
	bool* dl;
	int iidx;
	int maxc;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayLineBoolean)
public:
	DelayLineBoolean() 
	{
		dl  = NULL;
		iidx = 0;
		maxc=0;
	}
	DelayLineBoolean(int count)
	{
		iidx=0;
		maxc=count;
		dl = new bool[maxc];
		for(int i = 0 ; i < maxc;i++)
			dl[i]=0;
	}
	~DelayLineBoolean()
	{
		delete dl;
	}
	inline void feedDelay(bool sm)
	{
		dl[iidx] = sm;
		iidx++;
		if(iidx >= (maxc))
			iidx-=(maxc);
	}
	inline bool getDelayedSample()
	{
		int idx;
		idx = iidx-(maxc);
		if(idx <0)
			idx+=maxc;
		return dl[idx];
	}
};