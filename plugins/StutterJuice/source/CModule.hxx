#ifndef CMODULE_HXX_INCLUDED
#define CMODULE_HXX_INCLUDED

#include <time.h> //rand
#include <algorithm> //min


// -----------------------------------------------------------------------
// SubModules
class LPF
{
	private:
		float r; // reso - 0.1 - 1.4
		float f; // freq in Hz
		float a1, a2, a3, b1, b2;
		float in, in1, in2;
		float out, out1, out2;
		float c;
		float sampleRate;
		
		void compute() {
			c = 1.0 / tan(M_PI * f / sampleRate);
			a1 = 1.0 / (1.0 + r*c + c*c);
			a2 = 2 * a1;
			a3 = a1;
			b1 = 2.0 * (1.0 - c*c) * a1;
			b2 = (1.0 - r*c + c*c) * a1;
		}
	
	public:
	
		LPF() {
			r = 1.0f;
			f = 0.0f;
			a1=a2=a3=b1=b2=0.0f;
			in=in1=in2=out=out1=out2=0.0f;
		}
	
		void setFreq(float nFreq) {
			f = nFreq;
			if (f==0) {
				f=20;
			}
		}
		
		void setReso(float nReso) {
			r = nReso;
			if (r==0) {
				r = 0.1f;
			}
		}
		
		void setSampleRate(float nSampleRate) {
			sampleRate = nSampleRate;
		}
		
	
		float process(float sample) {
			compute();
			in = sample;
			out = a1 * in + a2 * in1 + a3 * in2 - b1*out1 - b2*out2;
			in2=in1;
			in1=in;
			out2=out1;
			out1 = out;
			
			return out;
		}


};

class HPF
{
	private:
		float r; // reso - 0.1 - 1.4
		float f; // freq in Hz
		float a1, a2, a3, b1, b2;
		float in, in1, in2;
		float out, out1, out2;
		float c;
		float sampleRate;
		
		void compute() {
			c = tan(M_PI * f / sampleRate);
			a1 = 1.0 / (1.0 + r*c + c*c);
			a2 = -2*a1;
			a3 = a1;
			b1 = 2.0 * (c*c - 1.0) * a1;
			b2 = (1.0 - r*c + c*c) * a1;
		}
	
	public:
	
		HPF() {
			r = 1.0f;
			f = 0.0f;
			a1=a2=a3=b1=b2=0.0f;
			in=in1=in2=out=out1=out2=0.0f;
		}
	
		void setFreq(float nFreq) {
			f = nFreq;
			if (f==0) {
				f=20;
			}
		}
		
		void setReso(float nReso) {
			r = nReso;
			if (r==0) {
				r = 0.1f;
			}
		}
		
		void setSampleRate(float nSampleRate) {
			sampleRate = nSampleRate;
		}
		
	
		float process(float sample) {
			compute();
			in = sample;
			out = a1 * in + a2 * in1 + a3 * in2 - b1*out1 - b2*out2;
			in2=in1;
			in1=in;
			out2=out1;
			out1 = out;
			
			return out;
		}


};

class CSHIFT {

	private:
	
		float fVec0[65536];
		float fRec0[2];
		int IOTA;
		float shiftAmt; //-12 .. 12 semi
		float windowSize; // 50 .. 10k samples
		float xfade; // 1 .. 10k samples
		float sampleRate;
		float output;
		
	public:
	
		CSHIFT() {
			IOTA = 0;
			for (int i = 0; i<65536; i++) {
				fVec0[i] = 0.0f;
			}
			for (int i = 0; i<2; i++) {
				fRec0[i] = 0.f;
			}
			
			shiftAmt = 0.1f;
			windowSize = 200.0f;
			xfade = 76.0f;
		}
		
		void setSampleRate(float nSampleRate) {
			sampleRate = nSampleRate;
		}
		
		void setWindowSize(float nWindowSize) {
			windowSize = nWindowSize;
		}
		
		void setXfade(float nXfade) {
			xfade = nXfade;
		}
		
		void setShiftAmt(float nShiftAmt) {
			shiftAmt = nShiftAmt;
		}
		
