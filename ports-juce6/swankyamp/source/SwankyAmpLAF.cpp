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

#include "Components/LevelMeter.h"
#include "Components/ParameterGroup.h"
#include "Components/RButton.h"
#include "Components/RSlider.h"
#include "Utils.h"

#include "SwankyAmpLAF.h"

const Colour SwankyAmpLAF::colourDark =
    Colour::fromHSV(0.0f, 0.0f, 0.25f, 1.0f);
const Colour SwankyAmpLAF::colourGrey =
    Colour::fromHSV(0.0f, 0.0f, 0.75f, 1.0f);
const Colour SwankyAmpLAF::colourBackground =
    Colour::fromHSV(0.0f, 0.0f, 1.0f, 1.0f);
const Colour SwankyAmpLAF::colourHighlight =
    Colour::fromHSV(0.98f, 0.6f, 0.75f, 1.0f);
const Colour SwankyAmpLAF::colourSteel =
    Colour::fromHSV(0.0f, 0.0f, 0.96f, 1.0f);

SwankyAmpLAF::SwankyAmpLAF()
{
  setColour(
      ResizableWindow::backgroundColourId, SwankyAmpLAF::colourBackground);
  setColour(Slider::textBoxTextColourId, SwankyAmpLAF::colourDark);

  setColour(ParameterGroup::borderColourId, SwankyAmpLAF::colourDark);
  setColour(ParameterGroup::steelColourId, SwankyAmpLAF::colourSteel);

  setColour(RSlider::dialColourId, SwankyAmpLAF::colourDark);
  setColour(RSlider::dialOutlineColourId, SwankyAmpLAF::colourGrey);
  setColour(RSlider::dialTextColourId, SwankyAmpLAF::colourGrey);
  setColour(RSlider::dialDotColourId, SwankyAmpLAF::colourHighlight);

  setColour(LevelMeter::outlineColourId, SwankyAmpLAF::colourDark);
  setColour(LevelMeter::backgroundColourId, SwankyAmpLAF::colourBackground);
  setColour(LevelMeter::meterColourId, SwankyAmpLAF::colourGrey);
  setColour(LevelMeter::textColourId, SwankyAmpLAF::colourDark);

  setColour(RButton::buttonColourId, SwankyAmpLAF::colourDark);
  setColour(RButton::textColourId, SwankyAmpLAF::colourGrey);

  setColour(ComboBox::backgroundColourId, SwankyAmpLAF::colourBackground);
  setColour(ComboBox::textColourId, SwankyAmpLAF::colourDark);
  setColour(ComboBox::outlineColourId, SwankyAmpLAF::colourDark);
  setColour(ComboBox::buttonColourId, SwankyAmpLAF::colourDark);
  setColour(ComboBox::arrowColourId, SwankyAmpLAF::colourDark);
  setColour(ComboBox::focusedOutlineColourId, SwankyAmpLAF::colourHighlight);

  setColour(PopupMenu::backgroundColourId, SwankyAmpLAF::colourBackground);
  setColour(PopupMenu::textColourId, SwankyAmpLAF::colourDark);
  setColour(PopupMenu::headerTextColourId, SwankyAmpLAF::colourDark);
  setColour(PopupMenu::highlightedBackgroundColourId, SwankyAmpLAF::colourDark);
  setColour(PopupMenu::highlightedTextColourId, SwankyAmpLAF::colourBackground);

  setColour(TextEditor::backgroundColourId, SwankyAmpLAF::colourBackground);
  setColour(TextEditor::textColourId, SwankyAmpLAF::colourDark);
  setColour(TextEditor::highlightColourId, SwankyAmpLAF::colourGrey);
  setColour(TextEditor::highlightedTextColourId, SwankyAmpLAF::colourDark);
  setColour(TextEditor::outlineColourId, Colours::transparentBlack);
  setColour(TextEditor::focusedOutlineColourId, Colours::transparentBlack);
  setColour(TextEditor::shadowColourId, Colours::transparentBlack);

  setColour(Label::textColourId, SwankyAmpLAF::colourDark);
  setColour(Label::backgroundColourId, Colours::transparentBlack);
  setColour(Label::outlineColourId, Colours::transparentBlack);
  setColour(Label::backgroundWhenEditingColourId, Colours::transparentBlack);
  setColour(Label::textWhenEditingColourId, SwankyAmpLAF::colourDark);
  setColour(Label::outlineWhenEditingColourId, Colours::transparentBlack);

  setColour(TooltipWindow::backgroundColourId, SwankyAmpLAF::colourBackground);
  setColour(TooltipWindow::textColourId, SwankyAmpLAF::colourDark);
  setColour(TooltipWindow::outlineColourId, SwankyAmpLAF::colourDark);
}

