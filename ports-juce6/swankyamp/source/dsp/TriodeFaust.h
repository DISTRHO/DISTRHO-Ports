/* ------------------------------------------------------------
name: "Triode"
Code generated with Faust 2.28.3 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 2
------------------------------------------------------------ */

#ifndef __TriodeFaust_H__
#define __TriodeFaust_H__

#include "FaustImpl.h"
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>

#ifndef FAUSTCLASS
#define FAUSTCLASS TriodeFaust
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

class TriodeFaust : public FaustImpl
{
private:
  FAUSTFLOAT fEntry0;
  FAUSTFLOAT fEntry1;
  FAUSTFLOAT fEntry2;
  FAUSTFLOAT fEntry3;
  FAUSTFLOAT fEntry4;
  FAUSTFLOAT fEntry5;
  int fSampleRate;
  float fConst0;
  FAUSTFLOAT fEntry6;
  FAUSTFLOAT fEntry7;
  FAUSTFLOAT fEntry8;
  FAUSTFLOAT fEntry9;
  FAUSTFLOAT fEntry10;
  FAUSTFLOAT fEntry11;
  FAUSTFLOAT fEntry12;
  FAUSTFLOAT fEntry13;
  float fConst1;
  FAUSTFLOAT fEntry14;
  float fVec0[2];
  float fRec2[2];
  FAUSTFLOAT fEntry15;
  FAUSTFLOAT fEntry16;
  FAUSTFLOAT fEntry17;
  FAUSTFLOAT fEntry18;
  FAUSTFLOAT fEntry19;
  float fRec4[2];
  float fRec3[2];
  FAUSTFLOAT fEntry20;
  FAUSTFLOAT fEntry21;
  FAUSTFLOAT fEntry22;
  float fConst2;
  FAUSTFLOAT fEntry23;
  float fRec5[2];
  FAUSTFLOAT fEntry24;
  float fRec1[2];
  FAUSTFLOAT fEntry25;
  float fVec1[2];
  float fRec0[2];

public:
  void metadata(Meta* m)
  {
    m->declare("basics.lib/name", "Faust Basic Element Library");
    m->declare("basics.lib/version", "0.1");
    m->declare("filename", "Triode.dsp");
    m->declare("filters.lib/highpass:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/highpass:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
    m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
    m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/lowpass:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
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
    m->declare("name", "Triode");
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
    fEntry8 = FAUSTFLOAT(0.0f);
    fEntry9 = FAUSTFLOAT(0.0f);
    fEntry10 = FAUSTFLOAT(0.0f);
    fEntry11 = FAUSTFLOAT(0.0f);
    fEntry12 = FAUSTFLOAT(0.0f);
    fEntry13 = FAUSTFLOAT(0.0f);
    fEntry14 = FAUSTFLOAT(0.0f);
    fEntry15 = FAUSTFLOAT(0.0f);
    fEntry16 = FAUSTFLOAT(0.0f);
    fEntry17 = FAUSTFLOAT(0.0f);
    fEntry18 = FAUSTFLOAT(0.0f);
    fEntry19 = FAUSTFLOAT(0.0f);
    fEntry20 = FAUSTFLOAT(0.0f);
    fEntry21 = FAUSTFLOAT(0.0f);
    fEntry22 = FAUSTFLOAT(0.0f);
    fEntry23 = FAUSTFLOAT(0.0f);
    fEntry24 = FAUSTFLOAT(0.0f);
    fEntry25 = FAUSTFLOAT(0.0f);
  }

  virtual void instanceClear()
  {
    for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) { fVec0[l0] = 0.0f; }
    for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) { fRec2[l1] = 0.0f; }
    for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) { fRec4[l2] = 0.0f; }
    for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) { fRec3[l3] = 0.0f; }
    for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) { fRec5[l4] = 0.0f; }
    for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) { fRec1[l5] = 0.0f; }
    for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) { fVec1[l6] = 0.0f; }
    for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) { fRec0[l7] = 0.0f; }
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

  virtual TriodeFaust* clone() { return new TriodeFaust(); }

  virtual int getSampleRate() { return fSampleRate; }

  virtual void buildUserInterface(UI* ui_interface)
  {
    ui_interface->openVerticalBox("Triode");
    ui_interface->addNumEntry("bias", &fEntry9, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("bias_corner", &fEntry20, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("cap", &fEntry17, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_cap", &fEntry5, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_corner", &fEntry25, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_depth", &fEntry4, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_level", &fEntry7, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_offset", &fEntry3, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_ratio", &fEntry24, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_tau", &fEntry6, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("drift_depth", &fEntry21, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("drift_level", &fEntry23, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("drift_tau", &fEntry22, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("grid_clip", &fEntry12, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("grid_corner", &fEntry13, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("hp_freq", &fEntry14, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("level", &fEntry18, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("mix", &fEntry0, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("overhead", &fEntry1, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("plate_clip", &fEntry8, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry(
        "plate_corner", &fEntry10, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("ratio", &fEntry19, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("scale", &fEntry11, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("smooth", &fEntry16, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("tau", &fEntry15, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("unscale", &fEntry2, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->closeBox();
  }

  virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs)
  {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float fSlow0 = float(fEntry0);
    float fSlow1 = (1.0f - fSlow0);
    float fSlow2 = float(fEntry1);
    float fSlow3 = ((fSlow2 * fSlow0) / float(fEntry2));
    float fSlow4 = float(fEntry3);
    float fSlow5 = float(fEntry4);
    float fSlow6 = float(fEntry5);
    float fSlow7 = float(fEntry6);
    float fSlow8 = (1.0f / (fSlow6 * ((fConst0 * fSlow7) + 1.0f)));
    float fSlow9 = float(fEntry7);
    float fSlow10 = float(fEntry8);
    float fSlow11 = float(fEntry9);
    float fSlow12 = float(fEntry10);
    float fSlow13 = (fSlow12 + fSlow10);
    float fSlow14 = float(fEntry11);
    float fSlow15 = float(fEntry12);
    float fSlow16 = float(fEntry13);
    float fSlow17 = std::tan((fConst1 * float(fEntry14)));
    float fSlow18 = (1.0f / fSlow17);
    float fSlow19 = (fSlow18 + 1.0f);
    float fSlow20 = (0.0f - (1.0f / (fSlow17 * fSlow19)));
    float fSlow21 = (1.0f / fSlow2);
    float fSlow22 = (1.0f / fSlow19);
    float fSlow23 = (1.0f - fSlow18);
    float fSlow24 = (1.0f / (fSlow17 * fSlow2));
    float fSlow25 = float(fEntry15);
    float fSlow26 = (1.0f / ((fConst0 * (fSlow25 * float(fEntry16))) + 1.0f));
    float fSlow27 = (1.0f - fSlow26);
    float fSlow28 = float(fEntry17);
    float fSlow29 = (1.0f / (fSlow28 * ((fConst0 * fSlow25) + 1.0f)));
    float fSlow30 = float(fEntry18);
    float fSlow31 =
        ((1.0f / ((fConst0 * (fSlow25 * float(fEntry19))) + 1.0f)) + -1.0f);
    float fSlow32 = (0.294117659f / fSlow16);
    float fSlow33 = float(fEntry20);
    float fSlow34 = (0.294117659f / fSlow33);
    float fSlow35 = (0.294117659f / fSlow12);
    float fSlow36 = float(fEntry21);
    float fSlow37 = float(fEntry22);
    int iSlow38 = (std::fabs(fSlow37) < 1.1920929e-07f);
    float fSlow39 =
        (iSlow38 ? 0.0f
                 : std::exp((0.0f - (fConst2 / (iSlow38 ? 1.0f : fSlow37)))));
    float fSlow40 = float(fEntry23);
    float fSlow41 = (1.0f - fSlow39);
    float fSlow42 =
        (1.0f - (1.0f / ((fConst0 * (fSlow7 * float(fEntry24))) + 1.0f)));
    float fSlow43 = float(fEntry25);
    float fSlow44 = (fSlow43 + fSlow10);
    float fSlow45 = (0.294117659f / fSlow43);
    for (int i = 0; (i < count); i = (i + 1))
    {
      float fTemp0 = float(input0[i]);
      fVec0[0] = (fSlow21 * fTemp0);
      float fTempFTZ0 =
          ((fSlow20 * fVec0[1])
           - (fSlow22 * ((fSlow23 * fRec2[1]) - (fSlow24 * fTemp0))));
      fRec2[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ0) & 2139095040) ? fTempFTZ0
                                                              : 0.0f);
      float fTempFTZ1 =
          ((fSlow29
            * (std::max<float>(0.0f, (fSlow28 - fRec4[1]))
               * std::max<float>(
                   0.0f,
                   (std::max<float>(0.0f, (fRec2[0] - fSlow30)) - fRec4[1]))))
           - (fSlow31 * fRec4[1]));
      fRec4[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ1) & 2139095040) ? fTempFTZ1
                                                              : 0.0f);
      float fTempFTZ2 = ((fSlow27 * fRec3[1]) + (fSlow26 * fRec4[0]));
      fRec3[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ2) & 2139095040) ? fTempFTZ2
                                                              : 0.0f);
      float fTemp1 = (fSlow16 + ((fRec2[0] - fRec3[0]) - fSlow15));
      float fTemp2 = std::max<float>(
          -1.0f,
          std::min<float>(1.0f, (fSlow32 * std::max<float>(0.0f, fTemp1))));
      float fTemp3 = (fTemp2 * (std::fabs(fTemp2) + -2.0f));
      float fTemp4 =
          (fSlow11
           + ((fSlow14
               * (fSlow15
                  + (std::min<float>(0.0f, fTemp1)
                     - (fSlow16
                        * (1.0f - (fTemp3 * (std::fabs(fTemp3) + -2.0f)))))))
              - fSlow33));
      float fTemp5 = std::max<float>(
          -1.0f,
          std::min<float>(1.0f, (fSlow34 * std::min<float>(0.0f, fTemp4))));
      float fTemp6 = (fTemp5 * (std::fabs(fTemp5) + -2.0f));
      float fTemp7 =
          (fSlow11
           - (fSlow13
              + (std::max<float>(0.0f, fTemp4)
                 + (fSlow33
                    * ((fTemp6 * (std::fabs(fTemp6) + -2.0f)) + 1.0f)))));
      float fTemp8 = std::max<float>(
          -1.0f,
          std::min<float>(1.0f, (fSlow35 * std::min<float>(0.0f, fTemp7))));
      float fTemp9 = (fTemp8 * (std::fabs(fTemp8) + -2.0f));
      float fTemp10 =
          (std::max<float>(0.0f, fTemp7)
           + (fSlow12 * ((fTemp9 * (std::fabs(fTemp9) + -2.0f)) + 1.0f)));
      float fTempFTZ3 =
          ((fRec5[1] * fSlow39)
           + ((std::max<float>(fSlow40, (fSlow10 + fTemp10)) - fSlow40)
              * fSlow41));
      fRec5[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ3) & 2139095040) ? fTempFTZ3
                                                              : 0.0f);
      float fTemp11 = (fSlow36 * fRec5[0]);
      float fTempFTZ4 =
          ((fSlow8
            * (std::max<float>(0.0f, (fSlow6 - fRec1[1]))
               * std::max<float>(
                   0.0f,
                   ((std::max<float>(fSlow9, (fSlow10 + (fTemp10 - fTemp11)))
                     - fRec1[1])
                    - fSlow9))))
           + (fSlow42 * fRec1[1]));
      fRec1[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ4) & 2139095040) ? fTempFTZ4
                                                              : 0.0f);
      float fTemp12 = (fSlow5 * fRec1[0]);
      float fTemp13 = (fSlow44 + ((fTemp10 - (fTemp11 + fTemp12)) - fSlow4));
      float fTemp14 = std::max<float>(
          -1.0f,
          std::min<float>(1.0f, (fSlow45 * std::max<float>(0.0f, fTemp13))));
      float fTemp15 = (fTemp14 * (std::fabs(fTemp14) + -2.0f));
      float fTemp16 =
          (fSlow4
           + ((fTemp12 + std::min<float>(0.0f, fTemp13))
              - (fSlow43 * (1.0f - (fTemp15 * (std::fabs(fTemp15) + -2.0f))))));
      fVec1[0] = fTemp16;
      float fTempFTZ5 =
          ((0.863271236f * (fTemp16 + fVec1[1])) - (0.726542532f * fRec0[1]));
      fRec0[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ5) & 2139095040) ? fTempFTZ5
                                                              : 0.0f);
      output0[i] =
          FAUSTFLOAT(((fSlow1 * fTemp0) + (fSlow3 * (0.0f - fRec0[0]))));
      fVec0[1] = fVec0[0];
      fRec2[1] = fRec2[0];
      fRec4[1] = fRec4[0];
      fRec3[1] = fRec3[0];
      fRec5[1] = fRec5[0];
      fRec1[1] = fRec1[0];
      fVec1[1] = fVec1[0];
      fRec0[1] = fRec0[0];
    }
  }
};

#endif
