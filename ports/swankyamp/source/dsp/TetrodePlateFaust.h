/* ------------------------------------------------------------
name: "TetrodePlate"
Code generated with Faust 2.28.3 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 2
------------------------------------------------------------ */

#ifndef __TetrodePlateFaust_H__
#define __TetrodePlateFaust_H__

#include "FaustImpl.h"
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>

static float TetrodePlateFaust_faustpower2_f(float value)
{
  return (value * value);
}

#ifndef FAUSTCLASS
#define FAUSTCLASS TetrodePlateFaust
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

class TetrodePlateFaust : public FaustImpl
{
private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  float fConst3;
  float fConst4;
  FAUSTFLOAT fEntry0;
  float fConst5;
  float fConst6;
  FAUSTFLOAT fEntry1;
  float fConst7;
  float fConst8;
  FAUSTFLOAT fEntry2;
  FAUSTFLOAT fEntry3;
  FAUSTFLOAT fEntry4;
  FAUSTFLOAT fEntry5;
  FAUSTFLOAT fEntry6;
  FAUSTFLOAT fEntry7;
  FAUSTFLOAT fEntry8;
  FAUSTFLOAT fEntry9;
  FAUSTFLOAT fEntry10;
  float fRec2[2];
  float fRec1[2];
  FAUSTFLOAT fEntry11;
  FAUSTFLOAT fEntry12;
  float fRec4[2];
  float fRec3[2];
  FAUSTFLOAT fEntry13;
  FAUSTFLOAT fEntry14;
  FAUSTFLOAT fEntry15;
  FAUSTFLOAT fEntry16;
  float fRec5[2];
  float fConst9;
  float fRec0[3];
  FAUSTFLOAT fEntry17;
  FAUSTFLOAT fEntry18;
  float fRec6[2];

public:
  void metadata(Meta* m)
  {
    m->declare("basics.lib/name", "Faust Basic Element Library");
    m->declare("basics.lib/version", "0.1");
    m->declare("filename", "TetrodePlate.dsp");
    m->declare("filters.lib/bandpass0_bandstop1:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/bandpass0_bandstop1:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare(
        "filters.lib/bandpass0_bandstop1:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/bandpass:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/bandpass:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/bandpass:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/fir:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/fir:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/iir:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/iir:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
    m->declare("filters.lib/name", "Faust Filters Library");
    m->declare("filters.lib/tf1sb:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/tf1sb:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/tf1sb:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/tf2:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/tf2:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
    m->declare("maths.lib/author", "GRAME");
    m->declare("maths.lib/copyright", "GRAME");
    m->declare("maths.lib/license", "LGPL with exception");
    m->declare("maths.lib/name", "Faust Math Library");
    m->declare("maths.lib/version", "2.3");
    m->declare("name", "TetrodePlate");
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
    fConst1 = (2.0f / fConst0);
    fConst2 = (2.0f * fConst0);
    fConst3 = (3.14159274f / fConst0);
    fConst4 = (0.449999988f * fConst0);
    fConst5 = (0.5f / fConst0);
    fConst6 = (4.0f * TetrodePlateFaust_faustpower2_f(fConst0));
    fConst7 = (1.0f / fConst0);
    fConst8 = TetrodePlateFaust_faustpower2_f(fConst7);
    fConst9 = (2.0f * fConst8);
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
  }

  virtual void instanceClear()
  {
    for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) { fRec2[l0] = 0.0f; }
    for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) { fRec1[l1] = 0.0f; }
    for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) { fRec4[l2] = 0.0f; }
    for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) { fRec3[l3] = 0.0f; }
    for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) { fRec5[l4] = 0.0f; }
    for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) { fRec0[l5] = 0.0f; }
    for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) { fRec6[l6] = 0.0f; }
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

  virtual TetrodePlateFaust* clone() { return new TetrodePlateFaust(); }

  virtual int getSampleRate() { return fSampleRate; }

  virtual void buildUserInterface(UI* ui_interface)
  {
    ui_interface->openVerticalBox("TetrodePlate");
    ui_interface->addNumEntry("clip", &fEntry4, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("clip_corner", &fEntry11, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_depth", &fEntry5, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("comp_tau", &fEntry6, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry(
        "cross_corner", &fEntry12, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry(
        "drift2_depth", &fEntry17, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry(
        "drift2_level", &fEntry18, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("drift_depth", &fEntry8, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("drift_level", &fEntry10, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("drift_tau", &fEntry9, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("hp_freq", &fEntry1, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("lp_freq", &fEntry0, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("sag_depth", &fEntry3, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("sag_factor", &fEntry14, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("sag_onset", &fEntry15, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("sag_ratio", &fEntry16, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("sag_tau", &fEntry13, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("sag_toggle", &fEntry2, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("scale", &fEntry7, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->closeBox();
  }

  virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs)
  {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float fSlow0 =
        std::tan((fConst3 * std::min<float>(fConst4, float(fEntry0))));
    float fSlow1 = TetrodePlateFaust_faustpower2_f(std::sqrt((
        fConst6
        * (fSlow0
           * std::tan((fConst3 * std::min<float>(fConst4, float(fEntry1))))))));
    float fSlow2 = ((fConst2 * fSlow0) - (fConst5 * (fSlow1 / fSlow0)));
    float fSlow3 = (fConst8 * fSlow1);
    float fSlow4 = (fConst1 * fSlow2);
    float fSlow5 = ((fSlow3 + fSlow4) + 4.0f);
    float fSlow6 = (fConst1 * (fSlow2 / fSlow5));
    float fSlow7 = (float(fEntry2) * float(fEntry3));
    float fSlow8 = (fSlow7 + 1.0f);
    float fSlow9 = float(fEntry4);
    float fSlow10 = float(fEntry5);
    float fSlow11 = float(fEntry6);
    int iSlow12 = (std::fabs(fSlow11) < 1.1920929e-07f);
    float fSlow13 =
        (iSlow12 ? 0.0f
                 : std::exp((0.0f - (fConst7 / (iSlow12 ? 1.0f : fSlow11)))));
    float fSlow14 = (1.0f - fSlow13);
    float fSlow15 = (1.0f / fSlow9);
    float fSlow16 = float(fEntry7);
    float fSlow17 = float(fEntry8);
    float fSlow18 = float(fEntry9);
    int iSlow19 = (std::fabs(fSlow18) < 1.1920929e-07f);
    float fSlow20 =
        (iSlow19 ? 0.0f
                 : std::exp((0.0f - (fConst7 / (iSlow19 ? 1.0f : fSlow18)))));
    float fSlow21 = (1.0f - fSlow20);
    float fSlow22 = float(fEntry10);
    float fSlow23 = float(fEntry11);
    float fSlow24 = (0.294117659f / fSlow23);
    float fSlow25 = float(fEntry12);
    float fSlow26 = (0.294117659f / fSlow25);
    float fSlow27 = float(fEntry13);
    float fSlow28 = (1.0f / ((fConst0 * fSlow27) + 1.0f));
    float fSlow29 = (1.0f / (std::max<float>(0.0f, float(fEntry14)) + 1.0f));
    float fSlow30 = float(fEntry15);
    float fSlow31 = (fSlow16 / fSlow9);
    float fSlow32 =
        (1.0f - (1.0f / ((fConst0 * (fSlow27 * float(fEntry16))) + 1.0f)));
    float fSlow33 = (1.0f / fSlow5);
    float fSlow34 = ((fConst9 * fSlow1) + -8.0f);
    float fSlow35 = (fSlow3 + (4.0f - fSlow4));
    float fSlow36 = (0.0f - fSlow6);
    float fSlow37 = float(fEntry17);
    float fSlow38 = float(fEntry18);
    for (int i = 0; (i < count); i = (i + 1))
    {
      float fTemp0 = float(input0[i]);
      float fTemp1 = (fSlow16 * fTemp0);
      float fTempFTZ0 =
          ((fSlow20 * fRec2[1])
           + (fSlow21 * (std::max<float>(fSlow22, fTemp1) - fSlow22)));
      fRec2[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ0) & 2139095040) ? fTempFTZ0
                                                              : 0.0f);
      float fTemp2 = (fSlow17 * fRec2[0]);
      float fTempFTZ1 =
          ((fSlow13 * fRec1[1])
           + (fSlow14
              * std::min<float>(
                  1.0f, std::fabs((fSlow15 * (fTemp1 - fTemp2))))));
      fRec1[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ1) & 2139095040) ? fTempFTZ1
                                                              : 0.0f);
      float fTemp3 = (fSlow9 / ((fSlow10 * fRec1[0]) + 1.0f));
      float fTemp4 = (fSlow23 + (fTemp1 - (fTemp2 + fTemp3)));
      float fTemp5 = std::max<float>(
          -1.0f,
          std::min<float>(1.0f, (fSlow24 * std::max<float>(0.0f, fTemp4))));
      float fTemp6 = (fTemp5 * (std::fabs(fTemp5) + -2.0f));
      float fTemp7 =
          (((fTemp3 + std::min<float>(0.0f, fTemp4))
            - (fSlow23 * (1.0f - (fTemp6 * (std::fabs(fTemp6) + -2.0f)))))
           - fSlow25);
      float fTempFTZ2 =
          ((fRec4[1] * fSlow20)
           + ((std::max<float>(fSlow22, (0.0f - fTemp1)) - fSlow22) * fSlow21));
      fRec4[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ2) & 2139095040) ? fTempFTZ2
                                                              : 0.0f);
      float fTemp8 = (fTemp1 + (fSlow17 * fRec4[0]));
      float fTempFTZ3 =
          ((fRec3[1] * fSlow13)
           + (std::min<float>(1.0f, std::fabs((fSlow15 * (0.0f - fTemp8))))
              * fSlow14));
      fRec3[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ3) & 2139095040) ? fTempFTZ3
                                                              : 0.0f);
      float fTemp9 = (fSlow9 / ((fSlow10 * fRec3[0]) + 1.0f));
      float fTemp10 = (fSlow23 - (fTemp8 + fTemp9));
      float fTemp11 = std::max<float>(
          -1.0f,
          std::min<float>(1.0f, (fSlow24 * std::max<float>(0.0f, fTemp10))));
      float fTemp12 = (fTemp11 * (std::fabs(fTemp11) + -2.0f));
      float fTemp13 =
          (((fTemp9 + std::min<float>(0.0f, fTemp10))
            - (fSlow23 * (1.0f - (fTemp12 * (std::fabs(fTemp12) + -2.0f)))))
           - fSlow25);
      float fTemp14 = std::max<float>(
          -1.0f,
          std::min<float>(1.0f, (fSlow26 * std::min<float>(0.0f, fTemp13))));
      float fTemp15 = (fTemp14 * (std::fabs(fTemp14) + -2.0f));
      float fTemp16 = std::max<float>(
          -1.0f,
          std::min<float>(1.0f, (fSlow26 * std::min<float>(0.0f, fTemp7))));
      float fTemp17 = (fTemp16 * (std::fabs(fTemp16) + -2.0f));
      float fTemp18 = std::fabs((fSlow31 * fTemp0));
      float fTempFTZ4 =
          ((fSlow28
            * std::max<float>(
                0.0f,
                ((fSlow29
                  * ((fSlow30 * std::min<float>(1.0f, fTemp18))
                     + std::max<float>(1.0f, fTemp18)))
                 - fRec5[1])))
           + (fSlow32 * fRec5[1]));
      fRec5[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ4) & 2139095040) ? fTempFTZ4
                                                              : 0.0f);
      float fTempFTZ5 =
          ((fSlow8
            * ((std::max<float>(0.0f, fTemp7)
                - (std::max<float>(0.0f, fTemp13)
                   + (fSlow25
                      * (((fTemp15 * (std::fabs(fTemp15) + -2.0f)) + 1.0f)
                         - ((fTemp17 * (std::fabs(fTemp17) + -2.0f)) + 1.0f)))))
               / ((fSlow7 * fRec5[0]) + 1.0f)))
           - (fSlow33 * ((fSlow34 * fRec0[1]) + (fSlow35 * fRec0[2]))));
      fRec0[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ5) & 2139095040) ? fTempFTZ5
                                                              : 0.0f);
      float fTemp19 = ((fSlow6 * fRec0[0]) + (fSlow36 * fRec0[2]));
      float fTempFTZ6 =
          ((fSlow20 * fRec6[1])
           + (fSlow21
              * (std::max<float>(fSlow38, std::fabs(fTemp19)) - fSlow38)));
      fRec6[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ6) & 2139095040) ? fTempFTZ6
                                                              : 0.0f);
      output0[i] = FAUSTFLOAT((fTemp19 + (fSlow37 * fRec6[0])));
      fRec2[1] = fRec2[0];
      fRec1[1] = fRec1[0];
      fRec4[1] = fRec4[0];
      fRec3[1] = fRec3[0];
      fRec5[1] = fRec5[0];
      fRec0[2] = fRec0[1];
      fRec0[1] = fRec0[0];
      fRec6[1] = fRec6[0];
    }
  }
};

#endif
