/*
  ==============================================================================

    Common.h
    Created: 10 Jun 2012 7:30:05pm
    Author:  David Rowland

  ==============================================================================
*/

#ifndef __COMMON_H_86A3DB6__
#define __COMMON_H_86A3DB6__

#include "JuceHeader.h"

using namespace drow;

/* Our list of parameters and the names that will be displayed for them.
 */
namespace Parameters
{
    enum Parameters
    {
        rate = 0,
        depth,
        shape,
        phase,
        numParameters
    };
    
    static const char* UNUSED_NOWARN names[numParameters] = 
    {
        "Rate",
        "Depth",
        "Shape",
        "Phase"
    };

    static const ParameterUnit UNUSED_NOWARN units[numParameters] = 
    {
        UnitHertz,
        UnitPercent,
        UnitGeneric,
        UnitDegrees
    };
    
    static const char* UNUSED_NOWARN descriptions[numParameters] = 
    {
        "The rate of the effect",
        "The depth of the effect",
        "The shape of the tremolo effect",
        "The level of offset of the second channel"
    };
    
    static const double UNUSED_NOWARN mins[numParameters] = 
    {
        0.0,
        0.0,
        0.2,
        -180.0
    };

    static const double UNUSED_NOWARN maxs[numParameters] = 
    {
        20.0,
        100.0,
        10.0,
        180.0
    };
    
    static const double UNUSED_NOWARN defaults[numParameters] = 
    {
        5.0,
        100.0,
        1.0,
        0.0,
    };
}


#endif  // __COMMON_H_86A3DB6__
