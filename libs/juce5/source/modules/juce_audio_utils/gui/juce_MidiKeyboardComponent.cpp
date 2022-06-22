/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

class MidiKeyboardUpDownButton  : public Button
{
public:
    MidiKeyboardUpDownButton (MidiKeyboardComponent& comp, const int d)
        : Button (String()), owner (comp), delta (d)
    {
    }

    void clicked() override
    {
        int note = owner.getLowestVisibleKey();

        if (delta < 0)
            note = (note - 1) / 12;
        else
            note = note / 12 + 1;

        owner.setLowestVisibleKey (note * 12);
    }

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        owner.drawUpDownButton (g, getWidth(), getHeight(),
                                isMouseOverButton, isButtonDown,
                                delta > 0);
    }

private:
    MidiKeyboardComponent& owner;
    const int delta;

    JUCE_DECLARE_NON_COPYABLE (MidiKeyboardUpDownButton)
};

//==============================================================================
MidiKeyboardComponent::MidiKeyboardComponent (MidiKeyboardState& s, Orientation o)
    : state (s),
      blackNoteLengthRatio (0.7f),
      xOffset (0),
      keyWidth (16.0f),
      orientation (o),
      midiChannel (1),
      midiInChannelMask (0xffff),
      velocity (1.0f),
      shouldCheckState (false),
      rangeStart (0),
      rangeEnd (127),
      firstKey (12 * 4.0f),
      canScroll (true),
      useMousePositionForVelocity (true),
      shouldCheckMousePos (false),
      keyMappingOctave (6),
      octaveNumForMiddleC (3)
{
    addChildComponent (scrollDown = new MidiKeyboardUpDownButton (*this, -1));
    addChildComponent (scrollUp   = new MidiKeyboardUpDownButton (*this, 1));

    // initialise with a default set of qwerty key-mappings..
    const char* const keymap = "awsedftgyhujkolp;";

    for (int i = 0; keymap[i] != 0; ++i)
        setKeyPressForNote (KeyPress (keymap[i], 0, 0), i);

    mouseOverNotes.insertMultiple (0, -1, 32);
    mouseDownNotes.insertMultiple (0, -1, 32);

    colourChanged();
    setWantsKeyboardFocus (true);

    state.addListener (this);

    startTimerHz (20);
}

MidiKeyboardComponent::~MidiKeyboardComponent()
{
    state.removeListener (this);
}

//==============================================================================
void MidiKeyboardComponent::setKeyWidth (const float widthInPixels)
{
    jassert (widthInPixels > 0);

    if (keyWidth != widthInPixels) // Prevent infinite recursion if the width is being computed in a 'resized()' call-back
    {
        keyWidth = widthInPixels;
        resized();
    }
}

void MidiKeyboardComponent::setOrientation (const Orientation newOrientation)
{
    if (orientation != newOrientation)
    {
        orientation = newOrientation;
        resized();
    }
}

void MidiKeyboardComponent::setAvailableRange (const int lowestNote,
                                               const int highestNote)
{
    jassert (lowestNote >= 0 && lowestNote <= 127);
    jassert (highestNote >= 0 && highestNote <= 127);
    jassert (lowestNote <= highestNote);

    if (rangeStart != lowestNote || rangeEnd != highestNote)
    {
        rangeStart = jlimit (0, 127, lowestNote);
        rangeEnd = jlimit (0, 127, highestNote);
        firstKey = jlimit ((float) rangeStart, (float) rangeEnd, firstKey);
        resized();
    }
}

void MidiKeyboardComponent::setLowestVisibleKey (int noteNumber)
{
    setLowestVisibleKeyFloat ((float) noteNumber);
}

void MidiKeyboardComponent::setLowestVisibleKeyFloat (float noteNumber)
{
    noteNumber = jlimit ((float) rangeStart, (float) rangeEnd, noteNumber);

    if (noteNumber != firstKey)
    {
        const bool hasMoved = (((int) firstKey) != (int) noteNumber);
        firstKey = noteNumber;

        if (hasMoved)
            sendChangeMessage();

        resized();
    }
}

void MidiKeyboardComponent::setScrollButtonsVisible (const bool newCanScroll)
{
    if (canScroll != newCanScroll)
    {
        canScroll = newCanScroll;
        resized();
    }
}

