#ifndef __faust2hpp_Triode_H__
#define __faust2hpp_Triode_H__

#include <cmath>

#define uscale(x, l, u) (x + 1.0f) / 2.0f * (u - l) + l;
#define ulscale(x, l, u) \
  std::exp((x + 1.0f) / 2.0f * (std::log(u) - std::log(l)) + std::log(l));

#include "TriodeFaust.h"

class Triode
{
public:
  Triode()
  {
    faustDsp.buildUserInterface(&faustDsp);
    par_bias = faustDsp.getParameter("bias");
    par_bias_corner = faustDsp.getParameter("bias_corner");
    par_cap = faustDsp.getParameter("cap");
    par_comp_cap = faustDsp.getParameter("comp_cap");
    par_comp_corner = faustDsp.getParameter("comp_corner");
    par_comp_depth = faustDsp.getParameter("comp_depth");
    par_comp_level = faustDsp.getParameter("comp_level");
    par_comp_offset = faustDsp.getParameter("comp_offset");
    par_comp_ratio = faustDsp.getParameter("comp_ratio");
    par_comp_tau = faustDsp.getParameter("comp_tau");
    par_drift_depth = faustDsp.getParameter("drift_depth");
    par_drift_level = faustDsp.getParameter("drift_level");
    par_drift_tau = faustDsp.getParameter("drift_tau");
    par_grid_clip = faustDsp.getParameter("grid_clip");
    par_grid_corner = faustDsp.getParameter("grid_corner");
    par_hp_freq = faustDsp.getParameter("hp_freq");
    par_level = faustDsp.getParameter("level");
    par_mix = faustDsp.getParameter("mix");
    par_overhead = faustDsp.getParameter("overhead");
    par_plate_clip = faustDsp.getParameter("plate_clip");
    par_plate_corner = faustDsp.getParameter("plate_corner");
    par_ratio = faustDsp.getParameter("ratio");
    par_scale = faustDsp.getParameter("scale");
    par_smooth = faustDsp.getParameter("smooth");
    par_tau = faustDsp.getParameter("tau");
    par_unscale = faustDsp.getParameter("unscale");
  }

  ~Triode() = default;

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

  inline void set_bias(FAUSTFLOAT x)
  {
    x += 2.368186e+00f;
    *par_bias = uscale(x, -100.0f, +100.0f);
  }
  inline void set_bias_corner(FAUSTFLOAT x)
  {
    x += 4.893700e-01f;
    *par_bias_corner = ulscale(x, 1e-1f, 1e+3f);
  }
  inline void set_cap(FAUSTFLOAT x)
  {
    x += 1.739277e+00f;
    *par_cap = ulscale(x, 1e-1f, 1e+1f);
  }
  inline void set_comp_cap(FAUSTFLOAT x)
  {
    x += 2.536884e+00f;
    *par_comp_cap = ulscale(x, 1e-1f, 1e+1f);
  }
  inline void set_comp_corner(FAUSTFLOAT x)
  {
    x += 1.507149e-02f;
    *par_comp_corner = ulscale(x, 1e-1f, 1e+2f);
  }
  inline void set_comp_depth(FAUSTFLOAT x)
  {
    x += -1.602357e-01f;
    *par_comp_depth = ulscale(x, 1e-1f, 1e+1f);
  }
  inline void set_comp_level(FAUSTFLOAT x)
  {
    x += -1.019789e+00f;
    *par_comp_level = uscale(x, -100.0f, +100.0f);
  }
  inline void set_comp_offset(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_comp_offset = uscale(x, -100.0f, +100.0f);
  }
  inline void set_comp_ratio(FAUSTFLOAT x)
  {
    x += 3.138570e+00f;
    *par_comp_ratio = ulscale(x, 1e0f, 1e+1f);
  }
  inline void set_comp_tau(FAUSTFLOAT x)
  {
    x += -1.100164e+00f;
    *par_comp_tau = ulscale(x, 1e-3f, 1e0f);
  }
  inline void set_drift_depth(FAUSTFLOAT x)
  {
    x += -8.249800e-01f;
    *par_drift_depth = ulscale(x, 1e-1f, 1e+1f);
  }
  inline void set_drift_level(FAUSTFLOAT x)
  {
    x += 1.018694e-01f;
    *par_drift_level = uscale(x, -100.0f, +100.0f);
  }
  inline void set_drift_tau(FAUSTFLOAT x)
  {
    x += -1.088060e+00f;
    *par_drift_tau = ulscale(x, 1e-3f, 1e0f);
  }
  inline void set_grid_clip(FAUSTFLOAT x)
  {
    x += 4.078275e-01f;
    *par_grid_clip = uscale(x, 0.0f, 5.0f);
  }
  inline void set_grid_corner(FAUSTFLOAT x)
  {
    x += 1.150913e-02f;
    *par_grid_corner = uscale(x, 0.0f, 5.0f);
  }
  inline void set_hp_freq(FAUSTFLOAT x)
  {
    x += 4.522232e-01f;
    *par_hp_freq = ulscale(x, 1e-1f, 1e+2f);
  }
  inline void set_level(FAUSTFLOAT x)
  {
    x += 3.357531e-01f;
    *par_level = uscale(x, -5.0f, +5.0f);
  }
  inline void set_mix(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_mix = x;
  }
  inline void set_overhead(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_overhead = x;
  }
  inline void set_plate_clip(FAUSTFLOAT x)
  {
    x += -1.075611e+00f;
    *par_plate_clip = uscale(x, -100.0f, +100.0f);
  }
  inline void set_plate_corner(FAUSTFLOAT x)
  {
    x += -2.447939e-02f;
    *par_plate_corner = ulscale(x, 1e-2f, 1e+2f);
  }
  inline void set_ratio(FAUSTFLOAT x)
  {
    x += 1.208988e+00f;
    *par_ratio = ulscale(x, 1e-1f, 1e+4f);
  }
  inline void set_scale(FAUSTFLOAT x)
  {
    x += 1.524156e+00f;
    *par_scale = ulscale(x, 1e-1f, 1e+1f);
  }
  inline void set_smooth(FAUSTFLOAT x)
  {
    x += 1.529416e+00f;
    *par_smooth = ulscale(x, 1e-5f, 1e+1f);
  }
  inline void set_tau(FAUSTFLOAT x)
  {
    x += -1.357776e+00f;
    *par_tau = ulscale(x, 1e-6f, 1e-3f);
  }
  inline void set_unscale(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_unscale = x;
  }

private:
  TriodeFaust faustDsp;

