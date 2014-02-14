/*
 * Trigger Juice Plugin
 * Copyright (C) 2014 Andre Sklenar <andre.sklenar@gmail.com>, www.juicelab.cz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#include "TriggerJuicePlugin.hpp"
#include <iostream>



START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------



TriggerJuicePlugin::TriggerJuicePlugin()
    : Plugin(paramCount, 1, 0) // 1 program, 0 states
{
    // set default values
    d_setProgram(0);
	
    // reset
    d_deactivate();
}

TriggerJuicePlugin::~TriggerJuicePlugin()
{
	
}

// -----------------------------------------------------------------------
// Init

void TriggerJuicePlugin::d_initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {
    case paramAttack:
        parameter.hints      = PARAMETER_IS_AUTOMABLE;
        parameter.name       = "Attack";
        parameter.symbol     = "att";
        parameter.unit       = "seconds";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;

    case paramRelease:
        parameter.hints      = PARAMETER_IS_AUTOMABLE;
        parameter.name       = "Release";
        parameter.symbol     = "rel";
        parameter.unit       = "seconds";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;

    case paramRev:
        parameter.hints      = PARAMETER_IS_AUTOMABLE;
        parameter.name       = "Reverse";
        parameter.symbol     = "rev";
        parameter.unit       = "";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;

    case paramSplit:
        parameter.hints      = PARAMETER_IS_AUTOMABLE;
        parameter.name       = "Split";
        parameter.symbol     = "spl";
        parameter.unit       = "";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;

    case paramMS:
        parameter.hints      = PARAMETER_IS_AUTOMABLE;
        parameter.name       = "M/S";
        parameter.symbol     = "ms";
        parameter.unit       = "";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;

    }

	
}

void TriggerJuicePlugin::d_initProgramName(uint32_t index, d_string& programName)
{
    if (index != 0)
        return;

    programName = "Default";
}

// -----------------------------------------------------------------------
// Internal data

float TriggerJuicePlugin::d_getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case paramAttack:
        return attack;
    case paramRelease:
        return release;
    case paramSplit:
        return split;
    case paramRev:
        return rev;
    case paramMS:
        return MS;
    default:
        return 0.0f;
    }
}

void TriggerJuicePlugin::d_setParameterValue(uint32_t index, float value)
{
    switch (index)
    {
    case paramSplit:
        split = value;
        break;
    case paramRev:
        rev = value;
		if (rev<0.5) {
			left.close(d_getSampleRate()/10); right.close(d_getSampleRate()/10); 
		} else {
			left.open(d_getSampleRate()/10); right.open(d_getSampleRate()/10);
		}
        break;
    case paramAttack:
        attack = value;
        break;
    case paramRelease:
        release = value;
        break;
    case paramMS:
        MS = value;
        break;
    }
}

void TriggerJuicePlugin::d_setProgram(uint32_t index)
{
    if (index != 0)
        return;

    /* Default parameter values */
    split = 0.0f;
    rev = 0.0f;
    attack = 0.0f;
    release = 0.0f;
    MS = 0.0f;

    /* Default variable values */
    attack=release=0;
	rev=split=MS=0;

	left.setSr(d_getSampleRate());
	right.setSr(d_getSampleRate());

	//parameter smoothing
	for (int i=0; i<2; i++) {
		sA[i] = 0.99f;
		sB[i] = 1.f - sA[i];
		sZ[i] = 0;
	}

    /* reset filter values */
    d_activate();
}

// -----------------------------------------------------------------------
// Process

void TriggerJuicePlugin::d_activate()
{
    //sinePos = 0;
}

void TriggerJuicePlugin::d_deactivate()
{
    // all values to zero
}

void TriggerJuicePlugin::d_run(float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{

	for (uint32_t i; i<frames; i++) {	

		float inL = inputs[0][i];
		float inR = inputs[1][i];
		if (MS<0.5) {
			//stereo mode
			left.update(inL);
			right.update(inR);
		} else {
			//M/S mode
			left.update(inL+inR);
			right.update(inL-inR);
		}
		
		float audioL = left.run();
		float audioR = right.run();

		if (MS>=0.5) {
			// decode from MS
			float mid = audioL;
			float side = audioR;
			audioL = (mid+side)/2;
			audioR = (mid-side)/2;
		}

		outputs[0][i] = audioL;
		outputs[1][i] = audioR;
	}

	int userNote = 64;//TODO

	for (uint32_t i; i<midiEventCount; i++) {

		int mType = midiEvents[i].buf[0] & 0xF0;
		int mChan = midiEvents[i].buf[0] & 0x0F;
		int mNum = midiEvents[i].buf[1];
		if (mType == 0x90) {
			//NOTE ON

			std::cout << mNum << std::endl;
			int mVelo = midiEvents[i].buf[2];
			if (mNum == userNote) {
				if (rev<0.5) {
					if (split<0.5) {
						//open both streams
						left.open(attack);
						right.open(attack);
					} else {
						//open just left stream
						left.open(attack);
					}
				} else {
					if (split<0.5) {
						//close both streams
						left.close(attack);
						right.close(attack);
					} else {
						//close just left stream
						left.close(attack);
					}
				}
			}
			if (split>=0.5) {
				if (mNum == userNote+1) {
					if (rev<0.5) {
						//open right stream
						right.open(attack);
					} else {
						//close right stream
						right.close(attack);
					}		
				}

			}
		} else if (mType == 0x80) {
			//NOTE OFF
			if (mNum == userNote) {
				if (rev<0.5) {
					if (split<0.5) {
						//close both streams
						left.close(release);
						right.close(release);
					} else {
						//close just left stream
						left.close(release);
					}
				} else {
					if (split<0.5) {
						//open both streams
						left.open(release);
						right.open(release);
					} else {
						//open just left stream
						left.open(release);
					}
				}
			}
			if (split>=0.5) {
				if (mNum == userNote+1) {
					if (rev<0.5) {
						//close right stream
						right.close(release);
					} else {
						//open right stream
						right.open(release);
					}		
				}

			}
		}
	}
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new TriggerJuicePlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