void MidiKeyboardComponent::colourChanged()
{
    setOpaque (findColour (whiteNoteColourId).isOpaque());
    repaint();
}

//==============================================================================
void MidiKeyboardComponent::setMidiChannel (const int midiChannelNumber)
{
    jassert (midiChannelNumber > 0 && midiChannelNumber <= 16);

    if (midiChannel != midiChannelNumber)
    {
        resetAnyKeysInUse();
        midiChannel = jlimit (1, 16, midiChannelNumber);
    }
}

void MidiKeyboardComponent::setMidiChannelsToDisplay (const int midiChannelMask)
{
    midiInChannelMask = midiChannelMask;
    shouldCheckState = true;
}

void MidiKeyboardComponent::setVelocity (const float v, const bool useMousePosition)
{
    velocity = jlimit (0.0f, 1.0f, v);
    useMousePositionForVelocity = useMousePosition;
}

//==============================================================================
void MidiKeyboardComponent::getKeyPosition (int midiNoteNumber, const float keyWidth_, int& x, int& w) const
{
    jassert (midiNoteNumber >= 0 && midiNoteNumber < 128);

    static const float blackNoteWidth = 0.7f;

    static const float notePos[] = { 0.0f, 1 - blackNoteWidth * 0.6f,
                                     1.0f, 2 - blackNoteWidth * 0.4f,
                                     2.0f,
                                     3.0f, 4 - blackNoteWidth * 0.7f,
                                     4.0f, 5 - blackNoteWidth * 0.5f,
                                     5.0f, 6 - blackNoteWidth * 0.3f,
                                     6.0f };

    const int octave = midiNoteNumber / 12;
    const int note   = midiNoteNumber % 12;

    x = roundToInt (octave * 7.0f * keyWidth_ + notePos [note] * keyWidth_);
    w = roundToInt (MidiMessage::isMidiNoteBlack (note) ? blackNoteWidth * keyWidth_ : keyWidth_);
}

void MidiKeyboardComponent::getKeyPos (int midiNoteNumber, int& x, int& w) const
{
    getKeyPosition (midiNoteNumber, keyWidth, x, w);

    int rx, rw;
    getKeyPosition (rangeStart, keyWidth, rx, rw);

    x -= xOffset + rx;
}

Rectangle<int> MidiKeyboardComponent::getRectangleForKey (const int note) const
{
    jassert (note >= rangeStart && note <= rangeEnd);

    int x, w;
    getKeyPos (note, x, w);

    if (MidiMessage::isMidiNoteBlack (note))
    {
        const int blackNoteLength = getBlackNoteLength();

        switch (orientation)
        {
            case horizontalKeyboard:            return Rectangle<int> (x, 0, w, blackNoteLength);
            case verticalKeyboardFacingLeft:    return Rectangle<int> (getWidth() - blackNoteLength, x, blackNoteLength, w);
            case verticalKeyboardFacingRight:   return Rectangle<int> (0, getHeight() - x - w, blackNoteLength, w);
            default:                            jassertfalse; break;
        }
    }
    else
    {
        switch (orientation)
        {
            case horizontalKeyboard:            return Rectangle<int> (x, 0, w, getHeight());
            case verticalKeyboardFacingLeft:    return Rectangle<int> (0, x, getWidth(), w);
            case verticalKeyboardFacingRight:   return Rectangle<int> (0, getHeight() - x - w, getWidth(), w);
            default:                            jassertfalse; break;
        }
    }

    return Rectangle<int>();
}

int MidiKeyboardComponent::getKeyStartPosition (const int midiNoteNumber) const
{
    int x, w;
    getKeyPos (midiNoteNumber, x, w);
    return x;
}

int MidiKeyboardComponent::getTotalKeyboardWidth() const noexcept
{
    int x, w;
    getKeyPos (rangeEnd, x, w);
    return x + w;
}

int MidiKeyboardComponent::getNoteAtPosition (Point<int> p)
{
    float v;
    return xyToNote (p, v);
}

const uint8 MidiKeyboardComponent::whiteNotes[] = { 0, 2, 4, 5, 7, 9, 11 };
const uint8 MidiKeyboardComponent::blackNotes[] = { 1, 3, 6, 8, 10 };