		float process(float audio) {
			float input0 = audio;
			float fSlow0 = windowSize;
			float fSlow1 = ((1.f + fSlow0) - powf(2.f, (0.0833333f * shiftAmt)));
			float fSlow2 = (1.f / xfade);
			float fSlow3 = (fSlow0 - 1.f);

			float fTemp0 = audio;
			fVec0[(IOTA & 65535)] = fTemp0;
			fRec0[0] = fmodf((fRec0[1] + fSlow1), fSlow0);
			int iTemp1 = int(fRec0[0]);
			int iTemp2 = (1 + iTemp1);
			float fTemp3 = std::min((fSlow2 * fRec0[0]), 1.f);
			float fTemp4 = (fRec0[0] + fSlow0);
			int iTemp5 = int(fTemp4);
			output = ((((fVec0[((IOTA - int((iTemp1 & 65535))) & 65535)] * (iTemp2 - fRec0[0])) + ((fRec0[0] - iTemp1) * fVec0[((IOTA - int((int(iTemp2) & 65535))) & 65535)])) * fTemp3) + (((fVec0[((IOTA - int((iTemp5 & 65535))) & 65535)] * (0.f - ((fRec0[0] + fSlow3) - float(iTemp5)))) + ((fTemp4 - float(iTemp5)) * fVec0[((IOTA - int((int((1 + iTemp5)) & 65535))) & 65535)])) * (1.f - fTemp3)));
			IOTA = (IOTA + 1);
			fRec0[1] = fRec0[0];
			
			return output;
		
		}

};

// -----------------------------------------------------------------------
// Modules

class CModule
{

public:
	CModule() {
		sampleRate = 0;
		params[0] = 0.5f;
		params[1] = 0.5f;
		params[2] = 0.5f;
		active = false;
		sinePos = 0;
		
	}
	
	virtual void setParam(float value, int index) {
		params[index] = value;
	}
	
	virtual void process(float &audioL, float &audioR) {}
	
	virtual void initBuffers() {}
	
	void setSampleRate(int nSR) {
		sampleRate = nSR;
	}
	
	void activate() {
		active = true;
	}
	
	void deactivate() {
		active = false;
	}
	
	virtual void setSinePos(float nSinePos) {
		sinePos = nSinePos;
	}
	
	virtual float getTempoDivider() {
		return 1.0f;
	}
	
	float getSinePos() {
		return sinePos;
	}
	
	float isActive() {
		return active;
	}
	
	void resetSinePos() {
		sinePos = 0;
	}
	
	float getOutputParam() {
		return (sinePos/(2*M_PI));
	}
	
protected:
	//vars
	float params[3];
	int sampleRate;
	bool active;//is open?
	
	float getSinePhase(float x) {
		return ((-std::cos(x)+1)/2);
	}
	float getSawPhase(float x) {
		return (-((2/M_PI * std::atan(1/std::tan(x/2)))-1)/2);
	}
	float getRevSawPhase(float x) {
		return (((2/M_PI * std::atan(1/std::tan(x/2)))+1)/2);
	}
	float getSquarePhase(float x) {
		return (std::round((std::sin(x)+1)/2));
	}
	
	void debug() {
		printf("Boo!\n");
	
	}

	//saw, sqr, sin, revSaw
	float getBlendedPhase(float x, float wave)
	{
		wave = wave*3+1;
		float waveBlend;
		//wave = 2;
		if (wave>=1 && wave<2) {
			/* saw vs sqr */
			waveBlend = wave-1;
			return (getSawPhase(x)*(1-waveBlend) + getSquarePhase(x)*waveBlend);
		} else if (wave>=2 && wave<3) {
			/* sqr vs sin */
			waveBlend = wave-2;
			return (getSquarePhase(x)*(1-waveBlend) + getSinePhase(x)*waveBlend);
		} else if (wave>=3 && wave<=4) {
			/* sin vs revSaw */
			waveBlend = wave-3;
			return (getSinePhase(x)*(1-waveBlend) + getRevSawPhase(x)*waveBlend);
		} else {
			return 0.0f;
		}
	}
	
	float sinePos;
	float tAudioL, tAudioR;
};



