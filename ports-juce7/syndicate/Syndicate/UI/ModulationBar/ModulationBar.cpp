#include "ModulationBar.h"
#include "UIUtils.h"

ModulationBar::ModulationBar(SyndicateAudioProcessor& processor,
                             juce::DragAndDropContainer* dragContainer) :
        _processor(processor),
        _dragContainer(dragContainer),
        _hasRestoredScroll(false) {
    _addButtonLookAndFeel.reset(new AddButtonLookAndFeel());

    auto setUpButtonsView = [&] (std::unique_ptr<juce::Viewport>& view) {
        view.reset(new juce::Viewport());
        view->setViewedComponent(new juce::Component());
        view->setScrollBarsShown(true, false);
        view->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::backgroundColourId, juce::Colour(0x00000000));
        view->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::thumbColourId, UIUtils::neutralColour.withAlpha(0.5f));
        view->getVerticalScrollBar().setColour(juce::ScrollBar::ColourIds::trackColourId, juce::Colour(0x00000000));
        addAndMakeVisible(view.get());
    };

    setUpButtonsView(_lfoButtonsView);
    setUpButtonsView(_envelopeButtonsView);
    setUpButtonsView(_rndButtonsView);

    needsRebuild();

    // Restore the selection
    if (_processor.mainWindowState.selectedModulationSource.has_value()) {
        const ModulationSourceDefinition definition = _processor.mainWindowState.selectedModulationSource.value();

        if (definition.type == MODULATION_TYPE::LFO) {
            if (definition.id <= _lfoButtons.size()) {
                _selectModulationSource(_lfoButtons[definition.id - 1].get());
            }
        } else if (definition.type == MODULATION_TYPE::ENVELOPE) {
            if (definition.id <= _envelopeButtons.size()) {
                _selectModulationSource(_envelopeButtons[definition.id - 1].get());
            }
        } else if (definition.type == MODULATION_TYPE::RANDOM) {
            if (definition.id <= _rndButtons.size()) {
                _selectModulationSource(_rndButtons[definition.id - 1].get());
            }
        }
    }
}

ModulationBar::~ModulationBar() {
    _processor.mainWindowState.lfoButtonsScrollPosition = _lfoButtonsView->getViewPositionY();
    _processor.mainWindowState.envButtonsScrollPosition = _envelopeButtonsView->getViewPositionY();
    _processor.mainWindowState.rndButtonsScrollPosition = _rndButtonsView->getViewPositionY();

    // Reset stored state since it might now be invalid
    _processor.mainWindowState.selectedModulationSource.reset();

    // Store the selection
    for (int buttonIndex {0}; buttonIndex < _lfoButtons.size() && !_processor.mainWindowState.selectedModulationSource.has_value(); buttonIndex++) {
        if (_lfoButtons[buttonIndex]->getIsSelected()) {
            _processor.mainWindowState.selectedModulationSource = _lfoButtons[buttonIndex]->definition;
        }
    }

    for (int buttonIndex {0}; buttonIndex < _envelopeButtons.size() && !_processor.mainWindowState.selectedModulationSource.has_value(); buttonIndex++) {
        if (_envelopeButtons[buttonIndex]->getIsSelected()) {
            _processor.mainWindowState.selectedModulationSource = _envelopeButtons[buttonIndex]->definition;
        }
    }

    for (int buttonIndex {0}; buttonIndex < _rndButtons.size() && !_processor.mainWindowState.selectedModulationSource.has_value(); buttonIndex++) {
        if (_rndButtons[buttonIndex]->getIsSelected()) {
            _processor.mainWindowState.selectedModulationSource = _rndButtons[buttonIndex]->definition;
        }
    }

    // Usual clean up
    _addLfoButton->setLookAndFeel(nullptr);
    _addEnvelopeButton->setLookAndFeel(nullptr);
    _addRndButton->setLookAndFeel(nullptr);

    _addButtonLookAndFeel = nullptr;

    _lfoButtonsView = nullptr;
    _envelopeButtonsView = nullptr;
    _rndButtonsView = nullptr;
}

