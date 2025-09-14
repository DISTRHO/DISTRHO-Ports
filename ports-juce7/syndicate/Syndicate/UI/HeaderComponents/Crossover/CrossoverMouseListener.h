#pragma once

#include <array>
#include <optional>

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "ParameterData.h"
#include "MONSTRFilters/MONSTRParameters.h"

class CrossoverMouseListener : public juce::MouseListener {
public:

    CrossoverMouseListener(SyndicateAudioProcessor& processor);
    virtual ~CrossoverMouseListener();

    void mouseDown(const juce::MouseEvent& event) override;

    void mouseDrag(const juce::MouseEvent& event) override;

    void mouseUp(const juce::MouseEvent& event) override;

private:
    SyndicateAudioProcessor& _processor;
    std::optional<std::function<void(const juce::MouseEvent&)>> _dragCallback;

    /**
     * If the mouse event occured inside a button the function will handle it and return null,
     * if the event occured inside a slider it will return the corresponding
     * FloatParameterInteraction for it to be handled by the appropriate event handlers.
     */
    void _resolveParameterInteraction(const juce::MouseEvent& event);
};
