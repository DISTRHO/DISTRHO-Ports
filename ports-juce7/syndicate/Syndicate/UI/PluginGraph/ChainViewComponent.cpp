#include "ChainViewComponent.h"
#include "UIUtils.h"
#include "EmptyPluginSlotComponent.h"
#include "GainStageSlotComponent.h"
#include "PluginSlotComponent.h"
#include "ChainMutators.hpp"

namespace {
    std::tuple<bool, int, int, bool> slotDetailsFromVariant(juce::var variant) {
        bool isValid {false};
        int chainNumber {0};
        int slotNumber {0};
        bool isCopy {false};

        if (variant.isArray() && variant.size() == 3 && variant[0].isInt() && variant[1].isInt() && variant[2].isBool()) {
            isValid = true;
            chainNumber = variant[0];
            slotNumber = variant[1];
            isCopy = variant[2];
        }

        return std::make_tuple(isValid, chainNumber, slotNumber, isCopy);
    }
}

ChainViewComponent::ChainViewComponent(int chainNumber,
                                       PluginSelectionInterface& pluginSelectionInterface,
                                       PluginModulationInterface& pluginModulationInterface) :
        _chainNumber(chainNumber),
        _pluginSelectionInterface(pluginSelectionInterface),
        _pluginModulationInterface(pluginModulationInterface),
        _shouldDrawDragHint(false),
        _dragHintSlotNumber(0) {

    _viewPort.reset(new juce::Viewport());
    _viewPort->setViewedComponent(new juce::Component());
    _viewPort->setScrollBarsShown(true, false);
    _viewPort->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::backgroundColourId, juce::Colour(0x00000000));
    _viewPort->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::thumbColourId, UIUtils::neutralColour.withAlpha(0.5f));
    _viewPort->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::trackColourId, juce::Colour(0x00000000));
    addAndMakeVisible(_viewPort.get());
    _viewPort->setBounds(getLocalBounds());
}

ChainViewComponent::~ChainViewComponent() {
    _viewPort = nullptr;
}

void ChainViewComponent::setPlugins(std::shared_ptr<PluginChain> newChain) {
    // Clear all slots and rebuild the chain
    _pluginSlots.clear();

    for (size_t index {0}; index < ChainMutators::getNumSlots(newChain); index++) {
        // Add the slot
        std::unique_ptr<BaseSlotComponent> newSlot;
        if (_pluginSelectionInterface.isPluginSlot(_chainNumber, index)) {
            newSlot.reset(new PluginSlotComponent(_pluginSelectionInterface, _pluginModulationInterface, _chainNumber, index));
        } else {
            newSlot.reset(new GainStageSlotComponent(_pluginSelectionInterface, _chainNumber, index));
        }

        // Add the component for this slot
        _viewPort->getViewedComponent()->addAndMakeVisible(newSlot.get());
        _pluginSlots.push_back(std::move(newSlot));
    }

    // Add the empty slot at the end
    std::unique_ptr<BaseSlotComponent> newSlot(
        new EmptyPluginSlotComponent(_pluginSelectionInterface, _chainNumber, _pluginSlots.size()));

    _viewPort->getViewedComponent()->addAndMakeVisible(newSlot.get());
    _pluginSlots.push_back(std::move(newSlot));

    resized();
}

