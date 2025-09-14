/*
  ==============================================================================

    PluginSelectorList.h
    Created: 23 May 2021 12:39:35am
    Author:  Jack Devlin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginScanClient.h"
#include "PluginSelectorListParameters.h"
#include "PluginSelectorState.h"
#include "SelectorComponentStyle.h"

class PluginListSorter {
public:
    PluginSelectorState& state;

    PluginListSorter(PluginSelectorState& state);

    ~PluginListSorter() = default;

    void setPluginList(juce::Array<juce::PluginDescription> pluginList);

    juce::Array<juce::PluginDescription> getFilteredPluginList() const;

    int compareElements(juce::PluginDescription first, juce::PluginDescription second) const;

private:
    juce::Array<juce::PluginDescription> _fullPluginList;

    bool _passesFilter(const juce::PluginDescription& plugin) const;
};

class PluginSelectorTableListBoxModel : public juce::TableListBoxModel {
public:
    PluginSelectorTableListBoxModel(PluginSelectorListParameters selectorListParameters,
                                    const SelectorComponentStyle& style);
    virtual ~PluginSelectorTableListBoxModel() = default;

    void onFiltersOrSortUpdate();

    virtual int getNumRows() override;

    virtual void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;

    virtual void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    void cellDoubleClicked(int rowNumber, int columnId, const juce::MouseEvent& event) override;

    void sortOrderChanged(int newSortColumnId, bool isForwards) override;

    void onPluginScanUpdate();

private:
    PluginScanClient& _scanner;
    PluginListSorter _pluginListSorter;
    juce::Array<juce::PluginDescription> _pluginList;
    std::function<void(std::unique_ptr<juce::AudioPluginInstance>, const juce::String&, bool)> _pluginCreationCallback;
    std::function<double()> _getSampleRateCallback;
    std::function<int()> _getBlockSizeCallback;
    juce::AudioPluginFormatManager& _formatManager;
    juce::Colour _rowBackgroundColour;
    juce::Colour _rowTextColour;
    const bool _isReplacingParameter;
};

class PluginSelectorTableListBox : public juce::TableListBox,
                                   public juce::MessageListener {
public:
    PluginSelectorTableListBox(PluginSelectorListParameters selectorListParameters,
                               const SelectorComponentStyle& style);
    virtual ~PluginSelectorTableListBox();

    void onFiltersOrSortUpdate();

    void handleMessage(const juce::Message& message) override;

private:
    PluginSelectorTableListBoxModel _pluginTableListBoxModel;
    PluginScanClient& _scanner;
};