class CGate: public CModule
{
	public:
		void process(float &audioL, float &audioR) {
			tAudioL = audioL;
			tAudioR = audioR;
			
			tAudioL *= getBlendedPhase(sinePos, params[1]);
			tAudioR *= getBlendedPhase(sinePos, params[1]);
			if (active) {
				audioL = tAudioL;
				audioR = tAudioR;
			}
		}

};

class CReverse: public CModule
{
	private:
	
		//two buffers, one is playing (back), one is recording
		struct BufferStack {
		    int32_t start;
		    float* data;
		} bufferL[2], bufferR[2];
		
		//buffer flipper
		int flipState;
		
		//buffer size
		uint32_t bufferStackCount;
		
		//flip record and play buffers
		void flip() {
			if (flipState==0) flipState = 1; else flipState = 0;
		}
		
		//which side is the play side
		int getPlayFlip() {
			if (flipState==0) return 1; else return 0;
		}

	public:
		void process(float &audioL, float &audioR) {
			
			tAudioL = audioL;
			tAudioR = audioR;
		
			//fill the buffer
			bufferL[flipState].data[bufferL[flipState].start] = tAudioL;
			bufferR[flipState].data[bufferR[flipState].start] = tAudioR;
			
			//roll playhead forward
			if (++bufferL[flipState].start>bufferStackCount) {
				bufferL[flipState].start = 0;
			}
			if (++bufferR[flipState].start>bufferStackCount) {
				bufferR[flipState].start = 0;
			}
			
			
			//play the buffer
			//audio
			tAudioL = bufferL[getPlayFlip()].data[bufferL[getPlayFlip()].start]*params[2] + tAudioL*(1-params[2]);
			tAudioR = bufferR[getPlayFlip()].data[bufferR[getPlayFlip()].start]*params[2] + tAudioR*(1-params[2]);
			
			
			//roll playhead backwards
			if (--bufferL[getPlayFlip()].start<0) {
				bufferL[getPlayFlip()].start = bufferStackCount;
			}
			if (--bufferR[getPlayFlip()].start<0) {
				bufferR[getPlayFlip()].start = bufferStackCount;
			}
			if (active) {
				audioL = tAudioL;
				audioR = tAudioR;
			}
		}
		
		//custom method, flip buffers when resetting sinePos
		void setSinePos(float nSinePos) {
			if (sinePos > nSinePos) {
				flip();
			}
			sinePos = nSinePos;
		}
		
		void initBuffers() {
			flipState = 0;
			//set up a 10 second buffer
			bufferStackCount = sampleRate*10;
			for (int i=0; i<2; i++) {
				//allocate
				bufferL[i].data = (float*) calloc(bufferStackCount, sizeof(float));
				bufferR[i].data = (float*) calloc(bufferStackCount, sizeof(float));
				//clear with zeroes
				std::memset(bufferL[i].data, 0, sizeof(float)*bufferStackCount);
				std::memset(bufferR[i].data, 0, sizeof(float)*bufferStackCount);
				//reset playhead
				bufferL[i].start = 0;
				bufferR[i].start = 0;
			}
		}	

};

class CRepeat: public CModule
{
	private:
	
		//two buffers, one is playing (back), one is recording
		struct BufferStack {
		    int32_t start;
		    float* data;
		} bufferL[2], bufferR[2];
		
		//buffer flipper
		int flipState;
		
		//buffer size
		uint32_t bufferStackCount;
		
		//flip record and play buffers
		void flip() {
			repeats++;
			if (repeats==(round(params[1]*6)+1)) {
				if (flipState==0) flipState = 1; else flipState = 0;
			}
			bufferL[getPlayFlip()].start = (bufferL[getPlayFlip()].start-lag) % bufferStackCount;
			bufferR[getPlayFlip()].start = (bufferR[getPlayFlip()].start-lag) % bufferStackCount;
			if (repeats==(round(params[1]*6)+1)) {
				lag = 0;
				repeats = 0;
			}
		}
		
		//which side is the play side
		int getPlayFlip() {
			if (flipState==0) return 1; else return 0;
		}
		
		int repeats;
		uint32_t lag;

