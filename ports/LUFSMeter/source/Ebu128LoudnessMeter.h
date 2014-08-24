/*
 ===============================================================================
 
 Ebu128LoudnessMeter.h
 
 
 This file is part of the LUFS Meter audio measurement plugin.
 Copyright 2011-14 by Klangfreund, Samuel Gaehwiler.
 
 -------------------------------------------------------------------------------
 
 The LUFS Meter can be redistributed and/or modified under the terms of the GNU 
 General Public License Version 2, as published by the Free Software Foundation.
 A copy of the license is included with these source files. It can also be found
 at www.gnu.org/licenses.
 
 The LUFS Meter is distributed WITHOUT ANY WARRANTY.
 See the GNU General Public License for more details.
 
 -------------------------------------------------------------------------------
 
 To release a closed-source product which uses the LUFS Meter or parts of it,
 a commercial license is available. Visit www.klangfreund.com/lufsmeter for more
 information.
 
 ===============================================================================
 */


#ifndef __EBU128_LOUDNESS_METER__
#define __EBU128_LOUDNESS_METER__

#include "MacrosAndJuceHeaders.h"
#include "filters/SecondOrderIIRFilter.h"
#include <vector>

using std::vector;

/**
 Measures the loudness of an audio stream.
 
 The loudness is measured according to the documents
 (List)
 - EBU - R 128
 - ITU 1770 Rev 2
 - EBU - Tech 3341 (EBU mode metering)
 - EBU - Tech 3342 (LRA, loudness range)
 - EBU - Tech 3343
 */
class Ebu128LoudnessMeter     //: public AudioProcessor
{
public:
    Ebu128LoudnessMeter ();
    ~Ebu128LoudnessMeter ();
    
    // --------- AudioProcessor methods ---------
//    const String getName ();
    
    /**
     @param sampleRate
     @param numberOfChannels
     @param estimatedSamplesPerBlock
     @param expectedRequestRate         Assumption about how many times 
        a second the measurement values will be requested. Internally,
        this will be changed to a multiple of 10 because exactly every
        0.1 second a gating block needs to be measured (for the
        integrated loudness measurement).
     */
    void prepareToPlay (double sampleRate,
                        int numberOfInputChannels,
                        int estimatedSamplesPerBlock, 
                        int expectedRequestRate);
    
    void processBlock (AudioSampleBuffer &buffer);
    
    const Array<float>& getShortTermLoudnessForIndividualChannels();
    float getShortTermLoudness();
    
    const Array<float>& getMomentaryLoudnessForIndividualChannels();
    float getMomentaryLoudness();
    
    float getIntegratedLoudness();
    
    float getLoudnessRangeStart();
    float getLoudnessRangeEnd();
    float getLoudnessRange();
    
    void reset();
    
private:
    
    /** The buffer given to processBlock() will be copied to this buffer, such
     that the filtering and squaring won't affect the audio output. I.e. thanks
     to this, the audio will pass through this without getting changed.
     
     It also stores the number of input channels implicitely, set in prepareToPlay.
     */
    AudioSampleBuffer bufferForMeasurement;
    
    SecondOrderIIRFilter preFilter;
    SecondOrderIIRFilter revisedLowFrequencyBCurveFilter;
    
    int numberOfBins;
    int numberOfSamplesPerBin;
    int numberOfSamplesInAllBins;
    int numberOfBinsToCover400ms;
    int numberOfSamplesIn400ms;
    
    int numberOfBinsToCover100ms;
    int numberOfBinsSinceLastGateMeasurementForI;
    int millisecondsSinceLastGateMeasurementForLRA;
    
    /**
     After the samples are filtered and squared, they need to be
     accumulated.
     For the measurement of the short-term loudness, the previous
     3 seconds of samples need to be accumulated. For the other
     measurements shorter windows are used.
     
     This task could be solved using a ring buffer capable of 
     holding 3 seconds of (multi-channel) audio and accumulate the
     samples every time the GUI wants to display the measurement.
     
     But it's easier on the CPU and needs far less memory if not
     all samples are stored but only the summation of all the
     samples in a 1/(GUI refresh rate) s - e.g. 1/20 s - time window.
     If we then need to determine the sum of all samples of the previous
     3s, we only have to accumulate 60 values.
     The downside of this method is that the displayed measurement
     has an accuracy jitter of max. e.g. 1/20 s. I guess this isn't
     an issue since GUI elements are refreshed asynchron anyway.
     Just to be clear, the accuracy of the measurement isn't effected.
     But if you ask for a measurement at time t, it will be the
     accurate measurement at time t - dt, where dt \in e.g. [0, 1/20s].
     */
    OwnedArray <OwnedArray<double>> bin;
    
    int currentBin;
    int numberOfSamplesInTheCurrentBin;
    
    /*
     The average of the filtered and squared samples of the last
     3 seconds.
     A value for each channel.
     */
    OwnedArray<double> averageOfTheLast3s;
    
    /*
     The average of the filtered and squared samples of the last
     400 milliseconds.
     A value for each channel.
     */
    OwnedArray<double> averageOfTheLast400ms;
    
    Array<double> channelWeighting;
    
    Array<float> shortTermLoudness;
    Array<float> momentaryLoudness;
    
    /** If there is no signal at all, the methods getShortTermLoudness() and
     getMomentaryLoudness() would perform a log10(0) which would result in
     a value -nan. To avoid this, the return value of this methods will be
     set to minimalReturnValue.
     */
    static const float minimalReturnValue;
    
    /** A gated window needs to be bigger than this value to
     contribute to the calculation of the relative threshold.
     
     absoluteThreshold = Gamma_a = -70 LUFS.
     */
    static const double absoluteThreshold;
    
    int numberOfBlocksToCalculateRelativeThreshold;
    double sumOfAllBlocksToCalculateRelativeThreshold;
    double relativeThreshold;
    
    int LRAnumberOfBlocksToCalculateRelativeThreshold;
    double LRAsumOfAllBlocksToCalculateRelativeThreshold;
    double LRArelativeThreshold;
    
    
    static const double lowestBlockLoudnessToConsider;
    static const double highestBlockLoudnessToConsider;
    /** The difference in loudness between two adjacent bins in the histogram.
     Measured in LU;
     */
    static const double histogramLoudnessStepSize;
    
    /** Two adjacant bins in the histogram also correspond to two weighted
     sums. They are related by this factor.
     WeightedSum2 = WeightedSum1 * histogramWeightedSumStepFactor.
     */
    static const double histogramWeightedSumStepFactor;
    
    /** Storage for the loudnesses of all 400ms blocks since the last reset.
     
     Because the relative threshold varies and all blocks with a loudness
     bigger than the relative threshold are needed to calculate the gated
     loudness (integrated loudness), it is mandatory to keep track of all
     block loudnesses.
     */
    vector<int> histogramOfBlockLoudness;
    
    /** The main loudness value of interest.
     
     It is the return value of getIntegratedLoudness().
     */
    float integratedLoudness;
    
    /** Like histogramOfBlockLoudness, but for the measurement of the
     loudness range.
     
     The histogramOfBlockLoudness can't be used simultaneous for the
     loudness range, because the measurement blocks for the loudness
     range need to be of length 3s. Vs 400ms.
     */
    vector<int> LRAhistogramOfBlockLoudness;
    
    /**
     The return values for the corresponding get member functions.
     
     Loudness Range = loudnessRangeEnd - loudnessRangeStart.
     */
    float loudnessRangeStart;
    float loudnessRangeEnd;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Ebu128LoudnessMeter);
    
};

#endif // __EBU128_LOUDNESS_METER__
