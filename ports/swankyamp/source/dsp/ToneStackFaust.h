/* ------------------------------------------------------------
name: "ToneStack"
Code generated with Faust 2.28.3 (https://faust.grame.fr)
Compilation options: -lang cpp -inpl -scal -ftz 2
------------------------------------------------------------ */

#ifndef __ToneStackFaust_H__
#define __ToneStackFaust_H__

#include "FaustImpl.h"
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <algorithm>
#include <cmath>
#include <math.h>

static float ToneStackFaust_faustpower2_f(float value)
{
  return (value * value);
}

#ifndef FAUSTCLASS
#define FAUSTCLASS ToneStackFaust
#endif

#ifdef __APPLE__
#define exp10f __exp10f
#define exp10 __exp10
#endif

class ToneStackFaust : public FaustImpl
{
private:
  int fSampleRate;
  float fConst0;
  float fConst1;
  float fConst2;
  FAUSTFLOAT fEntry0;
  float fConst3;
  float fConst4;
  float fConst5;
  FAUSTFLOAT fEntry1;
  float fConst6;
  float fConst7;
  float fRec1[3];
  float fConst8;
  float fConst9;
  FAUSTFLOAT fEntry2;
  FAUSTFLOAT fEntry3;
  float fRec2[3];
  float fConst10;
  float fConst11;
  float fConst12;
  float fConst13;
  float fConst14;
  float fConst15;
  float fRec3[3];
  float fConst16;
  float fConst17;
  float fRec4[3];
  float fConst18;
  float fConst19;
  float fConst20;
  float fConst21;
  float fConst22;
  float fConst23;
  float fConst24;
  float fConst25;
  float fRec6[3];
  float fConst26;
  float fRec7[3];
  float fConst27;
  float fConst28;
  float fConst29;
  float fRec5[3];
  float fConst30;
  FAUSTFLOAT fEntry4;
  float fConst31;
  float fRec0[3];

public:
  void metadata(Meta* m)
  {
    m->declare("basics.lib/name", "Faust Basic Element Library");
    m->declare("basics.lib/version", "0.1");
    m->declare("filename", "ToneStack.dsp");
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
    m->declare("name", "ToneStack");
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
    fConst2 = (2.0f * (1.0f - (1.0f / ToneStackFaust_faustpower2_f(fConst1))));
    fConst3 = (9.57499981f * fConst0);
    fConst4 = (fConst3 + 138000.0f);
    fConst5 = (2.5f * (fConst0 / fConst4));
    fConst6 = (1.0f / fConst4);
    fConst7 = (138000.0f - fConst3);
    fConst8 = ToneStackFaust_faustpower2_f(fConst0);
    fConst9 = (2.35000007e-05f * fConst8);
    fConst10 = (1.17500003e-05f * fConst0);
    fConst11 = (3.41219997f * fConst0);
    fConst12 = (fConst11 + 33000.0f);
    fConst13 = (0.511829972f * (fConst0 / fConst12));
    fConst14 = (1.0f / fConst12);
    fConst15 = (33000.0f - fConst11);
    fConst16 = (9.68000052e-16f * fConst8);
    fConst17 = (4.84000026e-16f * fConst0);
    fConst18 = std::tan((3141.59277f / fConst0));
    fConst19 =
        (2.0f * (1.0f - (1.0f / ToneStackFaust_faustpower2_f(fConst18))));
    fConst20 = (25.368f * fConst0);
    fConst21 = (fConst20 + 158000.0f);
    fConst22 = (1.5f / fConst21);
    fConst23 = (5.59999991e-10f * fConst0);
    fConst24 = (1.0f / fConst21);
    fConst25 = (158000.0f - fConst20);
    fConst26 = (412500.0f * fConst8);
    fConst27 = (206250.0f * fConst0);
    fConst28 = (1.0f / fConst18);
    fConst29 = (6283.18555f / (fConst0 * std::sin((6283.18555f / fConst0))));
    fConst30 = (1.0f / fConst1);
    fConst31 = (6283.18555f / (fConst0 * std::sin((25132.7422f / fConst0))));
  }

  virtual void instanceResetUserInterface()
  {
    fEntry0 = FAUSTFLOAT(0.0f);
    fEntry1 = FAUSTFLOAT(0.0f);
    fEntry2 = FAUSTFLOAT(0.0f);
    fEntry3 = FAUSTFLOAT(0.0f);
    fEntry4 = FAUSTFLOAT(0.0f);
  }