const Font& SwankyAmpLAF::getDefaultFont()
{
  const static Font font = Font(Typeface::createSystemTypefaceFor(
      BinaryData::PTSansRegular_ttf, BinaryData::PTSansRegular_ttfSize));

  return font;
}

const Font& SwankyAmpLAF::getDefaultFontNarrow()
{
  const static Font font = Font(Typeface::createSystemTypefaceFor(
      BinaryData::PTSansNarrowRegular_ttf,
      BinaryData::PTSansNarrowRegular_ttfSize));

  return font;
}

const Font& SwankyAmpLAF::getDefaultFontBold()
{
  const static Font font = Font(Typeface::createSystemTypefaceFor(
      BinaryData::PTSansBold_ttf, BinaryData::PTSansBold_ttfSize));

  return font;
}

const DropShadow& SwankyAmpLAF::getDropShadow()
{
  const static DropShadow dropShadow(
      Colour::fromHSV(0.0f, 0.0f, 0.0f, 0.5), 6, Point<int>(1, 1));
  return dropShadow;
}

Path SwankyAmpLAF::getSaveIconPath(float d, float stroke)
{
  Path path;

  path.startNewSubPath(0.5f, 0.0f);
  path.lineTo(0.875f, 0.0f);
  path.lineTo(1.0f, 0.125f);
  path.lineTo(1.0f, 0.875f);
  path.addCentredArc(
      0.875f,
      0.875f,
      0.125f,
      0.125f,
      0.0f,
      MathConstants<float>::halfPi,
      MathConstants<float>::pi);
  path.lineTo(0.125f, 1.0f);
  path.addCentredArc(
      0.125f,
      0.875f,
      0.125f,
      0.125f,
      0.0f,
      MathConstants<float>::pi,
      3.0f * MathConstants<float>::halfPi);
  path.lineTo(0.0f, 0.125f);
  path.addCentredArc(
      0.125f,
      0.125f,
      0.125f,
      0.125f,
      0.0f,
      3.0f * MathConstants<float>::halfPi,
      2.0f * MathConstants<float>::pi);
  path.closeSubPath();

  path.startNewSubPath(0.25f, 0.0f);
  path.lineTo(0.25f, 2.0f / 5.0f);
  path.lineTo(0.75f, 2.0f / 5.0f);
  path.lineTo(0.75f, 0.0f);

  path.startNewSubPath(0.75f - 1.0f / 6.0f, 2.0f / 5.0f);
  path.lineTo(0.75f - 1.0f / 6.0f, 0.0f);

  const float pad = stroke / 2.0f;
  path.scaleToFit(0.0f + pad, 0.0f + pad, d - stroke, d - stroke, true);
  return path;
}

Path SwankyAmpLAF::getRemoveIconPath(float d, float stroke)
{
  Path path;

  path.addRoundedRectangle(0.0f, 0.0f, 1.0f, 1.0f, 0.125f, 0.125f);

  path.startNewSubPath(0.25f, 0.25f);
  path.lineTo(0.75f, 0.75f);
  path.startNewSubPath(0.25f, 0.75f);
  path.lineTo(0.75f, 0.25f);

  const float pad = stroke / 2.0f;
  path.scaleToFit(0.0f + pad, 0.0f + pad, d - stroke, d - stroke, true);
  return path;
}

