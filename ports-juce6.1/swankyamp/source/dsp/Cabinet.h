#ifndef __faust2hpp_Cabinet_H__
#define __faust2hpp_Cabinet_H__

#include <cmath>

#define uscale(x, l, u) (x + 1.0f) / 2.0f * (u - l) + l;
#define ulscale(x, l, u) \
  std::exp((x + 1.0f) / 2.0f * (std::log(u) - std::log(l)) + std::log(l));

#include "CabinetFaust.h"

class Cabinet
{
public:
  Cabinet()
  {
    faustDsp.buildUserInterface(&faustDsp);
    par_brightness = faustDsp.getParameter("brightness");
    par_distance = faustDsp.getParameter("distance");
    par_dynamic = faustDsp.getParameter("dynamic");
    par_dynamic_level = faustDsp.getParameter("dynamic_level");
    par_hp_f = faustDsp.getParameter("hp_f");
    par_lp_f = faustDsp.getParameter("lp_f");
    par_offset = faustDsp.getParameter("offset");
    par_peak_10_b = faustDsp.getParameter("peak_10_b");
    par_peak_10_f = faustDsp.getParameter("peak_10_f");
    par_peak_10_l = faustDsp.getParameter("peak_10_l");
    par_peak_1_b = faustDsp.getParameter("peak_1_b");
    par_peak_1_f = faustDsp.getParameter("peak_1_f");
    par_peak_1_l = faustDsp.getParameter("peak_1_l");
    par_peak_2_b = faustDsp.getParameter("peak_2_b");
    par_peak_2_f = faustDsp.getParameter("peak_2_f");
    par_peak_2_l = faustDsp.getParameter("peak_2_l");
    par_peak_3_b = faustDsp.getParameter("peak_3_b");
    par_peak_3_f = faustDsp.getParameter("peak_3_f");
    par_peak_3_l = faustDsp.getParameter("peak_3_l");
    par_peak_4_b = faustDsp.getParameter("peak_4_b");
    par_peak_4_f = faustDsp.getParameter("peak_4_f");
    par_peak_4_l = faustDsp.getParameter("peak_4_l");
    par_peak_5_b = faustDsp.getParameter("peak_5_b");
    par_peak_5_f = faustDsp.getParameter("peak_5_f");
    par_peak_5_l = faustDsp.getParameter("peak_5_l");
    par_peak_6_b = faustDsp.getParameter("peak_6_b");
    par_peak_6_f = faustDsp.getParameter("peak_6_f");
    par_peak_6_l = faustDsp.getParameter("peak_6_l");
    par_peak_7_b = faustDsp.getParameter("peak_7_b");
    par_peak_7_f = faustDsp.getParameter("peak_7_f");
    par_peak_7_l = faustDsp.getParameter("peak_7_l");
    par_peak_8_b = faustDsp.getParameter("peak_8_b");
    par_peak_8_f = faustDsp.getParameter("peak_8_f");
    par_peak_8_l = faustDsp.getParameter("peak_8_l");
    par_peak_9_b = faustDsp.getParameter("peak_9_b");
    par_peak_9_f = faustDsp.getParameter("peak_9_f");
    par_peak_9_l = faustDsp.getParameter("peak_9_l");
    par_scoop_b = faustDsp.getParameter("scoop_b");
    par_scoop_f = faustDsp.getParameter("scoop_f");
    par_scoop_l = faustDsp.getParameter("scoop_l");
    par_shelf_f = faustDsp.getParameter("shelf_f");
    par_shelf_l = faustDsp.getParameter("shelf_l");
  }

  ~Cabinet() = default;

  void reset()
  {
    faustDsp.instanceClear();
    zeroParameters();
  }

  void prepare(int sampleRate)
  {
    faustDsp.init(sampleRate);
    zeroParameters();
  }

  void process(int count, FAUSTFLOAT** buffer)
  {
    faustDsp.compute(count, buffer, buffer);
  }