int MidiKeyboardComponent::xyToNote (Point<int> pos, float& mousePositionVelocity)
{
    if (! reallyContains (pos, false))
        return -1;

    Point<int> p (pos);

    if (orientation != horizontalKeyboard)
    {
        p = Point<int> (p.y, p.x);

        if (orientation == verticalKeyboardFacingLeft)
            p = Point<int> (p.x, getWidth() - p.y);
        else
            p = Point<int> (getHeight() - p.x, p.y);
    }

    return remappedXYToNote (p + Point<int> (xOffset, 0), mousePositionVelocity);
}

int MidiKeyboardComponent::remappedXYToNote (Point<int> pos, float& mousePositionVelocity) const
{
    const int blackNoteLength = getBlackNoteLength();

    if (pos.getY() < blackNoteLength)
    {
        for (int octaveStart = 12 * (rangeStart / 12); octaveStart <= rangeEnd; octaveStart += 12)
        {
            for (int i = 0; i < 5; ++i)
            {
                const int note = octaveStart + blackNotes [i];

                if (note >= rangeStart && note <= rangeEnd)
                {
                    int kx, kw;
                    getKeyPos (note, kx, kw);
                    kx += xOffset;

                    if (pos.x >= kx && pos.x < kx + kw)
                    {
                        mousePositionVelocity = pos.y / (float) blackNoteLength;
                        return note;
                    }
                }
            }
        }
    }

    for (int octaveStart = 12 * (rangeStart / 12); octaveStart <= rangeEnd; octaveStart += 12)
    {
        for (int i = 0; i < 7; ++i)
        {
            const int note = octaveStart + whiteNotes [i];

            if (note >= rangeStart && note <= rangeEnd)
            {
                int kx, kw;
                getKeyPos (note, kx, kw);
                kx += xOffset;

                if (pos.x >= kx && pos.x < kx + kw)
                {
                    const int whiteNoteLength = (orientation == horizontalKeyboard) ? getHeight() : getWidth();
                    mousePositionVelocity = pos.y / (float) whiteNoteLength;
                    return note;
                }
            }
        }
    }

    mousePositionVelocity = 0;
    return -1;
}

//==============================================================================
void MidiKeyboardComponent::repaintNote (const int noteNum)
{
    if (noteNum >= rangeStart && noteNum <= rangeEnd)
        repaint (getRectangleForKey (noteNum));
}

void MidiKeyboardComponent::paint (Graphics& g)
{
    g.fillAll (findColour (whiteNoteColourId));

    const Colour lineColour (findColour (keySeparatorLineColourId));
    const Colour textColour (findColour (textLabelColourId));

    for (int octave = 0; octave < 128; octave += 12)
    {
        for (int white = 0; white < 7; ++white)
        {
            const int noteNum = octave + whiteNotes [white];

            if (noteNum >= rangeStart && noteNum <= rangeEnd)
            {
                Rectangle<int> pos = getRectangleForKey (noteNum);

                drawWhiteNote (noteNum, g, pos.getX(), pos.getY(), pos.getWidth(), pos.getHeight(),
                               state.isNoteOnForChannels (midiInChannelMask, noteNum),
                               mouseOverNotes.contains (noteNum), lineColour, textColour);
            }
        }
    }

    float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
    const int width = getWidth();
    const int height = getHeight();

    if (orientation == verticalKeyboardFacingLeft)
    {
        x1 = width - 1.0f;
        x2 = width - 5.0f;
    }
    else if (orientation == verticalKeyboardFacingRight)
        x2 = 5.0f;
    else
        y2 = 5.0f;

    int x, w;
    getKeyPos (rangeEnd, x, w);
    x += w;

    const Colour shadowCol (findColour (shadowColourId));

    if (! shadowCol.isTransparent())
    {
        g.setGradientFill (ColourGradient (shadowCol, x1, y1, shadowCol.withAlpha (0.0f), x2, y2, false));

        switch (orientation)
        {
            case horizontalKeyboard:            g.fillRect (0, 0, x, 5); break;
            case verticalKeyboardFacingLeft:    g.fillRect (width - 5, 0, 5, x); break;
            case verticalKeyboardFacingRight:   g.fillRect (0, 0, 5, x); break;
            default: break;
        }
    }

    if (! lineColour.isTransparent())
    {
        g.setColour (lineColour);

        switch (orientation)
        {
            case horizontalKeyboard:            g.fillRect (0, height - 1, x, 1); break;
            case verticalKeyboardFacingLeft:    g.fillRect (0, 0, 1, x); break;
            case verticalKeyboardFacingRight:   g.fillRect (width - 1, 0, 1, x); break;
            default: break;
        }
    }

    const Colour blackNoteColour (findColour (blackNoteColourId));

    for (int octave = 0; octave < 128; octave += 12)
    {
        for (int black = 0; black < 5; ++black)
        {
            const int noteNum = octave + blackNotes [black];

            if (noteNum >= rangeStart && noteNum <= rangeEnd)
            {
                Rectangle<int> pos = getRectangleForKey (noteNum);

                drawBlackNote (noteNum, g, pos.getX(), pos.getY(), pos.getWidth(), pos.getHeight(),
                               state.isNoteOnForChannels (midiInChannelMask, noteNum),
                               mouseOverNotes.contains (noteNum), blackNoteColour);
            }
        }
    }
}

