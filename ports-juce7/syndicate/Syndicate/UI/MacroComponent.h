#pragma once

#include <JuceHeader.h>
#include "ModulationSourceDefinition.hpp"
#include "UIUtils.h"

class MacroComponent  : public juce::Component,
                        public juce::Slider::Listener,
                        public juce::Label::Listener,
                        public juce::SettableTooltipClient {
public:
    MacroComponent(int macroNumber,
                   juce::DragAndDropContainer* dragContainer,
                   juce::AudioParameterFloat* macroParam,
                   juce::String& macroName);
    ~MacroComponent() override;

    void onParameterUpdate();
    void updateName(juce::String name);

    void resized() override;
    void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;
    void sliderDragStarted(juce::Slider* slider) override;
    void sliderDragEnded(juce::Slider* slider) override;
    void labelTextChanged(juce::Label* labelThatHasChanged) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    juce::DragAndDropContainer* _dragContainer;
    ModulationSourceDefinition _modulationSourceDefinition;
    juce::AudioParameterFloat* _macroParam;
    UIUtils::StandardSliderLookAndFeel _sliderLookAndFeel;
    juce::String& _macroName;

    std::unique_ptr<juce::Slider> macroSld;
    std::unique_ptr<juce::Label> nameLbl;
    std::unique_ptr<UIUtils::DragHandle> dragHandle;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MacroComponent)
};
