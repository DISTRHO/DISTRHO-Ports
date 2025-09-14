#pragma once

#include "JuceHeader.h"

#include <memory>

#include "CrossoverImagerComponent.h"
#include "CrossoverMouseListener.h"
#include "CrossoverParameterComponent.h"

class CrossoverWrapperComponent : public juce::Component {
public:

    CrossoverWrapperComponent(SyndicateAudioProcessor& processor);
    virtual ~CrossoverWrapperComponent() = default;

    void resized() override;

    void onParameterUpdate();

    void onNumChainsChanged();

private:
    std::unique_ptr<CrossoverImagerComponent> _imagerComponent;
    std::unique_ptr<CrossoverParameterComponent> _parameterComponent;
    std::unique_ptr<CrossoverMouseListener> _mouseListener;
};
