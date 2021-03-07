#ifndef __PushPullAmP_H__
#define __PushPullAmP_H__

#include <algorithm>
#include <cmath>
#include <random>

#define USCALE(x, l, u) (x + 1.0f) / 2.0f * (u - l) + l
#define ULSCALE(x, l, u) std::exp(USCALE(x, std::log(l), std::log(u)));
#define IUSCALE(x, l, u) (x - l) / (u - l) * 2.0f - 1.0f
#define IULSCALE(x, l, u) IUSCALE(std::log(x), std::log(l), std::log(u))

#define LINEAR2DB(x) 20.0f * std::log10(x)
#define DB2LINEAR(x) std::pow(10.0f, (x) / 20.0f)

#define NUM_SWEEP_BINS 10

#include "Cabinet.h"
#include "TetrodeGrid.h"
#include "TetrodePlate.h"
#include "ToneStack.h"
#include "Triode.h"

inline void scaleBuffer(int count, FAUSTFLOAT** buffer, FAUSTFLOAT scale)
{
  for (int i = 0; i < count; i++) buffer[0][i] *= scale;
}

inline FAUSTFLOAT scaleBuffer(
    int count,
    FAUSTFLOAT** buffer,
    FAUSTFLOAT fromScale,
    FAUSTFLOAT toScale,
    FAUSTFLOAT dbPerSecond,
    int sampleRate)
{
  FAUSTFLOAT scale = fromScale;
  const FAUSTFLOAT dbPerSample = (toScale > fromScale ? 1.0f : -1.0f)
      * dbPerSecond / (FAUSTFLOAT)sampleRate;
  const FAUSTFLOAT factor = std::pow(10.0f, dbPerSample / 20.0f);
  const FAUSTFLOAT upper = toScale > fromScale ? toScale : fromScale;
  const FAUSTFLOAT lower = toScale <= fromScale ? toScale : fromScale;
  for (int i = 0; i < count; i++)
  {
    buffer[0][i] *= scale;
    scale = std::min(upper, std::max(lower, scale * factor));
  }
  return scale;
}

inline float interp1d(float x, float xl, float xh, const float* edges, size_t n)
{
  const float xu = (x - xl) / (xh - xl);
  const float xbin = xu * (float)n;
  size_t i = 0;
  if (xbin >= n)
    i = n - 1;
  else if (xbin >= 0)
    i = (int)(xbin);
  const float m = edges[i + 1] - edges[i];
  const float b = edges[i];
  return m * (xbin - i) + b;
}

class PreAmp
{
public:
  PreAmp() : rngDist(-0.2f, 0.2f) {}
  ~PreAmp() = default;

  void reset()
  {
    for (size_t i = 0; i < MAX_STAGES; i++) triode[i].reset();
    resetParameters();
  }

  void prepare(int sampleRate)
  {
    for (size_t i = 0; i < MAX_STAGES; i++) triode[i].prepare(sampleRate);
    resetParameters();
  }

  void process(int count, FAUSTFLOAT** buffer)
  {
    // drive below 0.5 can cause high-pitch artifacts due to float precision
    // loss, so bound it here to be sure and to allow UI to maintain old range
    scaleBuffer(count, buffer, drive < 0.5f ? 0.5f : drive);
    const int numStagesLow = (int)std::floor(numStagesActive());
    const int numStagesHigh = (int)std::ceil(numStagesActive());
    const float stageMix = numStagesActive() - numStagesLow;

    for (size_t i = 0; i < numStagesHigh; i++)
    {
      triode[i].set_overhead(i > 0 ? overhead : 1.0f);
      triode[i].set_mix(i < numStagesLow ? 1.0f : stageMix);
      triode[i].process(count, buffer);
    }

    // scale up to nominal output rms
    scaleBuffer(count, buffer, triodeScale);
  }

