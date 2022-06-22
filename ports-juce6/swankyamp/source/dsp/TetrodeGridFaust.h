/* ------------------------------------------------------------
name: "TetrodeGrid"
Code generated with Faust 2.28.3 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 2
------------------------------------------------------------ */

#ifndef __TetrodeGridFaust_H__
#define __TetrodeGridFaust_H__

#include "FaustImpl.h"
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>

#ifndef FAUSTCLASS
#define FAUSTCLASS TetrodeGridFaust
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

class TetrodeGridFaust : public FaustImpl
{
private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  FAUSTFLOAT fEntry0;
  FAUSTFLOAT fEntry1;
  float fVec0[2];
  float fRec0[2];
  FAUSTFLOAT fEntry2;
  float fConst2;
  FAUSTFLOAT fEntry3;
  float fRec1[2];
  FAUSTFLOAT fEntry4;
  FAUSTFLOAT fEntry5;
  FAUSTFLOAT fEntry6;
  FAUSTFLOAT fEntry7;
  float fRec2[2];

public:
  void metadata(Meta* m)
  {
    m->declare("basics.lib/name", "Faust Basic Element Library");
    m->declare("basics.lib/version", "0.1");
    m->declare("filename", "TetrodeGrid.dsp");
    m->declare("filters.lib/highpass:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/highpass:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
    m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
    m->declare("filters.lib/name", "Faust Filters Library");
    m->declare("filters.lib/tf1:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/tf1:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/tf1:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/tf1s:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/tf1s:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/tf1s:license", "MIT-style STK-4.3 license");
    m->declare("maths.lib/author", "GRAME");
    m->declare("maths.lib/copyright", "GRAME");
    m->declare("maths.lib/license", "LGPL with exception");
    m->declare("maths.lib/name", "Faust Math Library");
    m->declare("maths.lib/version", "2.3");
    m->declare("name", "TetrodeGrid");
    m->declare("platform.lib/name", "Generic Platform Library");
    m->declare("platform.lib/version", "0.1");
    m->declare("signals.lib/name", "Faust Signal Routing Library");
    m->declare("signals.lib/version", "0.0");
  }

  virtual int getNumInputs() { return 1; }
  virtual int getNumOutputs() { return 1; }
  virtual int getInputRate(int channel)
  {
    int rate;
    switch ((channel))
    {
    case 0:
    {
      rate = 1;
      break;
    }
    default:
    {
      rate = -1;
      break;
    }
    }
    return rate;
  }
  virtual int getOutputRate(int channel)
  {
    int rate;
    switch ((channel))
    {
    case 0:
    {
      rate = 1;
      break;
    }
    default:
    {
      rate = -1;
      break;
    }
    }
    return rate;
  }

  static void classInit(int) {}

  virtual void instanceConstants(int sample_rate)
  {
    fSampleRate = sample_rate;
    fConst0 =
        std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
    fConst1 = (3.14159274f / fConst0);
    fConst2 = (1.0f / fConst0);
  }

  virtual void instanceResetUserInterface()
  {
    fEntry0 = FAUSTFLOAT(0.0f);
    fEntry1 = FAUSTFLOAT(0.0f);
    fEntry2 = FAUSTFLOAT(0.0f);
    fEntry3 = FAUSTFLOAT(0.0f);
    fEntry4 = FAUSTFLOAT(0.0f);
    fEntry5 = FAUSTFLOAT(0.0f);
    fEntry6 = FAUSTFLOAT(0.0f);
    fEntry7 = FAUSTFLOAT(0.0f);
  }

  virtual void instanceClear()
  {
    for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) { fVec0[l0] = 0.0f; }
    for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) { fRec0[l1] = 0.0f; }
    for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) { fRec1[l2] = 0.0f; }
    for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) { fRec2[l3] = 0.0f; }
  }

  virtual void init(int sample_rate)
  {
    classInit(sample_rate);
    instanceInit(sample_rate);
  }
  virtual void instanceInit(int sample_rate)
  {
    instanceConstants(sample_rate);
    instanceResetUserInterface();
    instanceClear();
  }

  virtual TetrodeGridFaust* clone() { return new TetrodeGridFaust(); }

  virtual int getSampleRate() { return fSampleRate; }

  virtual void buildUserInterface(UI* ui_interface)
  {
    ui_interface->openVerticalBox("TetrodeGrid");
    ui_interface->addNumEntry("cap", &fEntry4, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("hp_freq", &fEntry0, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("level", &fEntry6, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("offset1", &fEntry1, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("offset2", &fEntry3, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("ratio", &fEntry7, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("tau", &fEntry5, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("taus", &fEntry2, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->closeBox();
  }

  virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs)
  {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float fSlow0 = std::tan((fConst1 * float(fEntry0)));
    float fSlow1 = (1.0f / fSlow0);
    float fSlow2 = (fSlow1 + 1.0f);
    float fSlow3 = (0.0f - (1.0f / (fSlow0 * fSlow2)));
    float fSlow4 = float(fEntry1);
    float fSlow5 = (1.0f / fSlow2);
    float fSlow6 = (1.0f - fSlow1);
    float fSlow7 = float(fEntry2);
    int iSlow8 = (std::fabs(fSlow7) < 1.1920929e-07f);
    float fSlow9 =
        (iSlow8 ? 0.0f
                : std::exp((0.0f - (fConst2 / (iSlow8 ? 1.0f : fSlow7)))));
    float fSlow10 = float(fEntry3);
    float fSlow11 = (1.0f - fSlow9);
    float fSlow12 = float(fEntry4);
    float fSlow13 = float(fEntry5);
    float fSlow14 = (1.0f / (fSlow12 * ((fConst0 * fSlow13) + 1.0f)));
    float fSlow15 = (fSlow10 + float(fEntry6));
    float fSlow16 =
        (1.0f - (1.0f / ((fConst0 * (fSlow13 * float(fEntry7))) + 1.0f)));
    for (int i = 0; (i < count); i = (i + 1))
    {
      float fTemp0 = float(input0[i]);
      float fTemp1 = (fTemp0 - fSlow4);
      fVec0[0] = fTemp1;
      float fTempFTZ0 =
          ((fSlow3 * fVec0[1])
           - (fSlow5 * ((fSlow6 * fRec0[1]) - (fSlow1 * fTemp1))));
      fRec0[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ0) & 2139095040) ? fTempFTZ0
                                                              : 0.0f);
      float fTempFTZ1 =
          ((fRec1[1] * fSlow9) + ((fRec0[0] - fSlow10) * fSlow11));
      fRec1[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ1) & 2139095040) ? fTempFTZ1
                                                              : 0.0f);
      float fTempFTZ2 =
          ((fSlow14
            * (std::max<float>(0.0f, (fSlow12 - fRec2[1]))
               * std::max<float>(
                   0.0f,
                   (std::max<float>(0.0f, ((fRec0[0] - fRec1[0]) - fSlow15))
                    - fRec2[1]))))
           + (fSlow16 * fRec2[1]));
      fRec2[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ2) & 2139095040) ? fTempFTZ2
                                                              : 0.0f);
      output0[i] = FAUSTFLOAT(((fRec0[0] - (fRec1[0] + fRec2[0])) - fSlow10));
      fVec0[1] = fVec0[0];
      fRec0[1] = fRec0[0];
      fRec1[1] = fRec1[0];
      fRec2[1] = fRec2[0];
    }
  }
};

#endif