void MidiKeyboardComponent::drawWhiteNote (int midiNoteNumber,
                                           Graphics& g, int x, int y, int w, int h,
                                           bool isDown, bool isOver,
                                           const Colour& lineColour,
                                           const Colour& textColour)
{
    Colour c (Colours::transparentWhite);

    if (isDown)  c = findColour (keyDownOverlayColourId);
    if (isOver)  c = c.overlaidWith (findColour (mouseOverKeyOverlayColourId));

    g.setColour (c);
    g.fillRect (x, y, w, h);

    const String text (getWhiteNoteText (midiNoteNumber));

    if (text.isNotEmpty())
    {
        const float fontHeight = jmin (12.0f, keyWidth * 0.9f);

        g.setColour (textColour);
        g.setFont (Font (fontHeight).withHorizontalScale (0.8f));

        switch (orientation)
        {
            case horizontalKeyboard:            g.drawText (text, x + 1, y,     w - 1, h - 2, Justification::centredBottom, false); break;
            case verticalKeyboardFacingLeft:    g.drawText (text, x + 2, y + 2, w - 4, h - 4, Justification::centredLeft,   false); break;
            case verticalKeyboardFacingRight:   g.drawText (text, x + 2, y + 2, w - 4, h - 4, Justification::centredRight,  false); break;
            default: break;
        }
    }

    if (! lineColour.isTransparent())
    {
        g.setColour (lineColour);

        switch (orientation)
        {
            case horizontalKeyboard:            g.fillRect (x, y, 1, h); break;
            case verticalKeyboardFacingLeft:    g.fillRect (x, y, w, 1); break;
            case verticalKeyboardFacingRight:   g.fillRect (x, y + h - 1, w, 1); break;
            default: break;
        }

        if (midiNoteNumber == rangeEnd)
        {
            switch (orientation)
            {
                case horizontalKeyboard:            g.fillRect (x + w, y, 1, h); break;
                case verticalKeyboardFacingLeft:    g.fillRect (x, y + h, w, 1); break;
                case verticalKeyboardFacingRight:   g.fillRect (x, y - 1, w, 1); break;
                default: break;
            }
        }
    }
}

void MidiKeyboardComponent::drawBlackNote (int /*midiNoteNumber*/,
                                           Graphics& g, int x, int y, int w, int h,
                                           bool isDown, bool isOver,
                                           const Colour& noteFillColour)
{
    Colour c (noteFillColour);

    if (isDown)  c = c.overlaidWith (findColour (keyDownOverlayColourId));
    if (isOver)  c = c.overlaidWith (findColour (mouseOverKeyOverlayColourId));

    g.setColour (c);
    g.fillRect (x, y, w, h);

    if (isDown)
    {
        g.setColour (noteFillColour);
        g.drawRect (x, y, w, h);
    }
    else
    {
        g.setColour (c.brighter());
        const int xIndent = jmax (1, jmin (w, h) / 8);

        switch (orientation)
        {
            case horizontalKeyboard:            g.fillRect (x + xIndent, y, w - xIndent * 2, 7 * h / 8); break;
            case verticalKeyboardFacingLeft:    g.fillRect (x + w / 8, y + xIndent, w - w / 8, h - xIndent * 2); break;
            case verticalKeyboardFacingRight:   g.fillRect (x, y + xIndent, 7 * w / 8, h - xIndent * 2); break;
            default: break;
        }
    }
}

