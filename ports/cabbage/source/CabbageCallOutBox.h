#ifndef __CABBAGE_CALLOUTBOX
#define __CABBAGE_CALLOUTBOX

#include "JuceHeader.h"

/*
  Copyright (C) 2031 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA
*/

class CabbageCallOutBox    : public Component
{
public:
    //==============================================================================
    CabbageCallOutBox (Component& contentComponent,
                const Rectangle<int>& areaToPointTo,
                Component* parentComponent);
    ~CabbageCallOutBox();

    void updatePosition (const Rectangle<int>& newAreaToPointTo,
                         const Rectangle<int>& newAreaToFitIn);
    static CabbageCallOutBox& launchAsynchronously (Component* contentComponent,
                                             const Rectangle<int>& areaToPointTo,
                                             Component* parentComponent);

    //==============================================================================
    void paint (Graphics& g);
    void resized();
    void moved();
    void childBoundsChanged (Component*);
    bool hitTest (int x, int y);
    void inputAttemptWhenModal();
    bool keyPressed (const KeyPress& key);
    void handleCommandMessage (int commandId);

private:
    //==============================================================================
    int borderSpace;
    float arrowSize;
    Component& content;
    Path outline;
    Point<float> targetPoint;
    Rectangle<int> availableArea, targetArea;
    Image background;

    void refreshPath();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CabbageCallOutBox)
};


#endif   // __JUCE_CALLOUTBOX_JUCEHEADER__
