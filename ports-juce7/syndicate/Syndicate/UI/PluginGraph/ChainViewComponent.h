#include <JuceHeader.h>
#include "BaseSlotComponent.h"
#include "PluginChain.hpp"
#include "PluginSelectionInterface.h"
#include "PluginModulationInterface.h"

/**
 * Manages a single chain of plugins.
 */
class ChainViewComponent : public juce::Component,
                           public juce::DragAndDropTarget {
public:
    ChainViewComponent(int chainNumber,
                       PluginSelectionInterface& pluginSelectionInterface,
                       PluginModulationInterface& pluginModulationInterface);
    ~ChainViewComponent();

    void setPlugins(std::shared_ptr<PluginChain> newChain);

    void resized() override;

    void paint(juce::Graphics& g) override;

    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragMove(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

    void setScrollPosition(int val) { _viewPort->setViewPosition(0, val); }
    int getScrollPosition() const { return _viewPort->getViewPositionY(); }

private:
    int _chainNumber;
    PluginSelectionInterface& _pluginSelectionInterface;
    PluginModulationInterface& _pluginModulationInterface;
    std::vector<std::unique_ptr<BaseSlotComponent>> _pluginSlots;
    std::unique_ptr<juce::Viewport> _viewPort;
    bool _shouldDrawDragHint;
    int _dragHintSlotNumber;

    int _dragCursorPositionToSlotNumber(juce::Point<int> cursorPosition);

};
