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
		active = false;
	}
	
	void setParam(float value, int index) {
		params[index] = value;
	}
	
	void process(float audio);
	void setSampleRate(int nSR) {
		sampleRate = nSR;
	}
	
	void setSinePos(float nSinePos) {
		sinePos = nSinePos;
	}
	
	float getSinePos() {
		return sinePos;
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

	//saw, sqr, sin, revSaw
	float getBlendedPhase(float x, float wave)
	{
		wave *= 4;
		int waveBlend;
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
	
	//timepos stuff
	float sinePos;
};



class CGate: public CModule
{
	public:
		void process(float audio) {
			//audio[0] *= (getBlendedPhase(sinePos, params[0])+1)/2;
			//audio[1] *= (getBlendedPhase(sinePos, params[0])+1)/2;
		}

};
#endif // CMODULE_HXX_INCLUDED