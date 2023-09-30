/* ------------------------------------------------------------
name: "ToneStackF"
Code generated with Faust 2.28.3 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 2
------------------------------------------------------------ */

#ifndef __ToneStackFFaust_H__
#define __ToneStackFFaust_H__

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

static float ToneStackFFaust_faustpower2_f(float value)
{
  return (value * value);
}

#ifndef FAUSTCLASS
#define FAUSTCLASS ToneStackFFaust
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

class ToneStackFFaust : public FaustImpl
{
private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  float fConst3;
  float fConst4;
  FAUSTFLOAT fEntry0;
  FAUSTFLOAT fEntry1;
  float fRec1[3];
  float fConst5;
  float fConst6;
  float fConst7;
  float fConst8;
  FAUSTFLOAT fEntry2;
  float fConst9;
  float fConst10;
  float fRec2[3];
  float fConst11;
  FAUSTFLOAT fEntry3;
  float fConst12;
  float fRec0[3];

public:
  void metadata(Meta* m)
  {
    m->declare("basics.lib/name", "Faust Basic Element Library");
    m->declare("basics.lib/version", "0.1");
    m->declare("filename", "ToneStackF.dsp");
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
    m->declare("name", "ToneStackF");
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
    fConst2 = (2.0f * (1.0f - (1.0f / ToneStackFFaust_faustpower2_f(fConst1))));
    fConst3 = ToneStackFFaust_faustpower2_f(fConst0);
    fConst4 = (2.35000007e-05f * fConst3);
    fConst5 = (1.17500003e-05f * fConst0);
    fConst6 = (9.57499981f * fConst0);
    fConst7 = (fConst6 + 138000.0f);
    fConst8 = (2.5f * (fConst0 / fConst7));
    fConst9 = (1.0f / fConst7);
    fConst10 = (138000.0f - fConst6);
    fConst11 = (1.0f / fConst1);
    fConst12 = (6283.18555f / (fConst0 * std::sin((25132.7422f / fConst0))));
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

  virtual ToneStackFFaust* clone() { return new ToneStackFFaust(); }

  virtual int getSampleRate() { return fSampleRate; }

  virtual void buildUserInterface(UI* ui_interface)
  {
    ui_interface->openVerticalBox("ToneStackF");
    ui_interface->addNumEntry("bass", &fEntry0, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("mids", &fEntry1, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("presence", &fEntry3, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("treble", &fEntry2, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->closeBox();
  }

  virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs)
  {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float fSlow0 = ToneStackFFaust_faustpower2_f(
        ((0.495000005f * float(fEntry0)) + 0.504999995f));
    float fSlow1 = ((0.495000005f * float(fEntry1)) + 0.504999995f);
    float fSlow2 = (fSlow0 * fSlow1);
    float fSlow3 = ((0.00147000002f * fSlow1) + (0.0250000004f * fSlow0));
    float fSlow4 = (fSlow3 + 0.0202859994f);
    float fSlow5 = (fSlow0 * ((1.17500003e-05f * fSlow1) + 0.000162149998f));
    float fSlow6 = (fConst0 * fSlow5);
    float fSlow7 = ((fConst0 * (fSlow4 + fSlow6)) + 1.0f);
    float fSlow8 = (0.0f - (fConst4 * (fSlow2 / fSlow7)));
    float fSlow9 = (1.0f / fSlow7);
    float fSlow10 = (1.0f - (fConst0 * (fSlow4 - fSlow6)));
    float fSlow11 = (2.0f * (1.0f - (fConst3 * fSlow5)));
    float fSlow12 = (fConst0 / fSlow7);
    float fSlow13 = (fConst5 * fSlow2);
    float fSlow14 = (fSlow3 + fSlow13);
    float fSlow15 = (fSlow13 - fSlow3);
    float fSlow16 =
        (fConst8 * ((0.495000005f * float(fEntry2)) + 0.504999995f));
    float fSlow17 = (0.0f - fSlow16);
    float fSlow18 = (10.0f * float(fEntry3));
    int iSlow19 = (fSlow18 > 0.0f);
    float fSlow20 =
        (fConst12 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow18))));
    float fSlow21 = (iSlow19 ? fConst12 : fSlow20);
    float fSlow22 = ((fConst11 * (fConst11 - fSlow21)) + 1.0f);
    float fSlow23 = ((fConst11 * (fConst11 + fSlow21)) + 1.0f);
    float fSlow24 = (iSlow19 ? fSlow20 : fConst12);
    float fSlow25 = ((fConst11 * (fConst11 + fSlow24)) + 1.0f);
    float fSlow26 = ((fConst11 * (fConst11 - fSlow24)) + 1.0f);
    for (int i = 0; (i < count); i = (i + 1))
    {
      float fTemp0 = float(input0[i]);
      float fTempFTZ0 =
          (fTemp0 - (fSlow9 * ((fSlow10 * fRec1[2]) + (fSlow11 * fRec1[1]))));
      fRec1[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ0) & 2139095040) ? fTempFTZ0
                                                              : 0.0f);
      float fTempFTZ1 =
          (fTemp0
           - (fConst9 * ((fConst10 * fRec2[2]) + (276000.0f * fRec2[1]))));
      fRec2[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ1) & 2139095040) ? fTempFTZ1
                                                              : 0.0f);
      float fTemp1 = (fConst2 * fRec0[1]);
      float fTempFTZ2 =
          (((4.0f
             * ((fSlow8 * fRec1[1])
                + (fSlow12 * ((fSlow14 * fRec1[0]) + (fSlow15 * fRec1[2])))))
            + (12.0f * ((fSlow16 * fRec2[0]) + (fSlow17 * fRec2[2]))))
           - (((fRec0[2] * fSlow22) + fTemp1) / fSlow23));
      fRec0[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ2) & 2139095040) ? fTempFTZ2
                                                              : 0.0f);
      output0[i] = FAUSTFLOAT(
          (((fTemp1 + (fRec0[0] * fSlow25)) + (fRec0[2] * fSlow26)) / fSlow23));
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