void MidiKeyboardComponent::setOctaveForMiddleC (const int octaveNum)
{
    octaveNumForMiddleC = octaveNum;
    repaint();
}

String MidiKeyboardComponent::getWhiteNoteText (const int midiNoteNumber)
{
    if (midiNoteNumber % 12 == 0)
        return MidiMessage::getMidiNoteName (midiNoteNumber, true, true, octaveNumForMiddleC);

    return {};
}

void MidiKeyboardComponent::drawUpDownButton (Graphics& g, int w, int h,
                                              const bool mouseOver,
                                              const bool buttonDown,
                                              const bool movesOctavesUp)
{
    g.fillAll (findColour (upDownButtonBackgroundColourId));

    float angle;

    switch (orientation)
    {
        case horizontalKeyboard:            angle = movesOctavesUp ? 0.0f  : 0.5f;  break;
        case verticalKeyboardFacingLeft:    angle = movesOctavesUp ? 0.25f : 0.75f; break;
        case verticalKeyboardFacingRight:   angle = movesOctavesUp ? 0.75f : 0.25f; break;
        default:                            jassertfalse; angle = 0; break;
    }

    Path path;
    path.addTriangle (0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.5f);
    path.applyTransform (AffineTransform::rotation (float_Pi * 2.0f * angle, 0.5f, 0.5f));

    g.setColour (findColour (upDownButtonArrowColourId)
                  .withAlpha (buttonDown ? 1.0f : (mouseOver ? 0.6f : 0.4f)));

    g.fillPath (path, path.getTransformToScaleToFit (1.0f, 1.0f, w - 2.0f, h - 2.0f, true));
}

void MidiKeyboardComponent::setBlackNoteLengthProportion (float ratio) noexcept
{
    jassert (ratio >= 0.0f && ratio <= 1.0f);
    if (blackNoteLengthRatio != ratio)
    {
        blackNoteLengthRatio = ratio;
        resized();
    }
}

int MidiKeyboardComponent::getBlackNoteLength() const noexcept
{
    const int whiteNoteLength = orientation == horizontalKeyboard ? getHeight() : getWidth();

    return roundToInt (whiteNoteLength * blackNoteLengthRatio);
}

void MidiKeyboardComponent::resized()
{
    int w = getWidth();
    int h = getHeight();

    if (w > 0 && h > 0)
    {
        if (orientation != horizontalKeyboard)
            std::swap (w, h);

        int kx2, kw2;
        getKeyPos (rangeEnd, kx2, kw2);

        kx2 += kw2;

        if ((int) firstKey != rangeStart)
        {
            int kx1, kw1;
            getKeyPos (rangeStart, kx1, kw1);

            if (kx2 - kx1 <= w)
            {
                firstKey = (float) rangeStart;
                sendChangeMessage();
                repaint();
            }
        }

        scrollDown->setVisible (canScroll && firstKey > (float) rangeStart);

        xOffset = 0;

        if (canScroll)
        {
            const int scrollButtonW = jmin (12, w / 2);
            Rectangle<int> r (getLocalBounds());

            if (orientation == horizontalKeyboard)
            {
                scrollDown->setBounds (r.removeFromLeft  (scrollButtonW));
                scrollUp  ->setBounds (r.removeFromRight (scrollButtonW));
            }
            else if (orientation == verticalKeyboardFacingLeft)
            {
                scrollDown->setBounds (r.removeFromTop    (scrollButtonW));
                scrollUp  ->setBounds (r.removeFromBottom (scrollButtonW));
            }
            else
            {
                scrollDown->setBounds (r.removeFromBottom (scrollButtonW));
                scrollUp  ->setBounds (r.removeFromTop    (scrollButtonW));
            }

            int endOfLastKey, kw;
            getKeyPos (rangeEnd, endOfLastKey, kw);
            endOfLastKey += kw;

            float mousePositionVelocity;
            const int spaceAvailable = w;
            const int lastStartKey = remappedXYToNote (Point<int> (endOfLastKey - spaceAvailable, 0), mousePositionVelocity) + 1;

            if (lastStartKey >= 0 && ((int) firstKey) > lastStartKey)
            {
                firstKey = (float) jlimit (rangeStart, rangeEnd, lastStartKey);
                sendChangeMessage();
            }

            int newOffset = 0;
            getKeyPos ((int) firstKey, newOffset, kw);
            xOffset = newOffset;
        }
        else
        {
            firstKey = (float) rangeStart;
        }

        getKeyPos (rangeEnd, kx2, kw2);
        scrollUp->setVisible (canScroll && kx2 > w);
        repaint();
    }
}

