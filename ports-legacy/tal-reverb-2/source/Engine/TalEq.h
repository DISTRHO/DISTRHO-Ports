/*
	==============================================================================
	This file is part of Tal-Reverb by Patrick Kunz.

	Copyright(c) 2005-2009 Patrick Kunz, TAL
	Togu Audio Line, Inc.
	http://kunz.corrupt.ch

	This file may be licensed under the terms of of the
	GNU General Public License Version 2 (the ``GPL'').

	Software distributed under the License is distributed
	on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
	express or implied. See the GPL for the specific language
	governing rights and limitations.

	You should have received a copy of the GPL along with this
	program. If not, go to http://www.gnu.org/licenses/gpl.html
	or write to the Free Software Foundation, Inc.,  
	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
	==============================================================================
 */


#if !defined(__TalEq_h)
#define __TalEq_h

#include "HighShelf.h"
#include "LowShelf.h"
#include "PeakEq.h"

class TalEq 
{
private:
	HighShelf *highShelf;
	LowShelf *lowShelf;
	PeakEq *peakEq;

	float lowShelfGain;
	float highShelfGain;

	float lowShelfFrequency;
	float highShelfFrequency;

	float peakFrequency;
	float peakGain;

	AudioUtils audioUtils;

public:
	TalEq(float sampleRate) 
	{
		initialize(sampleRate);
	}

	~TalEq()
	{
	}

#ifdef __MOD_DEVICES__
	void setLowShelfGain(float lowShelfGain)
	{
        lowShelfGain = (lowShelfGain + 18.0 ) / 36.0;
		this->lowShelfGain = lowShelfGain;
	}

	void setHighShelfGain(float highShelfGain)
	{
        highShelfGain = (highShelfGain + 18.0 ) / 36.0;
		this->highShelfGain = highShelfGain;
	}

	void setPeakGain(float peakGain)
	{
        peakGain = (peakGain + 18.0 ) / 36.0;
		this->peakGain = peakGain;
	}

	void setLowShelfFrequency(float lowShelfFrequency)
	{
		this->lowShelfFrequency = lowShelfFrequency;
	}

	void setHighShelfFrequency(float highShelfFrequency)
	{
		this->highShelfFrequency = highShelfFrequency;
	}

	void setPeakFrequency(float peakFrequency)
	{
		this->peakFrequency = peakFrequency;
	}
#else
	void setLowShelfGain(float lowShelfGain)
	{
		this->lowShelfGain = lowShelfGain * 0.5f;
	}

	void setHighShelfGain(float highShelfGain)
	{
		this->highShelfGain = highShelfGain * 0.5f;
	}

	void setPeakGain(float peakGain)
	{
		this->peakGain = peakGain * 0.5f;
	}

	void setLowShelfFrequency(float lowShelfFrequency)
	{
		this->lowShelfFrequency = audioUtils.getLogScaledFrequency(lowShelfFrequency);
	}

	void setHighShelfFrequency(float highShelfFrequency)
	{
		this->highShelfFrequency = audioUtils.getLogScaledFrequency(highShelfFrequency);
	}

	void setPeakFrequency(float peakFrequency)
	{
		this->peakFrequency = audioUtils.getLogScaledFrequency(peakFrequency);
	}
#endif

	void initialize(float sampleRate)
	{
		highShelf = new HighShelf(sampleRate, 18);
		lowShelf = new LowShelf(sampleRate, 18);
		peakEq = new PeakEq(sampleRate, 18);

		lowShelfGain = 0.5f;
		highShelfGain = 0.5f;
		peakGain = 0.5f;

		lowShelfFrequency = 1000.0f;
		highShelfFrequency = 1000.0f;
		peakFrequency = 1000.0f;
	}

	void process(float *sample) 
	{
		highShelf->tick(sample, highShelfFrequency, 1.05f, highShelfGain); // 0..0.5
		lowShelf->tick(sample, lowShelfFrequency, 1.05f, lowShelfGain); // 0..0.5
		peakEq->tick(sample, peakFrequency, 1.05f, peakGain); // 0..0.5
	}
};
#endif

