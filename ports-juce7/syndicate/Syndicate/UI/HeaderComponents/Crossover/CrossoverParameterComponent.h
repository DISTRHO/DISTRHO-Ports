#pragma once

#include <array>
#include <memory>
#include <optional>

#include "JuceHeader.h"
#include "PluginProcessor.h"

/**
 * Draws the clickable parts of the crossover component.
 */
class CrossoverParameterComponent : public juce::Component,
                                    public juce::SettableTooltipClient {
public:

    CrossoverParameterComponent(SyndicateAudioProcessor& processor);
    virtual ~CrossoverParameterComponent() = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void onParameterUpdate() {
        _resizeChainLabels();
        _setLabelsText();
        repaint();
    }

    void onNumChainsChanged();

private:
    SyndicateAudioProcessor& _processor;
    std::vector<std::unique_ptr<juce::Label>> _chainLabels;
    std::vector<std::unique_ptr<juce::Label>> _secondaryLabels;

    void _drawSliderThumbs(juce::Graphics& g);
    void _drawFrequencyText(juce::Graphics& g);

    void _rebuildChainLabels();
    void _resizeChainLabels();
    void _setLabelsText();
};