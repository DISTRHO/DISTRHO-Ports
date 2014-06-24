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
#include "SynthEngine.h"
#include "AudioUtils.h"
#include "BlepData.h"
#include "DelayLine.h"
#include "SawOsc.h"
#include "PulseOsc.h"
#include "TriangleOsc.h"

class ObxdOscillatorB
{
private:
	float SampleRate;
	float pitch1;
	float pitch2;
	float sampleRateInv;


	float x1,x2;

	float osc1Factor;
	float osc2Factor;

	float pw1w,pw2w;
	//blep const
	const int n;
	const int hsam;
	const float *Blep;
	//delay line implements fixed sample delay
	DelayLine *del1,*del2;
	DelayLine *xmodd;
	DelayLineBoolean *syncd;
	DelayLine *syncFracd;
	DelayLine *cvd;
	Random wn;
	SawOsc o1s,o2s;
	PulseOsc o1p,o2p;
	TriangleOsc o1t,o2t;
public:

	float tune;//+-1 
	int oct;

	float notePlaying;


	float totalDetune;

	float osc2Det;
	float pulseWidth;
	float pw1,pw2;


	bool quantizeCw;

	float o1mx,o2mx;
	float nmx;
	float pto1,pto2;


	//osc pitches
	float osc1Saw,osc2Saw,
		osc1Pul,osc2Pul;

	float osc1p,osc2p;
	bool hardSync;
	float xmod;


	ObxdOscillatorB() : 
		n(Samples*2),
		hsam(Samples),
		Blep(blep),
		o1s(),o2s(),
		o1p(),o2p(),
		o1t(),o2t()
	{
		totalDetune = 0;
		wn = Random(Random::getSystemRandom().nextInt64());
		osc1Factor = wn.nextFloat()-0.5;
		osc2Factor = wn.nextFloat()-0.5;
		nmx=0;
		oct=0;
		tune=0;
		pw1w=pw2w=0;
		pto1=pto2=0;
		pw1=pw2=0;
		xmod = 0;
		hardSync = false;
		osc1p=osc2p=10;
		osc1Saw=osc2Saw=osc1Pul=osc2Pul=false;
		osc2Det = 0;
		notePlaying = 30;
		pulseWidth = 0;
		o1mx=o2mx=0;
		Blep = blep;
		x1=wn.nextFloat();
		x2=wn.nextFloat();

		del1 = new DelayLine(hsam);
		del2 = new DelayLine(hsam);
		xmodd = new DelayLine(hsam);
		syncd = new DelayLineBoolean(hsam);
		syncFracd =  new DelayLine(hsam);
		cvd = new DelayLine(hsam);
	}
	~ObxdOscillatorB()
	{
		delete del1;
		delete del2;
		delete xmodd;
		delete cvd;
		delete syncd;
		delete syncFracd;
	}
	void setSampleRate(float sr)
	{
		SampleRate = sr;
		sampleRateInv = 1.0f / SampleRate;
	}
	inline float ProcessSample()
	{
		pitch1 = getPitch(notePlaying + (quantizeCw?((int)(osc1p)):osc1p)+ pto1 + tune + oct+totalDetune*osc1Factor);
		if(pitch1 > 21000)
			pitch1 = 21000;
		bool hsr = false;
		float hsfrac=0;
		float fs = pitch1*(sampleRateInv);
		x1+=fs;
		hsfrac = 0;
		float osc1mix=0.0f;
		float pwcalc =jlimit<float>(0.1f,1.0f,(pulseWidth + pw1)*0.5f + 0.5f);

		if(osc1Pul)
			o1p.processMaster(x1,fs,hsr,hsfrac,pwcalc,pw1w);
		if(osc1Saw)
			o1s.processMaster(x1,fs,hsr,hsfrac);
		else if(!osc1Pul)
			o1t.processMaster(x1,fs,hsr,hsfrac);

		if(x1 >= 1.0f)
			x1-=1.0f;

		pw1w = pwcalc;

		hsr &= hardSync;
		//Delaying our hard sync gate signal and frac
		syncd->feedDelay(hsr);
		syncFracd ->feedDelay(hsfrac);
		hsr = syncd->getDelayedSample();
		hsfrac = syncFracd->getDelayedSample();

		if(osc1Pul)
			osc1mix += o1p.getValue(x1,pwcalc) + o1p.aliasReduction();
		if(osc1Saw)
			osc1mix += o1s.getValue(x1) + o1s.aliasReduction();
		else if(!osc1Pul)
			osc1mix = o1t.getValue(x1) + o1t.aliasReduction();
		//Pitch control needs additional delay buffer to compensate
		//This will give us less aliasing on xmod
		//Hard sync gate signal delayed too
		cvd->feedDelay( getPitch(notePlaying + osc2Det + (quantizeCw?((int)(osc2p)):osc2p) + pto2+ osc1mix *xmod + tune + oct +totalDetune*osc2Factor));
		pitch2 = cvd->getDelayedSample();


		if(pitch2>21000)
			pitch2=21000;
		fs = pitch2 * (sampleRateInv);

		pwcalc = jlimit<float>(0.1f,1.0f,(pulseWidth + pw2)*0.5f + 0.5f);

		float osc2mix=0.0f;

		x2 +=fs;

		if(osc2Pul)
			o2p.processSlave(x2,fs,hsr,hsfrac,pwcalc,pw2w);
		if(osc2Saw)
			o2s.processSlave(x2,fs,hsr,hsfrac);
		else if(!osc2Pul)
			o2t.processSlave(x2,fs,hsr,hsfrac);


		if(x2 >= 1.0f)
			x2-=1.0;


		pw2w=pwcalc;
		//On hard sync reset slave phase is affected that way
		if(hsr)
		{
			float fracMaster = (fs * hsfrac);
			x2 =fracMaster;
		}
		//Delaying osc1 signal
		xmodd ->feedDelay(osc1mix);
		//And getting delayed back
		osc1mix = xmodd->getDelayedSample();

		if(osc2Pul)
			osc2mix += o2p.getValue(x2,pwcalc) + o2p.aliasReduction();
		if(osc2Saw)
			osc2mix += o2s.getValue(x2) + o2s.aliasReduction();
		else if(!osc2Pul)
			osc2mix = o2t.getValue(x2) + o2t.aliasReduction();

		//mixing 
		float res =o1mx*osc1mix + o2mx *osc2mix + (wn.nextFloat()-0.5)*(nmx*1.3);
		return res*3;
	}
};