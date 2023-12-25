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

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AirAudioProcessor::AirAudioProcessor()
{
    
    // Instanciate plugin parameters
	addParameter(crossFreq = new AudioParameterFloat("crossFreq", // param ID
													"Crossover Freq", // param name
													NormalisableRange<float> (1000.0f, 8000.0f), // param range
													4000.0f)); // default value
	addParameter(dryWet = new AudioParameterFloat("dryWet",
													"Dry / Wet",
													NormalisableRange<float>(0.0f, 1.0f),
													0.6f));
	addParameter(cThreshold = new AudioParameterFloat("threshold",
													"Threshold",
													NormalisableRange<float>(-40.0f, 0.0f),
													-18.0f));
	addParameter(hpGain = new AudioParameterFloat("gain",
													"Gain",
													NormalisableRange<float>(0.25f, 2.0f),
													1.0f));
	addParameter(airAmt = new AudioParameterFloat("air",
													"Air Amt",
													NormalisableRange<float>(0.0f, 1.0f),
													0.0f));

    // Initialize non-parameter variables
	hpPreGain = 0.0;
	cRatio = 1.0;
	airRatioAmt = 3.0;
	airGainAmt = 0.0;
	airSatAmt = 0.5;
    
    // Instanciate filters
    hp = std::make_unique<Dsp::SmoothedFilterDesign <Dsp::Bessel::Design::HighPass<4>, 2, Dsp::DirectFormII>>(1024);
    lp = std::make_unique<Dsp::SmoothedFilterDesign <Dsp::Bessel::Design::LowPass<4>, 2, Dsp::DirectFormII>>(1024);

    // Instanciate waveshaper
    waveShaper = std::make_unique<WaveShaper>(2);
    waveShaper->setAmount(0.0);

    // Instanciate compressor
    pCompressor = std::make_unique<Compressor>(2, 44100);
	
}

AirAudioProcessor::~AirAudioProcessor()
{
}

//==============================================================================
const String AirAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AirAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AirAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double AirAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AirAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AirAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AirAudioProcessor::setCurrentProgram (int index)
{
}

const String AirAudioProcessor::getProgramName (int index)
{
    return defProgName;
}

void AirAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AirAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..

	// Initialize filter parameters
	filterParams[0] = sampleRate; // Set sample rate
	filterParams[1] = 2; // Set order
	filterParams[2] = *crossFreq; // Set center freq
	filterParams[3] = 1600; // Set bandwidth

	// Set filter parameters
	lp->setParams(filterParams);
	hp->setParams(filterParams);

	// Initialize compressor
    pCompressor->setSampleRate(sampleRate);
	pCompressor->setThreshold(*cThreshold);
	pCompressor->setRatio(1.0 + (airRatioAmt * *airAmt));
    pCompressor->setAttackRate(0);
	pCompressor->setReleaseRate(75);
	pCompressor->setMakeupGain(0);
    pCompressor->resetSideChain();

	// Initialize waveshaper
	waveShaper->setAmount(0.0);
    
	// Preallocate buffer memory
	hpBuffer.setSize(2, samplesPerBlock);
	lpBuffer.setSize(2, samplesPerBlock);
}

void AirAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AirAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet)
{
    // Reject any bus arrangements that are not compatible with your plugin

    const int numChannels = preferredSet.size();

   #if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
   #elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
   #else
    if (numChannels != 1 && numChannels != 2)
        return false;

    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
   #endif

    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif

void AirAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
	int numSamples = buffer.getNumSamples();
    
    jassert((totalNumInputChannels == 1) || (totalNumInputChannels == 2));
	
    // Clear buffer in case of garbage
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, numSamples);
    
	// Copy buffer data into cross filter buffers
    if(totalNumInputChannels == 2)
    {
        // If we're running stereo (2,2), copy each channel of the input buffer
		hpBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
		hpBuffer.copyFrom(1, 0, buffer, 1, 0, numSamples);
		lpBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
		lpBuffer.copyFrom(1, 0, buffer, 1, 0, numSamples);
    }
    else if (totalNumInputChannels == 1)
    {
        // If we're running mono (1,1) copy input channel into both output channels (workaround)
        hpBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        hpBuffer.copyFrom(1, 0, buffer, 0, 0, numSamples);
        lpBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        lpBuffer.copyFrom(1, 0, buffer, 0, 0, numSamples);
    }

	// Set filter crossover param every block
	filterParams[2] = *crossFreq; // Set center freq
	lp->setParams(filterParams);
	hp->setParams(filterParams);

	// Apply the filters to respective buffers
	hp->process(numSamples, hpBuffer.getArrayOfWritePointers());
	lp->process(numSamples, lpBuffer.getArrayOfWritePointers());

	// Set waveshaper params and apply it
	waveShaper->setAmount(*airAmt * airSatAmt);
	waveShaper->processBlock(hpBuffer);
    
	// Set compressor parameters and apply it
	tempRatio = 1.0 + (airRatioAmt * *airAmt);
	pCompressor->setThreshold(*cThreshold);
	pCompressor->setRatio(tempRatio);
	pCompressor->processBlock(hpBuffer);

	// Apply post gain to air buffer
	hpBuffer.applyGain(*hpGain + (*airAmt * airGainAmt));

	// Mix the buffers together and apply Dry/Wet for each channel
	for (int channel = 0; channel < totalNumInputChannels; ++channel)
	{
		// Add lpBuffer to hpBuffer (after processing hpBuffer)
		hpBuffer.addFrom(channel, 0, lpBuffer, channel, 0, numSamples);

		// WET GAIN
		hpBuffer.applyGain(channel, 0, numSamples, *dryWet);
		// DRY GAIN
		buffer.applyGain(channel, 0, numSamples, 1.0 - *dryWet);

		// Add wet signal to buffer
		buffer.addFrom(channel, 0, hpBuffer, channel, 0, numSamples);
    }

}