  inline void set_hp_freq(FAUSTFLOAT x)
  {
    auto rng = std::minstd_rand(rngSeed + 1);
    for (size_t i = 0; i < MAX_STAGES; i++)
      triode[i].set_hp_freq(x + rngDist(rng));
  }
  inline void set_grid_tau(FAUSTFLOAT x)
  {
    auto rng = std::minstd_rand(rngSeed + 2);
    for (size_t i = 0; i < MAX_STAGES; i++) triode[i].set_tau(x + rngDist(rng));
  }
  inline void set_grid_ratio(FAUSTFLOAT x)
  {
    for (size_t i = 0; i < MAX_STAGES; i++) triode[i].set_ratio(x);
  }
  inline void set_grid_level(FAUSTFLOAT x)
  {
    for (size_t i = 0; i < MAX_STAGES; i++) triode[i].set_level(x);
  }
  inline void set_grid_clip(FAUSTFLOAT x)
  {
    auto rng = std::minstd_rand(rngSeed + 4);
    for (size_t i = 0; i < MAX_STAGES; i++)
      triode[i].set_grid_clip(x + rngDist(rng));
  }
  inline void set_plate_bias(FAUSTFLOAT x)
  {
    auto rng = std::minstd_rand(rngSeed + 5);
    for (size_t i = 0; i < MAX_STAGES; i++)
      triode[i].set_bias(x + rngDist(rng));
  }
  inline void set_plate_scale(FAUSTFLOAT x)
  {
    for (size_t i = 0; i < MAX_STAGES; i++) triode[i].set_scale(x);
  }
  inline void set_plate_clip(FAUSTFLOAT x)
  {
    auto rng = std::minstd_rand(rngSeed + 7);
    for (size_t i = 0; i < MAX_STAGES; i++)
      triode[i].set_plate_clip(x + rngDist(rng));
  }
  inline void set_plate_drift_level(FAUSTFLOAT x)
  {
    auto rng = std::minstd_rand(rngSeed + 8);
    for (size_t i = 0; i < MAX_STAGES; i++)
      triode[i].set_drift_level(x + rngDist(rng));
  }
  inline void set_plate_drift_tau(FAUSTFLOAT x)
  {
    auto rng = std::minstd_rand(rngSeed + 9);
    for (size_t i = 0; i < MAX_STAGES; i++)
      triode[i].set_drift_tau(x + rngDist(rng));
  }
  inline void set_plate_comp_ratio(FAUSTFLOAT x)
  {
    for (size_t i = 0; i < MAX_STAGES; i++) triode[i].set_comp_ratio(x);
  }
  inline void set_plate_comp_level(FAUSTFLOAT x)
  {
    auto rng = std::minstd_rand(rngSeed + 10);
    for (size_t i = 0; i < MAX_STAGES; i++)
      triode[i].set_comp_level(x + rngDist(rng));
  }
  inline void set_plate_comp_offset(FAUSTFLOAT x)
  {
    for (size_t i = 0; i < MAX_STAGES; i++) triode[i].set_comp_offset(x);
  }

  inline void set_num_stages(FAUSTFLOAT x) { numStages = x; }
  inline void set_drive(FAUSTFLOAT x) { drive = ULSCALE(x, 1e-1f, 2e3f); }
  inline float get_drive() const { return IULSCALE(drive, 1e-1f, 2e3f); }
  inline void set_overhead(FAUSTFLOAT x) { overhead = ULSCALE(x, 1e-1f, 1e1f); }
  inline float get_overhead() const { return IULSCALE(overhead, 1e-1f, 1e1f); }

private:
  static const size_t MAX_STAGES = 5;

  FAUSTFLOAT drive = 0.0f;
  FAUSTFLOAT overhead = 0.0f;
  float numStages = 0.0f;

  Triode triode[MAX_STAGES];

  std::uniform_real_distribution<float> rngDist;
  unsigned int rngSeed = 123;

  const float triodeScale = 3.501334e+01f;

  inline float numStagesActive() const
  {
    float num = numStages > MAX_STAGES ? (float)MAX_STAGES : numStages;
    // keep always some amount of stage 2 mixed in to smooth out artifcats
    // that can arise from a single stage
    num = num < 1.1f ? 1.1f : num;
    return num;
  }

  void resetParameters()
  {
    set_num_stages(3.0f);
    set_drive(0.0f);
    set_overhead(0.0f);

    // must manually set to get the randomization
    set_hp_freq(0.0f);
    set_grid_tau(0.0f);
    set_grid_ratio(0.0f);
    set_grid_clip(0.0f);
    set_plate_bias(0.0f);
    set_plate_clip(0.0f);
    set_plate_drift_level(0.0f);
    set_plate_drift_tau(0.0f);
    set_plate_comp_ratio(0.0f);
    set_plate_comp_level(0.0f);
    set_plate_comp_offset(0.0f);

    for (size_t i = 0; i < MAX_STAGES; i++)
    {
      triode[i].set_mix(1.0f);
      triode[i].set_overhead(1.0f);
      triode[i].set_unscale(triodeScale);
    }
  }
};

class PowerAmp
{
public:
  PowerAmp() = default;
  ~PowerAmp() = default;

  void reset()
  {
    tetrodeGrid.reset();
    tetrodePlate.reset();
    resetParameters();
  }

  void prepare(int sampleRate)
  {
    tetrodeGrid.prepare(sampleRate);
    tetrodePlate.prepare(sampleRate);
    resetParameters();
  }

  void process(int count, FAUSTFLOAT** buffer)
  {
    scaleBuffer(count, buffer, drive);
    tetrodeGrid.process(count, buffer);
    tetrodePlate.process(count, buffer);
  }