void ModulationBar::resized() {
    juce::Rectangle<int> availableArea = getLocalBounds();

    auto arrangeColumn = [&availableArea](std::vector<std::unique_ptr<ModulationButton>>& buttons,
                                          std::unique_ptr<juce::TextButton>& addButton,
                                          std::unique_ptr<juce::Viewport>& buttonsView) {

        const int columnHeight {static_cast<int>(UIUtils::MODULATION_LIST_BUTTON_HEIGHT * (buttons.size() + 1))};

        const juce::Rectangle<int> visibleButtonsArea =
            availableArea.removeFromLeft(UIUtils::MODULATION_LIST_COLUMN_WIDTH);
        buttonsView->setBounds(visibleButtonsArea);

        const int scrollPosition {buttonsView->getViewPositionY()};
        juce::Rectangle<int> scrollablebuttonsArea = visibleButtonsArea.withHeight(columnHeight);
        buttonsView->getViewedComponent()->setBounds(scrollablebuttonsArea);
        buttonsView->setViewPosition(0, scrollPosition);

        // Set the origin to 0 since we're now using it to position buttons relative to the inner
        // component
        scrollablebuttonsArea.setPosition(0, 0);

        const int scrollbarWidth {buttonsView->getScrollBarThickness()};

        for (std::unique_ptr<ModulationButton>& button : buttons) {
            button->setBounds(scrollablebuttonsArea.removeFromTop(UIUtils::MODULATION_LIST_BUTTON_HEIGHT).withTrimmedRight(scrollbarWidth));
        }

        if (addButton != nullptr) {
            addButton->setBounds(scrollablebuttonsArea.removeFromTop(UIUtils::MODULATION_LIST_BUTTON_HEIGHT).withTrimmedRight(scrollbarWidth));
        }
    };

    // LFO buttons
    arrangeColumn(_lfoButtons, _addLfoButton, _lfoButtonsView);

    // Envelope buttons
    arrangeColumn(_envelopeButtons, _addEnvelopeButton, _envelopeButtonsView);

    // Random buttons
    arrangeColumn(_rndButtons, _addRndButton, _rndButtonsView);

    _selectedSourceComponentArea = availableArea;

    if (_selectedSourceComponent != nullptr) {
        _selectedSourceComponent->setBounds(_selectedSourceComponentArea);
    }
}

void ModulationBar::buttonClicked(juce::Button* buttonThatWasClicked) {
    if (buttonThatWasClicked == _addLfoButton.get()) {
        _processor.addLfo();
        _selectModulationSource(_lfoButtons[_lfoButtons.size() - 1].get());
    } else if (buttonThatWasClicked == _addEnvelopeButton.get()) {
        _processor.addEnvelope();
        _selectModulationSource(_envelopeButtons[_envelopeButtons.size() - 1].get());
    } else if (buttonThatWasClicked == _addRndButton.get()) {
        _processor.addRandomSource();
    }
}

void ModulationBar::needsRebuild() {
    // Get the currently selected definition
    std::optional<ModulationSourceDefinition> selectedDefinition = _getSelectedDefinition();

    _resetButtons();

    // Re-select the already selected source from before the rebuild (if it still exists)
    if (selectedDefinition.has_value()) {
        _attemptToSelectByDefinition(selectedDefinition.value());
    } else if (_lfoButtons.size() > 0) {
        _selectModulationSource(_lfoButtons[0].get());
    } else if (_envelopeButtons.size() > 0) {
        _selectModulationSource(_envelopeButtons[0].get());
    } else if (_rndButtons.size() > 0) {
        _selectModulationSource(_rndButtons[0].get());
    }
}

void ModulationBar::needsSelectedSourceRebuild() {
    // Find the selected source and reselect it
    for (std::unique_ptr<ModulationButton>& button : _lfoButtons) {
        if (button->getIsSelected()) {
            _selectModulationSource(button.get());
            return;
        }
    }

    for (std::unique_ptr<ModulationButton>& button : _envelopeButtons) {
        if (button->getIsSelected()) {
            _selectModulationSource(button.get());
            return;
        }
    }

    for (std::unique_ptr<ModulationButton>& button : _rndButtons) {
        if (button->getIsSelected()) {
            _selectModulationSource(button.get());
            return;
        }
    }
}

void ModulationBar::AddButtonLookAndFeel::drawButtonBackground(juce::Graphics& g,
                                                               juce::Button& button,
                                                               const juce::Colour& /*backgroundColour*/,
                                                               bool /*shouldDrawButtonAsHighlighted*/,
                                                               bool /*shouldDrawButtonAsDown*/) {

    constexpr int MARGIN {1};
    juce::Rectangle<int> area = juce::Rectangle<int>(button.getWidth(), button.getHeight()).reduced(MARGIN, MARGIN);

    g.setColour(button.findColour(juce::TextButton::buttonColourId));
    g.fillRoundedRectangle(area.toFloat(), area.getHeight() / 2);
}

