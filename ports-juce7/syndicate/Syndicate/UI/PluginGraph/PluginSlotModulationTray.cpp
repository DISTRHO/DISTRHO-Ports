#include "PluginSlotModulationTray.h"
#include "UIUtils.h"

PluginSlotModulationTray::PluginSlotModulationTray(PluginModulationInterface& pluginModulationInterface,
                                                   int chainNumber,
                                                   int pluginNumber) {
    auto addModulationTarget = [&](int targetNumber) {
        std::unique_ptr<PluginModulationTarget> newTarget =
            std::make_unique<PluginModulationTarget>(pluginModulationInterface, chainNumber, pluginNumber, targetNumber);

        _targetsView->getViewedComponent()->addAndMakeVisible(newTarget.get());
        _modulationTargets.push_back(std::move(newTarget));
    };

    _targetsView.reset(new juce::Viewport());
    _targetsView->setViewedComponent(new juce::Component());
    _targetsView->setScrollBarsShown(false, true);
    _targetsView->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::backgroundColourId, juce::Colour(0x00000000));
    _targetsView->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::thumbColourId, UIUtils::neutralColour.withAlpha(0.5f));
    _targetsView->getHorizontalScrollBar().setColour(juce::ScrollBar::ColourIds::trackColourId, juce::Colour(0x00000000));
    addAndMakeVisible(_targetsView.get());

    // Restore existing targets
    const PluginModulationConfig& modulationConfig =
        pluginModulationInterface.getPluginModulationConfig(chainNumber, pluginNumber);

    for (int index {0}; index < modulationConfig.parameterConfigs.size(); index++) {
        addModulationTarget(index);
    }

    // Add an empty target
    const size_t emptyTargetNumber {modulationConfig.parameterConfigs.size()};
    addModulationTarget(emptyTargetNumber);
}

void PluginSlotModulationTray::resized() {
    juce::Rectangle<int> viewableArea = getLocalBounds().reduced(5);

    _targetsView->setBounds(viewableArea);

    const int targetWidth {static_cast<int>(viewableArea.getHeight() * 0.75)};

    juce::Rectangle<int> scrollableArea(std::max(targetWidth * static_cast<int>(_modulationTargets.size()), viewableArea.getWidth()),
                                        viewableArea.getHeight());
    _targetsView->getViewedComponent()->setBounds(scrollableArea);
    scrollableArea.removeFromBottom(_targetsView->getScrollBarThickness());

    juce::FlexBox flexBox;
    flexBox.flexWrap = juce::FlexBox::Wrap::wrap;
    flexBox.justifyContent = juce::FlexBox::JustifyContent::spaceAround;
    flexBox.alignContent = juce::FlexBox::AlignContent::center;

    for (std::unique_ptr<PluginModulationTarget>& target : _modulationTargets) {
        flexBox.items.add(juce::FlexItem(*target.get()).withMinWidth(targetWidth).withMinHeight(scrollableArea.getHeight()));
    }

    flexBox.performLayout(scrollableArea.toFloat());
}

void PluginSlotModulationTray::paint(juce::Graphics& g) {

    const juce::Rectangle<float> fillArea = getLocalBounds().reduced(1, 1).toFloat();

    g.setColour(UIUtils::modulationTrayBackgroundColour);
    g.fillRoundedRectangle(fillArea, UIUtils::PLUGIN_SLOT_CORNER_RADIUS);
    g.fillRect(
        static_cast<int>(fillArea.getX()),
        0,
        static_cast<int>(fillArea.getWidth()),
        UIUtils::PLUGIN_SLOT_CORNER_RADIUS
    );
}
