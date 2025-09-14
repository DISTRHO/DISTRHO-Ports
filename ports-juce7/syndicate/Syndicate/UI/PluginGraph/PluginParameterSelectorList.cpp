#include "PluginParameterSelectorList.h"
#include "PluginChain.hpp"
#include "PluginParameterSelectorState.h"
#include "UIUtils.h"

PluginParameterListSorter::PluginParameterListSorter(
        PluginParameterSelectorState& newState,
        const juce::Array<juce::AudioProcessorParameter*>& fullParameterList)
            : state(newState),
              _fullParameterList(fullParameterList) {
}


juce::Array<juce::AudioProcessorParameter*> PluginParameterListSorter::getFilteredParameterList() const {
    juce::Array<juce::AudioProcessorParameter*> filteredParameterList;

    // Do filtering first if needed
    if (_isFilterNeeded()) {
        for (juce::AudioProcessorParameter* thisParameter : _fullParameterList) {
            if (_passesFilter(thisParameter)) {
                filteredParameterList.add(thisParameter);
            }
        }
    } else {
        filteredParameterList = _fullParameterList;
    }

    // Now sort the list
    filteredParameterList.sort(*this, false);

    return filteredParameterList;
}

int PluginParameterListSorter::compareElements(juce::AudioProcessorParameter* first, juce::AudioProcessorParameter* second) const {
    return first->getName(PluginParameterModulationConfig::PLUGIN_PARAMETER_NAME_LENGTH_LIMIT).compare(
           second->getName(PluginParameterModulationConfig::PLUGIN_PARAMETER_NAME_LENGTH_LIMIT));
}

bool PluginParameterListSorter::_isFilterNeeded() const {
    return state.filterString.isNotEmpty();
}

bool PluginParameterListSorter::_passesFilter(const juce::AudioProcessorParameter* parameter) const {
    return parameter->getName(PluginParameterModulationConfig::PLUGIN_PARAMETER_NAME_LENGTH_LIMIT).containsIgnoreCase(state.filterString) || state.filterString.isEmpty();
}


PluginParameterSelectorTableListBoxModel::PluginParameterSelectorTableListBoxModel(
        PluginParameterSelectorListParameters selectorListParameters)
            : _parameterListSorter(selectorListParameters.state,
                                   selectorListParameters.fullParameterList),
              _parameterSelectedCallback(selectorListParameters.parameterSelectedCallback),
              _isReplacingParameter(selectorListParameters.isReplacingParameter) {
    _parameterList = _parameterListSorter.getFilteredParameterList();
}

void PluginParameterSelectorTableListBoxModel::onFilterUpdate() {
    _parameterList = _parameterListSorter.getFilteredParameterList();
}

int PluginParameterSelectorTableListBoxModel::getNumRows() {
    return _parameterList.size();
}

void PluginParameterSelectorTableListBoxModel::paintRowBackground(juce::Graphics& g,
                                                                  int /*rowNumber*/,
                                                                  int /*width*/,
                                                                  int /*height*/,
                                                                  bool /*rowIsSelected*/) {
    g.fillAll(UIUtils::backgroundColour);
}

void PluginParameterSelectorTableListBoxModel::paintCell(juce::Graphics& g,
                                                         int rowNumber,
                                                         int /*columnId*/,
                                                         int width,
                                                         int height,
                                                         bool /*rowIsSelected*/) {
    if (rowNumber < _parameterList.size()) {
        const juce::String text = _parameterList[rowNumber]->getName(PluginParameterModulationConfig::PLUGIN_PARAMETER_NAME_LENGTH_LIMIT);

        g.setColour(UIUtils::neutralColour);
        g.drawText(text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
    }
}

void PluginParameterSelectorTableListBoxModel::cellDoubleClicked(int rowNumber,
                                                                 int /*columnId*/,
                                                                 const juce::MouseEvent& /*event*/) {
    const bool shouldCloseWindow {!juce::ModifierKeys::currentModifiers.isCommandDown() || _isReplacingParameter};
    _parameterSelectedCallback(_parameterList[rowNumber], shouldCloseWindow);
}

PluginParameterSelectorTableListBox::PluginParameterSelectorTableListBox(
        PluginParameterSelectorListParameters selectorListParameters)
            : _parameterTableListBoxModel(selectorListParameters) {
    constexpr int flags {juce::TableHeaderComponent::visible};

    getHeader().addColumn("Name",
                          1, // ID
                          0, // Width - will be set by resized()
                          -1, // Min width
                          -1, // Max width
                          flags);

    setModel(&_parameterTableListBoxModel);
    setColour(juce::ListBox::backgroundColourId, juce::Colour(0x00000000));
}

void PluginParameterSelectorTableListBox::resized() {
    constexpr int scrollBarWidth {10};
    getHeader().setColumnWidth(1, getWidth() - scrollBarWidth);

    // Need to call this otherwise rows won't draw properly
    juce::TableListBox::resized();
}

void PluginParameterSelectorTableListBox::onFilterUpdate() {
    _parameterTableListBoxModel.onFilterUpdate();
    updateContent();
}
