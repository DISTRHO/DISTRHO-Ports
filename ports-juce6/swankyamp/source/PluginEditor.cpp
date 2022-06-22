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

#include "PluginProcessor.h"
#include "Utils.h"

#include "PluginEditor.h"

SwankyAmpAudioProcessorEditor::SwankyAmpAudioProcessorEditor(
    SwankyAmpAudioProcessor& p, AudioProcessorValueTreeState& vts) :
    AudioProcessorEditor(&p),
    StateChangeMonitor(30),
    processor(p),
    valueTreeState(vts),
    presetManager(
        processor,
        valueTreeState,
        presetGroup.presetSelector,
        presetGroup.buttonSave,
        presetGroup.buttonRemove,
        presetGroup.buttonNext,
        presetGroup.buttonPrev,
        presetGroup.buttonOpen),
    // TODO: how to handle localization?
    tooltipsData(XmlDocument::parse(BinaryData::tooltips_xml)),
    tooltipWindow(this)
{
  LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(
      laf.getDefaultFont().getTypeface());
  setLookAndFeel(&laf);
  tooltipWindow.setLookAndFeel(&laf);
  tooltipWindow.setOpaque(false);

  processor.meterListenersIn[0] =
      ampGroup.levelsGroup.getLevelMeterListenerInL();
  processor.meterListenersIn[1] =
      ampGroup.levelsGroup.getLevelMeterListenerInR();
  processor.meterListenersOut[0] =
      ampGroup.levelsGroup.getLevelMeterListenerOutL();
  processor.meterListenersOut[1] =
      ampGroup.levelsGroup.getLevelMeterListenerOutR();

  ampGroup.attachVTS(vts);
  ampGroup.attachTooltips(tooltipsData);

  for (const auto& parameterId : presetManager.getParameterIds())
  {
    valueTreeState.addParameterListener(parameterId, &presetManager);
    managerListenIds.push_back(parameterId);
  }

  logoSvg = Drawable::createFromSVG(*XmlDocument::parse(BinaryData::logo_svg));

  versionLabel.setText("v" JucePlugin_VersionString, dontSendNotification);
  versionLabel.setColour(Label::textColourId, SwankyAmpLAF::colourDark);
  versionLabel.setFont(16.0f);
  versionLabel.setJustificationType(Justification::centredRight);

  addAndMakeVisible(ampGroup);
  addAndMakeVisible(presetGroup);
  addAndMakeVisible(versionLabel);

  setSize((int)(1.5f * 600 + 0.5f), 600);
}

#undef ATTACH_SLIDER

SwankyAmpAudioProcessorEditor::~SwankyAmpAudioProcessorEditor()
{
  // the LAF can be called from ... who knows where, after the Editor goes out
  // of scope
  setLookAndFeel(nullptr);
  // not sure destruction order could be an issue
  tooltipWindow.setLookAndFeel(nullptr);

  // ensure the processor doesn't used cache pointers that were freed
  processor.meterListenersIn[0] = nullptr;
  processor.meterListenersIn[1] = nullptr;
  processor.meterListenersOut[0] = nullptr;
  processor.meterListenersOut[1] = nullptr;

  for (const auto& parameterId : managerListenIds)
    valueTreeState.removeParameterListener(parameterId, &presetManager);
}

void SwankyAmpAudioProcessorEditor::paint(Graphics& g)
{
  g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

  g.drawImage(bgNoise, getLocalBounds().toFloat());

  const float logoHeight = (float)headerHeight;
  const float logoPadding = (float)headerPadding;

  const float logoRatio =
      (float)logoSvg->getWidth() / (float)logoSvg->getHeight();
  const float logoWidth = logoRatio * (float)headerHeight;
  const float logoX = getBounds().getRight() - logoPadding - logoWidth;
  const float logoY = getBounds().getY() + logoPadding;
  logoSvg->setTransformToFit(
      Rectangle<float>(logoX, logoY, logoWidth, logoHeight),
      RectanglePlacement::centred);
  logoSvg->draw(g, 1.0f);

  g.setColour(Colour::fromHSV(0.0f, 0.0f, 0.0f, 0.1f));
  g.fillPath(bgPattern);
}

#undef GROUP_DROP_SHADOW

void SwankyAmpAudioProcessorEditor::buildBgPattern()
{
  bgPattern.clear();

  const float scale = (float)getLocalBounds().getHeight() * 0.667f;

  const Point<float> corner(getLocalBounds().getBottomRight().toFloat());

  bgPattern.startNewSubPath(corner - Point<float>(scale, 0.0f));
  bgPattern.addCentredArc(
      corner.getX(),
      corner.getY(),
      scale,
      scale,
      0.0f,
      -MathConstants<float>::halfPi,
      0.0f,
      false);
  bgPattern.lineTo(corner - Point<float>(0.0f, scale * 7.0f / 8.0f));
  bgPattern.addCentredArc(
      corner.getX(),
      corner.getY(),
      scale * 7.0f / 8.0f,
      scale * 7.0f / 8.0f,
      0.0f,
      0.0f,
      -MathConstants<float>::halfPi,
      false);
  bgPattern.closeSubPath();

  bgPattern.startNewSubPath(corner - Point<float>(scale * 6.0f / 8.0f, 0.0f));
  bgPattern.addCentredArc(
      corner.getX(),
      corner.getY(),
      scale * 6.0f / 8.0f,
      scale * 6.0f / 8.0f,
      0.0f,
      -MathConstants<float>::halfPi,
      0.0f,
      false);
  bgPattern.lineTo(corner - Point<float>(0.0f, scale * 2.0f / 8.0f));
  bgPattern.addCentredArc(
      corner.getX(),
      corner.getY(),
      scale * 2.0f / 8.0f,
      scale * 2.0f / 8.0f,
      0.0f,
      0.0f,
      -MathConstants<float>::halfPi,
      false);
  bgPattern.closeSubPath();
}

void SwankyAmpAudioProcessorEditor::resized()
{
  // build noise to texture background
  rng.setSeed(1234);
  bgNoise = buildImageNoise(getWidth(), getHeight(), rng, 0.04f);

  ampGroup.setGroupsHeight(128);
  const int marginX = jmax(0, getWidth() - ampGroup.getWidth());
  const int marginY = jmax(0, getHeight() - ampGroup.getHeight());

  ampGroup.setTopLeftPosition(marginX / 2, marginY / 2);

  presetGroup.setHeight(headerHeight + 4);
  presetGroup.setTopLeftPosition(headerPadding - 2, headerPadding - 2);

  const int fontSize = (int)(versionLabel.getFont().getHeight() + 0.5f);
  versionLabel.setSize(fontSize * 6, fontSize);
  versionLabel.setTopLeftPosition(
      getLocalBounds().getRight() - fontSize * 6 - 4,
      getLocalBounds().getBottom() - fontSize - 4);

  // rebuild on re-size to track bottom right corner, could be built once then
  // translated, but in future might want to re-scale as well
  buildBgPattern();
}

void SwankyAmpAudioProcessorEditor::stateChangeCallback()
{
  const String& text = processor.getPresetText();
  if (text.isNotEmpty()) presetManager.setStateText(text);
}
