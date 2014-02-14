/*
 modified version of juceCalloutbox
*/
#include "CabbageCallOutBox.h"

CabbageCallOutBox::CabbageCallOutBox (Component& c, const Rectangle<int>& area, Component* const parent)
    : borderSpace (2), arrowSize (1.0f), content (c)
{
    addAndMakeVisible (&content);

    if (parent != nullptr)
    {
        parent->addChildComponent (this);
        updatePosition (area, parent->getLocalBounds());
        setVisible (true);
    }
    else
    {
        setAlwaysOnTop (true);
        updatePosition (area, Desktop::getInstance().getDisplays()
                                .getDisplayContaining (area.getCentre()).userArea);

        addToDesktop (ComponentPeer::windowIsTemporary);
    }
}

CabbageCallOutBox::~CabbageCallOutBox()
{
}

//==============================================================================
class CallOutBoxCallback  : public ModalComponentManager::Callback
{
public:
    CallOutBoxCallback (Component* c, const Rectangle<int>& area, Component* parent)
        : content (c), callout (*c, area, parent)
    {
        callout.setVisible (true);
        callout.enterModalState (true, this);
    }

    void modalStateFinished (int) {}

    ScopedPointer<Component> content;
    CabbageCallOutBox callout;

    JUCE_DECLARE_NON_COPYABLE (CallOutBoxCallback)
};

CabbageCallOutBox& CabbageCallOutBox::launchAsynchronously (Component* content,
                                              const Rectangle<int>& area,
                                              Component* parent)
{
    jassert (content != nullptr); // must be a valid content component!

    return (new CallOutBoxCallback (content, area, parent))->callout;
}

//==============================================================================

void CabbageCallOutBox::paint (Graphics& g)
{
g.fillAll(Colours::black);
//    getLookAndFeel().drawCallOutBoxBackground (*this, g, outline, background);
}

void CabbageCallOutBox::resized()
{
    content.setTopLeftPosition (0, 0);
    refreshPath();
}

void CabbageCallOutBox::moved()
{
    refreshPath();
}

void CabbageCallOutBox::childBoundsChanged (Component*)
{
    updatePosition (targetArea, availableArea);
}

bool CabbageCallOutBox::hitTest (int x, int y)
{
    return outline.contains ((float) x, (float) y);
}

enum { callOutBoxDismissCommandId = 0x4f83a04b };

void CabbageCallOutBox::inputAttemptWhenModal()
{
    const Point<int> mousePos (getMouseXYRelative() + getBounds().getPosition());

    if (targetArea.contains (mousePos))
    {
        // if you click on the area that originally popped-up the callout, you expect it
        // to get rid of the box, but deleting the box here allows the click to pass through and
        // probably re-trigger it, so we need to dismiss the box asynchronously to consume the click..
        postCommandMessage (callOutBoxDismissCommandId);
    }
    else
    {
        exitModalState (0);
        setVisible (false);
    }
}

void CabbageCallOutBox::handleCommandMessage (int commandId)
{
    Component::handleCommandMessage (commandId);

    if (commandId == callOutBoxDismissCommandId)
    {
        exitModalState (0);
        setVisible (false);
    }
}

bool CabbageCallOutBox::keyPressed (const KeyPress& key)
{
    if (key.isKeyCode (KeyPress::escapeKey))
    {
        inputAttemptWhenModal();
        return true;
    }

    return false;
}

void CabbageCallOutBox::updatePosition (const Rectangle<int>& newAreaToPointTo, const Rectangle<int>& newAreaToFitIn)
{
    targetArea = newAreaToPointTo;
    availableArea = newAreaToFitIn;

    Rectangle<int> newBounds (content.getWidth(),
                              content.getHeight());

    const int hw = newBounds.getWidth() / 2;
    const int hh = newBounds.getHeight() / 2;
    const float hwReduced = 0;//(float) (hw - borderSpace);
    const float hhReduced = 0;//(float) (hh - borderSpace);
    const float arrowIndent = 0;//borderSpace - arrowSize;

    Point<float> targets[4] = { Point<float> ((float) targetArea.getCentreX(), (float) targetArea.getBottom()),
                                Point<float> ((float) targetArea.getRight(),   (float) targetArea.getCentreY()),
                                Point<float> ((float) targetArea.getX(),       (float) targetArea.getCentreY()),
                                Point<float> ((float) targetArea.getCentreX(), (float) targetArea.getY()) };

    Line<float> lines[4] = { Line<float> (targets[0].translated (-hwReduced, hh - arrowIndent),    targets[0].translated (hwReduced, hh - arrowIndent)),
                             Line<float> (targets[1].translated (hw - arrowIndent, -hhReduced),    targets[1].translated (hw - arrowIndent, hhReduced)),
                             Line<float> (targets[2].translated (-(hw - arrowIndent), -hhReduced), targets[2].translated (-(hw - arrowIndent), hhReduced)),
                             Line<float> (targets[3].translated (-hwReduced, -(hh - arrowIndent)), targets[3].translated (hwReduced, -(hh - arrowIndent))) };

    const Rectangle<float> centrePointArea (newAreaToFitIn.reduced (hw, hh).toFloat());
    const Point<float> targetCentre (targetArea.getCentre().toFloat());

    float nearest = 1.0e9f;

    for (int i = 0; i < 4; ++i)
    {
        Line<float> constrainedLine (centrePointArea.getConstrainedPoint (lines[i].getStart()),
                                     centrePointArea.getConstrainedPoint (lines[i].getEnd()));

        const Point<float> centre (constrainedLine.findNearestPointTo (targetCentre));
        float distanceFromCentre = centre.getDistanceFrom (targets[i]);

        if (! (centrePointArea.contains (lines[i].getStart()) || centrePointArea.contains (lines[i].getEnd())))
            distanceFromCentre += 1000.0f;

        if (distanceFromCentre < nearest)
        {
            nearest = distanceFromCentre;

            targetPoint = targets[i];
            newBounds.setPosition ((int) (centre.x - hw),
                                   (int) (centre.y - hh));
        }
    }

    setBounds (newBounds);
}

void CabbageCallOutBox::refreshPath()
{
    repaint();
    background = Image::null;
    outline.clear();

	outline.addRectangle(1, 1, getWidth()-2, getHeight()-2);
/*
    const float gap = 4.5f;

    outline.addBubble (content.getBounds().toFloat().expanded (gap, gap),
                       getLocalBounds().toFloat(),
                       targetPoint - getPosition().toFloat(),
                       9.0*/
}
