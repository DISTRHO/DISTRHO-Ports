/*
  ==============================================================================

    PluginSelectorList.cpp
    Created: 23 May 2021 12:39:35am
    Author:  Jack Devlin

  ==============================================================================
*/

#include "PluginSelectorList.h"

#include "PluginScanStatusMessage.h"

namespace {
    enum COLUMN_ID {
        NULL_ID, // Can't use 0 as a column ID
        NAME,
        MANUFACTURER,
        CATEGORY,
        FORMAT
    };
}

PluginListSorter::PluginListSorter(PluginSelectorState& state) : state(state) {
}

void PluginListSorter::setPluginList(juce::Array<juce::PluginDescription> pluginList) {
    _fullPluginList = pluginList;
}

juce::Array<juce::PluginDescription> PluginListSorter::getFilteredPluginList() const {
    juce::Array<juce::PluginDescription> filteredPluginList;

    // Apply the user's filters and don't display instruments
    for (const juce::PluginDescription& thisPlugin : _fullPluginList) {
        if (_passesFilter(thisPlugin) && !thisPlugin.isInstrument) {
            filteredPluginList.add(thisPlugin);
        }
    }

    // Now sort the list
    filteredPluginList.sort(*this, false);

    return filteredPluginList;
}

int PluginListSorter::compareElements(juce::PluginDescription first, juce::PluginDescription second) const {

    int retVal {0};

    switch (state.sortColumnId) {
        case NAME:
            retVal = first.name.compare(second.name);
            break;
        case MANUFACTURER:
            retVal = first.manufacturerName.compare(second.manufacturerName);
            break;
        case CATEGORY:
            retVal = first.category.compare(second.category);
            break;
        case FORMAT:
            retVal = first.pluginFormatName.compare(second.pluginFormatName);
            break;
    }

    return retVal * (state.sortForwards ? 1 : -1);
}

bool PluginListSorter::_passesFilter(const juce::PluginDescription& plugin) const {
    const bool passesTextFilter {
        plugin.name.containsIgnoreCase(state.filterString) ||
        plugin.manufacturerName.containsIgnoreCase(state.filterString) ||
        state.filterString.isEmpty()
    };

    const bool passesFormatFilter {
        (plugin.pluginFormatName == "VST" && state.includeVST) ||
        (plugin.pluginFormatName == "VST3" && state.includeVST3) ||
        (plugin.pluginFormatName == "AudioUnit" && state.includeAU)
    };

    return passesTextFilter && passesFormatFilter;
}

PluginSelectorTableListBoxModel::PluginSelectorTableListBoxModel(
        PluginSelectorListParameters selectorListParameters,
        const SelectorComponentStyle& style)
        : _scanner(selectorListParameters.scanner),
          _pluginListSorter(selectorListParameters.state),
          _pluginCreationCallback(selectorListParameters.pluginCreationCallback),
          _getSampleRateCallback(selectorListParameters.getSampleRate),
          _getBlockSizeCallback(selectorListParameters.getBlockSize),
          _formatManager(selectorListParameters.formatManager),
          _rowBackgroundColour(style.backgroundColour),
          _rowTextColour(style.neutralColour),
          _isReplacingParameter(selectorListParameters.isReplacingPlugin) {
    _pluginListSorter.setPluginList(_scanner.getPluginTypes());
    _pluginList = _pluginListSorter.getFilteredPluginList();

    juce::Logger::writeToLog("Created PluginSelectorTableListBoxModel, found " + juce::String(_pluginList.size()) + " plugins");
}

void PluginSelectorTableListBoxModel::onFiltersOrSortUpdate() {
    _pluginList = _pluginListSorter.getFilteredPluginList();
}

int PluginSelectorTableListBoxModel::getNumRows() {
    return _pluginList.size();
}

void PluginSelectorTableListBoxModel::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) {
    g.fillAll(_rowBackgroundColour);
}