Path SwankyAmpLAF::getNextIconPath(float d, float stroke)
{
  Path path;

  path.addRoundedRectangle(0.0f, 0.0f, 1.0f, 1.0f, 0.125f, 0.125f);

  path.startNewSubPath(0.333f, 0.25f);
  path.lineTo(0.667f, 0.5f);
  path.lineTo(0.333f, 0.75f);

  const float pad = stroke / 2.0f;
  path.scaleToFit(0.0f + pad, 0.0f + pad, d - stroke, d - stroke, true);
  return path;
}

Path SwankyAmpLAF::getPrevIconPath(float d, float stroke)
{
  Path path;

  path.addRoundedRectangle(0.0f, 0.0f, 1.0f, 1.0f, 0.125f, 0.125f);

  path.startNewSubPath(0.667f, 0.25f);
  path.lineTo(0.333f, 0.5f);
  path.lineTo(0.667f, 0.75f);

  const float pad = stroke / 2.0f;
  path.scaleToFit(0.0f + pad, 0.0f + pad, d - stroke, d - stroke, true);
  return path;
}

Path SwankyAmpLAF::getOpenIconPath(float d, float stroke)
{
  Path path;

  path.startNewSubPath(1.0f, 0.5f);
  path.lineTo(1.0f, 0.875f);
  path.addCentredArc(
      0.875f,
      0.875f,
      0.125f,
      0.125f,
      0.0f,
      MathConstants<float>::halfPi,
      2.0f * MathConstants<float>::halfPi);
  path.lineTo(0.125f, 1.0f);
  path.addCentredArc(
      0.125f,
      0.875f,
      0.125f,
      0.125f,
      0.0f,
      2.0f * MathConstants<float>::halfPi,
      3.0f * MathConstants<float>::halfPi);
  path.lineTo(0.0f, 0.125f);
  path.addCentredArc(
      0.125f,
      0.125f,
      0.125f,
      0.125f,
      0.0f,
      3.0f * MathConstants<float>::halfPi,
      4.0f * MathConstants<float>::halfPi);
  path.lineTo(0.5f, 0.0f);

  path.startNewSubPath(0.5f, 0.5f);
  path.lineTo(1.0f, 0.0f);

  path.startNewSubPath(0.75f, 0.0f);
  path.lineTo(1.0f, 0.0f);
  path.lineTo(1.0f, 0.25f);

  const float pad = stroke / 2.0f;
  path.scaleToFit(0.0f + pad, 0.0f + pad, d - stroke, d - stroke, true);
  return path;
}

Path SwankyAmpLAF::getTickShape(float height)
{
  const float padding = height / 5.0f;

  Path path;

  // explicitely add padding to the path otherwise it will get resized, this
  // is kind of nasty (e.g. if someones tries to draw instead of fill), but it
  // seems the alternative is to re-implemment the entire popup draw
  path.startNewSubPath(0, height / 2.0f);
  path.lineTo(padding, height / 2.0f);
  path.startNewSubPath(height, height / 2.0f);
  path.lineTo(height - padding, height / 2.0f);

  path.addEllipse(
      padding, padding, height - 2.0f * padding, height - 2.0f * padding);

  return path;
}

Slider::SliderLayout SwankyAmpLAF::getSliderLayout(Slider& slider)
{
  if (!slider.isRotary() && slider.getTextBoxPosition() != Slider::TextBoxBelow)
    return LookAndFeel_V4::getSliderLayout(slider);

  auto localBounds = slider.getLocalBounds();

  Slider::SliderLayout layout;
  layout.textBoxBounds = localBounds;
  layout.sliderBounds = localBounds;

  return layout;
}

