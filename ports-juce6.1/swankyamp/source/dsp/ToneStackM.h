#ifndef __faust2hpp_ToneStackM_H__
#define __faust2hpp_ToneStackM_H__

#include <cmath>

#define uscale(x, l, u) (x + 1.0f) / 2.0f * (u - l) + l;
#define ulscale(x, l, u) \
  std::exp((x + 1.0f) / 2.0f * (std::log(u) - std::log(l)) + std::log(l));

#include "ToneStackMFaust.h"

class ToneStackM
{
public:
  ToneStackM()
  {
    faustDsp.buildUserInterface(&faustDsp);
    par_bass = faustDsp.getParameter("bass");
    par_mids = faustDsp.getParameter("mids");
    par_presence = faustDsp.getParameter("presence");
    par_treble = faustDsp.getParameter("treble");
  }

  ~ToneStackM() = default;

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

  inline void set_bass(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_bass = uscale(x, -1.0f, 1.0f);
  }
  inline void set_mids(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_mids = uscale(x, -1.0f, 1.0f);
  }
  inline void set_presence(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_presence = uscale(x, -1.0f, 1.0f);
  }
  inline void set_treble(FAUSTFLOAT x)
  {
    x += 0.000000e+00f;
    *par_treble = uscale(x, -1.0f, 1.0f);
  }

private:
  ToneStackMFaust faustDsp;

  FAUSTFLOAT* par_bass = nullptr;
  FAUSTFLOAT* par_mids = nullptr;
  FAUSTFLOAT* par_presence = nullptr;
  FAUSTFLOAT* par_treble = nullptr;

  void zeroParameters()
  {
    set_bass(0.0f);
    set_mids(0.0f);
    set_presence(0.0f);
    set_treble(0.0f);
  }
};

#undef uscale
#undef ulscale

#endif