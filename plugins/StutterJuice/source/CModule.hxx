#ifndef CMODULE_HXX_INCLUDED
#define CMODULE_HXX_INCLUDED

class CModule
{

public:
	CModule() {
		sampleRate = 0;
		params[0] = 0.0f;
		params[1] = 0.0f;
		params[2] = 0.0f;
		active = true;
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
	
	virtual void setSinePos(float nSinePos) {
		sinePos = nSinePos;
	}
	
	float getSinePos() {
		return sinePos;
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
};



class CGate: public CModule
{
	public:
		void process(float &audioL, float &audioR) {
			if (active) {
				audioL *= getBlendedPhase(sinePos, params[1]);
				audioR *= getBlendedPhase(sinePos, params[1]);
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
			//fill the buffer
			bufferL[flipState].data[bufferL[flipState].start] = audioL;
			bufferR[flipState].data[bufferR[flipState].start] = audioR;
			
			//roll playhead forward
			if (++bufferL[flipState].start>bufferStackCount) {
				bufferL[flipState].start = 0;
			}
			if (++bufferR[flipState].start>bufferStackCount) {
				bufferR[flipState].start = 0;
			}
			
			
			//play the buffer
			if (active) {
				//audio
				audioL = bufferL[getPlayFlip()].data[bufferL[getPlayFlip()].start]*params[2] + audioL*(1-params[2]);
				audioR = bufferR[getPlayFlip()].data[bufferR[getPlayFlip()].start]*params[2] + audioR*(1-params[2]);
				
				
				//roll playhead backwards
				if (--bufferL[getPlayFlip()].start<0) {
					bufferL[getPlayFlip()].start = bufferStackCount;
				}
				if (--bufferR[getPlayFlip()].start<0) {
					bufferR[getPlayFlip()].start = bufferStackCount;
				}
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
			if (repeats>3) {
				if (flipState==0) flipState = 1; else flipState = 0;
			}
			bufferL[getPlayFlip()].start = (bufferL[getPlayFlip()].start-lag) % bufferStackCount;
			bufferR[getPlayFlip()].start = (bufferR[getPlayFlip()].start-lag) % bufferStackCount;
			if (repeats>4) {
				lag = 0;
				repeats = 0;
			}
		}
		
		//which side is the play side
		int getPlayFlip() {
			if (flipState==0) return 1; else return 0;
		}
		
		int repeats;
		uint32_t playStart;
		uint32_t lag;

	public:
		void process(float &audioL, float &audioR) {
			//fill the buffer
			bufferL[flipState].data[bufferL[flipState].start] = audioL;
			bufferR[flipState].data[bufferR[flipState].start] = audioR;

			//roll playhead forward
			if (++bufferL[flipState].start>bufferStackCount) {
				bufferL[flipState].start = 0;
			}
			if (++bufferR[flipState].start>bufferStackCount) {
				bufferR[flipState].start = 0;
			}
			if (repeats == 0)
				lag++;
			
			
			//play the buffer
			if (active) {
				//audio
				audioL = bufferL[getPlayFlip()].data[bufferL[getPlayFlip()].start]*params[2] + audioL*(1-params[2]);
				audioR = bufferR[getPlayFlip()].data[bufferR[getPlayFlip()].start]*params[2] + audioR*(1-params[2]);
				
				
				//roll playhead forward
				if (++bufferL[getPlayFlip()].start>bufferStackCount) {
					bufferL[getPlayFlip()].start = 0;
				}
				if (++bufferR[getPlayFlip()].start>bufferStackCount) {
					bufferR[getPlayFlip()].start = 0;
				}
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
			playStart = 0;
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
#endif // CMODULE_HXX_INCLUDED