  inline void set_brightness(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_brightness = x;
  }
  inline void set_distance(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_distance = x;
  }
  inline void set_dynamic(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_dynamic = uscale(x, 0.0f, 1.0f);
  }
  inline void set_dynamic_level(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_dynamic_level = ulscale(x, 0.5f, 2.0f);
  }
  inline void set_hp_f(FAUSTFLOAT x)
  {
    x += 1.110027e+02f;
    *par_hp_f = x;
  }
  inline void set_lp_f(FAUSTFLOAT x)
  {
    x += 1.099876e+04f;
    *par_lp_f = x;
  }
  inline void set_offset(FAUSTFLOAT x)
  {
    x += 1.937125e+01f;
    *par_offset = x;
  }
  inline void set_peak_10_b(FAUSTFLOAT x)
  {
    x += 1.000935e+03f;
    *par_peak_10_b = x;
  }
  inline void set_peak_10_f(FAUSTFLOAT x)
  {
    x += 4.900000e+03f;
    *par_peak_10_f = x;
  }
  inline void set_peak_10_l(FAUSTFLOAT x)
  {
    x += -4.999868e+00f;
    *par_peak_10_l = x;
  }
  inline void set_peak_1_b(FAUSTFLOAT x)
  {
    x += 2.993194e+01f;
    *par_peak_1_b = x;
  }
  inline void set_peak_1_f(FAUSTFLOAT x)
  {
    x += 6.000000e+02f;
    *par_peak_1_f = x;
  }
  inline void set_peak_1_l(FAUSTFLOAT x)
  {
    x += -2.500029e+01f;
    *par_peak_1_l = x;
  }
  inline void set_peak_2_b(FAUSTFLOAT x)
  {
    x += 1.000074e+02f;
    *par_peak_2_b = x;
  }
  inline void set_peak_2_f(FAUSTFLOAT x)
  {
    x += 6.400000e+02f;
    *par_peak_2_f = x;
  }
  inline void set_peak_2_l(FAUSTFLOAT x)
  {
    x += 1.597118e+01f;
    *par_peak_2_l = x;
  }
  inline void set_peak_3_b(FAUSTFLOAT x)
  {
    x += 4.999785e+01f;
    *par_peak_3_b = x;
  }
  inline void set_peak_3_f(FAUSTFLOAT x)
  {
    x += 1.120000e+03f;
    *par_peak_3_f = x;
  }
  inline void set_peak_3_l(FAUSTFLOAT x)
  {
    x += -1.507867e+01f;
    *par_peak_3_l = x;
  }
  inline void set_peak_4_b(FAUSTFLOAT x)
  {
    x += 1.599996e+02f;
    *par_peak_4_b = x;
  }
  inline void set_peak_4_f(FAUSTFLOAT x)
  {
    x += 1.180000e+03f;
    *par_peak_4_f = x;
  }
  inline void set_peak_4_l(FAUSTFLOAT x)
  {
    x += 8.909575e+00f;
    *par_peak_4_l = x;
  }
  inline void set_peak_5_b(FAUSTFLOAT x)
  {
    x += 7.901408e+02f;
    *par_peak_5_b = x;
  }
  inline void set_peak_5_f(FAUSTFLOAT x)
  {
    x += 1.660000e+03f;
    *par_peak_5_f = x;
  }
  inline void set_peak_5_l(FAUSTFLOAT x)
  {
    x += -3.531785e+00f;
    *par_peak_5_l = x;
  }
  inline void set_peak_6_b(FAUSTFLOAT x)
  {
    x += 3.309512e+02f;
    *par_peak_6_b = x;
  }
  inline void set_peak_6_f(FAUSTFLOAT x)
  {
    x += 3.050000e+03f;
    *par_peak_6_f = x;
  }
  inline void set_peak_6_l(FAUSTFLOAT x)
  {
    x += -9.011323e+00f;
    *par_peak_6_l = x;
  }
  inline void set_peak_7_b(FAUSTFLOAT x)
  {
    x += 7.499988e+02f;
    *par_peak_7_b = x;
  }
  inline void set_peak_7_f(FAUSTFLOAT x)
  {
    x += 3.800000e+03f;
    *par_peak_7_f = x;
  }
  inline void set_peak_7_l(FAUSTFLOAT x)
  {
    x += -5.999392e+00f;
    *par_peak_7_l = x;
  }
  inline void set_peak_8_b(FAUSTFLOAT x)
  {
    x += 1.999953e+02f;
    *par_peak_8_b = x;
  }
  inline void set_peak_8_f(FAUSTFLOAT x)
  {
    x += 7.200000e+03f;
    *par_peak_8_f = x;
  }
  inline void set_peak_8_l(FAUSTFLOAT x)
  {
    x += -2.999869e+01f;
    *par_peak_8_l = x;
  }
  inline void set_peak_9_b(FAUSTFLOAT x)
  {
    x += 7.500001e+02f;
    *par_peak_9_b = x;
  }
  inline void set_peak_9_f(FAUSTFLOAT x)
  {
    x += 2.300000e+03f;
    *par_peak_9_f = x;
  }
  inline void set_peak_9_l(FAUSTFLOAT x)
  {
    x += 3.000100e+00f;
    *par_peak_9_l = x;
  }
  inline void set_scoop_b(FAUSTFLOAT x)
  {
    x += 2.799926e+03f;
    *par_scoop_b = x;
  }
  inline void set_scoop_f(FAUSTFLOAT x)
  {
    x += 9.509019e+02f;
    *par_scoop_f = x;
  }
  inline void set_scoop_l(FAUSTFLOAT x)
  {
    x += -1.529571e+01f;
    *par_scoop_l = x;
  }
  inline void set_shelf_f(FAUSTFLOAT x)
  {
    x += 5.600000e+03f;
    *par_shelf_f = x;
  }
  inline void set_shelf_l(FAUSTFLOAT x)
  {
    x += -3.002781e+01f;
    *par_shelf_l = x;
  }

private:
  CabinetFaust faustDsp;

