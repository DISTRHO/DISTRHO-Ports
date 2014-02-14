/*-----------------------------------------------------------------------------

Kunz Patrick 15.08.2007

DC Blocking Filter                      
by Perry R. Cook, 1995-96               
                                         
This guy is very helpful in, uh,       
blocking DC.  Needed because a simple
low-pass reflection filter allows DC to
to build up inside recursive structures
-----------------------------------------------------------------------------*/

#if !defined(__DCBlock_h)
#define __DCBlock_h

class DCBlock 
{
 public:
  float inputs, outputs, lastOutput, vsa;

  DCBlock() 
  {
    lastOutput = inputs = outputs = 0.0f;
    vsa= 1.0f/4294967295.0f;   // Very small amount (Denormal Fix)
  }

  inline void tick(float *sample, float cutoff) 
  {
    outputs     = *sample-inputs+(0.9999f-cutoff*0.4f)*outputs;
    inputs      = *sample+vsa;
    lastOutput  = outputs;
    *sample     = lastOutput;
  }
};

#endif