void ModulationBar::_resetButtons() {
    // Add the LFO buttons
    _lfoButtons.clear();
    ModelInterface::forEachLfo(_processor.manager, [&](int lfoNumber) {
        _createModulationSourceButton(ModulationSourceDefinition(lfoNumber, MODULATION_TYPE::LFO));
    });

    // And the add button
    _addLfoButton.reset(new juce::TextButton("Add LFO Button"));
    _lfoButtonsView->getViewedComponent()->addAndMakeVisible(_addLfoButton.get());
    _addLfoButton->setButtonText("+ LFO");
    _addLfoButton->addListener(this);
    _addLfoButton->setTooltip("Create a new LFO");
    _addLfoButton->setColour(juce::TextButton::buttonColourId, UIUtils::slotBackgroundColour);
    _addLfoButton->setColour(juce::TextButton::textColourOffId, UIUtils::getColourForModulationType(MODULATION_TYPE::LFO));
    _addLfoButton->setLookAndFeel(_addButtonLookAndFeel.get());

    // Add the envelope buttons
    _envelopeButtons.clear();
    ModelInterface::forEachEnvelope(_processor.manager, [&](int envelopeNumber) {
        _createModulationSourceButton(ModulationSourceDefinition(envelopeNumber, MODULATION_TYPE::ENVELOPE));
    });

    // And the add button
    _addEnvelopeButton.reset(new juce::TextButton("Add Envelope Button"));
    _envelopeButtonsView->getViewedComponent()->addAndMakeVisible(_addEnvelopeButton.get());
    _addEnvelopeButton->setButtonText("+ ENV");
    _addEnvelopeButton->addListener(this);
    _addEnvelopeButton->setTooltip("Create a new envelope");
    _addEnvelopeButton->setColour(juce::TextButton::buttonColourId, UIUtils::slotBackgroundColour);
    _addEnvelopeButton->setColour(juce::TextButton::textColourOffId, UIUtils::getColourForModulationType(MODULATION_TYPE::ENVELOPE));
    _addEnvelopeButton->setLookAndFeel(_addButtonLookAndFeel.get());

    // Add the random buttons
    _rndButtons.clear();
    ModelInterface::forEachRandom(_processor.manager, [&](int randomNumber) {
        _createModulationSourceButton(ModulationSourceDefinition(randomNumber, MODULATION_TYPE::RANDOM));
    });

    // Add the add button
    _addRndButton.reset(new juce::TextButton("Add Random Button"));
    _rndButtonsView->getViewedComponent()->addAndMakeVisible(_addRndButton.get());
    _addRndButton->setButtonText("+ RND");
    _addRndButton->addListener(this);
    _addRndButton->setTooltip("Create a new random source");
    _addRndButton->setColour(juce::TextButton::buttonColourId, UIUtils::slotBackgroundColour);
    _addRndButton->setColour(juce::TextButton::textColourOffId, UIUtils::getColourForModulationType(MODULATION_TYPE::RANDOM));
    _addRndButton->setLookAndFeel(_addButtonLookAndFeel.get());

    resized();

    // We need to run this only once after the graph view has been constructed to restore the scroll
    // position to the same as before the UI was last closed
    if (!_hasRestoredScroll) {
        _hasRestoredScroll = true;

        _lfoButtonsView->setViewPosition(0, _processor.mainWindowState.lfoButtonsScrollPosition);
        _envelopeButtonsView->setViewPosition(0, _processor.mainWindowState.envButtonsScrollPosition);
        _rndButtonsView->setViewPosition(0, _processor.mainWindowState.rndButtonsScrollPosition);
    }
}

void ModulationBar::_createModulationSourceButton(ModulationSourceDefinition definition) {
    std::unique_ptr<ModulationButton> newButton;
    newButton.reset(new ModulationButton(
        definition,
        [&](ModulationButton* button) { _selectModulationSource(button); },
        [&, definition]() { _removeModulationSource(definition); },
        _dragContainer));

    if (definition.type == MODULATION_TYPE::LFO) {
        _lfoButtonsView->getViewedComponent()->addAndMakeVisible(newButton.get());
        _lfoButtons.push_back(std::move(newButton));
    } else if (definition.type == MODULATION_TYPE::ENVELOPE) {
        _envelopeButtonsView->getViewedComponent()->addAndMakeVisible(newButton.get());
        _envelopeButtons.push_back(std::move(newButton));
    } else if (definition.type == MODULATION_TYPE::RANDOM) {
        _rndButtonsView->getViewedComponent()->addAndMakeVisible(newButton.get());
        _rndButtons.push_back(std::move(newButton));
    }
}

void ModulationBar::_selectModulationSource(ModulationButton* selectedButton) {
    // Deactivate all buttons
    for (std::unique_ptr<ModulationButton>& button : _lfoButtons) {
        button->setIsSelected(false);
    }

    for (std::unique_ptr<ModulationButton>& button : _envelopeButtons) {
        button->setIsSelected(false);
    }

    for (std::unique_ptr<ModulationButton>& button : _rndButtons) {
        button->setIsSelected(false);
    }

    // Activate just the one that's selected
    selectedButton->setIsSelected(true);

    // Draw the modulaton source UI
    if (selectedButton->definition.type == MODULATION_TYPE::LFO) {
        _selectedSourceComponent.reset(new ModulationBarLfo(_processor, selectedButton->definition.id - 1));
    } else if (selectedButton->definition.type == MODULATION_TYPE::ENVELOPE) {
        _selectedSourceComponent.reset(new ModulationBarEnvelope(_processor, selectedButton->definition.id - 1));
    } else if (selectedButton->definition.type == MODULATION_TYPE::RANDOM) {
        _selectedSourceComponent.reset(new ModulationBarRandom(_processor, selectedButton->definition.id - 1));
    }

    addAndMakeVisible(_selectedSourceComponent.get());
    _selectedSourceComponent->setBounds(_selectedSourceComponentArea);
}