	public:
		void process(float &audioL, float &audioR) {
			tAudioL = audioL;
			tAudioR = audioR;
			//fill the buffer
			bufferL[flipState].data[bufferL[flipState].start] = tAudioL;
			bufferR[flipState].data[bufferR[flipState].start] = tAudioR;

			//roll playhead forward
			if (++bufferL[flipState].start>bufferStackCount) {
				bufferL[flipState].start = 0;
			}
			if (++bufferR[flipState].start>bufferStackCount) {
				bufferR[flipState].start = 0;
			}
			if (repeats == 0) {
				lag++;
			}
			
			
			//play the buffer
			//audio
			tAudioL = bufferL[getPlayFlip()].data[bufferL[getPlayFlip()].start]*params[2] + tAudioL*(1-params[2]);
			tAudioR = bufferR[getPlayFlip()].data[bufferR[getPlayFlip()].start]*params[2] + tAudioR*(1-params[2]);
			
			
			//roll playhead forward
			if (++bufferL[getPlayFlip()].start>bufferStackCount) {
				bufferL[getPlayFlip()].start = 0;
			}
			if (++bufferR[getPlayFlip()].start>bufferStackCount) {
				bufferR[getPlayFlip()].start = 0;
			}
			if (active) {
				audioL = tAudioL;
				audioR = tAudioR;
			}
		}
		
		//custom method, flip buffers when resetting sinePos
		void setSinePos(float nSinePos) {
			if (sinePos > nSinePos) {
				flip();
				
			}
			sinePos = nSinePos;
		}
		
		void setParam(float value, int index) {
			params[index] = value;
			repeats = 0;
		}
		
		void initBuffers() {
			repeats = 0;
			flipState = 0;
			lag = 0;
			//set up a 10 second buffer
			bufferStackCount = sampleRate*10;
			for (int i=0; i<2; i++) {
				//allocate
				bufferL[i].data = (float*) calloc(bufferStackCount, sizeof(float));
				bufferR[i].data = (float*) calloc(bufferStackCount, sizeof(float));
				//clear with zeroes
				std::memset(bufferL[i].data, 0, sizeof(float)*bufferStackCount);
				std::memset(bufferR[i].data, 0, sizeof(float)*bufferStackCount);
				//reset playhead
				bufferL[i].start = 0;
				bufferR[i].start = 0;
			}
		}	

};

class CSequence: public CModule
{
	private:
	
		LPF lpf[2];
		HPF hpf[2];
		
		float sequence[9][8] = {
				{200.0f, 400.0f, 600.0f, 800.0f, 200.0f, 400.0f, 600.0f, 800.0f}, //1
				{800.0f, 200.0f, 400.0f, 800.0f, 200.0f, 800.0f, 400.0f, 200.0f}, //2
				{800.0f, 600.0f, 400.0f, 200.0f, 300.0f, 400.0f, 600.0f, 800.0f}, //3
				{200.0f, 800.0f, 200.0f, 600.0f, 200.0f, 400.0f, 200.0f, 300.0f}, //4
				{200.0f, 800.0f, 200.0f, 800.0f, 200.0f, 400.0f, 600.0f, 800.0f}, //5
				{800.0f, 400.0f, 800.0f, 200.0f, 800.0f, 400.0f, 200.0f, 400.0f}, //6
				{400.0f, 800.0f, 200.0f, 800.0f, 200.0f, 800.0f, 400.0f, 300.0f}, //7
				{200.0f, 800.0f, 200.0f, 800.0f, 200.0f, 800.0f, 300.0f, 500.0f}, //8
				{800.0f, 300.0f, 200.0f, 800.0f, 300.0f, 200.0f, 400.0f, 600.0f}, //9
		};
		int sequenceHead;
		float peakFreq;

	public:
		void process(float &audioL, float &audioR) {
			tAudioL = audioL;
			tAudioR = audioR;
			lpf[0].setFreq(peakFreq*2);
			lpf[1].setFreq(peakFreq*2);
			
			hpf[0].setFreq(peakFreq);
			hpf[1].setFreq(peakFreq);
			
			tAudioL = lpf[0].process(tAudioL);
			tAudioR = lpf[1].process(tAudioR);
			tAudioL = hpf[0].process(tAudioL);
			tAudioR = hpf[1].process(tAudioR);
			if (active) {
				audioL = tAudioL;
				audioR = tAudioR;	
			}
		}
		
