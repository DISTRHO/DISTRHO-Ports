#pragma once

#include <JuceHeader.h>
#include "UIUtils.h"
#include "PluginProcessor.h"
#include "MetadataEditComponent.hpp"

class SyndicateAudioProcessorEditor;

class UndoRedoComponent : public juce::Component {
public:
    UndoRedoComponent(SyndicateAudioProcessor& processor);
    ~UndoRedoComponent() override;

    void resized() override;

    void refresh();

private:
    UIUtils::StaticButtonLookAndFeel _buttonLookAndFeel;

    std::unique_ptr<juce::TextButton> _undoButton;
    std::unique_ptr<juce::TextButton> _redoButton;

    SyndicateAudioProcessor& _processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UndoRedoComponent)
};
