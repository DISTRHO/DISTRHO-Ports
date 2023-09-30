/* ------------------------------------------------------------
name: "ToneStackM"
Code generated with Faust 2.28.3 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 2
------------------------------------------------------------ */

#ifndef __ToneStackMFaust_H__
#define __ToneStackMFaust_H__

#ifndef __faust2hpp_FaustImpl_H__
#define __faust2hpp_FaustImpl_H__

#include <stdexcept>
#include <unordered_map>

#include "Meta.h"
#include "UI.h"

class FaustImpl : public UI, public Meta
{
public:
  FaustImpl() = default;
  ~FaustImpl() = default;

  FAUSTFLOAT* getParameter(const char* name)
  {
    const auto entry = parameterMap.find(name);
    if (entry == parameterMap.end())
      throw std::invalid_argument(
          std::string("FaustImpl::getParameter: invalid parameter name: ")
          + name);
    return entry->second;
  }

  void setParameter(const char* name, FAUSTFLOAT* value)
  {
    const auto entry = parameterMap.find(name);
    if (entry != parameterMap.end()) entry->second = value;
  }

  // blank implementations for UI
  virtual void openTabBox(const char*){};
  virtual void openHorizontalBox(const char*){};
  virtual void openVerticalBox(const char*){};
  virtual void closeBox(){};
  virtual void addButton(const char*, FAUSTFLOAT*){};
  virtual void addCheckButton(const char*, FAUSTFLOAT*){};
  virtual void addVerticalSlider(
      const char*,
      FAUSTFLOAT*,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT){};
  virtual void addHorizontalSlider(
      const char*,
      FAUSTFLOAT*,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT){};
  virtual void
  // use UI entry to expose user parameters
  addNumEntry(
      const char* label,
      FAUSTFLOAT* zone,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT,
      FAUSTFLOAT)
  {
    if (zone == nullptr) return;
    parameterMap.insert_or_assign(label, zone);
  }
  virtual void
  addHorizontalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT){};
  virtual void
  addVerticalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT){};
  virtual void addSoundfile(const char*, const char*, Soundfile**){};

  // blank implememtation for Meta
  virtual void declare(const char*, const char*){};

private:
  std::unordered_map<const char*, FAUSTFLOAT*> parameterMap;
};

#endif
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>

static float ToneStackMFaust_faustpower2_f(float value)
{
  return (value * value);
}

#ifndef FAUSTCLASS
#define FAUSTCLASS ToneStackMFaust
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