void SwankyAmpLAF::drawRotarySlider(
    Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    const float rotaryStartAngle,
    const float rotaryEndAngle,
    Slider& slider)
{
  // should make this rslider specific, then move the cast to the paint call
  // of the rslider (cast the LAF)
  RSlider* rslider = dynamic_cast<RSlider*>(&slider);

  if (!rslider)
    return LookAndFeel_V4::drawRotarySlider(
        g,
        x,
        y,
        width,
        height,
        sliderPos,
        rotaryStartAngle,
        rotaryEndAngle,
        slider);

  const RSliderDims dims = rslider->calcDims();

  const auto sliderAngle = angleModulo(
      rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle));

  // dial

  auto dialBounds = Rectangle<float>();
  dialBounds.setSize(dims.radius * 2.0f, dims.radius * 2.0f);
  dialBounds.setX(dims.centre.getX() - dims.radius);
  dialBounds.setY(dims.centre.getY() - dims.radius);

  Path dialPath;
  dialPath.addCentredArc(
      dims.centre.getX(),
      dims.centre.getY(),
      dims.radius,
      dims.radius,
      0.0f,
      -dims.theta,
      dims.theta,
      true);
  dialPath.closeSubPath();

  // dots outline

  const float dotSize = dims.margin - 2.0f * dims.gap;

  g.setColour(findColour(RSlider::dialOutlineColourId));

  for (int iangle = 1; iangle < 10; iangle++)
  {
    const float angle = dims.theta - iangle * dims.theta / 5.0f;
    const auto dotCenter = dialBounds.getCentre()
        - (dims.radius + dims.margin / 2.0f)
            * Point<float>(sinf(angle), cosf(angle));
    auto dotBounds = Rectangle<float>();
    dotBounds.setCentre(dotCenter);
    dotBounds.translate(-dotSize / 2.0f, -dotSize / 2.0f);
    dotBounds.setSize(dotSize, dotSize);
    g.fillEllipse(dotBounds);
  }

  const auto overAngle = rslider->getOverValue() >= 0.0f
      ? angleModulo(
          rotaryStartAngle
          + rslider->getOverValue() * (rotaryEndAngle - rotaryStartAngle))
      : MathConstants<float>::pi;

  // show range
  if (rslider->getOverValue() >= 0)
  {
    auto rangePath = Path();
    rangePath.startNewSubPath(dialBounds.getCentre());
    rangePath.addCentredArc(
        dialBounds.getCentreX(),
        dialBounds.getCentreY(),
        dims.radius + dims.margin,
        dims.radius + dims.margin,
        0.0f,
        overAngle,
        dims.theta);
    rangePath.addCentredArc(
        dialBounds.getCentreX(),
        dialBounds.getCentreY(),
        dims.radius,
        dims.radius,
        0.0f,
        dims.theta,
        overAngle);
    rangePath.closeSubPath();

    g.setColour(
        findColour(RSlider::dialDotColourId).withMultipliedSaturation(0.45f));
    g.fillPath(rangePath);
  }

  // dots filled

  auto dotClipPath = Path();
  dotClipPath.startNewSubPath(dialBounds.getCentre());
  dotClipPath.addCentredArc(
      dialBounds.getCentreX(),
      dialBounds.getCentreY(),
      dims.radius + dims.margin,
      dims.radius + dims.margin,
      0.0f,
      -dims.theta,
      jmin(overAngle, sliderAngle));
  dotClipPath.closeSubPath();

  g.setColour(findColour(RSlider::dialDotColourId));
  g.saveState();
  g.reduceClipRegion(dotClipPath);

  for (int iangle = 1; iangle < 10; iangle++)
  {
    const float angle = dims.theta - iangle * dims.theta / 5.0f;
    const auto dotCenter = dialBounds.getCentre()
        - (dims.radius + dims.margin / 2.0f)
            * Point<float>(sinf(angle), cosf(angle));
    auto dotBounds = Rectangle<float>();
    dotBounds.setCentre(dotCenter);
    dotBounds.translate(-dotSize / 2.0f, -dotSize / 2.0f);
    dotBounds.setSize(dotSize, dotSize);
    g.fillEllipse(dotBounds);
  }

  g.restoreState();

  // dots filled range

  if (sliderAngle > overAngle)
  {
    auto overClipPath = Path();
    overClipPath.startNewSubPath(dialBounds.getCentre());
    overClipPath.addCentredArc(
        dialBounds.getCentreX(),
        dialBounds.getCentreY(),
        dims.radius + dims.margin,
        dims.radius + dims.margin,
        0.0f,
        overAngle,
        sliderAngle);
    overClipPath.closeSubPath();

    g.setColour(findColour(RSlider::dialColourId));
    g.saveState();
    g.reduceClipRegion(overClipPath);

    for (int iangle = 1; iangle < 10; iangle++)
    {
      const float angle = dims.theta - iangle * dims.theta / 5.0f;
      const auto dotCenter = dialBounds.getCentre()
          - (dims.radius + dims.margin / 2.0f)
              * Point<float>(sinf(angle), cosf(angle));
      auto dotBounds = Rectangle<float>();
      dotBounds.setCentre(dotCenter);
      dotBounds.translate(-dotSize / 2.0f, -dotSize / 2.0f);
      dotBounds.setSize(dotSize, dotSize);
      g.fillEllipse(dotBounds);
    }

    g.restoreState();
  }

  // Shadow
  SwankyAmpLAF::getDropShadow().drawForPath(g, dialPath);

  g.setColour(findColour(RSlider::dialColourId));
  g.fillPath(dialPath);

  // texture the dial

  ColourGradient gradient;
  gradient.addColour(0.0f, Colour::fromHSV(0.0f, 0.0f, 1.0f, 0.1f));
  gradient.addColour(0.75f, Colour::fromHSV(0.0f, 0.0f, 1.0f, 0.0f));
  gradient.point1 = dialBounds.getTopLeft();
  gradient.point2 = dialBounds.getBottomLeft();
  g.setGradientFill(gradient);
  g.fillPath(dialPath);

  g.saveState();
  g.reduceClipRegion(dialPath);
  g.drawImage(rslider->getBgNoise(), slider.getLocalBounds().toFloat());
  g.restoreState();

  // dial outline

  g.setColour(findColour(RSlider::dialOutlineColourId));

  Path dialOutlinePath;
  dialOutlinePath.addCentredArc(
      dims.centre.getX(),
      dims.centre.getY(),
      dims.radius - 1.5f,
      dims.radius - 1.5f,
      0.0f,
      -dims.theta,
      dims.theta,
      true);
  dialOutlinePath.closeSubPath();
  g.strokePath(dialOutlinePath, PathStrokeType(1.0f));

  // label text

  const String labelText = rslider->fmtSliderPos(sliderPos);
  g.setFont(
      SwankyAmpLAF::getDefaultFontNarrow().withHeight(dims.radius / 1.5f));
  g.setColour(findColour(RSlider::dialTextColourId));
  g.drawText(labelText, dialBounds, Justification::centred, true);

  // tick

  Path pointerPath;
  auto pointerLength = dims.radius * 0.33f;
  auto lineThickness = 2.0f;
  pointerPath.addRoundedRectangle(
      -lineThickness * 2.0f * 0.5f,
      -dims.radius,
      lineThickness * 2.0f,
      pointerLength,
      lineThickness);
  pointerPath.applyTransform(AffineTransform::rotation(sliderAngle)
                                 .translated(dialBounds.getCentre()));
  g.setColour(findColour(RSlider::dialOutlineColourId));
  g.fillPath(pointerPath);
}