//==============================================================================
void MidiKeyboardComponent::handleNoteOn (MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/)
{
    shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
}

void MidiKeyboardComponent::handleNoteOff (MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/)
{
    shouldCheckState = true; // (probably being called from the audio thread, so avoid blocking in here)
}

//==============================================================================
void MidiKeyboardComponent::resetAnyKeysInUse()
{
    if (! keysPressed.isZero())
    {
        for (int i = 128; --i >= 0;)
            if (keysPressed[i])
                state.noteOff (midiChannel, i, 0.0f);

        keysPressed.clear();
    }

    for (int i = mouseDownNotes.size(); --i >= 0;)
    {
        const int noteDown = mouseDownNotes.getUnchecked(i);

        if (noteDown >= 0)
        {
            state.noteOff (midiChannel, noteDown, 0.0f);
            mouseDownNotes.set (i, -1);
        }

        mouseOverNotes.set (i, -1);
    }
}

void MidiKeyboardComponent::updateNoteUnderMouse (const MouseEvent& e, bool isDown)
{
    updateNoteUnderMouse (e.getEventRelativeTo (this).getPosition(), isDown, e.source.getIndex());
}

void MidiKeyboardComponent::updateNoteUnderMouse (Point<int> pos, bool isDown, int fingerNum)
{
    float mousePositionVelocity = 0.0f;
    const int newNote = xyToNote (pos, mousePositionVelocity);
    const int oldNote = mouseOverNotes.getUnchecked (fingerNum);
    const int oldNoteDown = mouseDownNotes.getUnchecked (fingerNum);
    const float eventVelocity = useMousePositionForVelocity ? mousePositionVelocity * velocity : 1.0f;

    if (oldNote != newNote)
    {
        repaintNote (oldNote);
        repaintNote (newNote);
        mouseOverNotes.set (fingerNum, newNote);
    }

    if (isDown)
    {
        if (newNote != oldNoteDown)
        {
            if (oldNoteDown >= 0)
            {
                mouseDownNotes.set (fingerNum, -1);

                if (! mouseDownNotes.contains (oldNoteDown))
                    state.noteOff (midiChannel, oldNoteDown, eventVelocity);
            }

            if (newNote >= 0 && ! mouseDownNotes.contains (newNote))
            {
                state.noteOn (midiChannel, newNote, eventVelocity);
                mouseDownNotes.set (fingerNum, newNote);
            }
        }
    }
    else if (oldNoteDown >= 0)
    {
        mouseDownNotes.set (fingerNum, -1);

        if (! mouseDownNotes.contains (oldNoteDown))
            state.noteOff (midiChannel, oldNoteDown, eventVelocity);
    }
}

void MidiKeyboardComponent::mouseMove (const MouseEvent& e)
{
    updateNoteUnderMouse (e, false);
    shouldCheckMousePos = false;
}

void MidiKeyboardComponent::mouseDrag (const MouseEvent& e)
{
    float mousePositionVelocity;
    const int newNote = xyToNote (e.getPosition(), mousePositionVelocity);

    if (newNote >= 0)
        mouseDraggedToKey (newNote, e);

    updateNoteUnderMouse (e, true);
}

bool MidiKeyboardComponent::mouseDownOnKey    (int, const MouseEvent&)  { return true; }
void MidiKeyboardComponent::mouseDraggedToKey (int, const MouseEvent&)  {}
void MidiKeyboardComponent::mouseUpOnKey      (int, const MouseEvent&)  {}