void ChainViewComponent::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();
    availableArea.removeFromLeft(1);
    availableArea.removeFromRight(1);

    const int scrollPosition {_viewPort->getViewPositionY()};

    _viewPort->setBounds(availableArea);

    // First pass to calculate the scrollable height
    // We need to do this since we don't know if the modulation tray will be open or not
    // There's always an empty slot so start with that
    constexpr int MARGIN_HEIGHT {2};
    int scrollableHeight {UIUtils::PLUGIN_SLOT_HEIGHT};
    for (int slotNumber {0}; slotNumber < _pluginSlots.size() - 1; slotNumber++) {
        const bool hasModulationTray {_pluginModulationInterface.getPluginModulationConfig(_chainNumber, slotNumber).isActive};
        scrollableHeight += (UIUtils::PLUGIN_SLOT_HEIGHT + MARGIN_HEIGHT +
                             (hasModulationTray ? UIUtils::PLUGIN_SLOT_MOD_TRAY_HEIGHT : 0));
    }

    // Take the full height if the scrollable area is smaller
    scrollableHeight = std::max(availableArea.getHeight(), scrollableHeight);
    const int scrollableWidth {availableArea.getWidth()};
    _viewPort->getViewedComponent()->setBounds(juce::Rectangle<int>(scrollableWidth, scrollableHeight));

    juce::Rectangle<int> scrollableArea = _viewPort->getViewedComponent()->getLocalBounds();

    const int scrollbarWidth {_viewPort->getScrollBarThickness()};

    for (int slotNumber {0}; slotNumber < _pluginSlots.size(); slotNumber++) {
        if (slotNumber < _pluginSlots.size() - 1) {
            const bool hasModulationTray {_pluginModulationInterface.getPluginModulationConfig(_chainNumber, slotNumber).isActive};
            const int slotHeight {
                UIUtils::PLUGIN_SLOT_HEIGHT + (hasModulationTray ? UIUtils::PLUGIN_SLOT_MOD_TRAY_HEIGHT : 0)
            };

            _pluginSlots[slotNumber]->setBounds(scrollableArea.removeFromTop(slotHeight).withTrimmedRight(scrollbarWidth));
            scrollableArea.removeFromTop(MARGIN_HEIGHT);
        } else {
            // The last slot is an empty one
            _pluginSlots[slotNumber]->setBounds(scrollableArea.removeFromTop(UIUtils::PLUGIN_SLOT_HEIGHT).withTrimmedRight(scrollbarWidth));
        }
    }

    // Maintain the previous scroll position
    _viewPort->setViewPosition(0, scrollPosition);
}

void ChainViewComponent::paint(juce::Graphics& g) {
    if (_shouldDrawDragHint) {
        g.setColour(UIUtils::neutralColour);
        const int hintYPos {_dragHintSlotNumber < _pluginSlots.size() ?
            _pluginSlots[_dragHintSlotNumber]->getY() - _viewPort->getViewPositionY() : 0
        };
        g.drawLine(0, hintYPos, getWidth(), hintYPos, 2.0f);
    }
}

bool ChainViewComponent::isInterestedInDragSource(const SourceDetails& dragSourceDetails) {
    auto [isValid, fromChainNumber, fromSlotNumber, isCopy] = slotDetailsFromVariant(dragSourceDetails.description);

    // TODO only interested if slot has actually moved or isCopy

    return isValid;
}

void ChainViewComponent::itemDragEnter(const SourceDetails& dragSourceDetails) {
    auto [isValid, fromChainNumber, fromSlotNumber, isCopy] = slotDetailsFromVariant(dragSourceDetails.description);

    if (isValid) {
        _shouldDrawDragHint = true;
        _dragHintSlotNumber = _dragCursorPositionToSlotNumber(dragSourceDetails.localPosition);
        repaint();
    }
}

void ChainViewComponent::itemDragMove(const SourceDetails& dragSourceDetails) {
    auto [isValid, fromChainNumber, fromSlotNumber, isCopy] = slotDetailsFromVariant(dragSourceDetails.description);

    if (isValid) {
        _dragHintSlotNumber = _dragCursorPositionToSlotNumber(dragSourceDetails.localPosition);
        repaint();
    }
}

void ChainViewComponent::itemDragExit(const SourceDetails& /*dragSourceDetails*/) {
    _shouldDrawDragHint = false;
    repaint();
}

void ChainViewComponent::itemDropped(const SourceDetails& dragSourceDetails) {
    _shouldDrawDragHint = false;
    repaint();

    // Actually move the slot
    auto [isValid, fromChainNumber, fromSlotNumber, isCopy] = slotDetailsFromVariant(dragSourceDetails.description);

    if (isValid) {
        if (isCopy) {
            _pluginSelectionInterface.copySlot(fromChainNumber, fromSlotNumber, _chainNumber, _dragHintSlotNumber);
        } else {
            _pluginSelectionInterface.moveSlot(fromChainNumber, fromSlotNumber, _chainNumber, _dragHintSlotNumber);
        }
    }
}

int ChainViewComponent::_dragCursorPositionToSlotNumber(juce::Point<int> cursorPosition) {
    int retVal {static_cast<int>(_pluginSlots.size() - 1)};

    // Check which component the drag is over
    const int scrollPosition {_viewPort->getViewPositionY()};
    for (size_t slotNumber {0}; slotNumber < _pluginSlots.size(); slotNumber++) {

        if (cursorPosition.getY() < _pluginSlots[slotNumber]->getBottom() - scrollPosition) {
            retVal = slotNumber;
            break;
        }
    }

    return retVal;
}
