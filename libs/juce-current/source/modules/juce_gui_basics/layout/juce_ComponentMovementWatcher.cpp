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

ComponentMovementWatcher::ComponentMovementWatcher (Component* const comp)
    : component (comp),
      wasShowing (comp->isShowing())
{
    jassert (component != nullptr); // can't use this with a null pointer..

    component->addComponentListener (this);
    registerWithParentComps();
}

ComponentMovementWatcher::~ComponentMovementWatcher()
{
    if (component != nullptr)
        component->removeComponentListener (this);

    unregister();
}

//==============================================================================
void ComponentMovementWatcher::componentParentHierarchyChanged (Component&)
{
    if (component != nullptr && ! reentrant)
    {
        const ScopedValueSetter<bool> setter (reentrant, true);

        auto* peer = component->getPeer();
        auto peerID = peer != nullptr ? peer->getUniqueID() : 0;

        if (peerID != lastPeerID)
        {
            componentPeerChanged();

            if (component == nullptr)
                return;

            lastPeerID = peerID;
        }

        unregister();
        registerWithParentComps();

        componentMovedOrResized (*component, true, true);

        if (component != nullptr)
            componentVisibilityChanged (*component);
    }
}

void ComponentMovementWatcher::componentMovedOrResized (Component&, bool wasMoved, bool wasResized)
{
    if (component != nullptr)
    {
        if (wasMoved)
        {
            Point<int> newPos;
            auto* top = component->getTopLevelComponent();

            if (top != component)
                newPos = top->getLocalPoint (component, Point<int>());
            else
                newPos = top->getPosition();

            wasMoved = lastBounds.getPosition() != newPos;
            lastBounds.setPosition (newPos);
        }

        wasResized = (lastBounds.getWidth() != component->getWidth() || lastBounds.getHeight() != component->getHeight());
        lastBounds.setSize (component->getWidth(), component->getHeight());

        if (wasMoved || wasResized)
            componentMovedOrResized (wasMoved, wasResized);
    }
}

void ComponentMovementWatcher::componentBeingDeleted (Component& comp)
{
    registeredParentComps.removeFirstMatchingValue (&comp);

    if (component == &comp)
        unregister();
}

void ComponentMovementWatcher::componentVisibilityChanged (Component&)
{
    if (component != nullptr)
    {
        const bool isShowingNow = component->isShowing();

        if (wasShowing != isShowingNow)
        {
            wasShowing = isShowingNow;
            componentVisibilityChanged();
        }
    }
}

void ComponentMovementWatcher::registerWithParentComps()
{
    for (auto* p = component->getParentComponent(); p != nullptr; p = p->getParentComponent())
    {
        p->addComponentListener (this);
        registeredParentComps.add (p);
    }
}

void ComponentMovementWatcher::unregister()
{
    for (auto* c : registeredParentComps)
        c->removeComponentListener (this);

    registeredParentComps.clear();
}

} // namespace juce