void MidiKeyboardComponent::mouseDown (const MouseEvent& e)
{
    float mousePositionVelocity;
    const int newNote = xyToNote (e.getPosition(), mousePositionVelocity);

    if (newNote >= 0 && mouseDownOnKey (newNote, e))
    {
        updateNoteUnderMouse (e, true);
        shouldCheckMousePos = true;
    }
}

void MidiKeyboardComponent::mouseUp (const MouseEvent& e)
{
    updateNoteUnderMouse (e, false);
    shouldCheckMousePos = false;

    float mousePositionVelocity;
    const int note = xyToNote (e.getPosition(), mousePositionVelocity);
    if (note >= 0)
        mouseUpOnKey (note, e);
}

void MidiKeyboardComponent::mouseEnter (const MouseEvent& e)
{
    updateNoteUnderMouse (e, false);
}

void MidiKeyboardComponent::mouseExit (const MouseEvent& e)
{
    updateNoteUnderMouse (e, false);
}

void MidiKeyboardComponent::mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel)
{
    const float amount = (orientation == horizontalKeyboard && wheel.deltaX != 0)
                            ? wheel.deltaX : (orientation == verticalKeyboardFacingLeft ? wheel.deltaY
                                                                                        : -wheel.deltaY);

    setLowestVisibleKeyFloat (firstKey - amount * keyWidth);
}

void MidiKeyboardComponent::timerCallback()
{
    if (shouldCheckState)
    {
        shouldCheckState = false;

        for (int i = rangeStart; i <= rangeEnd; ++i)
        {
            if (keysCurrentlyDrawnDown[i] != state.isNoteOnForChannels (midiInChannelMask, i))
            {
                keysCurrentlyDrawnDown.setBit (i, state.isNoteOnForChannels (midiInChannelMask, i));
                repaintNote (i);
            }
        }
    }

    if (shouldCheckMousePos)
    {
        for (auto& ms : Desktop::getInstance().getMouseSources())
            if (ms.getComponentUnderMouse() == this || isParentOf (ms.getComponentUnderMouse()))
                updateNoteUnderMouse (getLocalPoint (nullptr, ms.getScreenPosition()).roundToInt(), ms.isDragging(), ms.getIndex());
    }
}

//==============================================================================
void MidiKeyboardComponent::clearKeyMappings()
{
    resetAnyKeysInUse();
    keyPressNotes.clear();
    keyPresses.clear();
}

void MidiKeyboardComponent::setKeyPressForNote (const KeyPress& key, int midiNoteOffsetFromC)
{
    removeKeyPressForNote (midiNoteOffsetFromC);

    keyPressNotes.add (midiNoteOffsetFromC);
    keyPresses.add (key);
}

void MidiKeyboardComponent::removeKeyPressForNote (const int midiNoteOffsetFromC)
{
    for (int i = keyPressNotes.size(); --i >= 0;)
    {
        if (keyPressNotes.getUnchecked (i) == midiNoteOffsetFromC)
        {
            keyPressNotes.remove (i);
            keyPresses.remove (i);
        }
    }
}

void MidiKeyboardComponent::setKeyPressBaseOctave (const int newOctaveNumber)
{
    jassert (newOctaveNumber >= 0 && newOctaveNumber <= 10);

    keyMappingOctave = newOctaveNumber;
}

bool MidiKeyboardComponent::keyStateChanged (const bool /*isKeyDown*/)
{
    bool keyPressUsed = false;

    for (int i = keyPresses.size(); --i >= 0;)
    {
        const int note = 12 * keyMappingOctave + keyPressNotes.getUnchecked (i);

        if (keyPresses.getReference(i).isCurrentlyDown())
        {
            if (! keysPressed [note])
            {
                keysPressed.setBit (note);
                state.noteOn (midiChannel, note, velocity);
                keyPressUsed = true;
            }
        }
        else
        {
            if (keysPressed [note])
            {
                keysPressed.clearBit (note);
                state.noteOff (midiChannel, note, 0.0f);
                keyPressUsed = true;
            }
        }
    }

    return keyPressUsed;
}

bool MidiKeyboardComponent::keyPressed (const KeyPress& key)
{
    return keyPresses.contains (key);
}

void MidiKeyboardComponent::focusLost (FocusChangeType)
{
    resetAnyKeysInUse();
}

} // namespace juce
