/*
 * Power Juice Plugin
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

#ifndef POWERJUICEPLUGIN_HPP_INCLUDED
#define POWERJUICEPLUGIN_HPP_INCLUDED

#include "DistrhoPlugin.hpp"
#include "carla/CarlaShmUtils.hpp"

static const int kFloatStackCount = 563;
static const int kFloatRMSStackCount = 1000;
static const int kFloatLookaheadStackCount = 500;

struct FloatStack {
    int32_t start;
    float data[kFloatStackCount];
};

struct FloatRMSStack {
    int32_t start;
    float data[kFloatRMSStackCount];
};

struct LookaheadStack {
    int32_t start;
    float data[kFloatLookaheadStackCount];
};

struct SharedMemData {
    float input[kFloatStackCount];
    float rms[kFloatStackCount];
    float gainReduction[kFloatStackCount];
};

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class PowerJuicePlugin : public Plugin
{
public:
    enum Parameters
    {
        paramAttack = 0,
        paramRelease,
        paramThreshold,
        paramRatio,
        paramMakeup,
        paramMix,
        paramInput,
        paramOutput,
        paramGainReduction,
        paramCount
    };

    PowerJuicePlugin();
    ~PowerJuicePlugin() override;

protected:
    // -------------------------------------------------------------------
    // Information

    const char* d_getLabel() const noexcept override
    {
        return "PowerJuice";
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
        return d_cconst('P', 'w', 'r', 'J');
    }

    // -------------------------------------------------------------------
    // Init

    void d_initParameter(uint32_t index, Parameter& parameter) override;
    void d_initProgramName(uint32_t index, d_string& programName) override;
    void d_initStateKey(uint32_t, d_string&) override;

    // -------------------------------------------------------------------
    // Internal data

    float d_getParameterValue(uint32_t index) const override;
    void  d_setParameterValue(uint32_t index, float value) override;
    void  d_setProgram(uint32_t index) override;
    void  d_setState(const char* key, const char* value) override;

    // -------------------------------------------------------------------
    // Process

    void d_activate() override;
    void d_deactivate() override;
    void d_run(float** inputs, float** outputs, uint32_t frames) override;

    // -------------------------------------------------------------------

private:
    // params
    float attack, release, threshold, ratio, makeup, mix;
	float attackSamples, releaseSamples;
	float balancer;
	float targetGR;
	float GR;

    int averageCounter;
    float inputMax;

    // this was unused
    // float averageInputs[150];

    FloatStack input, rms, gainReduction;
	FloatRMSStack RMSStack;
	LookaheadStack lookaheadStack;

    shm_t shm;
    SharedMemData* shmData;

    void initShm(const char* shmKey);
    void closeShm();

	double computeRMS()
	{
	  	float sum = 0.0;

		for (int j=0; j < kFloatRMSStackCount; ++j)
        	sum += RMSStack.data[(RMSStack.start+j) % kFloatRMSStackCount];
	  	return sqrt(sum / kFloatRMSStackCount);
	}
	
	float fromDB(float gdb) {
		return (exp(gdb*(log(10.f)*0.05)));
	};

	float toDB(float g) {
		return (20.f*log10(g));
	}

	float cosineInterpolate(float y1, float y2, float mu)
	{
	   float mu2;

	   mu2 = (1-cos(mu*M_PI))/2;
	   return(y1*(1-mu2)+y2*mu2);
	}

	float toIEC(float db) {
         float def = 0.0f; /* Meter deflection %age */
 
         if (db < -70.0f) {
                 def = 0.0f;
         } else if (db < -60.0f) {
                 def = (db + 70.0f) * 0.25f;
         } else if (db < -50.0f) {
                 def = (db + 60.0f) * 0.5f + 5.0f;
         } else if (db < -40.0f) {
                 def = (db + 50.0f) * 0.75f + 7.5;
         } else if (db < -30.0f) {
                 def = (db + 40.0f) * 1.5f + 15.0f;
         } else if (db < -20.0f) {
                 def = (db + 30.0f) * 2.0f + 30.0f;
         } else if (db < 0.0f) {
                 def = (db + 20.0f) * 2.5f + 50.0f;
         } else {
                 def = 100.0f;
         }
 
         return (def * 2.0f);
	}

};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif  // POWERJUICE_HPP_INCLUDED