void ModulationBar::_removeModulationSource(ModulationSourceDefinition definition) {
    // Get the currently selected definition
    std::optional<ModulationSourceDefinition> selectedDefinition = _getSelectedDefinition();

    // Delete selected component before changing anything in the processor to make sure nothing in
    // it is referring to anything we might change
    _selectedSourceComponent.reset();

    // Remove the modulation source and rebuild the UI
    _processor.removeModulationSource(definition);
    _resetButtons();

    if (!selectedDefinition.has_value()) {
        return;
    }

    // Update the selected ID if we're deleting something ahead of it
    if (selectedDefinition.value().id > definition.id) {
        selectedDefinition.value().id--;
    }

    // Decide which definition should now be selected
    _attemptToSelectByDefinition(selectedDefinition.value());
}

std::optional<ModulationSourceDefinition> ModulationBar::_getSelectedDefinition() {
    std::optional<ModulationSourceDefinition> selectedDefinition;
    for (int buttonIndex {0}; buttonIndex < _lfoButtons.size(); buttonIndex++) {
        if (_lfoButtons[buttonIndex]->getIsSelected()) {
            return _lfoButtons[buttonIndex]->definition;
        }
    }

    for (int buttonIndex {0}; buttonIndex < _envelopeButtons.size(); buttonIndex++) {
        if (_envelopeButtons[buttonIndex]->getIsSelected()) {
            return _envelopeButtons[buttonIndex]->definition;
        }
    }

    for (int buttonIndex {0}; buttonIndex < _rndButtons.size(); buttonIndex++) {
        if (_rndButtons[buttonIndex]->getIsSelected()) {
            return _rndButtons[buttonIndex]->definition;
        }
    }

    return selectedDefinition;
}

void ModulationBar::_attemptToSelectByDefinition(ModulationSourceDefinition definition) {
    if (definition.type == MODULATION_TYPE::LFO) {
        if (_lfoButtons.size() > definition.id - 1) {
            // Select the previously selected definition
            _selectModulationSource(_lfoButtons[definition.id - 1].get());
        } else if (_lfoButtons.size() > 0) {
            // We deleted the last definition, so select the current last one
            _selectModulationSource(_lfoButtons[_lfoButtons.size() - 1].get());
        } else if (_envelopeButtons.size() > 0) {
            // We deleted all the LFOs, so select an envelope follower
            _selectModulationSource(_envelopeButtons[0].get());
        } else if (_rndButtons.size() > 0) {
            // We deleted all the LFOs and envelope followers, so select a random source
            _selectModulationSource(_rndButtons[0].get());
        } else {
            // We deleted everything, select nothing
            _selectedSourceComponent.reset();
        }
    } else if (definition.type == MODULATION_TYPE::ENVELOPE) {
        if (_envelopeButtons.size() > definition.id - 1) {
            // Restore the previously selected definition
            _selectModulationSource(_envelopeButtons[definition.id - 1].get());
        } else if (_envelopeButtons.size() > 0) {
            // We deleted the last definition, so select the current last one
            _selectModulationSource(_envelopeButtons[0].get());
        } else if (_lfoButtons.size() > 0) {
            // We deleted all the envelope followers, so select an LFO
            _selectModulationSource(_lfoButtons[0].get());
        } else if (_rndButtons.size() > 0) {
            // We deleted all the envelope followers and LFOs, so select a random source
            _selectModulationSource(_rndButtons[0].get());
        } else {
            // We deleted everything, select nothing
            _selectedSourceComponent.reset();
        }
    } else if (definition.type == MODULATION_TYPE::RANDOM) {
        if (_rndButtons.size() > definition.id - 1) {
            // Restore the previously selected definition
            _selectModulationSource(_rndButtons[definition.id - 1].get());
        } else if (_rndButtons.size() > 0) {
            // We deleted the last definition, so select the current last one
            _selectModulationSource(_rndButtons[0].get());
        } else if (_lfoButtons.size() > 0) {
            // We deleted all the random sources, so select an LFO
            _selectModulationSource(_lfoButtons[0].get());
        } else if (_envelopeButtons.size() > 0) {
            // We deleted all the random sources and LFOs, so select an envelope follower
            _selectModulationSource(_envelopeButtons[0].get());
        } else {
            // We deleted everything, select nothing
            _selectedSourceComponent.reset();
        }
    }
}