		//custom method, move sequenceHead when needed
		void setSinePos(float nSinePos) {
			if (sinePos > nSinePos) {
				sequenceHead++;
				if (sequenceHead>7) {
					sequenceHead = 0;
				}
				peakFreq = sequence[(int) round(params[1]*8)][sequenceHead];
				peakFreq += (rand() % 400 - 200)*params[2];
				peakFreq*=2;
				
			}
			sinePos = nSinePos;
		}
		
		void initBuffers() {
			srand (time(NULL));
			sequenceHead = 0;	
			peakFreq = 200.0f;
			for (int i=0; i<2; i++) {
				lpf[i].setSampleRate(sampleRate);
				lpf[i].setReso(0.5);
				hpf[i].setSampleRate(sampleRate);
				hpf[i].setReso(0.5);
			}
		}

};

class CShift: public CModule
{
	private:
		CSHIFT shifter[2];
	
	public:
		void process(float &audioL, float &audioR) {
		
			tAudioL = audioL;
			tAudioR = audioR;
			
			float range = 96*params[1];
			int variation = round(params[2]*3)+1;
			float shiftAmt;
			switch (variation) {
				case 1:
					shiftAmt = (sinePos/(M_PI*2))*range-range/2;
					break;
				case 2:
					shiftAmt = -((sinePos/(M_PI*2))*range-range/2);
					break;
				case 3:
					shiftAmt = (sinePos/(M_PI*2))*range;
					break;
				case 4:
					shiftAmt = -(sinePos/(M_PI*2))*range;
					break;
			
			}
			
			//round up to one decimal point
			shiftAmt = round(shiftAmt*10)/10;

			for (int i=0; i<2; i++) {
				shifter[i].setShiftAmt(shiftAmt);
			}
			
			//shift!
			tAudioL = shifter[0].process(tAudioL);
			tAudioR = shifter[1].process(tAudioR);
			
			
			if (active) {
				audioL = tAudioL;
				audioR = tAudioR;
			}
		}
		
		void initBuffers() {
			for (int i=0; i<2; i++) {
				shifter[i].setSampleRate(sampleRate);
			}
			
		}
			
		float getTempoDivider() {
			return 8.0f;
		}

};

class CFilter: public CModule
{
	private:
	
		LPF lpf[2];
		HPF hpf[2];
		float peakFreq;

	public:
		void process(float &audioL, float &audioR) {
		
			tAudioL = audioL;
			tAudioR = audioR;
			
			//to log scale
			peakFreq = std::exp((std::log(16000)-std::log(300))*getBlendedPhase(sinePos, params[1])+std::log(300));
			//peakFreq = 500.0f;
			lpf[0].setFreq(peakFreq);
			lpf[1].setFreq(peakFreq);
			
			hpf[0].setFreq(peakFreq);
			hpf[1].setFreq(peakFreq);
			int type = params[2]*2+1;
			
			
			switch (type) {
				case 1:
					tAudioL = lpf[0].process(tAudioL);
					tAudioR = lpf[1].process(tAudioR);
					//printf("type: %f\n", tAudioL);
					break;
					
				case 2:
					tAudioL = lpf[0].process(tAudioL);
					tAudioR = lpf[1].process(tAudioR);
					tAudioL = hpf[0].process(tAudioL);
					tAudioR = hpf[1].process(tAudioR);
					break;
				case 3:
					tAudioL = hpf[0].process(tAudioL);
					tAudioR = hpf[1].process(tAudioR);
					break;
			
			}
			
			
			if (active) {
				audioL = tAudioL;
				audioR = tAudioR;	
			}
		}
		
		void initBuffers() {
			peakFreq = 200.0f;
			for (int i=0; i<2; i++) {
				lpf[i].setSampleRate(sampleRate);
				lpf[i].setReso(0.5);
				hpf[i].setSampleRate(sampleRate);
				hpf[i].setReso(0.5);
			}
		}
		
		float getTempoDivider() {
			return 8.0f;
		}

};


#endif // CMODULE_HXX_INCLUDED