void PluginSelectorTableListBoxModel::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) {
    if (rowNumber < _pluginList.size()) {
        const juce::PluginDescription& thisPlugin = _pluginList[rowNumber];

        juce::String text;

        switch (columnId) {
            case NAME:
                text = thisPlugin.name;
                break;
            case MANUFACTURER:
                text = thisPlugin.manufacturerName;
                break;
            case CATEGORY:
                text = thisPlugin.category;
                break;
            case FORMAT:
                text = thisPlugin.pluginFormatName;
                break;
            default:
                break;
        }

        g.setColour(_rowTextColour);
        g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
}

void PluginSelectorTableListBoxModel::cellDoubleClicked(int rowNumber,
                                                        int columnId,
                                                        const juce::MouseEvent& event) {

    juce::Logger::writeToLog("PluginSelectorTableListBoxModel: Row " + juce::String(rowNumber) + " clicked, attempting to load plugin: " + _pluginList[rowNumber].name);
    const bool shouldCloseWindow {!juce::ModifierKeys::currentModifiers.isCommandDown() || _isReplacingParameter};
    _formatManager.createPluginInstanceAsync(_pluginList[rowNumber],
                                             _getSampleRateCallback(),
                                             _getBlockSizeCallback(),
                                             [&, shouldCloseWindow](std::unique_ptr<juce::AudioPluginInstance> plugin, const juce::String& error) { _pluginCreationCallback(std::move(plugin), error, shouldCloseWindow); });
};


void PluginSelectorTableListBoxModel::sortOrderChanged(int newSortColumnId, bool isForwards) {
    _pluginListSorter.state.sortColumnId = newSortColumnId;
    _pluginListSorter.state.sortForwards = isForwards;
    _pluginList = _pluginListSorter.getFilteredPluginList();
}

void PluginSelectorTableListBoxModel::onPluginScanUpdate() {
    _pluginListSorter.setPluginList(_scanner.getPluginTypes());
    _pluginList = _pluginListSorter.getFilteredPluginList();
    juce::Logger::writeToLog("PluginSelectorTableListBoxModel: now listing " + juce::String(_pluginList.size()) + " plugins");
}

PluginSelectorTableListBox::PluginSelectorTableListBox(PluginSelectorListParameters selectorListParameters,
                                                       const SelectorComponentStyle& style)
        : _pluginTableListBoxModel(selectorListParameters, style), _scanner(selectorListParameters.scanner) {

    // Start listening for scan messages and update the plugin list
    _scanner.addListener(this);

    constexpr int flags {juce::TableHeaderComponent::visible | juce::TableHeaderComponent::sortable};

    getHeader().addColumn("Name", NAME, 265, 265, 265, flags);
    getHeader().addColumn("Manufacturer", MANUFACTURER, 265, 265, 265, flags);
    getHeader().addColumn("Category", CATEGORY, 178, 178, 178, flags);
    getHeader().addColumn("Format", FORMAT, 64, 64, 64, flags);

    setModel(&_pluginTableListBoxModel);
    setColour(juce::ListBox::backgroundColourId, style.backgroundColour);

    // Restore the previous sort state
    getHeader().setSortColumnId(selectorListParameters.state.sortColumnId,
                                selectorListParameters.state.sortForwards);
}

PluginSelectorTableListBox::~PluginSelectorTableListBox() {
    _scanner.removeListener(this);
}

void PluginSelectorTableListBox::onFiltersOrSortUpdate() {
    _pluginTableListBoxModel.onFiltersOrSortUpdate();
    updateContent();
}

void PluginSelectorTableListBox::handleMessage(const juce::Message& message) {
    const PluginScanStatusMessage* statusMessage {
            dynamic_cast<const PluginScanStatusMessage*>(&message)
    };

    if (statusMessage != nullptr) {
        _pluginTableListBoxModel.onPluginScanUpdate();
        updateContent();
        juce::Logger::writeToLog("PluginSelectorTableListBox: received PluginScanStatusMessage");
    }
}
