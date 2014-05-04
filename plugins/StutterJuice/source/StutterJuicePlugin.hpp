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

#ifndef STUTTERJUICEPLUGIN_HPP_INCLUDED
#define STUTTERJUICEPLUGIN_HPP_INCLUDED

#include "DistrhoPlugin.hpp"
#include "CModule.hxx"


START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class StutterJuicePlugin : public Plugin
{
public:
	enum Parameters
	{
		p1 = 0, p2, p3, p4, p5, p6, p7, p8, p9,
		p11, p12, p13, p14, p15, p16, p17, p18,
		p19, p20, p21, p22, p23, p24, p25, p26,
		o1, o2, o3, o4, o5, o6, o7, o8, o9,
		paramCount
	};
	
	
	void prepareOutputParams() {
		for (int i=0; i<9; i++) {
			outputParams[i] = modules[i]->getOutputParam();
		}
	}
	
	void rollLFOs() {
		float bar, tick, tickOffset, sinePos, percentage;
		
		for (int i=0; i<1; i++) {
			sinePos = modules[i]->getSinePos();
			/* sample count for one bar */
			const TimePos& time = d_getTimePos();
			bar = ((120.0/(time.bbt.valid ? time.bbt.beatsPerMinute : 120.0))*(d_getSampleRate())); //ONE, two, three, four
			tick = bar/(std::round(params[i][0]*16+2)); //size of one target wob
			//printf("%d \n", time.bbt.beatsPerMinute);
			if (time.bbt.valid) printf("hell yeah!\n");
			if (time.playing)
			{
				/* if rolling then sync to timepos */
				tickOffset = time.frame-std::floor(time.frame/tick)*tick; //how much after last tick

				if (tickOffset!=0) {
					//TODO: why do we need this??
					percentage = tickOffset/tick;
				} else {
					percentage = 0;
				}
				sinePos = (M_PI*2)*percentage;

				if (sinePos>2*M_PI) {
					//TODO: can this ever happen??
				  sinePos = 0;
				}
			}
			else
			{
				/* else just keep on wobblin' */
				sinePos += (2*M_PI)/(tick); //wtf, but works
				if (sinePos>2*M_PI) {
					sinePos = 0;
				}
			}
			modules[i]->setSinePos(sinePos);
			//printf("sinepos: %f\n", sinePos);
		}
	
	}
	

	StutterJuicePlugin();
	//~StutterJuicePlugin() override;

protected:
    // -------------------------------------------------------------------
    // Information

    const char* d_getLabel() const noexcept override
    {
        return "StutterJuice";
    }

    const char* d_getMaker() const noexcept override
    {
        return "Andre Sklenar";
    }

    const char* d_getLicense() const noexcept override
    {
        return "GPL v2+";
    }

    uint32_t d_getVersion() const noexcept override
    {
        return 0x1000;
    }

    long d_getUniqueId() const noexcept override
    {
        return d_cconst('S', 't', 't', 'J');
    }

    // -------------------------------------------------------------------
    // Init

    void d_initParameter(uint32_t index, Parameter& parameter) override;
    void d_initProgramName(uint32_t index, d_string& programName) override;

    // -------------------------------------------------------------------
    // Internal data

    float d_getParameterValue(uint32_t index) const override;
    void  d_setParameterValue(uint32_t index, float value) override;
    void  d_setProgram(uint32_t index) override;

    // -------------------------------------------------------------------
    // Process

    void d_activate() override;
    void d_deactivate() override;
    void d_run(float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount) override;

    // -------------------------------------------------------------------

private:

	float params[9][3]; // left-> right, top->bottom
	bool moduleActive[9];
	float outputParams[9];
	
	
	
	CModule* modules[9];
	
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif  // TRIGGERJUICE_HPP_INCLUDED
