/*
 ==============================================================================
 
 JUCE library : Starting point code, v1.26
 Copyright 2005 by Julian Storer. [edited by haydxn, 3rd April 2007]
 Modified 2008 by David Rowland
 
 This is a modified base application project that uses the recent Juce cocoa code
 It is split into separate cpp and h files and uses some preprocessor macros
 
 ==============================================================================
 */
#ifndef _DROWBUFFERVIEW_H_
#define _DROWBUFFERVIEW_H_

#include "Common.h"

class TremoloBufferView  :  public Component
{
public:
	//==============================================================================
	TremoloBufferView (float* buffer, int size);
	
	~TremoloBufferView();
	
	//==============================================================================
	void resized();
	
	void paint (Graphics& g);
    
    void refreshBuffer();
	
private:

	//==============================================================================	
	const int tremoloBufferSize;
	const float* tremoloBuffer;
	
	Path displayPath;
};

#endif//_DROWBUFFERVIEW_H_ 