  FAUSTFLOAT* par_brightness = nullptr;
  FAUSTFLOAT* par_distance = nullptr;
  FAUSTFLOAT* par_dynamic = nullptr;
  FAUSTFLOAT* par_dynamic_level = nullptr;
  FAUSTFLOAT* par_hp_f = nullptr;
  FAUSTFLOAT* par_lp_f = nullptr;
  FAUSTFLOAT* par_offset = nullptr;
  FAUSTFLOAT* par_peak_10_b = nullptr;
  FAUSTFLOAT* par_peak_10_f = nullptr;
  FAUSTFLOAT* par_peak_10_l = nullptr;
  FAUSTFLOAT* par_peak_1_b = nullptr;
  FAUSTFLOAT* par_peak_1_f = nullptr;
  FAUSTFLOAT* par_peak_1_l = nullptr;
  FAUSTFLOAT* par_peak_2_b = nullptr;
  FAUSTFLOAT* par_peak_2_f = nullptr;
  FAUSTFLOAT* par_peak_2_l = nullptr;
  FAUSTFLOAT* par_peak_3_b = nullptr;
  FAUSTFLOAT* par_peak_3_f = nullptr;
  FAUSTFLOAT* par_peak_3_l = nullptr;
  FAUSTFLOAT* par_peak_4_b = nullptr;
  FAUSTFLOAT* par_peak_4_f = nullptr;
  FAUSTFLOAT* par_peak_4_l = nullptr;
  FAUSTFLOAT* par_peak_5_b = nullptr;
  FAUSTFLOAT* par_peak_5_f = nullptr;
  FAUSTFLOAT* par_peak_5_l = nullptr;
  FAUSTFLOAT* par_peak_6_b = nullptr;
  FAUSTFLOAT* par_peak_6_f = nullptr;
  FAUSTFLOAT* par_peak_6_l = nullptr;
  FAUSTFLOAT* par_peak_7_b = nullptr;
  FAUSTFLOAT* par_peak_7_f = nullptr;
  FAUSTFLOAT* par_peak_7_l = nullptr;
  FAUSTFLOAT* par_peak_8_b = nullptr;
  FAUSTFLOAT* par_peak_8_f = nullptr;
  FAUSTFLOAT* par_peak_8_l = nullptr;
  FAUSTFLOAT* par_peak_9_b = nullptr;
  FAUSTFLOAT* par_peak_9_f = nullptr;
  FAUSTFLOAT* par_peak_9_l = nullptr;
  FAUSTFLOAT* par_scoop_b = nullptr;
  FAUSTFLOAT* par_scoop_f = nullptr;
  FAUSTFLOAT* par_scoop_l = nullptr;
  FAUSTFLOAT* par_shelf_f = nullptr;
  FAUSTFLOAT* par_shelf_l = nullptr;

  void zeroParameters()
  {
    set_brightness(0.0f);
    set_distance(0.0f);
    set_dynamic(0.0f);
    set_dynamic_level(0.0f);
    set_hp_f(0.0f);
    set_lp_f(0.0f);
    set_offset(0.0f);
    set_peak_10_b(0.0f);
    set_peak_10_f(0.0f);
    set_peak_10_l(0.0f);
    set_peak_1_b(0.0f);
    set_peak_1_f(0.0f);
    set_peak_1_l(0.0f);
    set_peak_2_b(0.0f);
    set_peak_2_f(0.0f);
    set_peak_2_l(0.0f);
    set_peak_3_b(0.0f);
    set_peak_3_f(0.0f);
    set_peak_3_l(0.0f);
    set_peak_4_b(0.0f);
    set_peak_4_f(0.0f);
    set_peak_4_l(0.0f);
    set_peak_5_b(0.0f);
    set_peak_5_f(0.0f);
    set_peak_5_l(0.0f);
    set_peak_6_b(0.0f);
    set_peak_6_f(0.0f);
    set_peak_6_l(0.0f);
    set_peak_7_b(0.0f);
    set_peak_7_f(0.0f);
    set_peak_7_l(0.0f);
    set_peak_8_b(0.0f);
    set_peak_8_f(0.0f);
    set_peak_8_l(0.0f);
    set_peak_9_b(0.0f);
    set_peak_9_f(0.0f);
    set_peak_9_l(0.0f);
    set_scoop_b(0.0f);
    set_scoop_f(0.0f);
    set_scoop_l(0.0f);
    set_shelf_f(0.0f);
    set_shelf_l(0.0f);
  }
};

#undef uscale
#undef ulscale

#endif