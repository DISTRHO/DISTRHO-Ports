#ifndef CSTREAM_HXX_INCLUDED
#define CSTREAM_HXX_INCLUDED

class CStream
{

public:
	CStream() {
		sample = 0;
		time = 0;
		state = 0;
		counter = 0;

	}

	void setSr(int sr) {
		sampleRate = sr;
	}

	void open(float nTime) {//in samples
		state = true;
		if (counter<time) {
			counter = time-counter;
		} else {
			counter = 0;
		}
		time = nTime*sampleRate;
	}

	void close(float nTime) {//in samples
		state = false;
		if (counter<time) {
			counter = time-counter;
		} else {
			counter = 0;
		}
		time = nTime*sampleRate;
	}

	void update(float nSample) {
		sample = nSample;
	}

	float run() {
		if (state) {
			if (counter<time) {
				counter++;
				sample*=(counter/time); //linear fade-in
			} else {
				sample*=1;
			}
		} else {
			if (counter<time) {
				counter++;
				sample*=((time-counter)/time); //lineaar fade-out
			} else {
				sample=0;
			}
		}
		return sample;
	}

private:
	//vars
	float sample; 
	float time;
	float counter;
	int sampleRate;
	bool state;//is open?
};
#endif // CSTREAM_HXX_INCLUDED
