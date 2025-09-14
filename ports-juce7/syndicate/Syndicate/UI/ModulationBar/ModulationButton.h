#pragma once

#include <JuceHeader.h>
#include "ModulationSourceDefinition.hpp"
#include "UIUtils.h"

class ModulationButton : public juce::Component,
                         public juce::Button::Listener {
public:
    const ModulationSourceDefinition definition;

    ModulationButton(ModulationSourceDefinition newDefinition,
                     std::function<void(ModulationButton*)> onSelectCallback,
                     std::function<void()> onRemoveCallback,
                     juce::DragAndDropContainer* dragContainer);

    ~ModulationButton();

    void setIsSelected(bool isSelected);
    bool getIsSelected() const;

    void resized() override;
    void paint(juce::Graphics& g) override;
    void buttonClicked(juce::Button* buttonThatWasClicked) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    class ButtonLookAndFeel : public juce::LookAndFeel_V2 {
    public:
        ButtonLookAndFeel() = default;
        ~ButtonLookAndFeel() = default;

        void drawButtonBackground(juce::Graphics& g,
                                  juce::Button& button,
                                  const juce::Colour& backgroundColour,
                                  bool shouldDrawButtonAsHighlighted,
                                  bool shouldDrawButtonAsDown) override;
    };

    class ModulationSelectButton : public juce::TextButton {
    public:
        ModulationSelectButton(std::function<void()> onRemoveCallback) : _onRemoveCallback(onRemoveCallback), _isRightClick(false) {}

        void mouseDown(const juce::MouseEvent& event) override;
        void mouseUp(const juce::MouseEvent& event) override;

    private:
        std::function<void()> _onRemoveCallback;
        bool _isRightClick;
    };

    std::unique_ptr<ModulationSelectButton> selectButton;
    std::unique_ptr<UIUtils::DragHandle> dragHandle;
    std::function<void(ModulationButton*)> _onSelectCallback;
    juce::DragAndDropContainer* _dragContainer;
    std::unique_ptr<ButtonLookAndFeel> _buttonLookAndFeel;
};
