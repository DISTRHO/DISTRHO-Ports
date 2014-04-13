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
#include <deque>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

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

    //DSP functions

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

    // -------------------------------------------------------------------
    // Internal data

    float d_getParameterValue(uint32_t index) const override;
    void  d_setParameterValue(uint32_t index, float value) override;
    void  d_setProgram(uint32_t index) override;

    // -------------------------------------------------------------------
    // Process

    void d_activate() override;
    void d_deactivate() override;
    void d_run(float** inputs, float** outputs, uint32_t frames) override;

    // -------------------------------------------------------------------

private:
    float attack, release, threshold, ratio, makeup, mix;
	std::deque<float> input, output, gainReduction;
	float averageInputs[150];
	int averageCounter;
	float inputMin, inputMax;

	boost::interprocess::shared_memory_object* shm_obj;
	boost::interprocess::mapped_region* region;
	
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO

#endif  // POWERJUICE_HPP_INCLUDED
