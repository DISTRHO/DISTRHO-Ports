#ifndef __faust2hpp_TetrodeGrid_H__
#define __faust2hpp_TetrodeGrid_H__

#include <cmath>

#define uscale(x, l, u) (x + 1.0f) / 2.0f * (u - l) + l;
#define ulscale(x, l, u) \
  std::exp((x + 1.0f) / 2.0f * (std::log(u) - std::log(l)) + std::log(l));

#include "TetrodeGridFaust.h"

class TetrodeGrid
{
public:
  TetrodeGrid()
  {
    faustDsp.buildUserInterface(&faustDsp);
    par_cap = faustDsp.getParameter("cap");
    par_hp_freq = faustDsp.getParameter("hp_freq");
    par_level = faustDsp.getParameter("level");
    par_offset1 = faustDsp.getParameter("offset1");
    par_offset2 = faustDsp.getParameter("offset2");
    par_ratio = faustDsp.getParameter("ratio");
    par_tau = faustDsp.getParameter("tau");
    par_taus = faustDsp.getParameter("taus");
  }

  ~TetrodeGrid() = default;

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

  inline void set_cap(FAUSTFLOAT x)
  {
    x += 2.465974e+00f;
    *par_cap = ulscale(x, 1e-1f, 1e+1f);
  }
  inline void set_hp_freq(FAUSTFLOAT x)
  {
    x += -7.134431e+00f;
    *par_hp_freq = ulscale(x, 1e-1f, 1e+2f);
  }
  inline void set_level(FAUSTFLOAT x)
  {
    x += 4.248251e-01f;
    *par_level = uscale(x, -100.0f, +100.0f);
  }
  inline void set_offset1(FAUSTFLOAT x)
  {
    x += 4.783875e-02f;
    *par_offset1 = uscale(x, 0.0f, 500.0f);
  }
  inline void set_offset2(FAUSTFLOAT x)
  {
    x += -2.047840e+00f;
    *par_offset2 = uscale(x, 0.0f, 500.0f);
  }
  inline void set_ratio(FAUSTFLOAT x)
  {
    x += 6.316976e-01f;
    *par_ratio = ulscale(x, 1e-2f, 1e+2f);
  }
  inline void set_tau(FAUSTFLOAT x)
  {
    x += -6.193241e-01f;
    *par_tau = ulscale(x, 1e-4f, 1e0f);
  }
  inline void set_taus(FAUSTFLOAT x)
  {
    x += -4.923804e-05f;
    *par_taus = ulscale(x, 1e-4f, 1e0f);
  }

private:
  TetrodeGridFaust faustDsp;

  FAUSTFLOAT* par_cap = nullptr;
  FAUSTFLOAT* par_hp_freq = nullptr;
  FAUSTFLOAT* par_level = nullptr;
  FAUSTFLOAT* par_offset1 = nullptr;
  FAUSTFLOAT* par_offset2 = nullptr;
  FAUSTFLOAT* par_ratio = nullptr;
  FAUSTFLOAT* par_tau = nullptr;
  FAUSTFLOAT* par_taus = nullptr;

  void zeroParameters()
  {
    set_cap(0.0f);
    set_hp_freq(0.0f);
    set_level(0.0f);
    set_offset1(0.0f);
    set_offset2(0.0f);
    set_ratio(0.0f);
    set_tau(0.0f);
    set_taus(0.0f);
  }
};

#undef uscale
#undef ulscale

#endif