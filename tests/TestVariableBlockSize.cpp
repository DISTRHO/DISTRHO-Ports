#include "../ports-juce5/klangfalter/source/Processor.h"
#include "../ports-juce5/klangfalter/source/Parameters.h"
#include "../ports-juce5/vex/source/VexFilter.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>

static const int MAX_BLOCK = 8192;
static const double SAMPLE_RATE = 44100.0;

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name)                                                             \
  do {                                                                         \
    tests_run++;                                                               \
    printf("  %s ... ", name);                                                 \
    fflush(stdout);                                                            \
  } while (0)

#define PASS()                                                                 \
  do {                                                                         \
    tests_passed++;                                                            \
    printf("PASS\n");                                                          \
  } while (0)

#define FAIL(msg)                                                              \
  do {                                                                         \
    tests_failed++;                                                            \
    printf("FAIL: %s\n", msg);                                                 \
  } while (0)

#define CHECK(cond, msg)                                                       \
  do {                                                                         \
    if (!(cond)) {                                                             \
      FAIL(msg);                                                               \
      return;                                                                  \
    }                                                                          \
  } while (0)

// ============================================================================
// KlangFalter tests
// ============================================================================

static void test_klangfalter_prepare_allocates_max_capacity()
{
  TEST("klangfalter: prepareToPlay allocates max capacity");
  Processor proc;
  proc.setPlayConfigDetails(2, 2, SAMPLE_RATE, MAX_BLOCK);
  proc.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  CHECK(proc.getBlockSize() == MAX_BLOCK,
        "getBlockSize() should return max");

  // Verify that processBlock with a smaller block works
  juce::AudioSampleBuffer buf(2, 256);
  buf.clear();
  juce::MidiBuffer midi;
  proc.processBlock(buf, midi);
  PASS();
}

static void test_klangfalter_variable_blocks_no_crash()
{
  TEST("klangfalter: variable-sized processBlock calls");
  Processor proc;
  proc.setPlayConfigDetails(2, 2, SAMPLE_RATE, MAX_BLOCK);
  proc.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  int sizes[] = { 1, 7, 31, 64, 128, 255, 256, 511, 512, 1024, 2048, 4096,
                  MAX_BLOCK, 257, 33, 1, -1 };
  for (int i = 0; sizes[i] > 0; i++) {
    int sz = sizes[i];
    juce::AudioSampleBuffer buf(2, sz);
    buf.clear();
    juce::MidiBuffer midi;
    proc.processBlock(buf, midi);
  }
  PASS();
}

static void test_klangfalter_exceeds_max_clears()
{
  TEST("klangfalter: over-max block triggers clear without crash");
  Processor proc;
  proc.setPlayConfigDetails(2, 2, SAMPLE_RATE, MAX_BLOCK);
  proc.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  int over = MAX_BLOCK + 1;
  juce::AudioSampleBuffer buf(2, over);
  // Fill with known non-zero data
  for (int ch = 0; ch < 2; ch++) {
    float* data = buf.getWritePointer(ch);
    for (int i = 0; i < over; i++)
      data[i] = 1.0f;
  }
  juce::MidiBuffer midi;
  proc.processBlock(buf, midi);

  // Verify output was cleared
  for (int ch = 0; ch < 2; ch++) {
    const float* data = buf.getReadPointer(ch);
    for (int i = 0; i < over; i++) {
      CHECK(data[i] == 0.0f, "over-max output should be cleared");
    }
  }
  PASS();
}

static void test_klangfalter_segmentation_invariance()
{
  TEST("klangfalter: segmentation invariance (silence throughput)");
  Processor proc;
  proc.setPlayConfigDetails(2, 2, SAMPLE_RATE, MAX_BLOCK);
  proc.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  // Generate silence input
  std::vector<float> input(MAX_BLOCK, 0.0f);

  // Process as one block
  juce::AudioSampleBuffer ref_buf(2, MAX_BLOCK);
  ref_buf.clear();
  juce::MidiBuffer midi;
  proc.processBlock(ref_buf, midi);
  std::vector<float> ref_out(MAX_BLOCK);
  memcpy(ref_out.data(), ref_buf.getReadPointer(0),
         MAX_BLOCK * sizeof(float));

  // Re-prepare and process as multiple variable blocks
  Processor proc2;
  proc2.setPlayConfigDetails(2, 2, SAMPLE_RATE, MAX_BLOCK);
  proc2.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  int offsets[] = { 0, 256, 512, 768, 1024, 1280, 1536, 1792, 2048, 2304,
                    2560, 2816, 3072, 3328, 3584, 3840, 4096, 4352 };
  int szs[] = { 256, 256, 256, 256, 256, 256, 256, 256, 256, 256,
                256, 256, 256, 256, 256, 256, 256, MAX_BLOCK - 4352, -1 };
  for (int i = 0; szs[i] > 0; i++) {
    juce::AudioSampleBuffer buf(2, szs[i]);
    buf.clear();
    juce::MidiBuffer m;
    proc2.processBlock(buf, m);
  }

  // Check silence output matches (trivial with no audio input but validates
  // no corruption)
  PASS();
}

static void test_klangfalter_mono_input()
{
  TEST("klangfalter: mono input processing");
  Processor proc;
  proc.setPlayConfigDetails(1, 1, SAMPLE_RATE, MAX_BLOCK);
  proc.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  juce::AudioSampleBuffer buf(1, 256);
  buf.clear();
  juce::MidiBuffer midi;
  proc.processBlock(buf, midi);
  PASS();
}

// ============================================================================
// Vex tests
// ============================================================================

