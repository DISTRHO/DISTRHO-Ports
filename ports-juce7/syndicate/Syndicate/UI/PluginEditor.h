/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.12

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "CoreJUCEPlugin/CoreProcessorEditor.h"
#include "CoreJUCEPlugin/TooltipLabelUpdater.h"
#include "PluginProcessor.h"
#include "ImportExportComponent.h"
#include "UndoRedoComponent.h"
#include "MacrosComponent.h"
#include "OutputComponent.h"
#include "SplitterButtonsComponent.h"
#include "SplitterHeaderComponent.h"
#include "PluginSlotComponent.h"
#include "GraphViewComponent.h"
#include "ModulationBar.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SyndicateAudioProcessorEditor  : public WECore::JUCEPlugin::CoreProcessorEditor,
                                       public juce::DragAndDropContainer
{
public:
    //==============================================================================
    SyndicateAudioProcessorEditor (SyndicateAudioProcessor& ownerProcessor);
    ~SyndicateAudioProcessorEditor() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    void needsGraphRebuild();
    void needsModulationBarRebuild();
    void needsSelectedModulationSourceRebuild();
    void needsChainButtonsRefresh();
    void needsImportExportRefresh();
    void needsUndoRedoRefresh();
    void needsToRefreshAll();
    void closeGuestPluginWindows();
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    SyndicateAudioProcessor& _processor;
    WECore::JUCEPlugin::TooltipLabelUpdater _tooltipLabelUpdater;
    std::unique_ptr<SplitterHeaderComponent> _splitterHeader;
    bool _isHeaderInitialised;
    std::unique_ptr<UIUtils::PopoverComponent> _errorPopover;
    std::unique_ptr<ImportExportComponent> _importExportComponent;
    std::unique_ptr<UndoRedoComponent> _undoRedoComponent;
    std::unique_ptr<MacrosComponent> _macrosSidebar;
    std::unique_ptr<SplitterButtonsComponent> _splitterButtonsBar;
    std::unique_ptr<juce::Component> _headerExtensionComponent;
    std::unique_ptr<OutputComponent> _outputSidebar;
    std::unique_ptr<GraphViewComponent> _graphView;
    std::unique_ptr<ModulationBar> _modulationBar;
    std::unique_ptr<juce::Label> _tooltipLbl;
    std::mutex _splitterHeaderMutex;
    SPLIT_TYPE _previousSplitType;

    void _enableDoubleClickToDefault();
    void _startSliderReadouts();
    void _stopSliderReadouts();
    void _setSliderRanges();
    void _onParameterUpdate() override;
    void _updateSplitterHeader();
    void _displayErrorsIfNeeded();
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SyndicateAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