//==============================================================================
bool AirAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AirAudioProcessor::createEditor()
{
    return new AirAudioProcessorEditor (*this);
}

//==============================================================================
void AirAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

	// Create XML element
	XmlElement xml("AIRPLUGINSETTINGS");

	// Store values of all paramters, using their param ID as XML attribute
	auto params = getParameters();
	for (int i = 0; i < params.size(); ++i)
	{
		if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (params.getUnchecked(i)))
		{
			xml.setAttribute(p->paramID, p->getValue());
		}
	}
	
	// Copy the XML to binary to be returned later
	copyXmlToBinary(xml, destData);
}

void AirAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

	// Retrieves XML from binary
	auto xmlState = std::make_unique<XmlElement>(*getXmlFromBinary(data, sizeInBytes));

	// Make sure it's not null
	if (xmlState != nullptr)
	{
		// Make sure it's the right type of XML object
		if (xmlState->hasTagName("AIRPLUGINSETTINGS"))
		{
			// Reload the parameters from XML attributes to param with matching ID
			auto params = getParameters();
			for (int i = 0; i < params.size(); ++i)
			{
				if (AudioProcessorParameterWithID* p = dynamic_cast<AudioProcessorParameterWithID*> (params.getUnchecked(i)))
				{
					p->setValue((float)xmlState->getDoubleAttribute(p->paramID, p->getValue()));
				}
			}
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AirAudioProcessor();
}

//=============================================================================
// Getters and setters for the dsp params
double AirAudioProcessor::getCrossFreq()
{
	return *crossFreq;
}

void AirAudioProcessor::setCrossFreq(double newFreq)
{
	*crossFreq = newFreq;
}

double AirAudioProcessor::getDryWet()
{
	return *dryWet;
}

void AirAudioProcessor::setDryWet(double newDryWet)
{
	*dryWet = newDryWet;
}

double AirAudioProcessor::getThreshold()
{
	return *cThreshold;
}

void AirAudioProcessor::setThreshold(double newThreshold)
{
	*cThreshold = newThreshold;
}

double AirAudioProcessor::getRatio()
{
	return cRatio;
}

void AirAudioProcessor::updateRatio()
{
	cRatio = 1 + (airRatioAmt * *airAmt);
}

double AirAudioProcessor::getHpGain()
{
	return *hpGain;
}

void AirAudioProcessor::setHpGain(double newHpGain)
{
	*hpGain = newHpGain;
}

double AirAudioProcessor::getHpPreGain()
{
	return hpPreGain;
}

void AirAudioProcessor::setHpPreGain(double newHpPreGain)
{
	hpPreGain = newHpPreGain;
}

double AirAudioProcessor::getSatAmt()
{
	return satAmt;
}

void AirAudioProcessor::setSatAmt(double newSatAmt)
{
	satAmt = newSatAmt;
}

double AirAudioProcessor::getAirRatioAmt()
{
	return airRatioAmt;
}

void AirAudioProcessor::setAirRatioAmt(double newAirRatioAmt)
{
	airRatioAmt = newAirRatioAmt;
}

double AirAudioProcessor::getAirGainAmt()
{
	return airGainAmt;
}

void AirAudioProcessor::setAirGainAmt(double newAirGainAmt)
{
	airGainAmt = newAirGainAmt;
}

double AirAudioProcessor::getAirSatAmt()
{
	return airSatAmt;
}

void AirAudioProcessor::setAirSatAmt(double newAirSatAmt)
{
	airSatAmt = newAirSatAmt;
}

double AirAudioProcessor::getAirAmt()
{
	return *airAmt;
}

void AirAudioProcessor::setAirAmt(double newAirAmt)
{
	*airAmt = newAirAmt;
}

double AirAudioProcessor::getTempGainReduction()
{
	return pCompressor->getTempGainReduction();
}