  inline void set_hp_freq(FAUSTFLOAT x) { tetrodeGrid.set_hp_freq(x); }
  inline void set_grid_tau(FAUSTFLOAT x) { tetrodeGrid.set_tau(x); }
  inline void set_grid_ratio(FAUSTFLOAT x) { tetrodeGrid.set_ratio(x); }
  inline void set_plate_comp_depth(FAUSTFLOAT x)
  {
    tetrodePlate.set_comp_depth(x);
  }
  inline void set_plate_sag_tau(FAUSTFLOAT x) { tetrodePlate.set_sag_tau(x); }
  inline void set_plate_sag_toggle(FAUSTFLOAT x)
  {
    tetrodePlate.set_sag_toggle(x);
  }
  inline void set_plate_sag_depth(FAUSTFLOAT x)
  {
    tetrodePlate.set_sag_depth(x);
  }
  inline void set_plate_sag_ratio(FAUSTFLOAT x)
  {
    tetrodePlate.set_sag_ratio(x);
  }
  inline void set_plate_sag_factor(FAUSTFLOAT x)
  {
    tetrodePlate.set_sag_factor(x);
  }
  inline void set_plate_sag_onset(FAUSTFLOAT x)
  {
    tetrodePlate.set_sag_onset(x);
  }

  inline void set_drive(FAUSTFLOAT x) { drive = ULSCALE(x, 3e1f, 3e4f); }
  inline float get_drive() const { return IULSCALE(drive, 3e1f, 3e4f); }

private:
  TetrodeGrid tetrodeGrid;
  TetrodePlate tetrodePlate;

  FAUSTFLOAT drive;

  void resetParameters() { set_drive(0.0f); }
};

class PushPullAmp
{
public:
  PushPullAmp() = default;
  ~PushPullAmp() = default;

  void reset()
  {
    preAmp.reset();
    toneStack.reset();
    powerAmp.reset();
    cabinet.reset();
    resetParameters();
  }

  void prepare(int sampleRate)
  {
    preAmp.prepare(sampleRate);
    toneStack.prepare(sampleRate);
    powerAmp.prepare(sampleRate);
    cabinet.prepare(sampleRate);
    resetParameters();
  }

  void process(int count, FAUSTFLOAT** buffer)
  {
    scaleBuffer(count, buffer, DB2LINEAR(inputLevel));

    preAmp.process(count, buffer);

    const float preAmpScale = interp1d(
        preAmp.get_drive(),
        -1.0f,
        1.0f,
        preAmpSweepScales,
        (size_t)NUM_SWEEP_BINS);
    const float preAmpTarget = 3.228806e+01f;

    const float toneStackScale = 1.0f / 5.302220e-01f;

    toneStack.process(count, buffer);

    scaleBuffer(count, buffer, toneStackScale * preAmpScale * preAmpTarget);

    powerAmp.process(count, buffer);

    scaleBuffer(count, buffer, 1.0f / preAmpTarget);

    const float cabinetScale = cabinetOn ? 1.0f / 2.821151e+00f : 1.0f;
    if (cabinetOn) cabinet.process(count, buffer);

    const float powerAmpScale = interp1d(
        powerAmp.get_drive(),
        -1.0f,
        1.0f,
        powerAmpSweepScales,
        (size_t)NUM_SWEEP_BINS);

    const float outputScale = DB2LINEAR(outputLevel);
    scaleBuffer(count, buffer, powerAmpScale * cabinetScale * outputScale);
  }

  inline void set_triode_num_stages(FAUSTFLOAT x) { preAmp.set_num_stages(x); }
  inline void set_triode_overhead(FAUSTFLOAT x) { preAmp.set_overhead(x); }
  inline void set_triode_hp_freq(FAUSTFLOAT x) { preAmp.set_hp_freq(x); }
  inline void set_triode_grid_tau(FAUSTFLOAT x) { preAmp.set_grid_tau(x); }
  inline void set_triode_grid_ratio(FAUSTFLOAT x) { preAmp.set_grid_ratio(x); }
  inline void set_triode_grid_level(FAUSTFLOAT x) { preAmp.set_grid_level(x); }
  inline void set_triode_grid_clip(FAUSTFLOAT x) { preAmp.set_grid_clip(x); }
  inline void set_triode_plate_bias(FAUSTFLOAT x) { preAmp.set_plate_bias(x); }
  inline void set_triode_plate_comp_ratio(FAUSTFLOAT x)
  {
    preAmp.set_plate_comp_ratio(x);
  }
  inline void set_triode_plate_comp_level(FAUSTFLOAT x)
  {
    preAmp.set_plate_comp_level(x);
  }
  inline void set_triode_plate_comp_offset(FAUSTFLOAT x)
  {
    preAmp.set_plate_comp_offset(x);
  }
  inline void set_triode_drive(FAUSTFLOAT x) { preAmp.set_drive(x); }

