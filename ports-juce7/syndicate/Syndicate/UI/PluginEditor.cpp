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

//[Headers] You can add your own extra header files here...
#include "CoreJUCEPlugin/CoreProcessorEditor.h"
#include "LeftrightSplitterSubComponent.h"
#include "MidsideSplitterSubComponent.h"
#include "MultibandSplitterSubComponent.h"
#include "ParallelSplitterSubComponent.h"
#include "ParameterData.h"
#include "SeriesSplitterSubComponent.h"
//[/Headers]

#include "PluginEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SyndicateAudioProcessorEditor::SyndicateAudioProcessorEditor (SyndicateAudioProcessor& ownerProcessor)
    : CoreProcessorEditor(ownerProcessor), _processor(ownerProcessor), _isHeaderInitialised(false), _previousSplitType(SPLIT_TYPE::SERIES)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]


    //[UserPreSize]
    _importExportComponent.reset(new ImportExportComponent(_processor, *this));
    addAndMakeVisible(_importExportComponent.get());
    _importExportComponent->setName("Import/Export");

    _undoRedoComponent.reset(new UndoRedoComponent(_processor));
    addAndMakeVisible(_undoRedoComponent.get());
    _undoRedoComponent->setName("Undo/Redo");

    _macrosSidebar.reset(new MacrosComponent(this, _processor.macros, _processor.macroNames));
    addAndMakeVisible(_macrosSidebar.get());
    _macrosSidebar->setName("Macros");

    _splitterButtonsBar.reset(new SplitterButtonsComponent(_processor));
    addAndMakeVisible(_splitterButtonsBar.get());
    _splitterButtonsBar->setName("Splitter Buttons");

    _headerExtensionComponent.reset(new juce::Component());
    addAndMakeVisible(_headerExtensionComponent.get());
    _headerExtensionComponent->setName("Header Extension Component");

    _outputSidebar.reset(new OutputComponent(_processor));
    addAndMakeVisible(_outputSidebar.get());
    _outputSidebar->setName("Output");

    _graphView.reset(new GraphViewComponent(_processor));
    addAndMakeVisible(_graphView.get());
    _graphView->setName("Graph View");

    _splitterHeader.reset(new SeriesSplitterSubComponent(_processor, _graphView->getViewport()));
    addAndMakeVisible(_splitterHeader.get());
    _splitterHeader->setName("Splitter Header");

    _modulationBar.reset(new ModulationBar(_processor, this));
    addAndMakeVisible(_modulationBar.get());
    _modulationBar->setName("Modulation");

    _tooltipLbl.reset(new juce::Label("Tooltip Label", juce::String()));
    addAndMakeVisible(_tooltipLbl.get());
    _tooltipLbl->setFont(juce::Font (15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
    _tooltipLbl->setJustificationType(juce::Justification::centred);
    _tooltipLbl->setEditable(false, false, false);
    _tooltipLbl->setColour(juce::Label::textColourId, UIUtils::tooltipColour);
    _tooltipLbl->setColour(juce::TextEditor::textColourId, juce::Colours::black);
    _tooltipLbl->setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x00000000));
    //[/UserPreSize]

    //[Constructor] You can add your own custom stuff here..
    setResizable(true, true);

    _processor.setEditor(this);
    setBounds(_processor.mainWindowState.bounds);

    _setSliderRanges();

    // Start tooltip label
    addMouseListener(&_tooltipLabelUpdater, true);
    _tooltipLabelUpdater.start(
        _tooltipLbl.get(),
        getAudioProcessor()->wrapperType
#ifdef DEMO_BUILD
        ,true
#endif
        );

    // Call this once to force an update
    needsGraphRebuild();
    _onParameterUpdate();

    _displayErrorsIfNeeded();

    //[/Constructor]
}

SyndicateAudioProcessorEditor::~SyndicateAudioProcessorEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    _processor.mainWindowState.bounds = getBounds();
    _processor.setEditor(nullptr);
    _tooltipLabelUpdater.stop();

    _importExportComponent = nullptr;
    _undoRedoComponent = nullptr;
    _macrosSidebar = nullptr;
    _splitterButtonsBar = nullptr;
    _splitterHeader = nullptr;
    _headerExtensionComponent = nullptr;
    _outputSidebar = nullptr;
    _graphView = nullptr;
    _modulationBar = nullptr;
    _tooltipLbl = nullptr;
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SyndicateAudioProcessorEditor::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff272727));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void SyndicateAudioProcessorEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    if (_errorPopover != nullptr) {
        _errorPopover->setBounds(getLocalBounds());
    }

    constexpr int SIDEBAR_WIDTH {64};
    constexpr int SPLITTER_BUTTONS_HEIGHT {40};
    constexpr int TOOLTIP_HEIGHT {24};

    juce::Rectangle<int> availableArea = getLocalBounds().withTrimmedBottom(1);

    _importExportComponent->setBounds(availableArea.removeFromTop(28));
    _tooltipLbl->setBounds(availableArea.removeFromBottom(TOOLTIP_HEIGHT));

    juce::Rectangle<int> leftArea = availableArea.removeFromLeft(SIDEBAR_WIDTH);
    _undoRedoComponent->setBounds(leftArea.removeFromTop(8 + 24 + 4 + 24 + 8));
    _macrosSidebar->setBounds(leftArea);

    juce::Rectangle<int> rightArea = availableArea.removeFromRight(SIDEBAR_WIDTH).withTrimmedTop(SPLITTER_BUTTONS_HEIGHT);
    _headerExtensionComponent->setBounds(rightArea.removeFromTop(80));
    _outputSidebar->setBounds(rightArea);

    _splitterButtonsBar->setBounds(availableArea.removeFromTop(SPLITTER_BUTTONS_HEIGHT));
    _splitterHeader->setBounds(availableArea.removeFromTop(80));

    _modulationBar->setBounds(availableArea.removeFromBottom(130));

    _graphView->setBounds(availableArea);
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void SyndicateAudioProcessorEditor::needsGraphRebuild() {
    _splitterButtonsBar->onParameterUpdate();
    _updateSplitterHeader();
    _graphView->onParameterUpdate();
    needsChainButtonsRefresh();
    needsUndoRedoRefresh();
}

