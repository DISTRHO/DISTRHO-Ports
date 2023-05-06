/*
------------------------------------------------------------------------------

Plugin processor
================
A Juce class containing the Roth-AIR plugin parameters and audio processing instructions

By Daniel Rothmann

Provided under GNU General Public license:
http://www.gnu.org/licenses/

------------------------------------------------------------------------------
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "JuceHeader.h"
#include "Compressor.h"
#include "DspFilters/Dsp.h"
#include "WaveShaper.h"

//==============================================================================
class AirAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    AirAudioProcessor();
    ~AirAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet) override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//=============================================================================
	// Declare some getters and setters
	double getCrossFreq();
	void setCrossFreq(double newFreq);

	double getDryWet();
	void setDryWet(double newDryWet);

	double getThreshold();
	void setThreshold(double newThreshold);

	double getRatio();
	void updateRatio();

	double getHpGain();
	void setHpGain(double newHpGain);

	double getHpPreGain();
	void setHpPreGain(double newHpPreGain);

	double getSatAmt();
	void setSatAmt(double newSatAmt);

	double getAirRatioAmt();
	void setAirRatioAmt(double newAirRatioAmt);

	double getAirGainAmt();
	void setAirGainAmt(double newAirGainAmt);

	double getAirSatAmt();
	void setAirSatAmt(double newAirSatAmt);

	double getAirAmt();
	void setAirAmt(double newAirAmt);

	double getTempGainReduction();

	// Declare automatable parameters that host will understand
	AudioParameterFloat* crossFreq;
	AudioParameterFloat* dryWet;
	AudioParameterFloat* cThreshold;
	AudioParameterFloat* hpGain;
	AudioParameterFloat* airAmt;

private:
	// Declare buffers
	AudioSampleBuffer hpBuffer;
	AudioSampleBuffer lpBuffer;

	// Declare filters
	std::unique_ptr<Dsp::Filter> lp;
	std::unique_ptr<Dsp::Filter> hp;
	Dsp::Params filterParams;

	// Declare compressor
	std::unique_ptr<Compressor> pCompressor;

	// Declare waveshaper
	std::unique_ptr<WaveShaper> waveShaper;

	// Declare parameters
	double cRatio = 1.0;
	double hpPreGain = 0;
	double satAmt = 0.0;

	double airRatioAmt = 3.0;
	double airGainAmt = 0.0;
	double airSatAmt = 0.5;

	double tempRatio;

	// Declare program name
	String defProgName = "Roth-AIR";

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AirAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
