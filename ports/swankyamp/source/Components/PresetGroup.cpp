/*
 *  Swanky Amp tube amplifier simulation
 *  Copyright (C) 2020  Garrin McGoldrick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <JuceHeader.h>

#include "../SwankyAmpLAF.h"
#include "LevelMeter.h"
#include "ParameterGroup.h"
#include "PresetGroup.h"

PresetGroup::PresetGroup() :
    buttonSave("presetGroupButtonSave", DrawableButton::ButtonStyle::ImageRaw),
    buttonRemove(
        "presetGroupButtonRemove", DrawableButton::ButtonStyle::ImageRaw),
    buttonNext("presetGroupButtonNext", DrawableButton::ButtonStyle::ImageRaw),
    buttonPrev("presetGroupButtonPrev", DrawableButton::ButtonStyle::ImageRaw),
    buttonOpen("presetGroupButtonOpen", DrawableButton::ButtonStyle::ImageRaw)
{
  presetSelector.setEditableText(true);
  addAndMakeVisible(presetSelector);

  saveIcon.setFill(Colours::transparentBlack);
  saveIcon.setStrokeFill(SwankyAmpLAF::colourDark);
  saveIconHighlight.setFill(Colours::transparentBlack);
  saveIconHighlight.setStrokeFill(SwankyAmpLAF::colourHighlight);
  addAndMakeVisible(buttonSave);

  removeIcon.setFill(Colours::transparentBlack);
  removeIcon.setStrokeFill(SwankyAmpLAF::colourDark);
  removeIconHighlight.setFill(Colours::transparentBlack);
  removeIconHighlight.setStrokeFill(SwankyAmpLAF::colourHighlight);
  addAndMakeVisible(buttonRemove);

  nextIcon.setFill(Colours::transparentBlack);
  nextIcon.setStrokeFill(SwankyAmpLAF::colourDark);
  nextIconHighlight.setFill(Colours::transparentBlack);
  nextIconHighlight.setStrokeFill(SwankyAmpLAF::colourHighlight);
  addAndMakeVisible(buttonNext);

  prevIcon.setFill(Colours::transparentBlack);
  prevIcon.setStrokeFill(SwankyAmpLAF::colourDark);
  prevIconHighlight.setFill(Colours::transparentBlack);
  prevIconHighlight.setStrokeFill(SwankyAmpLAF::colourHighlight);
  addAndMakeVisible(buttonPrev);

  openIcon.setFill(Colours::transparentBlack);
  openIcon.setStrokeFill(SwankyAmpLAF::colourDark);
  openIconHighlight.setFill(Colours::transparentBlack);
  openIconHighlight.setStrokeFill(SwankyAmpLAF::colourHighlight);
  addAndMakeVisible(buttonOpen);
}

void PresetGroup::paint(Graphics& g)
{
  // skip the group paint to avoid drawing border
  Component::paint(g);
}

void PresetGroup::resized()
{
  ParameterGroup::resized();
  presetSelector.setTopLeftPosition(0, 0);
  presetSelector.setSize(getHeight() * 8, getHeight());

  const float stroke = 2.0f;
  const float pad = 8.0f;

  prevIcon.setPath(SwankyAmpLAF::getPrevIconPath((float)getHeight(), pad));
  prevIcon.setStrokeType(PathStrokeType(stroke));
  prevIconHighlight.setStrokeType(PathStrokeType(stroke));
  prevIconHighlight.setPath(
      SwankyAmpLAF::getPrevIconPath((float)getHeight(), pad));
  buttonPrev.setImages(&prevIcon, &prevIconHighlight);

  buttonPrev.setSize(getHeight(), getHeight());
  buttonPrev.setTopLeftPosition(
      presetSelector.getBounds().getTopRight() + Point<int>(spacing, 0));

  nextIcon.setPath(SwankyAmpLAF::getNextIconPath((float)getHeight(), pad));
  nextIcon.setStrokeType(PathStrokeType(stroke));
  nextIconHighlight.setStrokeType(PathStrokeType(stroke));
  nextIconHighlight.setPath(
      SwankyAmpLAF::getNextIconPath((float)getHeight(), pad));
  buttonNext.setImages(&nextIcon, &nextIconHighlight);

  buttonNext.setSize(getHeight(), getHeight());
  buttonNext.setTopLeftPosition(buttonPrev.getBounds().getTopRight());

  saveIcon.setPath(SwankyAmpLAF::getSaveIconPath((float)getHeight(), pad));
  saveIcon.setStrokeType(PathStrokeType(stroke));
  saveIconHighlight.setStrokeType(PathStrokeType(stroke));
  saveIconHighlight.setPath(
      SwankyAmpLAF::getSaveIconPath((float)getHeight(), pad));
  buttonSave.setImages(&saveIcon, &saveIconHighlight);

  buttonSave.setSize(getHeight(), getHeight());
  buttonSave.setTopLeftPosition(buttonNext.getBounds().getTopRight());

  removeIcon.setPath(SwankyAmpLAF::getRemoveIconPath((float)getHeight(), pad));
  removeIcon.setStrokeType(PathStrokeType(stroke));
  removeIconHighlight.setStrokeType(PathStrokeType(stroke));
  removeIconHighlight.setPath(
      SwankyAmpLAF::getRemoveIconPath((float)getHeight(), pad));
  buttonRemove.setImages(&removeIcon, &removeIconHighlight);

  buttonRemove.setSize(getHeight(), getHeight());
  buttonRemove.setTopLeftPosition(buttonSave.getBounds().getTopRight());

  openIcon.setPath(SwankyAmpLAF::getOpenIconPath((float)getHeight(), pad));
  openIcon.setStrokeType(PathStrokeType(stroke));
  openIconHighlight.setStrokeType(PathStrokeType(stroke));
  openIconHighlight.setPath(
      SwankyAmpLAF::getOpenIconPath((float)getHeight(), pad));
  buttonOpen.setImages(&openIcon, &openIconHighlight);

  buttonOpen.setSize(getHeight(), getHeight());
  buttonOpen.setTopLeftPosition(buttonRemove.getBounds().getTopRight());

  setSize(buttonOpen.getRight(), getHeight());
}