void SyndicateAudioProcessorEditor::needsModulationBarRebuild() {
    _modulationBar->needsRebuild();
    needsUndoRedoRefresh();
}

void SyndicateAudioProcessorEditor::needsSelectedModulationSourceRebuild() {
    _modulationBar->needsSelectedSourceRebuild();
    needsUndoRedoRefresh();
}

void SyndicateAudioProcessorEditor::needsChainButtonsRefresh() {
    _splitterHeader->refreshChainButtons();
    needsUndoRedoRefresh();
}

void SyndicateAudioProcessorEditor::needsImportExportRefresh() {
    _importExportComponent->refresh();
}

void SyndicateAudioProcessorEditor::needsUndoRedoRefresh() {
    _undoRedoComponent->refresh();

    juce::Component* mouseOverComponent = getComponentAt(getMouseXYRelative());
    if (mouseOverComponent != nullptr) {
        _tooltipLabelUpdater.refreshTooltip(mouseOverComponent);
    }
}

void SyndicateAudioProcessorEditor::needsToRefreshAll() {
    needsGraphRebuild();
    _onParameterUpdate();
    needsModulationBarRebuild();
    _macrosSidebar->updateNames(_processor.macroNames);
}

void SyndicateAudioProcessorEditor::closeGuestPluginWindows() {
    _graphView->closeGuestPluginWindows();
}

void SyndicateAudioProcessorEditor::_enableDoubleClickToDefault() {
    // TODO
}

void SyndicateAudioProcessorEditor::_startSliderReadouts() {
    // TODO
}

void SyndicateAudioProcessorEditor::_stopSliderReadouts() {
    // TODO
}

void SyndicateAudioProcessorEditor::_setSliderRanges() {
    // TODO
}

void SyndicateAudioProcessorEditor::_onParameterUpdate() {
    {
        std::scoped_lock lock(_splitterHeaderMutex);
        _splitterHeader->onParameterUpdate();
    }

    _outputSidebar->onParameterUpdate();
    _macrosSidebar->onParameterUpdate();

    _undoRedoComponent->refresh();
}

void SyndicateAudioProcessorEditor::_updateSplitterHeader() {
    std::scoped_lock lock(_splitterHeaderMutex);

    // Cache the previous value so we don't have recreate the component when nothing has changed
    if (_processor.getSplitType() != _previousSplitType || !_isHeaderInitialised) {
        _previousSplitType = _processor.getSplitType();
        _isHeaderInitialised = true;
        const juce::Rectangle<int> bounds = _splitterHeader->getBounds();

        switch (_processor.getSplitType()) {
            case SPLIT_TYPE::SERIES:
                _splitterHeader.reset(new SeriesSplitterSubComponent(_processor, _graphView->getViewport()));
                break;
            case SPLIT_TYPE::PARALLEL:
                _splitterHeader.reset(new ParallelSplitterSubComponent(_processor, _headerExtensionComponent.get(), _graphView->getViewport()));
                break;
            case SPLIT_TYPE::MULTIBAND:
                _splitterHeader.reset(new MultibandSplitterSubComponent(_processor, _headerExtensionComponent.get(), _graphView->getViewport()));
                break;
            case SPLIT_TYPE::LEFTRIGHT:
                _splitterHeader.reset(new LeftrightSplitterSubComponent(_processor, _graphView->getViewport()));
                break;
            case SPLIT_TYPE::MIDSIDE:
                _splitterHeader.reset(new MidsideSplitterSubComponent(_processor, _graphView->getViewport()));
                break;
        }

        addAndMakeVisible(_splitterHeader.get());
        _splitterHeader->setName("Splitter Header");
        _splitterHeader->setBounds(bounds);
    }

    _splitterHeader->onParameterUpdate();
}

void SyndicateAudioProcessorEditor::_displayErrorsIfNeeded() {

    if (_processor.restoreErrors.size() > 0) {
        juce::String bodyText;

        for (juce::String& error : _processor.restoreErrors) {
            bodyText += error;
            bodyText += "\n";
        }

        _errorPopover.reset(new UIUtils::PopoverComponent("Encountered the following errors while restoring plugin state:", bodyText, [&]() {_errorPopover.reset(); }));
        addAndMakeVisible(_errorPopover.get());
        _errorPopover->setBounds(getLocalBounds());

        _processor.restoreErrors.clear();
    }
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SyndicateAudioProcessorEditor"
                 componentName="" parentClasses="public WECore::JUCEPlugin::CoreProcessorEditor, public juce::DragAndDropContainer"
                 constructorParams="SyndicateAudioProcessor&amp; ownerProcessor"
                 variableInitialisers="CoreProcessorEditor(ownerProcessor), _processor(ownerProcessor), _isHeaderInitialised(false), _previousSplitType(SPLIT_TYPE::SERIES)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="820" initialHeight="656">
  <BACKGROUND backgroundColour="ff272727"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

