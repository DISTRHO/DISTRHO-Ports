/*
  ==============================================================================

   This file is part of the JUCE 7 technical preview.
   Copyright (c) 2022 - Raw Material Software Limited

   You may use this code under the terms of the GPL v3
   (see www.gnu.org/licenses).

   For the technical preview this file cannot be licensed commercially.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

MouseInactivityDetector::MouseInactivityDetector (Component& c)  : targetComp (c)
{
    targetComp.addMouseListener (this, true);
}

MouseInactivityDetector::~MouseInactivityDetector()
{
    targetComp.removeMouseListener (this);
}

void MouseInactivityDetector::setDelay (int newDelay) noexcept                  { delayMs = newDelay; }
void MouseInactivityDetector::setMouseMoveTolerance (int newDistance) noexcept  { toleranceDistance = newDistance; }

void MouseInactivityDetector::addListener    (Listener* l)   { listenerList.add (l); }
void MouseInactivityDetector::removeListener (Listener* l)   { listenerList.remove (l); }

void MouseInactivityDetector::timerCallback()
{
    setActive (false);
}

void MouseInactivityDetector::wakeUp (const MouseEvent& e, bool alwaysWake)
{
    auto newPos = e.getEventRelativeTo (&targetComp).getPosition();

    if ((! isActive) && (alwaysWake || e.source.isTouch() || newPos.getDistanceFrom (lastMousePos) > toleranceDistance))
        setActive (true);

    if (lastMousePos != newPos)
    {
        lastMousePos = newPos;
        startTimer (delayMs);
    }
}

void MouseInactivityDetector::setActive (bool b)
{
    if (isActive != b)
    {
        isActive = b;

        if (isActive)
            listenerList.call ([] (Listener& l) { l.mouseBecameActive(); });
        else
            listenerList.call ([] (Listener& l) { l.mouseBecameInactive(); });
    }
}

} // namespace juce
