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


const float sq2_12 = 1.0594630943592953f;

const float dc = 1e-18;
const float ln2 = 0.69314718056f;
const float mult = ln2 / 12.0;
inline static float getPitch(float index)
{
	return (440* expf( mult*index));
};

inline static float nlp(float& state,float inp,float cutoff,float sr)
{
	cutoff =atan((cutoff / (sr)))/juce::float_Pi;
	state = state + (inp - state) * cutoff +dc;
	return state;
};
inline static float tptlpupw(float & state , float inp , float cutoff , float srInv)
{
	cutoff = (cutoff * srInv)*juce::float_Pi;
	double v = (inp - state) * cutoff / (1 + cutoff);
	double res = v + state;
	state = res + v;
	return res;
}
inline static float tptlp(float& state,float inp,float cutoff,float srInv)
{
	cutoff = tan(cutoff * (srInv)* (juce::float_Pi)) ;
	double v = (inp - state) * cutoff / (1 + cutoff);
	double res = v + state;
	state = res + v;
	return res;
};
inline static float tptpc(float& state,float inp,float cutoff)
{
	double v = (inp - state) * cutoff / (1 + cutoff);
	double res = v + state;
	state = res + v;
	return res;
}
inline static float linsc(float param,const float min,const float max)
{
	 return (param) * (max - min) + min;
}
inline static float logsc(float param, const float min,const float max,const float rolloff = 19.0f)
{
	return ((expf(param * logf(rolloff+1)) - 1.0f) / (rolloff)) * (max-min) + min;
}