  inline void set_tetrode_hp_freq(FAUSTFLOAT x) { powerAmp.set_hp_freq(x); }
  inline void set_tetrode_grid_tau(FAUSTFLOAT x) { powerAmp.set_grid_tau(x); }
  inline void set_tetrode_grid_ratio(FAUSTFLOAT x)
  {
    powerAmp.set_grid_ratio(x);
  }

  inline void set_tetrode_plate_comp_depth(FAUSTFLOAT x)
  {
    powerAmp.set_plate_comp_depth(x);
  }
  inline void set_tetrode_plate_sag_tau(FAUSTFLOAT x)
  {
    powerAmp.set_plate_sag_tau(x);
  }
  inline void set_tetrode_plate_sag_toggle(FAUSTFLOAT x)
  {
    powerAmp.set_plate_sag_toggle(x);
  }
  inline void set_tetrode_plate_sag_depth(FAUSTFLOAT x)
  {
    powerAmp.set_plate_sag_depth(x);
  }
  inline void set_tetrode_plate_sag_ratio(FAUSTFLOAT x)
  {
    powerAmp.set_plate_sag_ratio(x);
  }
  inline void set_tetrode_plate_sag_factor(FAUSTFLOAT x)
  {
    powerAmp.set_plate_sag_factor(x);
  }
  inline void set_tetrode_plate_sag_onset(FAUSTFLOAT x)
  {
    powerAmp.set_plate_sag_onset(x);
  }
  inline void set_tetrode_drive(FAUSTFLOAT x) { powerAmp.set_drive(x); }

  inline void set_tonestack_bass(FAUSTFLOAT x) { toneStack.set_bass(x); }
  inline void set_tonestack_mids(FAUSTFLOAT x) { toneStack.set_mids(x); }
  inline void set_tonestack_treble(FAUSTFLOAT x) { toneStack.set_treble(x); }
  inline void set_tonestack_presence(FAUSTFLOAT x)
  {
    toneStack.set_presence(x);
  }
  inline void set_tonestack_selection(FAUSTFLOAT x)
  {
    toneStack.set_selection(x);
  }

  inline void set_cabinet_brightness(FAUSTFLOAT x)
  {
    cabinet.set_brightness(x);
  }
  inline void set_cabinet_distance(FAUSTFLOAT x) { cabinet.set_distance(x); }
  inline void set_cabinet_dynamic(FAUSTFLOAT x) { cabinet.set_dynamic(x); }
  inline void set_cabinet_dynamic_level(FAUSTFLOAT x)
  {
    cabinet.set_dynamic_level(x);
  }

  inline void set_input_level(FAUSTFLOAT x)
  {
    inputLevel = USCALE(x, -35.0f, 35.0f);
  }
  inline void set_output_level(FAUSTFLOAT x)
  {
    outputLevel = USCALE(x, -35.0f, 35.0f);
  }
  inline void set_cabinet_on(bool x) { cabinetOn = x; }

  float get_tetrode_drive() const { return powerAmp.get_drive(); }
  float get_triode_drive() const { return preAmp.get_drive(); }

private:
  PreAmp preAmp;
  ToneStack toneStack;
  PowerAmp powerAmp;
  Cabinet cabinet;

  FAUSTFLOAT inputLevel = 0.0f;
  FAUSTFLOAT outputLevel = 0.0f;
  bool cabinetOn = true;

  const float preAmpSweepScales[NUM_SWEEP_BINS + 1] = {
      4.487723e-03f,
      3.323652e-03f,
      1.606984e-03f,
      6.262808e-04f,
      2.279750e-04f,
      7.981833e-05f,
      2.851987e-05f,
      1.361236e-05f,
      1.102758e-05f,
      1.065001e-05f,
      9.354531e-06f};

  const float powerAmpSweepScales[NUM_SWEEP_BINS + 1] = {
      8.572513e-01f,
      4.489064e-01f,
      2.412848e-01f,
      1.278829e-01f,
      6.694987e-02f,
      4.332370e-02f,
      3.664175e-02f,
      3.589781e-02f,
      3.497760e-02f,
      3.313881e-02f,
      3.162063e-02f};

  void resetParameters()
  {
    set_input_level(0.0f);
    set_output_level(0.0f);
    set_cabinet_on(true);
  }
};

#undef USCALE
#undef ULSCALE
#undef IUSCALE
#undef IULSCALE
#undef LINEAR2DB
#undef DB2LINEAR

#endif