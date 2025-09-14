#pragma once

#include <JuceHeader.h>
#include "UIUtils.h"

class BaseSlotComponent : public juce::Component {
public:
    /**
     * Default construct without a drag handle.
     */
    BaseSlotComponent();

    /**
     * Construct with a drag handle on the left of the slot.
     */
    BaseSlotComponent(int chainNumber, int slotNumber);

    virtual ~BaseSlotComponent() = default;

    /**
     * Returns the area available for the slot to draw in.
     */
    juce::Rectangle<int> getAvailableSlotArea();

    virtual void mouseDrag(const juce::MouseEvent& e) override;

    virtual void resized() override;

    virtual void paint(juce::Graphics& g) override;

private:
    std::unique_ptr<UIUtils::DragHandle> dragHandle;

protected:
    int _chainNumber;
    int _slotNumber;

    static constexpr int MARGIN {1};

    void _drawBackground(juce::Graphics& g) const;
};