void SwankyAmpLAF::drawToggleButton(
    Graphics& g,
    ToggleButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
  RButton* rbutton = dynamic_cast<RButton*>(&button);

  if (!rbutton)
    return LookAndFeel_V4::drawToggleButton(
        g, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

  ignoreUnused(shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

  const float lineWidth = 2.0f;

  const float radiusForHeight =
      (float)g.getClipBounds().getHeight() / 4.0f - lineWidth;
  const float radiusForWidth =
      (float)g.getClipBounds().getWidth() / 2.0f - lineWidth;

  const float radius = jmin(radiusForHeight, radiusForWidth);

  Rectangle<float> outer;
  outer.setSize(2.0f * radius, 4.0f * radius);
  outer.setPosition(
      g.getClipBounds().getCentreX() - radius,
      g.getClipBounds().getCentreY() - 2.0f * radius);

  // draw the circle

  Rectangle<float> circleBounds;
  circleBounds.setSize(2.0f * radius, 2.0f * radius);
  if (button.getToggleState())
    circleBounds.setPosition(outer.getTopLeft());
  else
    circleBounds.setPosition(
        outer.getTopLeft().translated(0.0f, 2.0f * radius));

  Path circlePath;
  circlePath.addEllipse(circleBounds);

  g.setColour(findColour(RButton::buttonColourId));
  g.fillPath(circlePath);

  // texture the circle

  ColourGradient gradient;
  gradient.addColour(0.0f, Colour::fromHSV(0.0f, 0.0f, 1.0f, 0.1f));
  gradient.addColour(0.75f, Colour::fromHSV(0.0f, 0.0f, 1.0f, 0.0f));
  gradient.point1 = circleBounds.getTopLeft();
  gradient.point2 = circleBounds.getBottomLeft();
  g.setGradientFill(gradient);
  g.fillPath(circlePath);

  g.saveState();
  g.reduceClipRegion(circlePath);
  g.drawImage(rbutton->getBgNoise(), button.getLocalBounds().toFloat());
  g.restoreState();

  // draw the text

  g.setColour(findColour(RButton::textColourId));
  g.setFont(SwankyAmpLAF::getDefaultFontNarrow().withHeight(radius));
  g.drawText(
      button.getToggleState() ? rbutton->textOn : rbutton->textOff,
      circleBounds,
      Justification::centred,
      true);

  g.setColour(findColour(RButton::buttonColourId));
  g.drawRoundedRectangle(outer, radius, lineWidth);
}

void SwankyAmpLAF::drawComboBox(
    Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box)
{
  const float lineThickness = 2.0f;
  const float cornerSize = 2.0f;
  Rectangle<int> boxBounds(0, 0, width, height);

  g.setColour(box.findColour(ComboBox::backgroundColourId));
  g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

  g.setColour(box.findColour(ComboBox::outlineColourId));
  g.drawRoundedRectangle(
      boxBounds.toFloat().reduced(lineThickness / 2.0f, lineThickness / 2.0f),
      cornerSize,
      lineThickness);

  Rectangle<float> arrowZone(
      (float)(width - height), 0.0f, (float)height, (float)height);
  arrowZone.reduce(height / 3.0f, height / 2.5f);

  Path path;
  path.startNewSubPath(arrowZone.getTopLeft());
  path.lineTo(arrowZone.getCentre().withY(arrowZone.getBottom()));
  path.lineTo(arrowZone.getTopRight());

  g.setColour(box.findColour(ComboBox::arrowColourId)
                  .withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
  g.strokePath(path, PathStrokeType(2.0f));
}

Rectangle<int> SwankyAmpLAF::getTooltipBounds(
    const String&, Point<int>, Rectangle<int> parentArea)
{
  const int lineHeight = 16;
  const int margin = lineHeight;
  const int padding = 4;
  const int shadowMargin = getDropShadow().radius;

  Rectangle<int> area(parentArea.withPosition(0, 0));
  area.setBottom(area.getBottom() - margin);
  area.setTop(
      area.getBottom() - 2 * lineHeight - 2 * padding - 2 * shadowMargin);
  area.setLeft((int)(area.getWidth() * 0.4f + 0.5f));
  area.setRight(area.getRight() - margin);

  return area;
}

void SwankyAmpLAF::drawTooltip(
    Graphics& g, const String& text, int width, int height)
{
  const float lineHeight = 16.0f;
  const float lineThickness = 1.0f;
  const float padding = 4.0f;

  const DropShadow& shadow = getDropShadow();
  Rectangle<int> areaShadow(width, height);
  Rectangle<float> area = areaShadow.reduced(shadow.radius).toFloat();

  shadow.drawForRectangle(g, areaShadow.reduced(shadow.radius));

  g.setColour(findColour(TooltipWindow::backgroundColourId));
  g.fillRect(area);

  g.setColour(findColour(TooltipWindow::outlineColourId));
  g.drawRect(area, lineThickness);

  AttributedString text2;
  text2.setJustification(Justification::centredLeft);
  text2.append(text, Font(lineHeight), findColour(TooltipWindow::textColourId));

  TextLayout text3;
  text3.createLayout(text2, (float)(area.reduced(padding).getWidth()));
  text3.draw(g, area.reduced(padding));
}
