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

ButtonPropertyComponent::ButtonPropertyComponent (const String& name, bool triggerOnMouseDown)
    : PropertyComponent (name)
{
    addAndMakeVisible (button);
    button.setTriggeredOnMouseDown (triggerOnMouseDown);
    button.onClick = [this] { buttonClicked(); };
}

ButtonPropertyComponent::~ButtonPropertyComponent()
{
}

void ButtonPropertyComponent::refresh()
{
    button.setButtonText (getButtonText());
}

} // namespace juce
