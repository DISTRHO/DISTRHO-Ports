/* ------------------------------------------------------------
name: "Cabinet"
Code generated with Faust 2.28.3 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 2
------------------------------------------------------------ */

#ifndef __CabinetFaust_H__
#define __CabinetFaust_H__

#include "FaustImpl.h"
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>

static float CabinetFaust_faustpower2_f(float value) { return (value * value); }

#ifndef FAUSTCLASS
#define FAUSTCLASS CabinetFaust
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

class CabinetFaust : public FaustImpl
{
private:
  FAUSTFLOAT fEntry0;
  FAUSTFLOAT fEntry1;
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  float fConst3;
  float fConst4;
  float fConst5;
  float fConst6;
  float fConst7;
  float fConst8;
  float fConst9;
  float fConst10;
  float fConst11;
  float fConst12;
  float fConst13;
  float fConst14;
  float fConst15;
  float fConst16;
  float fConst17;
  float fConst18;
  float fConst19;
  float fConst20;
  float fConst21;
  float fConst22;
  float fConst23;
  float fConst24;
  float fConst25;
  float fConst26;
  float fConst27;
  FAUSTFLOAT fEntry2;
  FAUSTFLOAT fEntry3;
  FAUSTFLOAT fEntry4;
  FAUSTFLOAT fEntry5;
  FAUSTFLOAT fEntry6;
  FAUSTFLOAT fEntry7;
  FAUSTFLOAT fEntry8;
  FAUSTFLOAT fEntry9;
  FAUSTFLOAT fEntry10;
  FAUSTFLOAT fEntry11;
  FAUSTFLOAT fEntry12;
  FAUSTFLOAT fEntry13;
  FAUSTFLOAT fEntry14;
  float fRec26[3];
  float fRec25[3];
  float fVec0[2];
  float fRec24[2];
  float fRec23[3];
  float fVec1[2];
  float fRec22[2];
  float fRec21[3];
  float fRec20[3];
  float fRec19[3];
  FAUSTFLOAT fEntry15;
  float fRec30[2];
  float fRec29[3];
  float fRec28[3];
  float fRec27[3];
  FAUSTFLOAT fEntry16;
  FAUSTFLOAT fEntry17;
  float fConst28;
  float fRec18[3];
  FAUSTFLOAT fEntry18;
  FAUSTFLOAT fEntry19;
  float fRec17[3];
  FAUSTFLOAT fEntry20;
  FAUSTFLOAT fEntry21;
  float fRec16[3];
  FAUSTFLOAT fEntry22;
  FAUSTFLOAT fEntry23;
  float fRec15[3];
  FAUSTFLOAT fEntry24;
  FAUSTFLOAT fEntry25;
  float fRec14[3];
  FAUSTFLOAT fEntry26;
  FAUSTFLOAT fEntry27;
  float fRec13[3];
  FAUSTFLOAT fEntry28;
  FAUSTFLOAT fEntry29;
  float fRec12[3];
  FAUSTFLOAT fEntry30;
  FAUSTFLOAT fEntry31;
  FAUSTFLOAT fEntry32;
  float fConst29;
  float fConst30;
  float fRec31[2];
  FAUSTFLOAT fEntry33;
  FAUSTFLOAT fEntry34;
  float fRec11[3];
  FAUSTFLOAT fEntry35;
  FAUSTFLOAT fEntry36;
  float fRec10[3];
  FAUSTFLOAT fEntry37;
  FAUSTFLOAT fEntry38;
  float fRec9[3];
  FAUSTFLOAT fEntry39;
  FAUSTFLOAT fEntry40;
  float fRec8[3];
  float fConst31;
  float fRec7[3];
  float fConst32;
  float fConst33;
  float fConst34;
  float fVec2[2];
  float fConst35;
  float fConst36;
  float fConst37;
  float fRec6[2];
  float fConst38;
  float fConst39;
  float fRec5[3];
  float fConst40;
  float fConst41;
  float fRec33[2];
  float fRec32[3];
  FAUSTFLOAT fEntry41;
  float fConst42;
  float fConst43;
  float fRec4[3];
  float fVec3[2];
  float fConst44;
  float fConst45;
  float fRec3[2];
  float fConst46;
  float fConst47;
  float fRec2[3];
  float fConst48;
  float fRec35[2];
  float fRec34[3];
  float fConst49;
  float fConst50;
  float fRec1[3];
  float fConst51;
  float fConst52;
  float fRec0[3];

public:
  void metadata(Meta* m)
  {
    m->declare("analyzers.lib/name", "Faust Analyzer Library");
    m->declare("analyzers.lib/version", "0.1");
    m->declare("basics.lib/name", "Faust Basic Element Library");
    m->declare("basics.lib/version", "0.1");
    m->declare("filename", "Cabinet.dsp");
    m->declare("filters.lib/filterbank:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/filterbank:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/filterbank:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/fir:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/fir:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/highpass:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/highpass:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/highshelf:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/highshelf:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/highshelf:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/iir:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/iir:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/low_shelf:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/low_shelf:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/low_shelf:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
    m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
    m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/lowpass:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/lowshelf:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/lowshelf:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/lowshelf:license", "MIT-style STK-4.3 license");
    m->declare("filters.lib/name", "Faust Filters Library");
    m->declare("filters.lib/peak_eq:author", "Julius O. Smith III");
    m->declare(
        "filters.lib/peak_eq:copyright",
        "Copyright (C) 2003-2019 by Julius O. Smith III "
        "<jos@ccrma.stanford.edu>");
    m->declare("filters.lib/peak_eq:license", "MIT-style STK-4.3 license");
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
    m->declare("name", "Cabinet");
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
    fConst1 = std::tan((3769.91113f / fConst0));
    fConst2 = (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fConst1))));
    fConst3 = std::tan((219.911484f / fConst0));
    fConst4 = (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fConst3))));
    fConst5 = std::tan((20420.3516f / fConst0));
    fConst6 = (1.0f / fConst5);
    fConst7 = (1.0f / (((fConst6 + 1.0f) / fConst5) + 1.0f));
    fConst8 = CabinetFaust_faustpower2_f(fConst5);
    fConst9 = (1.0f / fConst8);
    fConst10 = (fConst6 + 1.0f);
    fConst11 = (0.0f - (1.0f / (fConst5 * fConst10)));
    fConst12 = std::tan((18849.5566f / fConst0));
    fConst13 = (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fConst12))));
    fConst14 = std::tan((3455.75195f / fConst0));
    fConst15 = (1.0f / fConst14);
    fConst16 = (1.0f / (((fConst15 + 1.0f) / fConst14) + 1.0f));
    fConst17 = CabinetFaust_faustpower2_f(fConst14);
    fConst18 = (1.0f / fConst17);
    fConst19 = std::tan((314.159271f / fConst0));
    fConst20 = (1.0f / fConst19);
    fConst21 = (fConst0 * std::sin((628.318542f / fConst0)));
    fConst22 = (1117.32593f / fConst21);
    fConst23 = (1.0f / (((fConst20 + fConst22) / fConst19) + 1.0f));
    fConst24 = (fConst15 + 1.0f);
    fConst25 = (1.0f / (fConst14 * fConst24));
    fConst26 = (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fConst19))));
    fConst27 = (3.14159274f / fConst0);
    fConst28 = (6.28318548f / fConst0);
    fConst29 = std::exp((0.0f - (10.0f / fConst0)));
    fConst30 = (1.0f - fConst29);
    fConst31 = (((fConst20 - fConst22) / fConst19) + 1.0f);
    fConst32 = (628.318542f / fConst21);
    fConst33 = (((fConst20 + fConst32) / fConst19) + 1.0f);
    fConst34 = (((fConst20 - fConst32) / fConst19) + 1.0f);
    fConst35 = (0.0f - fConst25);
    fConst36 = (1.0f - fConst15);
    fConst37 = (fConst36 / fConst24);
    fConst38 = (((fConst15 + -1.0f) / fConst14) + 1.0f);
    fConst39 = (2.0f * (1.0f - fConst18));
    fConst40 = (0.0f - (2.0f / fConst17));
    fConst41 = (1.0f / fConst24);
    fConst42 = (1.0f / fConst12);
    fConst43 = (3141.59277f / (fConst0 * std::sin((37699.1133f / fConst0))));
    fConst44 = (1.0f / fConst10);
    fConst45 = (1.0f - fConst6);
    fConst46 = (((fConst6 + -1.0f) / fConst5) + 1.0f);
    fConst47 = (2.0f * (1.0f - fConst9));
    fConst48 = (0.0f - (2.0f / fConst8));
    fConst49 = (1.0f / fConst3);
    fConst50 = (314.159271f / (fConst0 * std::sin((439.822968f / fConst0))));
    fConst51 = (1.0f / fConst1);
    fConst52 = (942.477783f / (fConst0 * std::sin((7539.82227f / fConst0))));
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
    fEntry26 = FAUSTFLOAT(0.0f);
    fEntry27 = FAUSTFLOAT(0.0f);
    fEntry28 = FAUSTFLOAT(0.0f);
    fEntry29 = FAUSTFLOAT(0.0f);
    fEntry30 = FAUSTFLOAT(0.0f);
    fEntry31 = FAUSTFLOAT(0.0f);
    fEntry32 = FAUSTFLOAT(0.0f);
    fEntry33 = FAUSTFLOAT(0.0f);
    fEntry34 = FAUSTFLOAT(0.0f);
    fEntry35 = FAUSTFLOAT(0.0f);
    fEntry36 = FAUSTFLOAT(0.0f);
    fEntry37 = FAUSTFLOAT(0.0f);
    fEntry38 = FAUSTFLOAT(0.0f);
    fEntry39 = FAUSTFLOAT(0.0f);
    fEntry40 = FAUSTFLOAT(0.0f);
    fEntry41 = FAUSTFLOAT(0.0f);
  }

  virtual void instanceClear()
  {
    for (int l0 = 0; (l0 < 3); l0 = (l0 + 1)) { fRec26[l0] = 0.0f; }
    for (int l1 = 0; (l1 < 3); l1 = (l1 + 1)) { fRec25[l1] = 0.0f; }
    for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) { fVec0[l2] = 0.0f; }
    for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) { fRec24[l3] = 0.0f; }
    for (int l4 = 0; (l4 < 3); l4 = (l4 + 1)) { fRec23[l4] = 0.0f; }
    for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) { fVec1[l5] = 0.0f; }
    for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) { fRec22[l6] = 0.0f; }
    for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) { fRec21[l7] = 0.0f; }
    for (int l8 = 0; (l8 < 3); l8 = (l8 + 1)) { fRec20[l8] = 0.0f; }
    for (int l9 = 0; (l9 < 3); l9 = (l9 + 1)) { fRec19[l9] = 0.0f; }
    for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) { fRec30[l10] = 0.0f; }
    for (int l11 = 0; (l11 < 3); l11 = (l11 + 1)) { fRec29[l11] = 0.0f; }
    for (int l12 = 0; (l12 < 3); l12 = (l12 + 1)) { fRec28[l12] = 0.0f; }
    for (int l13 = 0; (l13 < 3); l13 = (l13 + 1)) { fRec27[l13] = 0.0f; }
    for (int l14 = 0; (l14 < 3); l14 = (l14 + 1)) { fRec18[l14] = 0.0f; }
    for (int l15 = 0; (l15 < 3); l15 = (l15 + 1)) { fRec17[l15] = 0.0f; }
    for (int l16 = 0; (l16 < 3); l16 = (l16 + 1)) { fRec16[l16] = 0.0f; }
    for (int l17 = 0; (l17 < 3); l17 = (l17 + 1)) { fRec15[l17] = 0.0f; }
    for (int l18 = 0; (l18 < 3); l18 = (l18 + 1)) { fRec14[l18] = 0.0f; }
    for (int l19 = 0; (l19 < 3); l19 = (l19 + 1)) { fRec13[l19] = 0.0f; }
    for (int l20 = 0; (l20 < 3); l20 = (l20 + 1)) { fRec12[l20] = 0.0f; }
    for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) { fRec31[l21] = 0.0f; }
    for (int l22 = 0; (l22 < 3); l22 = (l22 + 1)) { fRec11[l22] = 0.0f; }
    for (int l23 = 0; (l23 < 3); l23 = (l23 + 1)) { fRec10[l23] = 0.0f; }
    for (int l24 = 0; (l24 < 3); l24 = (l24 + 1)) { fRec9[l24] = 0.0f; }
    for (int l25 = 0; (l25 < 3); l25 = (l25 + 1)) { fRec8[l25] = 0.0f; }
    for (int l26 = 0; (l26 < 3); l26 = (l26 + 1)) { fRec7[l26] = 0.0f; }
    for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) { fVec2[l27] = 0.0f; }
    for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) { fRec6[l28] = 0.0f; }
    for (int l29 = 0; (l29 < 3); l29 = (l29 + 1)) { fRec5[l29] = 0.0f; }
    for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) { fRec33[l30] = 0.0f; }
    for (int l31 = 0; (l31 < 3); l31 = (l31 + 1)) { fRec32[l31] = 0.0f; }
    for (int l32 = 0; (l32 < 3); l32 = (l32 + 1)) { fRec4[l32] = 0.0f; }
    for (int l33 = 0; (l33 < 2); l33 = (l33 + 1)) { fVec3[l33] = 0.0f; }
    for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) { fRec3[l34] = 0.0f; }
    for (int l35 = 0; (l35 < 3); l35 = (l35 + 1)) { fRec2[l35] = 0.0f; }
    for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) { fRec35[l36] = 0.0f; }
    for (int l37 = 0; (l37 < 3); l37 = (l37 + 1)) { fRec34[l37] = 0.0f; }
    for (int l38 = 0; (l38 < 3); l38 = (l38 + 1)) { fRec1[l38] = 0.0f; }
    for (int l39 = 0; (l39 < 3); l39 = (l39 + 1)) { fRec0[l39] = 0.0f; }
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

  virtual CabinetFaust* clone() { return new CabinetFaust(); }

  virtual int getSampleRate() { return fSampleRate; }

  virtual void buildUserInterface(UI* ui_interface)
  {
    ui_interface->openVerticalBox("Cabinet");
    ui_interface->addNumEntry("brightness", &fEntry41, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("distance", &fEntry0, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("dynamic", &fEntry31, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry(
        "dynamic_level", &fEntry32, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("hp_f", &fEntry14, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("lp_f", &fEntry13, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("offset", &fEntry1, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_10_b", &fEntry38, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_10_f", &fEntry3, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_10_l", &fEntry37, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_1_b", &fEntry17, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_1_f", &fEntry11, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_1_l", &fEntry16, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_2_b", &fEntry19, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_2_f", &fEntry10, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_2_l", &fEntry18, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_3_b", &fEntry21, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_3_f", &fEntry9, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_3_l", &fEntry20, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_4_b", &fEntry23, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_4_f", &fEntry8, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_4_l", &fEntry22, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_5_b", &fEntry25, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_5_f", &fEntry7, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_5_l", &fEntry24, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_6_b", &fEntry27, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_6_f", &fEntry6, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_6_l", &fEntry26, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_7_b", &fEntry29, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_7_f", &fEntry5, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_7_l", &fEntry28, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_8_b", &fEntry34, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_8_f", &fEntry30, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_8_l", &fEntry33, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_9_b", &fEntry36, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_9_f", &fEntry4, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("peak_9_l", &fEntry35, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("scoop_b", &fEntry40, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("scoop_f", &fEntry2, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("scoop_l", &fEntry39, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("shelf_f", &fEntry12, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("shelf_l", &fEntry15, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->closeBox();
  }

  virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs)
  {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float fSlow0 = float(fEntry0);
    float fSlow1 =
        (std::pow(10.0f, (0.100000001f * fSlow0))
         * std::pow(10.0f, (0.0500000007f * float(fEntry1))));
    float fSlow2 = float(fEntry2);
    float fSlow3 = std::tan((fConst27 * fSlow2));
    float fSlow4 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow3))));
    float fSlow5 = float(fEntry3);
    float fSlow6 = std::tan((fConst27 * fSlow5));
    float fSlow7 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow6))));
    float fSlow8 = float(fEntry4);
    float fSlow9 = std::tan((fConst27 * fSlow8));
    float fSlow10 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow9))));
    float fSlow11 = float(fEntry5);
    float fSlow12 = std::tan((fConst27 * fSlow11));
    float fSlow13 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow12))));
    float fSlow14 = float(fEntry6);
    float fSlow15 = std::tan((fConst27 * fSlow14));
    float fSlow16 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow15))));
    float fSlow17 = float(fEntry7);
    float fSlow18 = std::tan((fConst27 * fSlow17));
    float fSlow19 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow18))));
    float fSlow20 = float(fEntry8);
    float fSlow21 = std::tan((fConst27 * fSlow20));
    float fSlow22 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow21))));
    float fSlow23 = float(fEntry9);
    float fSlow24 = std::tan((fConst27 * fSlow23));
    float fSlow25 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow24))));
    float fSlow26 = float(fEntry10);
    float fSlow27 = std::tan((fConst27 * fSlow26));
    float fSlow28 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow27))));
    float fSlow29 = float(fEntry11);
    float fSlow30 = std::tan((fConst27 * fSlow29));
    float fSlow31 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow30))));
    float fSlow32 = std::tan((fConst27 * float(fEntry12)));
    float fSlow33 = (1.0f / fSlow32);
    float fSlow34 = (1.0f / (((fSlow33 + 0.445041865f) / fSlow32) + 1.0f));
    float fSlow35 = (1.0f / (((fSlow33 + 1.24697959f) / fSlow32) + 1.0f));
    float fSlow36 = (1.0f / (((fSlow33 + 1.8019377f) / fSlow32) + 1.0f));
    float fSlow37 = (fSlow33 + 1.0f);
    float fSlow38 = (1.0f / fSlow37);
    float fSlow39 = (1.0f - fSlow33);
    float fSlow40 = std::tan((fConst27 * float(fEntry13)));
    float fSlow41 = (1.0f / fSlow40);
    float fSlow42 = (((fSlow41 + 1.0f) / fSlow40) + 1.0f);
    float fSlow43 = (1.0f / fSlow42);
    float fSlow44 = (1.0f / (fSlow41 + 1.0f));
    float fSlow45 = (1.0f - fSlow41);
    float fSlow46 = std::tan((fConst27 * float(fEntry14)));
    float fSlow47 = (1.0f / fSlow46);
    float fSlow48 = (1.0f / (((fSlow47 + 0.765366852f) / fSlow46) + 1.0f));
    float fSlow49 = CabinetFaust_faustpower2_f(fSlow46);
    float fSlow50 = (1.0f / fSlow49);
    float fSlow51 = (1.0f / (((fSlow47 + 1.84775901f) / fSlow46) + 1.0f));
    float fSlow52 = (((fSlow47 + -1.84775901f) / fSlow46) + 1.0f);
    float fSlow53 = (2.0f * (1.0f - fSlow50));
    float fSlow54 = (0.0f - (2.0f / fSlow49));
    float fSlow55 = (((fSlow47 + -0.765366852f) / fSlow46) + 1.0f);
    float fSlow56 = (((fSlow41 + -1.0f) / fSlow40) + 1.0f);
    float fSlow57 =
        (2.0f * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fSlow40))));
    float fSlow58 = (((fSlow33 + -1.8019377f) / fSlow32) + 1.0f);
    float fSlow59 = CabinetFaust_faustpower2_f(fSlow32);
    float fSlow60 = (1.0f / fSlow59);
    float fSlow61 = (2.0f * (1.0f - fSlow60));
    float fSlow62 = (((fSlow33 + -1.24697959f) / fSlow32) + 1.0f);
    float fSlow63 = (((fSlow33 + -0.445041865f) / fSlow32) + 1.0f);
    float fSlow64 = std::pow(10.0f, (0.0500000007f * float(fEntry15)));
    float fSlow65 = (0.0f - (1.0f / (fSlow32 * fSlow37)));
    float fSlow66 = (1.0f / (fSlow32 * fSlow42));
    float fSlow67 = (0.0f - (2.0f / fSlow59));
    float fSlow68 = (1.0f / fSlow30);
    float fSlow69 = float(fEntry16);
    int iSlow70 = (fSlow69 > 0.0f);
    float fSlow71 = float(fEntry17);
    float fSlow72 = std::sin((fConst28 * fSlow29));
    float fSlow73 =
        (fConst27
         * ((fSlow71 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow69))))
            / fSlow72));
    float fSlow74 = (fConst27 * (fSlow71 / fSlow72));
    float fSlow75 = (iSlow70 ? fSlow74 : fSlow73);
    float fSlow76 = ((fSlow68 * (fSlow68 - fSlow75)) + 1.0f);
    float fSlow77 = ((fSlow68 * (fSlow68 + fSlow75)) + 1.0f);
    float fSlow78 = (iSlow70 ? fSlow73 : fSlow74);
    float fSlow79 = ((fSlow68 * (fSlow68 + fSlow78)) + 1.0f);
    float fSlow80 = ((fSlow68 * (fSlow68 - fSlow78)) + 1.0f);
    float fSlow81 = (1.0f / fSlow27);
    float fSlow82 = float(fEntry18);
    int iSlow83 = (fSlow82 > 0.0f);
    float fSlow84 = float(fEntry19);
    float fSlow85 = std::sin((fConst28 * fSlow26));
    float fSlow86 =
        (fConst27
         * ((fSlow84 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow82))))
            / fSlow85));
    float fSlow87 = (fConst27 * (fSlow84 / fSlow85));
    float fSlow88 = (iSlow83 ? fSlow87 : fSlow86);
    float fSlow89 = ((fSlow81 * (fSlow81 - fSlow88)) + 1.0f);
    float fSlow90 = ((fSlow81 * (fSlow81 + fSlow88)) + 1.0f);
    float fSlow91 = (iSlow83 ? fSlow86 : fSlow87);
    float fSlow92 = ((fSlow81 * (fSlow81 + fSlow91)) + 1.0f);
    float fSlow93 = ((fSlow81 * (fSlow81 - fSlow91)) + 1.0f);
    float fSlow94 = (1.0f / fSlow24);
    float fSlow95 = float(fEntry20);
    int iSlow96 = (fSlow95 > 0.0f);
    float fSlow97 = float(fEntry21);
    float fSlow98 = std::sin((fConst28 * fSlow23));
    float fSlow99 =
        (fConst27
         * ((fSlow97 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow95))))
            / fSlow98));
    float fSlow100 = (fConst27 * (fSlow97 / fSlow98));
    float fSlow101 = (iSlow96 ? fSlow100 : fSlow99);
    float fSlow102 = ((fSlow94 * (fSlow94 - fSlow101)) + 1.0f);
    float fSlow103 = ((fSlow94 * (fSlow94 + fSlow101)) + 1.0f);
    float fSlow104 = (iSlow96 ? fSlow99 : fSlow100);
    float fSlow105 = ((fSlow94 * (fSlow94 + fSlow104)) + 1.0f);
    float fSlow106 = ((fSlow94 * (fSlow94 - fSlow104)) + 1.0f);
    float fSlow107 = (1.0f / fSlow21);
    float fSlow108 = float(fEntry22);
    int iSlow109 = (fSlow108 > 0.0f);
    float fSlow110 = float(fEntry23);
    float fSlow111 = std::sin((fConst28 * fSlow20));
    float fSlow112 =
        (fConst27
         * ((fSlow110 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow108))))
            / fSlow111));
    float fSlow113 = (fConst27 * (fSlow110 / fSlow111));
    float fSlow114 = (iSlow109 ? fSlow113 : fSlow112);
    float fSlow115 = ((fSlow107 * (fSlow107 - fSlow114)) + 1.0f);
    float fSlow116 = ((fSlow107 * (fSlow107 + fSlow114)) + 1.0f);
    float fSlow117 = (iSlow109 ? fSlow112 : fSlow113);
    float fSlow118 = ((fSlow107 * (fSlow107 + fSlow117)) + 1.0f);
    float fSlow119 = ((fSlow107 * (fSlow107 - fSlow117)) + 1.0f);
    float fSlow120 = (1.0f / fSlow18);
    float fSlow121 = float(fEntry24);
    int iSlow122 = (fSlow121 > 0.0f);
    float fSlow123 = float(fEntry25);
    float fSlow124 = std::sin((fConst28 * fSlow17));
    float fSlow125 =
        (fConst27
         * ((fSlow123 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow121))))
            / fSlow124));
    float fSlow126 = (fConst27 * (fSlow123 / fSlow124));
    float fSlow127 = (iSlow122 ? fSlow126 : fSlow125);
    float fSlow128 = ((fSlow120 * (fSlow120 - fSlow127)) + 1.0f);
    float fSlow129 = ((fSlow120 * (fSlow120 + fSlow127)) + 1.0f);
    float fSlow130 = (iSlow122 ? fSlow125 : fSlow126);
    float fSlow131 = ((fSlow120 * (fSlow120 + fSlow130)) + 1.0f);
    float fSlow132 = ((fSlow120 * (fSlow120 - fSlow130)) + 1.0f);
    float fSlow133 = (1.0f / fSlow15);
    float fSlow134 = float(fEntry26);
    int iSlow135 = (fSlow134 > 0.0f);
    float fSlow136 = float(fEntry27);
    float fSlow137 = std::sin((fConst28 * fSlow14));
    float fSlow138 =
        (fConst27
         * ((fSlow136 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow134))))
            / fSlow137));
    float fSlow139 = (fConst27 * (fSlow136 / fSlow137));
    float fSlow140 = (iSlow135 ? fSlow139 : fSlow138);
    float fSlow141 = ((fSlow133 * (fSlow133 - fSlow140)) + 1.0f);
    float fSlow142 = ((fSlow133 * (fSlow133 + fSlow140)) + 1.0f);
    float fSlow143 = (iSlow135 ? fSlow138 : fSlow139);
    float fSlow144 = ((fSlow133 * (fSlow133 + fSlow143)) + 1.0f);
    float fSlow145 = ((fSlow133 * (fSlow133 - fSlow143)) + 1.0f);
    float fSlow146 = (1.0f / fSlow12);
    float fSlow147 = float(fEntry28);
    int iSlow148 = (fSlow147 > 0.0f);
    float fSlow149 = float(fEntry29);
    float fSlow150 = std::sin((fConst28 * fSlow11));
    float fSlow151 =
        (fConst27
         * ((fSlow149 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow147))))
            / fSlow150));
    float fSlow152 = (fConst27 * (fSlow149 / fSlow150));
    float fSlow153 = (iSlow148 ? fSlow152 : fSlow151);
    float fSlow154 = ((fSlow146 * (fSlow146 - fSlow153)) + 1.0f);
    float fSlow155 = ((fSlow146 * (fSlow146 + fSlow153)) + 1.0f);
    float fSlow156 = (iSlow148 ? fSlow151 : fSlow152);
    float fSlow157 = ((fSlow146 * (fSlow146 + fSlow156)) + 1.0f);
    float fSlow158 = ((fSlow146 * (fSlow146 - fSlow156)) + 1.0f);
    float fSlow159 = float(fEntry30);
    float fSlow160 = float(fEntry31);
    float fSlow161 = (250.0f * fSlow160);
    float fSlow162 = float(fEntry32);
    float fSlow163 = (0.0500000007f * fSlow162);
    float fSlow164 =
        (1.0f
         / (std::max<float>((0.100000001f * fSlow162), (0.5f * fSlow162))
            - fSlow163));
    float fSlow165 = float(fEntry33);
    float fSlow166 = (2.5f * fSlow160);
    float fSlow167 = float(fEntry34);
    float fSlow168 = (100.0f * fSlow160);
    float fSlow169 = (1.0f / fSlow9);
    float fSlow170 = float(fEntry35);
    int iSlow171 = (fSlow170 > 0.0f);
    float fSlow172 = float(fEntry36);
    float fSlow173 = std::sin((fConst28 * fSlow8));
    float fSlow174 =
        (fConst27
         * ((fSlow172 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow170))))
            / fSlow173));
    float fSlow175 = (fConst27 * (fSlow172 / fSlow173));
    float fSlow176 = (iSlow171 ? fSlow175 : fSlow174);
    float fSlow177 = ((fSlow169 * (fSlow169 - fSlow176)) + 1.0f);
    float fSlow178 = ((fSlow169 * (fSlow169 + fSlow176)) + 1.0f);
    float fSlow179 = (iSlow171 ? fSlow174 : fSlow175);
    float fSlow180 = ((fSlow169 * (fSlow169 + fSlow179)) + 1.0f);
    float fSlow181 = ((fSlow169 * (fSlow169 - fSlow179)) + 1.0f);
    float fSlow182 = (1.0f / fSlow6);
    float fSlow183 = float(fEntry37);
    int iSlow184 = (fSlow183 > 0.0f);
    float fSlow185 = float(fEntry38);
    float fSlow186 = std::sin((fConst28 * fSlow5));
    float fSlow187 =
        (fConst27
         * ((fSlow185 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow183))))
            / fSlow186));
    float fSlow188 = (fConst27 * (fSlow185 / fSlow186));
    float fSlow189 = (iSlow184 ? fSlow188 : fSlow187);
    float fSlow190 = ((fSlow182 * (fSlow182 - fSlow189)) + 1.0f);
    float fSlow191 = ((fSlow182 * (fSlow182 + fSlow189)) + 1.0f);
    float fSlow192 = (iSlow184 ? fSlow187 : fSlow188);
    float fSlow193 = ((fSlow182 * (fSlow182 + fSlow192)) + 1.0f);
    float fSlow194 = ((fSlow182 * (fSlow182 - fSlow192)) + 1.0f);
    float fSlow195 = (1.0f / fSlow3);
    float fSlow196 = float(fEntry39);
    int iSlow197 = (fSlow196 > 0.0f);
    float fSlow198 = float(fEntry40);
    float fSlow199 = std::sin((fConst28 * fSlow2));
    float fSlow200 =
        (fConst27
         * ((fSlow198 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow196))))
            / fSlow199));
    float fSlow201 = (fConst27 * (fSlow198 / fSlow199));
    float fSlow202 = (iSlow197 ? fSlow201 : fSlow200);
    float fSlow203 = ((fSlow195 * (fSlow195 - fSlow202)) + 1.0f);
    float fSlow204 = ((fSlow195 * (fSlow195 + fSlow202)) + 1.0f);
    float fSlow205 = (iSlow197 ? fSlow200 : fSlow201);
    float fSlow206 = ((fSlow195 * (fSlow195 + fSlow205)) + 1.0f);
    float fSlow207 = ((fSlow195 * (fSlow195 - fSlow205)) + 1.0f);
    float fSlow208 = (1.5f * fSlow160);
    float fSlow209 = float(fEntry41);
    float fSlow210 = (3.0f * fSlow209);
    float fSlow211 = (15.0f * fSlow209);
    int iSlow212 = (fSlow211 > 0.0f);
    float fSlow213 =
        (fConst43 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow211))));
    float fSlow214 = (iSlow212 ? fConst43 : fSlow213);
    float fSlow215 = ((fConst42 * (fConst42 - fSlow214)) + 1.0f);
    float fSlow216 = ((fConst42 * (fConst42 + fSlow214)) + 1.0f);
    float fSlow217 = (iSlow212 ? fSlow213 : fConst43);
    float fSlow218 = ((fConst42 * (fConst42 + fSlow217)) + 1.0f);
    float fSlow219 = ((fConst42 * (fConst42 - fSlow217)) + 1.0f);
    float fSlow220 = (0.0250000004f * (0.0f - (5.0f * fSlow160)));
    float fSlow221 = (0.0f - (10.0f * fSlow0));
    int iSlow222 = (fSlow221 > 0.0f);
    float fSlow223 =
        (fConst50 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow221))));
    float fSlow224 = (iSlow222 ? fConst50 : fSlow223);
    float fSlow225 = ((fConst49 * (fConst49 - fSlow224)) + 1.0f);
    float fSlow226 = ((fConst49 * (fConst49 + fSlow224)) + 1.0f);
    float fSlow227 = (iSlow222 ? fSlow223 : fConst50);
    float fSlow228 = ((fConst49 * (fConst49 + fSlow227)) + 1.0f);
    float fSlow229 = ((fConst49 * (fConst49 - fSlow227)) + 1.0f);
    float fSlow230 = (0.0f - (17.0f * fSlow0));
    int iSlow231 = (fSlow230 > 0.0f);
    float fSlow232 =
        (fConst52 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow230))));
    float fSlow233 = (iSlow231 ? fConst52 : fSlow232);
    float fSlow234 = ((fConst51 * (fConst51 - fSlow233)) + 1.0f);
    float fSlow235 = ((fConst51 * (fConst51 + fSlow233)) + 1.0f);
    float fSlow236 = (iSlow231 ? fSlow232 : fConst52);
    float fSlow237 = ((fConst51 * (fConst51 + fSlow236)) + 1.0f);
    float fSlow238 = ((fConst51 * (fConst51 - fSlow236)) + 1.0f);
    for (int i = 0; (i < count); i = (i + 1))
    {
      float fTemp0 = float(input0[i]);
      float fTempFTZ0 =
          (fTemp0
           - (fSlow51 * ((fSlow52 * fRec26[2]) + (fSlow53 * fRec26[1]))));
      fRec26[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ0) & 2139095040) ? fTempFTZ0
                                                              : 0.0f);
      float fTempFTZ1 =
          ((fSlow51
            * (((fSlow50 * fRec26[0]) + (fSlow54 * fRec26[1]))
               + (fSlow50 * fRec26[2])))
           - (fSlow48 * ((fSlow55 * fRec25[2]) + (fSlow53 * fRec25[1]))));
      fRec25[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ1) & 2139095040) ? fTempFTZ1
                                                              : 0.0f);
      float fTemp1 =
          (fSlow48
           * (((fSlow50 * fRec25[0]) + (fSlow54 * fRec25[1]))
              + (fSlow50 * fRec25[2])));
      fVec0[0] = fTemp1;
      float fTempFTZ2 =
          (0.0f - (fSlow44 * ((fSlow45 * fRec24[1]) - (fTemp1 + fVec0[1]))));
      fRec24[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ2) & 2139095040) ? fTempFTZ2
                                                              : 0.0f);
      float fTempFTZ3 =
          (fRec24[0]
           - (fSlow43 * ((fSlow56 * fRec23[2]) + (fSlow57 * fRec23[1]))));
      fRec23[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ3) & 2139095040) ? fTempFTZ3
                                                              : 0.0f);
      float fTemp2 = (fRec23[2] + (fRec23[0] + (2.0f * fRec23[1])));
      float fTemp3 = (fSlow43 * fTemp2);
      fVec1[0] = fTemp3;
      float fTempFTZ4 =
          (0.0f - (fSlow38 * ((fSlow39 * fRec22[1]) - (fTemp3 + fVec1[1]))));
      fRec22[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ4) & 2139095040) ? fTempFTZ4
                                                              : 0.0f);
      float fTempFTZ5 =
          (fRec22[0]
           - (fSlow36 * ((fSlow58 * fRec21[2]) + (fSlow61 * fRec21[1]))));
      fRec21[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ5) & 2139095040) ? fTempFTZ5
                                                              : 0.0f);
      float fTempFTZ6 =
          ((fSlow36 * (fRec21[2] + (fRec21[0] + (2.0f * fRec21[1]))))
           - (fSlow35 * ((fSlow62 * fRec20[2]) + (fSlow61 * fRec20[1]))));
      fRec20[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ6) & 2139095040) ? fTempFTZ6
                                                              : 0.0f);
      float fTempFTZ7 =
          ((fSlow35 * (fRec20[2] + (fRec20[0] + (2.0f * fRec20[1]))))
           - (fSlow34 * ((fSlow63 * fRec19[2]) + (fSlow61 * fRec19[1]))));
      fRec19[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ7) & 2139095040) ? fTempFTZ7
                                                              : 0.0f);
      float fTempFTZ8 =
          ((fSlow65 * fVec1[1])
           - (fSlow38 * ((fSlow39 * fRec30[1]) - (fSlow66 * fTemp2))));
      fRec30[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ8) & 2139095040) ? fTempFTZ8
                                                              : 0.0f);
      float fTempFTZ9 =
          (fRec30[0]
           - (fSlow36 * ((fSlow58 * fRec29[2]) + (fSlow61 * fRec29[1]))));
      fRec29[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ9) & 2139095040) ? fTempFTZ9
                                                              : 0.0f);
      float fTempFTZ10 =
          ((fSlow36
            * (((fSlow60 * fRec29[0]) + (fSlow67 * fRec29[1]))
               + (fSlow60 * fRec29[2])))
           - (fSlow35 * ((fSlow62 * fRec28[2]) + (fSlow61 * fRec28[1]))));
      fRec28[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ10) & 2139095040) ? fTempFTZ10
                                                               : 0.0f);
      float fTempFTZ11 =
          ((fSlow35
            * (((fSlow60 * fRec28[0]) + (fSlow67 * fRec28[1]))
               + (fSlow60 * fRec28[2])))
           - (fSlow34 * ((fSlow63 * fRec27[2]) + (fSlow61 * fRec27[1]))));
      fRec27[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ11) & 2139095040) ? fTempFTZ11
                                                               : 0.0f);
      float fTemp4 = (fSlow31 * fRec18[1]);
      float fTempFTZ12 =
          ((fSlow34
            * ((fRec19[2] + (fRec19[0] + (2.0f * fRec19[1])))
               + (fSlow64
                  * (((fSlow60 * fRec27[0]) + (fSlow67 * fRec27[1]))
                     + (fSlow60 * fRec27[2])))))
           - (((fRec18[2] * fSlow76) + fTemp4) / fSlow77));
      fRec18[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ12) & 2139095040) ? fTempFTZ12
                                                               : 0.0f);
      float fTemp5 = (fSlow28 * fRec17[1]);
      float fTempFTZ13 =
          ((((fTemp4 + (fRec18[0] * fSlow79)) + (fRec18[2] * fSlow80))
            / fSlow77)
           - (((fRec17[2] * fSlow89) + fTemp5) / fSlow90));
      fRec17[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ13) & 2139095040) ? fTempFTZ13
                                                               : 0.0f);
      float fTemp6 = (fSlow25 * fRec16[1]);
      float fTempFTZ14 =
          ((((fTemp5 + (fRec17[0] * fSlow92)) + (fRec17[2] * fSlow93))
            / fSlow90)
           - (((fRec16[2] * fSlow102) + fTemp6) / fSlow103));
      fRec16[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ14) & 2139095040) ? fTempFTZ14
                                                               : 0.0f);
      float fTemp7 = (fSlow22 * fRec15[1]);
      float fTempFTZ15 =
          ((((fTemp6 + (fRec16[0] * fSlow105)) + (fRec16[2] * fSlow106))
            / fSlow103)
           - (((fRec15[2] * fSlow115) + fTemp7) / fSlow116));
      fRec15[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ15) & 2139095040) ? fTempFTZ15
                                                               : 0.0f);
      float fTemp8 = (fSlow19 * fRec14[1]);
      float fTempFTZ16 =
          ((((fTemp7 + (fRec15[0] * fSlow118)) + (fRec15[2] * fSlow119))
            / fSlow116)
           - (((fRec14[2] * fSlow128) + fTemp8) / fSlow129));
      fRec14[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ16) & 2139095040) ? fTempFTZ16
                                                               : 0.0f);
      float fTemp9 = (fSlow16 * fRec13[1]);
      float fTempFTZ17 =
          ((((fTemp8 + (fRec14[0] * fSlow131)) + (fRec14[2] * fSlow132))
            / fSlow129)
           - (((fRec13[2] * fSlow141) + fTemp9) / fSlow142));
      fRec13[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ17) & 2139095040) ? fTempFTZ17
                                                               : 0.0f);
      float fTemp10 = (fSlow13 * fRec12[1]);
      float fTempFTZ18 =
          ((((fTemp9 + (fRec13[0] * fSlow144)) + (fRec13[2] * fSlow145))
            / fSlow142)
           - (((fRec12[2] * fSlow154) + fTemp10) / fSlow155));
      fRec12[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ18) & 2139095040) ? fTempFTZ18
                                                               : 0.0f);
      float fTempFTZ19 =
          ((fConst29 * fRec31[1]) + (fConst30 * std::fabs(fTemp0)));
      fRec31[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ19) & 2139095040) ? fTempFTZ19
                                                               : 0.0f);
      float fTemp11 = std::max<float>(
          -1.0f,
          std::min<float>(
              1.0f,
              (0.588235319f * ((fSlow164 * (fRec31[0] - fSlow163)) + -0.5f))));
      float fTemp12 = (fTemp11 * (std::fabs(fTemp11) + -2.0f));
      float fTemp13 = ((fTemp12 * (std::fabs(fTemp12) + -2.0f)) + 1.0f);
      float fTemp14 = (fSlow159 - (fSlow161 * fTemp13));
      float fTemp15 = std::tan((fConst27 * fTemp14));
      float fTemp16 = (1.0f / fTemp15);
      float fTemp17 = (fSlow165 + (fSlow166 * fTemp13));
      int iTemp18 = (fTemp17 > 0.0f);
      float fTemp19 = (fSlow167 + (fSlow168 * fTemp13));
      float fTemp20 = std::sin((fConst28 * fTemp14));
      float fTemp21 =
          (fConst27
           * ((std::pow(10.0f, (0.0500000007f * std::fabs(fTemp17))) * fTemp19)
              / fTemp20));
      float fTemp22 = (fConst27 * (fTemp19 / fTemp20));
      float fTemp23 = (iTemp18 ? fTemp22 : fTemp21);
      float fTemp24 =
          (2.0f
           * (fRec11[1]
              * (1.0f - (1.0f / CabinetFaust_faustpower2_f(fTemp15)))));
      float fTemp25 = (((fTemp16 + fTemp23) / fTemp15) + 1.0f);
      float fTempFTZ20 =
          ((((fTemp10 + (fRec12[0] * fSlow157)) + (fRec12[2] * fSlow158))
            / fSlow155)
           - (((fRec11[2] * (((fTemp16 - fTemp23) / fTemp15) + 1.0f)) + fTemp24)
              / fTemp25));
      fRec11[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ20) & 2139095040) ? fTempFTZ20
                                                               : 0.0f);
      float fTemp26 = (iTemp18 ? fTemp21 : fTemp22);
      float fTemp27 = (fSlow10 * fRec10[1]);
      float fTempFTZ21 =
          ((((fTemp24 + (fRec11[0] * (((fTemp16 + fTemp26) / fTemp15) + 1.0f)))
             + (fRec11[2] * (((fTemp16 - fTemp26) / fTemp15) + 1.0f)))
            / fTemp25)
           - (((fRec10[2] * fSlow177) + fTemp27) / fSlow178));
      fRec10[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ21) & 2139095040) ? fTempFTZ21
                                                               : 0.0f);
      float fTemp28 = (fSlow7 * fRec9[1]);
      float fTempFTZ22 =
          ((((fTemp27 + (fRec10[0] * fSlow180)) + (fRec10[2] * fSlow181))
            / fSlow178)
           - (((fRec9[2] * fSlow190) + fTemp28) / fSlow191));
      fRec9[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ22) & 2139095040) ? fTempFTZ22
                                                               : 0.0f);
      float fTemp29 = (fSlow4 * fRec8[1]);
      float fTempFTZ23 =
          ((((fTemp28 + (fRec9[0] * fSlow193)) + (fRec9[2] * fSlow194))
            / fSlow191)
           - (((fRec8[2] * fSlow203) + fTemp29) / fSlow204));
      fRec8[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ23) & 2139095040) ? fTempFTZ23
                                                               : 0.0f);
      float fTemp30 = (fConst26 * fRec7[1]);
      float fTempFTZ24 =
          ((((fTemp29 + (fRec8[0] * fSlow206)) + (fRec8[2] * fSlow207))
            / fSlow204)
           - (fConst23 * ((fConst31 * fRec7[2]) + fTemp30)));
      fRec7[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ24) & 2139095040) ? fTempFTZ24
                                                               : 0.0f);
      float fTemp31 =
          ((fTemp30 + (fConst33 * fRec7[0])) + (fConst34 * fRec7[2]));
      fVec2[0] = fTemp31;
      float fTempFTZ25 =
          ((fConst23 * ((fConst25 * fTemp31) + (fConst35 * fVec2[1])))
           - (fConst37 * fRec6[1]));
      fRec6[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ25) & 2139095040) ? fTempFTZ25
                                                               : 0.0f);
      float fTempFTZ26 =
          (fRec6[0]
           - (fConst16 * ((fConst38 * fRec5[2]) + (fConst39 * fRec5[1]))));
      fRec5[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ26) & 2139095040) ? fTempFTZ26
                                                               : 0.0f);
      float fTempFTZ27 =
          (0.0f
           - (fConst41
              * ((fConst36 * fRec33[1]) - (fConst23 * (fTemp31 + fVec2[1])))));
      fRec33[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ27) & 2139095040) ? fTempFTZ27
                                                               : 0.0f);
      float fTempFTZ28 =
          (fRec33[0]
           - (fConst16 * ((fConst38 * fRec32[2]) + (fConst39 * fRec32[1]))));
      fRec32[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ28) & 2139095040) ? fTempFTZ28
                                                               : 0.0f);
      float fTemp32 = (fConst13 * fRec4[1]);
      float fTempFTZ29 =
          ((fConst16
            * ((((fConst18 * fRec5[0]) + (fConst40 * fRec5[1]))
                + (fConst18 * fRec5[2]))
               + ((fRec32[2] + (fRec32[0] + (2.0f * fRec32[1])))
                  * std::pow(
                      10.0f,
                      (0.0500000007f * ((fSlow208 * fTemp13) - fSlow210))))))
           - (((fRec4[2] * fSlow215) + fTemp32) / fSlow216));
      fRec4[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ29) & 2139095040) ? fTempFTZ29
                                                               : 0.0f);
      float fTemp33 =
          (((fTemp32 + (fRec4[0] * fSlow218)) + (fRec4[2] * fSlow219))
           / fSlow216);
      fVec3[0] = fTemp33;
      float fTempFTZ30 =
          ((fConst11 * fVec3[1])
           - (fConst44 * ((fConst45 * fRec3[1]) - (fConst6 * fTemp33))));
      fRec3[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ30) & 2139095040) ? fTempFTZ30
                                                               : 0.0f);
      float fTempFTZ31 =
          (fRec3[0]
           - (fConst7 * ((fConst46 * fRec2[2]) + (fConst47 * fRec2[1]))));
      fRec2[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ31) & 2139095040) ? fTempFTZ31
                                                               : 0.0f);
      float fTempFTZ32 =
          (0.0f - (fConst44 * ((fConst45 * fRec35[1]) - (fTemp33 + fVec3[1]))));
      fRec35[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ32) & 2139095040) ? fTempFTZ32
                                                               : 0.0f);
      float fTempFTZ33 =
          (fRec35[0]
           - (fConst7 * ((fConst46 * fRec34[2]) + (fConst47 * fRec34[1]))));
      fRec34[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ33) & 2139095040) ? fTempFTZ33
                                                               : 0.0f);
      float fTemp34 = (fConst4 * fRec1[1]);
      float fTempFTZ34 =
          ((fConst7
            * (((((fConst9 * fRec2[0]) + (fConst48 * fRec2[1]))
                 + (fConst9 * fRec2[2]))
                * std::pow(10.0f, (fSlow220 * fTemp13)))
               + (fRec34[2] + (fRec34[0] + (2.0f * fRec34[1])))))
           - (((fRec1[2] * fSlow225) + fTemp34) / fSlow226));
      fRec1[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ34) & 2139095040) ? fTempFTZ34
                                                               : 0.0f);
      float fTemp35 = (fConst2 * fRec0[1]);
      float fTempFTZ35 =
          ((((fTemp34 + (fRec1[0] * fSlow228)) + (fRec1[2] * fSlow229))
            / fSlow226)
           - (((fRec0[2] * fSlow234) + fTemp35) / fSlow235));
      fRec0[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ35) & 2139095040) ? fTempFTZ35
                                                               : 0.0f);
      output0[i] = FAUSTFLOAT(
          (fSlow1
           * (((fTemp35 + (fRec0[0] * fSlow237)) + (fRec0[2] * fSlow238))
              / fSlow235)));
      fRec26[2] = fRec26[1];
      fRec26[1] = fRec26[0];
      fRec25[2] = fRec25[1];
      fRec25[1] = fRec25[0];
      fVec0[1] = fVec0[0];
      fRec24[1] = fRec24[0];
      fRec23[2] = fRec23[1];
      fRec23[1] = fRec23[0];
      fVec1[1] = fVec1[0];
      fRec22[1] = fRec22[0];
      fRec21[2] = fRec21[1];
      fRec21[1] = fRec21[0];
      fRec20[2] = fRec20[1];
      fRec20[1] = fRec20[0];
      fRec19[2] = fRec19[1];
      fRec19[1] = fRec19[0];
      fRec30[1] = fRec30[0];
      fRec29[2] = fRec29[1];
      fRec29[1] = fRec29[0];
      fRec28[2] = fRec28[1];
      fRec28[1] = fRec28[0];
      fRec27[2] = fRec27[1];
      fRec27[1] = fRec27[0];
      fRec18[2] = fRec18[1];
      fRec18[1] = fRec18[0];
      fRec17[2] = fRec17[1];
      fRec17[1] = fRec17[0];
      fRec16[2] = fRec16[1];
      fRec16[1] = fRec16[0];
      fRec15[2] = fRec15[1];
      fRec15[1] = fRec15[0];
      fRec14[2] = fRec14[1];
      fRec14[1] = fRec14[0];
      fRec13[2] = fRec13[1];
      fRec13[1] = fRec13[0];
      fRec12[2] = fRec12[1];
      fRec12[1] = fRec12[0];
      fRec31[1] = fRec31[0];
      fRec11[2] = fRec11[1];
      fRec11[1] = fRec11[0];
      fRec10[2] = fRec10[1];
      fRec10[1] = fRec10[0];
      fRec9[2] = fRec9[1];
      fRec9[1] = fRec9[0];
      fRec8[2] = fRec8[1];
      fRec8[1] = fRec8[0];
      fRec7[2] = fRec7[1];
      fRec7[1] = fRec7[0];
      fVec2[1] = fVec2[0];
      fRec6[1] = fRec6[0];
      fRec5[2] = fRec5[1];
      fRec5[1] = fRec5[0];
      fRec33[1] = fRec33[0];
      fRec32[2] = fRec32[1];
      fRec32[1] = fRec32[0];
      fRec4[2] = fRec4[1];
      fRec4[1] = fRec4[0];
      fVec3[1] = fVec3[0];
      fRec3[1] = fRec3[0];
      fRec2[2] = fRec2[1];
      fRec2[1] = fRec2[0];
      fRec35[1] = fRec35[0];
      fRec34[2] = fRec34[1];
      fRec34[1] = fRec34[0];
      fRec1[2] = fRec1[1];
      fRec1[1] = fRec1[0];
      fRec0[2] = fRec0[1];
      fRec0[1] = fRec0[0];
    }
  }
};

#endif