class ToneStackMFaust : public FaustImpl
{
private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  float fConst3;
  FAUSTFLOAT fEntry0;
  FAUSTFLOAT fEntry1;
  float fConst4;
  float fRec1[3];
  float fConst5;
  float fConst6;
  float fConst7;
  FAUSTFLOAT fEntry2;
  float fConst8;
  float fConst9;
  float fRec2[3];
  float fConst10;
  FAUSTFLOAT fEntry3;
  float fConst11;
  float fRec0[3];

public:
  void metadata(Meta* m)
  {
    m->declare("basics.lib/name", "Faust Basic Element Library");
    m->declare("basics.lib/version", "0.1");
    m->declare("filename", "ToneStackM.dsp");
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
    m->declare("filters.lib/peak_eq:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/peak_eq:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/peak_eq:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/tf2:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/tf2:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/tf2s:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
    m->declare("maths.lib/author", "GRAME");
    m->declare("maths.lib/copyright", "GRAME");
    m->declare("maths.lib/license", "LGPL with exception");
    m->declare("maths.lib/name", "Faust Math Library");
    m->declare("maths.lib/version", "2.3");
    m->declare("name", "ToneStackM");
    m->declare("platform.lib/name", "Generic Platform Library");
    m->declare("platform.lib/version", "0.1");
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
    fConst1 = std::tan((12566.3711f / fConst0));
    fConst2 = (2.0f * (1.0f - (1.0f / ToneStackMFaust_faustpower2_f(fConst1))));
    fConst3 = (9.68000052e-16f * ToneStackMFaust_faustpower2_f(fConst0));
    fConst4 = (4.84000026e-16f * fConst0);
    fConst5 = (3.41219997f * fConst0);
    fConst6 = (fConst5 + 33000.0f);
    fConst7 = (0.511829972f * (fConst0 / fConst6));
    fConst8 = (1.0f / fConst6);
    fConst9 = (33000.0f - fConst5);
    fConst10 = (1.0f / fConst1);
    fConst11 = (6283.18555f / (fConst0 * std::sin((25132.7422f / fConst0))));
  }

  virtual void instanceResetUserInterface()
  {
    fEntry0 = FAUSTFLOAT(0.0f);
    fEntry1 = FAUSTFLOAT(0.0f);
    fEntry2 = FAUSTFLOAT(0.0f);
    fEntry3 = FAUSTFLOAT(0.0f);
  }

  virtual void instanceClear()
  {
    for (int l0 = 0; (l0 < 3); l0 = (l0 + 1)) { fRec1[l0] = 0.0f; }
    for (int l1 = 0; (l1 < 3); l1 = (l1 + 1)) { fRec2[l1] = 0.0f; }
    for (int l2 = 0; (l2 < 3); l2 = (l2 + 1)) { fRec0[l2] = 0.0f; }
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

  virtual ToneStackMFaust* clone() { return new ToneStackMFaust(); }

  virtual int getSampleRate() { return fSampleRate; }

  virtual void buildUserInterface(UI* ui_interface)
  {
    ui_interface->openVerticalBox("ToneStackM");
    ui_interface->addNumEntry("bass", &fEntry1, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("mids", &fEntry0, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("presence", &fEntry3, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("treble", &fEntry2, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->closeBox();
  }

  virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs)
  {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float fSlow0 = float(fEntry0);
    float fSlow1 = (0.495000005f * fSlow0);
    float fSlow2 = (fSlow1 + 0.504999995f);
    float fSlow3 =
        (484000000.0f
         * (ToneStackMFaust_faustpower2_f(fSlow2) + (-0.504999995f - fSlow1)));
    float fSlow4 = ((0.495000005f * float(fEntry1)) + 0.504999995f);
    float fSlow5 =
        ((0.200000003f * (fSlow4 * float((fSlow4 <= 0.5f))))
         + (float((fSlow4 > 0.5f)) * ((1.60000002f * fSlow4) + -0.699999988f)));
    float fSlow6 = (fSlow2 * fSlow5);
    float fSlow7 = (fSlow3 - (2.2000001e+10f * fSlow6));
    float fSlow8 = (22000.0f * (0.0f - (2.2e-08f * (fSlow2 + 1.0f))));
    float fSlow9 = (0.0219999999f * fSlow5);
    float fSlow10 = (fSlow8 + (-0.001452f - fSlow9));
    float fSlow11 =
        ((fSlow3
          + (22000.0f
             * ((1000000.0f * (0.0f - fSlow6))
                + (16335.0f * (fSlow0 + -1.0f)))))
         + (1000000.0f * (0.0f - (33000.0f * fSlow5))));
    float fSlow12 = (fConst4 * fSlow11);
    float fSlow13 = ((fConst0 * (fSlow10 + fSlow12)) + -1.0f);
    float fSlow14 = (0.0f - (fConst3 * (fSlow7 / fSlow13)));
    float fSlow15 = (1.0f / fSlow13);
    float fSlow16 = (-2.0f - (fConst3 * fSlow11));
    float fSlow17 = ((fConst0 * (fSlow12 - fSlow10)) + -1.0f);
    float fSlow18 = (fConst0 / fSlow13);
    float fSlow19 = (fSlow8 - fSlow9);
    float fSlow20 = (fConst4 * fSlow7);
    float fSlow21 = (fSlow19 + fSlow20);
    float fSlow22 = (fSlow20 - fSlow19);
    float fSlow23 =
        (fConst7 * ((0.495000005f * float(fEntry2)) + 0.504999995f));
    float fSlow24 = (0.0f - fSlow23);
    float fSlow25 = (10.0f * float(fEntry3));
    int iSlow26 = (fSlow25 > 0.0f);
    float fSlow27 =
        (fConst11 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow25))));
    float fSlow28 = (iSlow26 ? fConst11 : fSlow27);
    float fSlow29 = ((fConst10 * (fConst10 - fSlow28)) + 1.0f);
    float fSlow30 = ((fConst10 * (fConst10 + fSlow28)) + 1.0f);
    float fSlow31 = (iSlow26 ? fSlow27 : fConst11);
    float fSlow32 = ((fConst10 * (fConst10 + fSlow31)) + 1.0f);
    float fSlow33 = ((fConst10 * (fConst10 - fSlow31)) + 1.0f);
    for (int i = 0; (i < count); i = (i + 1))
    {
      float fTemp0 = float(input0[i]);
      float fTempFTZ0 =
          (fTemp0 - (fSlow15 * ((fSlow16 * fRec1[1]) + (fSlow17 * fRec1[2]))));
      fRec1[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ0) & 2139095040) ? fTempFTZ0
                                                              : 0.0f);
      float fTempFTZ1 =
          (fTemp0 - (fConst8 * ((fConst9 * fRec2[2]) + (66000.0f * fRec2[1]))));
      fRec2[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ1) & 2139095040) ? fTempFTZ1
                                                              : 0.0f);
      float fTemp1 = (fConst2 * fRec0[1]);
      float fTempFTZ2 =
          (((1.39999998f
             * ((fSlow14 * fRec1[1])
                + (fSlow18 * ((fSlow21 * fRec1[0]) + (fSlow22 * fRec1[2])))))
            + (6.0f * ((fSlow23 * fRec2[0]) + (fSlow24 * fRec2[2]))))
           - (((fRec0[2] * fSlow29) + fTemp1) / fSlow30));
      fRec0[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ2) & 2139095040) ? fTempFTZ2
                                                              : 0.0f);
      output0[i] = FAUSTFLOAT(
          (((fTemp1 + (fRec0[0] * fSlow32)) + (fRec0[2] * fSlow33)) / fSlow30));
      fRec1[2] = fRec1[1];
      fRec1[1] = fRec1[0];
      fRec2[2] = fRec2[1];
      fRec2[1] = fRec2[0];
      fRec0[2] = fRec0[1];
      fRec0[1] = fRec0[0];
    }
  }
};

#endif
