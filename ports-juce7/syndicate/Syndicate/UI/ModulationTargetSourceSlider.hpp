#pragma once

#include <JuceHeader.h>
#include "ModulationSourceDefinition.hpp"
#include "ChainSlots.hpp"

/**
 * Displays the small modulation source slider below the modulation target. A single target may have
 * multiple slots each representing a different modulation source.
 */
class ModulationTargetSourceSlider : public juce::Slider {
public:

    ModulationTargetSourceSlider(ModulationSourceDefinition definition,
                                 std::function<void(ModulationSourceDefinition)> onRemoveCallback);
    ~ModulationTargetSourceSlider() = default;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

private:
    const ModulationSourceDefinition _definition;
    std::function<void(ModulationSourceDefinition)> _onRemoveCallback;

    std::unique_ptr<juce::Label> _idLabel;
    std::unique_ptr<juce::GlowEffect> _glowEffect;
    bool _isRightClick;
};

class ModulationTargetSourceSliders : public juce::Component,
                                      public juce::Slider::Listener {
public:
    ModulationTargetSourceSliders(
        std::vector<std::shared_ptr<PluginParameterModulationSource>> sources,
        std::function<void(int, float)> onSliderMovedCallback,
        std::function<void(ModulationSourceDefinition)> onRemoveCallback);
    ~ModulationTargetSourceSliders() = default;

    void resized() override;
    void paint(juce::Graphics& g) override;
    void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;

    void addSource(ModulationSourceDefinition definition);
    void removeLastSource();

private:
    std::function<void(int, float)> _onSliderMovedCallback;
    std::function<void(ModulationSourceDefinition)> _onRemoveCallback;
    std::vector<std::unique_ptr<ModulationTargetSourceSlider>> _sliders;

    void _refreshSlotPositions();
};