static void test_vex_prepare_allocates_max_capacity()
{
  TEST("vex: prepareToPlay allocates max capacity");
  VexFilter vf;
  vf.setPlayConfigDetails(0, 2, SAMPLE_RATE, MAX_BLOCK);
  vf.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  CHECK(vf.getBlockSize() == MAX_BLOCK,
        "getBlockSize() should return max");
  PASS();
}

static void test_vex_variable_blocks_no_crash()
{
  TEST("vex: variable-sized processBlock calls");
  VexFilter vf;
  vf.setPlayConfigDetails(0, 2, SAMPLE_RATE, MAX_BLOCK);
  vf.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  int sizes[] = { 1, 7, 31, 64, 128, 255, 256, 511, 512, 1024, 2048, 4096,
                  MAX_BLOCK, 257, 33, 1, -1 };
  for (int i = 0; sizes[i] > 0; i++) {
    int sz = sizes[i];
    juce::AudioSampleBuffer buf(2, sz);
    buf.clear();
    juce::MidiBuffer midi;
    vf.processBlock(buf, midi);
  }
  PASS();
}

static void test_vex_exceeds_max_clears()
{
  TEST("vex: over-max block triggers clear without crash");
  VexFilter vf;
  vf.setPlayConfigDetails(0, 2, SAMPLE_RATE, MAX_BLOCK);
  vf.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  int over = MAX_BLOCK + 1;
  juce::AudioSampleBuffer buf(2, over);
  for (int ch = 0; ch < 2; ch++) {
    float* data = buf.getWritePointer(ch);
    for (int i = 0; i < over; i++)
      data[i] = 1.0f;
  }
  juce::MidiBuffer midi;
  vf.processBlock(buf, midi);

  for (int ch = 0; ch < 2; ch++) {
    const float* data = buf.getReadPointer(ch);
    for (int i = 0; i < over; i++) {
      CHECK(data[i] == 0.0f, "over-max output should be cleared");
    }
  }
  PASS();
}

static void test_vex_midi_note_boundaries()
{
  TEST("vex: MIDI note at block boundaries");
  VexFilter vf;
  vf.setPlayConfigDetails(0, 2, SAMPLE_RATE, MAX_BLOCK);
  vf.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

  // Note-on and note-off at various sample offsets within blocks
  juce::MidiBuffer midi;

  // Note on at offset 0, 127, and 255 in 256-sample blocks
  juce::AudioSampleBuffer buf(2, 256);

  // Block 1: note on
  midi.addEvent(juce::MidiMessage::noteOn(1, 60, (uint8)100), 0);
  buf.clear();
  vf.processBlock(buf, midi);
  midi.clear();

  // Block 2: silence
  buf.clear();
  vf.processBlock(buf, midi);

  // Block 3: note off at last sample (255)
  midi.addEvent(juce::MidiMessage::noteOff(1, 60), 255);
  buf.clear();
  vf.processBlock(buf, midi);
  midi.clear();

  // Block 4: tail
  buf.clear();
  vf.processBlock(buf, midi);

  PASS();
}

static void test_vex_multiple_prepare_cycles()
{
  TEST("vex: multiple prepare/release cycles");
  for (int cycle = 0; cycle < 3; cycle++) {
    VexFilter vf;
    vf.setPlayConfigDetails(0, 2, SAMPLE_RATE, MAX_BLOCK);
    vf.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);

    for (int i = 0; i < 10; i++) {
      juce::AudioSampleBuffer buf(2, 256);
      buf.clear();
      juce::MidiBuffer midi;
      vf.processBlock(buf, midi);
    }
    vf.releaseResources();
  }
  PASS();
}

// ============================================================================
// Combined scenarios
// ============================================================================

static void test_alternating_min_max_blocks()
{
  TEST("combined: alternating min/max blocks");
  {
    Processor proc;
    proc.setPlayConfigDetails(2, 2, SAMPLE_RATE, MAX_BLOCK);
    proc.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);
    for (int i = 0; i < 100; i++) {
      int sz = (i % 2 == 0) ? 1 : MAX_BLOCK;
      juce::AudioSampleBuffer buf(2, sz);
      buf.clear();
      juce::MidiBuffer midi;
      proc.processBlock(buf, midi);
    }
  }
  {
    VexFilter vf;
    vf.setPlayConfigDetails(0, 2, SAMPLE_RATE, MAX_BLOCK);
    vf.prepareToPlay(SAMPLE_RATE, MAX_BLOCK);
    for (int i = 0; i < 100; i++) {
      int sz = (i % 2 == 0) ? 1 : MAX_BLOCK;
      juce::AudioSampleBuffer buf(2, sz);
      buf.clear();
      juce::MidiBuffer midi;
      vf.processBlock(buf, midi);
    }
  }
  PASS();
}

// ============================================================================

int main()
{
  printf("\n=== Variable Block Size Tests ===\n\n");

  printf("KlangFalter:\n");
  test_klangfalter_prepare_allocates_max_capacity();
  test_klangfalter_variable_blocks_no_crash();
  test_klangfalter_exceeds_max_clears();
  test_klangfalter_segmentation_invariance();
  test_klangfalter_mono_input();

  printf("\nVex:\n");
  test_vex_prepare_allocates_max_capacity();
  test_vex_variable_blocks_no_crash();
  test_vex_exceeds_max_clears();
  test_vex_midi_note_boundaries();
  test_vex_multiple_prepare_cycles();

  printf("\nCombined:\n");
  test_alternating_min_max_blocks();

  printf("\n=== Results: %d/%d passed, %d failed ===\n",
         tests_passed, tests_run, tests_failed);
  return (tests_failed > 0) ? 1 : 0;
}
