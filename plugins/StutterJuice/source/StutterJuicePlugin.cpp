/*
 * Stutter Juice Plugin
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

#include "StutterJuicePlugin.hpp"
#include <iostream>



START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------



StutterJuicePlugin::StutterJuicePlugin()
    : Plugin(paramCount, 1, 0) // 1 program, 0 states
{
    // set default values
    d_setProgram(0);
	
    // reset
    d_deactivate();
}

StutterJuicePlugin::~StutterJuicePlugin()
{
	
}

// -----------------------------------------------------------------------
// Init

void StutterJuicePlugin::d_initParameter(uint32_t index, Parameter& parameter)
{

	parameter.hints      = PARAMETER_IS_AUTOMABLE;
	parameter.name       = "";
	parameter.symbol     = "";
	parameter.unit       = "";
	parameter.ranges.def = 0.0f;
	parameter.ranges.min = 0.0f;
	parameter.ranges.max = 1.0f;
	
	
	
}

void StutterJuicePlugin::d_initProgramName(uint32_t index, d_string& programName)
{
    if (index != 0)
        return;

    programName = "Default";
}

// -----------------------------------------------------------------------
// Internal data

float StutterJuicePlugin::d_getParameterValue(uint32_t index) const
{
	int module = index/3;
	int param = index%3;
	return params[module][param];
}

void StutterJuicePlugin::d_setParameterValue(uint32_t index, float value)
{
	int module = index/3;
	int param = index%3;
	params[module][param] = value;
	modules[module]->setParam(value, param);
}

void StutterJuicePlugin::d_setProgram(uint32_t index)
{
	if (index != 0)
		return;

	//default params[][] values
	for (int module=0; module<9; module++) {
		moduleActive[module] = false;
		for (int param=0; param<3; param++) {
			params[module][param] = 0.5;
		}
	}
	
	modules[0] = new CGate();
	modules[0]->setSampleRate(d_getSampleRate());

}

// -----------------------------------------------------------------------
// Process

void StutterJuicePlugin::d_activate()
{
}

void StutterJuicePlugin::d_deactivate()
{
}

void StutterJuicePlugin::d_run(float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{

	for (uint32_t i; i<frames; i++) {	
		
		float* audio;
		audio[0] = inputs[0][i];
		audio[1] = inputs[1][i];
		
		//modules[0]->process(audio);
		modules[0]->process(9.0f);
		
		*outputs[0] = audio[0];
		*outputs[1] = audio[1];
		
	}

	

	for (uint32_t i; i<midiEventCount; i++) {
		int userNote = 64;//TODO
		int range=9;

		int mType = midiEvents[i].buf[0] & 0xF0;
		int mChan = midiEvents[i].buf[0] & 0x0F;
		int mNum = midiEvents[i].buf[1];
		if (mNum>=userNote && mNum<userNote+range) {
			int module = mNum-userNote;
			if (mType == 0x90) {
				//NOTE ON
				moduleActive[module] = true;
			} else if (mType == 0x80) {
				//NOTE OFF
				moduleActive[module] = false;
			}
		}
	}
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new StutterJuicePlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