  virtual void instanceClear()
  {
    for (int l0 = 0; (l0 < 3); l0 = (l0 + 1)) { fRec1[l0] = 0.0f; }
    for (int l1 = 0; (l1 < 3); l1 = (l1 + 1)) { fRec2[l1] = 0.0f; }
    for (int l2 = 0; (l2 < 3); l2 = (l2 + 1)) { fRec3[l2] = 0.0f; }
    for (int l3 = 0; (l3 < 3); l3 = (l3 + 1)) { fRec4[l3] = 0.0f; }
    for (int l4 = 0; (l4 < 3); l4 = (l4 + 1)) { fRec6[l4] = 0.0f; }
    for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) { fRec7[l5] = 0.0f; }
    for (int l6 = 0; (l6 < 3); l6 = (l6 + 1)) { fRec5[l6] = 0.0f; }
    for (int l7 = 0; (l7 < 3); l7 = (l7 + 1)) { fRec0[l7] = 0.0f; }
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

  virtual ToneStackFaust* clone() { return new ToneStackFaust(); }

  virtual int getSampleRate() { return fSampleRate; }

  virtual void buildUserInterface(UI* ui_interface)
  {
    ui_interface->openVerticalBox("ToneStack");
    ui_interface->addNumEntry("bass", &fEntry2, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("mids", &fEntry3, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("presence", &fEntry4, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("selection", &fEntry0, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->addNumEntry("treble", &fEntry1, 0.0f, 0.0f, 1.0f, 1.0f);
    ui_interface->closeBox();
  }

  virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs)
  {
    FAUSTFLOAT* input0 = inputs[0];
    FAUSTFLOAT* output0 = outputs[0];
    float fSlow0 = float(fEntry0);
    float fSlow1 =
        (1.0f - std::min<float>(std::max<float>(fSlow0, 0.0f), 1.0f));
    float fSlow2 = ((0.495000005f * float(fEntry1)) + 0.504999995f);
    float fSlow3 = (fConst5 * fSlow2);
    float fSlow4 = (0.0f - fSlow3);
    float fSlow5 = ((0.495000005f * float(fEntry2)) + 0.504999995f);
    float fSlow6 = ToneStackFaust_faustpower2_f(fSlow5);
    float fSlow7 = float(fEntry3);
    float fSlow8 = (0.495000005f * fSlow7);
    float fSlow9 = (fSlow8 + 0.504999995f);
    float fSlow10 = (fSlow6 * fSlow9);
    float fSlow11 = ((0.00147000002f * fSlow9) + (0.0250000004f * fSlow6));
    float fSlow12 = (fSlow11 + 0.0202859994f);
    float fSlow13 = (fSlow6 * ((1.17500003e-05f * fSlow9) + 0.000162149998f));
    float fSlow14 = (fConst0 * fSlow13);
    float fSlow15 = ((fConst0 * (fSlow12 + fSlow14)) + 1.0f);
    float fSlow16 = (0.0f - (fConst9 * (fSlow10 / fSlow15)));
    float fSlow17 = (1.0f / fSlow15);
    float fSlow18 = (1.0f - (fConst0 * (fSlow12 - fSlow14)));
    float fSlow19 = (2.0f * (1.0f - (fConst8 * fSlow13)));
    float fSlow20 = (fConst0 / fSlow15);
    float fSlow21 = (fConst10 * fSlow10);
    float fSlow22 = (fSlow11 + fSlow21);
    float fSlow23 = (fSlow21 - fSlow11);
    float fSlow24 =
        (1.0f
         - std::min<float>(
             std::max<float>(std::fabs((fSlow0 + -1.0f)), 0.0f), 1.0f));
    float fSlow25 = (fConst13 * fSlow2);
    float fSlow26 = (0.0f - fSlow25);
    float fSlow27 =
        (484000000.0f
         * (ToneStackFaust_faustpower2_f(fSlow9) + (-0.504999995f - fSlow8)));
    float fSlow28 =
        ((0.200000003f * (fSlow5 * float((fSlow5 <= 0.5f))))
         + (float((fSlow5 > 0.5f)) * ((1.60000002f * fSlow5) + -0.699999988f)));
    float fSlow29 = (fSlow9 * fSlow28);
    float fSlow30 = (fSlow27 - (2.2000001e+10f * fSlow29));
    float fSlow31 = (22000.0f * (0.0f - (2.2e-08f * (fSlow9 + 1.0f))));
    float fSlow32 = (0.0219999999f * fSlow28);
    float fSlow33 = (fSlow31 + (-0.001452f - fSlow32));
    float fSlow34 =
        ((fSlow27
          + (22000.0f
             * ((1000000.0f * (0.0f - fSlow29))
                + (16335.0f * (fSlow7 + -1.0f)))))
         + (1000000.0f * (0.0f - (33000.0f * fSlow28))));
    float fSlow35 = (fConst17 * fSlow34);
    float fSlow36 = ((fConst0 * (fSlow33 + fSlow35)) + -1.0f);
    float fSlow37 = (0.0f - (fConst16 * (fSlow30 / fSlow36)));
    float fSlow38 = (1.0f / fSlow36);
    float fSlow39 = (-2.0f - (fConst16 * fSlow34));
    float fSlow40 = ((fConst0 * (fSlow35 - fSlow33)) + -1.0f);
    float fSlow41 = (fConst0 / fSlow36);
    float fSlow42 = (fSlow31 - fSlow32);
    float fSlow43 = (fConst17 * fSlow30);
    float fSlow44 = (fSlow42 + fSlow43);
    float fSlow45 = (fSlow43 - fSlow42);
    float fSlow46 =
        (std::min<float>(std::max<float>(fSlow0, 1.0f), 2.0f) + -1.0f);
    float fSlow47 =
        (fConst23 * ((250000.0f * ((100000.0f * fSlow2) + 10000.0f)) + 1e+09f));
    float fSlow48 = (fSlow47 + 10000.0f);
    float fSlow49 = (10000.0f - fSlow47);
    float fSlow50 = (ToneStackFaust_faustpower2_f(fSlow28) - fSlow28);
    float fSlow51 = (fSlow28 + -1.0f);
    float fSlow52 =
        (((150000.0f * ((250000.0f * fSlow51) + -10000.0f))
          + (6.24999997e+10f * fSlow50))
         + (250000.0f * ((250000.0f * (fSlow2 * fSlow51)) + -10000.0f)));
    float fSlow53 = (6250.0f * fSlow50);
    float fSlow54 = ((2.2e-08f * fSlow51) + -1.00000001e-07f);
    float fSlow55 =
        ((((150000.0f * ((fSlow53 + (4813.0f * fSlow51)) + -440.320007f))
           + (6.24999997e+10f
              * (fSlow50 * ((0.0250000004f * fSlow2) + 0.0192520004f))))
          + (250000.0f
             * ((250000.0f
                 * (fSlow2
                    * ((((0.0121999998f * fSlow51) + (10000.0f * fSlow54))
                        + (0.0055999998f * fSlow51))
                       + (0.00123199995f * fSlow51))))
                + -190.320007f)))
         + (2.5e+09f
            * ((100000.0f * (0.0f - (1.22000003e-07f * fSlow2)))
               - (0.00683199987f * fSlow2))));
    float fSlow56 =
        (((150000.0f * ((22.8250008f * fSlow50) - (0.85799998f * fSlow28)))
          + (6.24999997e+10f
             * (fSlow50 * ((9.12999967e-05f * fSlow2) + 3.43200009e-06f))))
         - (214500.0f * (fSlow2 * fSlow28)));
    float fSlow57 = (fConst0 * fSlow56);
    float fSlow58 = (2.5e+09f * fSlow2);
    float fSlow59 = ((fSlow52 + (fConst0 * (fSlow55 + fSlow57))) - fSlow58);
    float fSlow60 = (0.0f - (fConst26 * (fSlow50 / fSlow59)));
    float fSlow61 = (1.0f / fSlow59);
    float fSlow62 = (fSlow52 - (fSlow58 + (fConst0 * (fSlow55 - fSlow57))));
    float fSlow63 = (2.0f * ((fSlow52 - fSlow58) - (fConst8 * fSlow56)));
    float fSlow64 = (fConst0 / fSlow59);
    float fSlow65 = (150000.0f * (fSlow53 + (2.5e+09f * fSlow54)));
    float fSlow66 = (fConst27 * fSlow50);
    float fSlow67 = (fSlow65 + fSlow66);
    float fSlow68 = (fSlow66 - fSlow65);
    float fSlow69 = (10.0f * fSlow7);
    int iSlow70 = (fSlow69 > 0.0f);
    float fSlow71 =
        (fConst29 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow69))));
    float fSlow72 = (iSlow70 ? fConst29 : fSlow71);
    float fSlow73 = ((fConst28 * (fConst28 - fSlow72)) + 1.0f);
    float fSlow74 = ((fConst28 * (fConst28 + fSlow72)) + 1.0f);
    float fSlow75 = (iSlow70 ? fSlow71 : fConst29);
    float fSlow76 = ((fConst28 * (fConst28 + fSlow75)) + 1.0f);
    float fSlow77 = ((fConst28 * (fConst28 - fSlow75)) + 1.0f);
    float fSlow78 = (10.0f * float(fEntry4));
    int iSlow79 = (fSlow78 > 0.0f);
    float fSlow80 =
        (fConst31 * std::pow(10.0f, (0.0500000007f * std::fabs(fSlow78))));
    float fSlow81 = (iSlow79 ? fConst31 : fSlow80);
    float fSlow82 = ((fConst30 * (fConst30 - fSlow81)) + 1.0f);
    float fSlow83 = ((fConst30 * (fConst30 + fSlow81)) + 1.0f);
    float fSlow84 = (iSlow79 ? fSlow80 : fConst31);
    float fSlow85 = ((fConst30 * (fConst30 + fSlow84)) + 1.0f);
    float fSlow86 = ((fConst30 * (fConst30 - fSlow84)) + 1.0f);
    for (int i = 0; (i < count); i = (i + 1))
    {
      float fTemp0 = float(input0[i]);
      float fTempFTZ0 =
          (fTemp0
           - (fConst6 * ((fConst7 * fRec1[2]) + (276000.0f * fRec1[1]))));
      fRec1[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ0) & 2139095040) ? fTempFTZ0
                                                              : 0.0f);
      float fTempFTZ1 =
          (fTemp0 - (fSlow17 * ((fSlow18 * fRec2[2]) + (fSlow19 * fRec2[1]))));
      fRec2[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ1) & 2139095040) ? fTempFTZ1
                                                              : 0.0f);
      float fTempFTZ2 =
          (fTemp0
           - (fConst14 * ((fConst15 * fRec3[2]) + (66000.0f * fRec3[1]))));
      fRec3[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ2) & 2139095040) ? fTempFTZ2
                                                              : 0.0f);
      float fTempFTZ3 =
          (fTemp0 - (fSlow38 * ((fSlow39 * fRec4[1]) + (fSlow40 * fRec4[2]))));
      fRec4[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ3) & 2139095040) ? fTempFTZ3
                                                              : 0.0f);
      float fTempFTZ4 =
          (fTemp0
           - (fConst24 * ((fConst25 * fRec6[2]) + (316000.0f * fRec6[1]))));
      fRec6[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ4) & 2139095040) ? fTempFTZ4
                                                              : 0.0f);
      float fTempFTZ5 =
          (fTemp0 - (fSlow61 * ((fSlow62 * fRec7[2]) + (fSlow63 * fRec7[1]))));
      fRec7[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ5) & 2139095040) ? fTempFTZ5
                                                              : 0.0f);
      float fTemp1 = (fConst19 * fRec5[1]);
      float fTempFTZ6 =
          (((fConst22
             * (((fSlow48 * fRec6[0]) + (20000.0f * fRec6[1]))
                + (fSlow49 * fRec6[2])))
            + (8.0f
               * ((fSlow60 * fRec7[1])
                  + (fSlow64 * ((fSlow67 * fRec7[0]) + (fSlow68 * fRec7[2]))))))
           - (((fRec5[2] * fSlow73) + fTemp1) / fSlow74));
      fRec5[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ6) & 2139095040) ? fTempFTZ6
                                                              : 0.0f);
      float fTemp2 = (fConst2 * fRec0[1]);
      float fTempFTZ7 =
          ((((fSlow1
              * ((12.0f * ((fSlow3 * fRec1[0]) + (fSlow4 * fRec1[2])))
                 + (4.0f
                    * ((fSlow16 * fRec2[1])
                       + (fSlow20
                          * ((fSlow22 * fRec2[0]) + (fSlow23 * fRec2[2])))))))
             + (fSlow24
                * ((6.0f * ((fSlow25 * fRec3[0]) + (fSlow26 * fRec3[2])))
                   + (1.39999998f
                      * ((fSlow37 * fRec4[1])
                         + (fSlow41
                            * ((fSlow44 * fRec4[0])
                               + (fSlow45 * fRec4[2]))))))))
            + (fSlow46
               * (((fTemp1 + (fRec5[0] * fSlow76)) + (fRec5[2] * fSlow77))
                  / fSlow74)))
           - (((fRec0[2] * fSlow82) + fTemp2) / fSlow83));
      fRec0[0] =
          ((*reinterpret_cast<int*>(&fTempFTZ7) & 2139095040) ? fTempFTZ7
                                                              : 0.0f);
      output0[i] = FAUSTFLOAT(
          (((fTemp2 + (fRec0[0] * fSlow85)) + (fRec0[2] * fSlow86)) / fSlow83));
      fRec1[2] = fRec1[1];
      fRec1[1] = fRec1[0];
      fRec2[2] = fRec2[1];
      fRec2[1] = fRec2[0];
      fRec3[2] = fRec3[1];
      fRec3[1] = fRec3[0];
      fRec4[2] = fRec4[1];
      fRec4[1] = fRec4[0];
      fRec6[2] = fRec6[1];
      fRec6[1] = fRec6[0];
      fRec7[2] = fRec7[1];
      fRec7[1] = fRec7[0];
      fRec5[2] = fRec5[1];
      fRec5[1] = fRec5[0];
      fRec0[2] = fRec0[1];
      fRec0[1] = fRec0[0];
    }
  }
};

#endif
