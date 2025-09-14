#pragma once

#include <JuceHeader.h>
#include "MacroComponent.h"
#include "ParameterData.h"
class MacrosComponent  : public juce::Component
{
public:
    MacrosComponent(juce::DragAndDropContainer* dragContainer,
                    std::array<juce::AudioParameterFloat*, NUM_MACROS>& macroParams,
                    std::array<juce::String, NUM_MACROS>& macroNames);
    ~MacrosComponent() override;

    void resized() override;

    void onParameterUpdate();

    void updateNames(std::array<juce::String, NUM_MACROS>& macroNames);

private:
    std::vector<std::unique_ptr<MacroComponent>> _macroComponents;
};
