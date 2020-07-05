/* ------------------------------------------------------------
name: "temper"
Code generated with Faust 2.5.32 (https://faust.grame.fr)
Compilation options: cpp, -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __TemperDsp_H__
#define  __TemperDsp_H__

/************************************************************************
 ************************************************************************
    FAUST Architecture File
    Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------

    This is sample code. This file is provided as an example of minimal
    FAUST architecture file. Redistribution and use in source and binary
    forms, with or without modification, in part or in full are permitted.
    In particular you can create a derived work of this FAUST architecture
    and distribute that work under terms of your choice.

    This sample code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 ************************************************************************
 ************************************************************************/
 
#include <math.h>
#include <algorithm>

#include "faust/gui/UI.h"
#include "faust/gui/meta.h"
#include "faust/dsp/dsp.h"

using std::max;
using std::min;

/******************************************************************************
*******************************************************************************

							       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/


/******************************************************************************
*******************************************************************************

			ABSTRACT USER INTERFACE

*******************************************************************************
*******************************************************************************/

//----------------------------------------------------------------------------
//  FAUST generated signal processor
//----------------------------------------------------------------------------

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <cmath>
#include <math.h>

float TemperDsp_faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS TemperDsp
#endif
#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class TemperDsp : public ::dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	float fRec3[2];
	FAUSTFLOAT fHslider1;
	float fRec4[2];
	int fSamplingFreq;
	float fConst0;
	float fConst1;
	FAUSTFLOAT fHslider2;
	float fRec6[2];
	FAUSTFLOAT fHslider3;
	float fRec7[2];
	float fRec5[3];
	float fConst2;
	float fConst3;
	float fRec8[2];
	FAUSTFLOAT fHslider4;
	float fRec9[2];
	FAUSTFLOAT fHslider5;
	float fRec10[2];
	float fVec0[2];
	float fRec2[2];
	float fRec1[2];
	float fRec0[2];
	FAUSTFLOAT fHslider6;
	float fRec11[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "0.0");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("filename", "temper");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "temper");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
	}

	virtual int getNumInputs() {
		return 1;
		
	}
	virtual int getNumOutputs() {
		return 1;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	static void classInit(int samplingFreq) {
		
	}
	
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = min(192000.0f, max(1.0f, float(fSamplingFreq)));
		fConst1 = (3.14159274f / fConst0);
		fConst2 = expf((0.0f - (25.0f / fConst0)));
		fConst3 = (1.0f - fConst2);
		
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(1.0f);
		fHslider1 = FAUSTFLOAT(-60.0f);
		fHslider2 = FAUSTFLOAT(20000.0f);
		fHslider3 = FAUSTFLOAT(1.0f);
		fHslider4 = FAUSTFLOAT(4.0f);
		fHslider5 = FAUSTFLOAT(1.0f);
		fHslider6 = FAUSTFLOAT(-3.0f);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec3[l0] = 0.0f;
			
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec4[l1] = 0.0f;
			
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec6[l2] = 0.0f;
			
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec7[l3] = 0.0f;
			
		}
		for (int l4 = 0; (l4 < 3); l4 = (l4 + 1)) {
			fRec5[l4] = 0.0f;
			
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec8[l5] = 0.0f;
			
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec9[l6] = 0.0f;
			
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec10[l7] = 0.0f;
			
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fVec0[l8] = 0.0f;
			
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fRec2[l9] = 0.0f;
			
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec1[l10] = 0.0f;
			
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec0[l11] = 0.0f;
			
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec11[l12] = 0.0f;
			
		}
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void instanceInit(int samplingFreq) {
		instanceConstants(samplingFreq);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual TemperDsp* clone() {
		return new TemperDsp();
	}
	virtual int getSampleRate() {
		return fSamplingFreq;
		
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("temper");
		ui_interface->addHorizontalSlider("Curve", &fHslider5, 1.0f, 0.100000001f, 4.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("Cutoff", &fHslider2, 20000.0f, 100.0f, 20000.0f, 1.0f);
		ui_interface->addHorizontalSlider("Drive", &fHslider4, 4.0f, -10.0f, 10.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("Feedback", &fHslider1, -60.0f, -60.0f, -24.0f, 1.0f);
		ui_interface->addHorizontalSlider("Level", &fHslider6, -3.0f, -24.0f, 24.0f, 1.0f);
		ui_interface->addHorizontalSlider("Resonance", &fHslider3, 1.0f, 1.0f, 8.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("Saturation", &fHslider0, 1.0f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = (0.00499999989f * float(fHslider0));
		float fSlow1 = (0.00499999989f * powf(10.0f, (0.0500000007f * float(fHslider1))));
		float fSlow2 = (0.00499999989f / tanf((fConst1 * float(fHslider2))));
		float fSlow3 = (0.00499999989f * float(fHslider3));
		float fSlow4 = (0.00499999989f * float(fHslider4));
		float fSlow5 = (0.00499999989f * float(fHslider5));
		float fSlow6 = (0.00499999989f * powf(10.0f, (0.0500000007f * float(fHslider6))));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec3[0] = (fSlow0 + (0.995000005f * fRec3[1]));
			fRec4[0] = (fSlow1 + (0.995000005f * fRec4[1]));
			fRec6[0] = (fSlow2 + (0.995000005f * fRec6[1]));
			fRec7[0] = (fSlow3 + (0.995000005f * fRec7[1]));
			float fTemp0 = (1.0f / fRec7[0]);
			float fTemp1 = ((fRec6[0] * (fRec6[0] + fTemp0)) + 1.0f);
			fRec5[0] = (float(input0[i]) - (((((fRec6[0] * (fRec6[0] - fTemp0)) + 1.0f) * fRec5[2]) + (2.0f * (fRec5[1] * (1.0f - TemperDsp_faustpower2_f(fRec6[0]))))) / fTemp1));
			float fTemp2 = ((fRec4[0] * fRec0[1]) + (((fRec5[0] + (2.0f * fRec5[1])) + fRec5[2]) / fTemp1));
			float fTemp3 = fabsf(fTemp2);
			fRec8[0] = max(fTemp3, ((fConst3 * fTemp3) + (fConst2 * fRec8[1])));
			fRec9[0] = (fSlow4 + (0.995000005f * fRec9[1]));
			float fTemp4 = min(3.0f, max(-3.0f, (fRec8[0] + (fRec9[0] * fTemp2))));
			fRec10[0] = (fSlow5 + (0.995000005f * fRec10[1]));
			float fTemp5 = TemperDsp_faustpower2_f(fRec10[0]);
			float fTemp6 = (TemperDsp_faustpower2_f(fTemp4) * fTemp5);
			float fTemp7 = ((fTemp4 * (fTemp6 + 27.0f)) * ((9.0f * fTemp5) + 27.0f));
			float fTemp8 = (((9.0f * fTemp6) + 27.0f) * (fTemp5 + 27.0f));
			float fTemp9 = (((1.0f - fRec3[0]) * fTemp2) + (0.239999995f * ((fTemp7 * fRec3[0]) / fTemp8)));
			fVec0[0] = fTemp9;
			fRec2[0] = (fVec0[1] + (((0.0f - (0.239999995f * (fTemp7 / fTemp8))) * fRec2[1]) + (0.239999995f * ((fTemp7 * fTemp9) / fTemp8))));
			fRec1[0] = ((fRec2[0] + (0.995000005f * fRec1[1])) - fRec2[1]);
			fRec0[0] = fRec1[0];
			fRec11[0] = (fSlow6 + (0.995000005f * fRec11[1]));
			output0[i] = FAUSTFLOAT((4.0f * (fRec0[0] * fRec11[0])));
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			fRec10[1] = fRec10[0];
			fVec0[1] = fVec0[0];
			fRec2[1] = fRec2[0];
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec11[1] = fRec11[0];
			
		}
		
	}

	
};


#endif