  FAUSTFLOAT* par_bias = nullptr;
  FAUSTFLOAT* par_bias_corner = nullptr;
  FAUSTFLOAT* par_cap = nullptr;
  FAUSTFLOAT* par_comp_cap = nullptr;
  FAUSTFLOAT* par_comp_corner = nullptr;
  FAUSTFLOAT* par_comp_depth = nullptr;
  FAUSTFLOAT* par_comp_level = nullptr;
  FAUSTFLOAT* par_comp_offset = nullptr;
  FAUSTFLOAT* par_comp_ratio = nullptr;
  FAUSTFLOAT* par_comp_tau = nullptr;
  FAUSTFLOAT* par_drift_depth = nullptr;
  FAUSTFLOAT* par_drift_level = nullptr;
  FAUSTFLOAT* par_drift_tau = nullptr;
  FAUSTFLOAT* par_grid_clip = nullptr;
  FAUSTFLOAT* par_grid_corner = nullptr;
  FAUSTFLOAT* par_hp_freq = nullptr;
  FAUSTFLOAT* par_level = nullptr;
  FAUSTFLOAT* par_mix = nullptr;
  FAUSTFLOAT* par_overhead = nullptr;
  FAUSTFLOAT* par_plate_clip = nullptr;
  FAUSTFLOAT* par_plate_corner = nullptr;
  FAUSTFLOAT* par_ratio = nullptr;
  FAUSTFLOAT* par_scale = nullptr;
  FAUSTFLOAT* par_smooth = nullptr;
  FAUSTFLOAT* par_tau = nullptr;
  FAUSTFLOAT* par_unscale = nullptr;

  void zeroParameters()
  {
    set_bias(0.0f);
    set_bias_corner(0.0f);
    set_cap(0.0f);
    set_comp_cap(0.0f);
    set_comp_corner(0.0f);
    set_comp_depth(0.0f);
    set_comp_level(0.0f);
    set_comp_offset(0.0f);
    set_comp_ratio(0.0f);
    set_comp_tau(0.0f);
    set_drift_depth(0.0f);
    set_drift_level(0.0f);
    set_drift_tau(0.0f);
    set_grid_clip(0.0f);
    set_grid_corner(0.0f);
    set_hp_freq(0.0f);
    set_level(0.0f);
    set_mix(0.0f);
    set_overhead(0.0f);
    set_plate_clip(0.0f);
    set_plate_corner(0.0f);
    set_ratio(0.0f);
    set_scale(0.0f);
    set_smooth(0.0f);
    set_tau(0.0f);
    set_unscale(0.0f);
  }
};

#undef uscale
#undef ulscale

#endif