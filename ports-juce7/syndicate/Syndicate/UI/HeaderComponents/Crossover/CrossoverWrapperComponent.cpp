#include "CrossoverWrapperComponent.h"

CrossoverWrapperComponent::CrossoverWrapperComponent(SyndicateAudioProcessor& processor) {

    _parameterComponent.reset(new CrossoverParameterComponent(processor));
    addAndMakeVisible(_parameterComponent.get());
    _parameterComponent->setTooltip("Drag the handles to change each band's crossover point");

    _imagerComponent.reset(new CrossoverImagerComponent(processor));
    addAndMakeVisible(_imagerComponent.get());
    _imagerComponent->toBack();

    _mouseListener = std::make_unique<CrossoverMouseListener>(processor);
    _parameterComponent->addMouseListener(_mouseListener.get(), false);
}

void CrossoverWrapperComponent::resized() {
    _parameterComponent->setBounds(getLocalBounds());
    _imagerComponent->setBounds(getLocalBounds());
}

void CrossoverWrapperComponent::onParameterUpdate() {
    _parameterComponent->onParameterUpdate();
}

void CrossoverWrapperComponent::onNumChainsChanged() {
    _parameterComponent->onNumChainsChanged();
}