/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 5 End-User License
   Agreement and JUCE 5 Privacy Policy (both updated and effective as of the
   27th April 2017).

   End User License Agreement: www.juce.com/juce-5-licence
   Privacy Policy: www.juce.com/juce-5-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

class SimpleDeviceManagerInputLevelMeter  : public Component,
                                            public Timer
{
public:
    SimpleDeviceManagerInputLevelMeter (AudioDeviceManager& m)
        : manager (m), level (0)
    {
        startTimer (50);
        manager.enableInputLevelMeasurement (true);
    }

    ~SimpleDeviceManagerInputLevelMeter()
    {
        manager.enableInputLevelMeasurement (false);
    }

    void timerCallback() override
    {
        if (isShowing())
        {
            const float newLevel = (float) manager.getCurrentInputLevel();

            if (std::abs (level - newLevel) > 0.005f)
            {
                level = newLevel;
                repaint();
            }
        }
        else
        {
            level = 0;
        }
    }

    void paint (Graphics& g) override
    {
        getLookAndFeel().drawLevelMeter (g, getWidth(), getHeight(),
                                         (float) exp (log (level) / 3.0)); // (add a bit of a skew to make the level more obvious)
    }

private:
    AudioDeviceManager& manager;
    float level;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDeviceManagerInputLevelMeter)
};


//==============================================================================
class AudioDeviceSelectorComponent::MidiInputSelectorComponentListBox  : public ListBox,
                                                                         private ListBoxModel
{
public:
    MidiInputSelectorComponentListBox (AudioDeviceManager& dm, const String& noItems)
        : ListBox (String(), nullptr),
          deviceManager (dm),
          noItemsMessage (noItems)
    {
        updateDevices();
        setModel (this);
        setOutlineThickness (1);
    }

    void updateDevices()
    {
        items = MidiInput::getDevices();
    }

    int getNumRows() override
    {
        return items.size();
    }

    void paintListBoxItem (int row, Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (isPositiveAndBelow (row, items.size()))
        {
            if (rowIsSelected)
                g.fillAll (findColour (TextEditor::highlightColourId)
                               .withMultipliedAlpha (0.3f));

            const String item (items [row]);
            bool enabled = deviceManager.isMidiInputEnabled (item);

            const int x = getTickX();
            const float tickW = height * 0.75f;

            getLookAndFeel().drawTickBox (g, *this, x - tickW, (height - tickW) / 2, tickW, tickW,
                                          enabled, true, true, false);

            g.setFont (height * 0.6f);
            g.setColour (findColour (ListBox::textColourId, true).withMultipliedAlpha (enabled ? 1.0f : 0.6f));
            g.drawText (item, x, 0, width - x - 2, height, Justification::centredLeft, true);
        }
    }

    void listBoxItemClicked (int row, const MouseEvent& e) override
    {
        selectRow (row);

        if (e.x < getTickX())
            flipEnablement (row);
    }

    void listBoxItemDoubleClicked (int row, const MouseEvent&) override
    {
        flipEnablement (row);
    }

    void returnKeyPressed (int row) override
    {
        flipEnablement (row);
    }

    void paint (Graphics& g) override
    {
        ListBox::paint (g);

        if (items.size() == 0)
        {
            g.setColour (Colours::grey);
            g.setFont (13.0f);
            g.drawText (noItemsMessage,
                        0, 0, getWidth(), getHeight() / 2,
                        Justification::centred, true);
        }
    }

    int getBestHeight (const int preferredHeight)
    {
        const int extra = getOutlineThickness() * 2;

        return jmax (getRowHeight() * 2 + extra,
                     jmin (getRowHeight() * getNumRows() + extra,
                           preferredHeight));
    }

private:
    //==============================================================================
    AudioDeviceManager& deviceManager;
    const String noItemsMessage;
    StringArray items;

    void flipEnablement (const int row)
    {
        if (isPositiveAndBelow (row, items.size()))
        {
            const String item (items [row]);
            deviceManager.setMidiInputEnabled (item, ! deviceManager.isMidiInputEnabled (item));
        }
    }

    int getTickX() const
    {
        return getRowHeight() + 5;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiInputSelectorComponentListBox)
};


//==============================================================================
struct AudioDeviceSetupDetails
{
    AudioDeviceManager* manager;
    int minNumInputChannels, maxNumInputChannels;
    int minNumOutputChannels, maxNumOutputChannels;
    bool useStereoPairs;
};

static String getNoDeviceString()   { return "<< " + TRANS("none") + " >>"; }

//==============================================================================
class AudioDeviceSettingsPanel : public Component,
                                 private ChangeListener,
                                 private ComboBox::Listener,
                                 private Button::Listener
{
public:
    AudioDeviceSettingsPanel (AudioIODeviceType& t, AudioDeviceSetupDetails& setupDetails,
                              const bool hideAdvancedOptionsWithButton)
        : type (t), setup (setupDetails)
    {
        if (hideAdvancedOptionsWithButton)
        {
            addAndMakeVisible (showAdvancedSettingsButton = new TextButton (TRANS("Show advanced settings...")));
            showAdvancedSettingsButton->addListener (this);
        }

        type.scanForDevices();

        setup.manager->addChangeListener (this);
    }

    ~AudioDeviceSettingsPanel()
    {
        setup.manager->removeChangeListener (this);
    }

    void resized() override
    {
        if (AudioDeviceSelectorComponent* parent = findParentComponentOfClass<AudioDeviceSelectorComponent>())
        {
            Rectangle<int> r (proportionOfWidth (0.35f), 0, proportionOfWidth (0.6f), 3000);

            const int maxListBoxHeight = 100;
            const int h = parent->getItemHeight();
            const int space = h / 4;

            if (outputDeviceDropDown != nullptr)
            {
                Rectangle<int> row (r.removeFromTop (h));

                if (testButton != nullptr)
                {
                    testButton->changeWidthToFitText (h);
                    testButton->setBounds (row.removeFromRight (testButton->getWidth()));
                    row.removeFromRight (space);
                }

                outputDeviceDropDown->setBounds (row);
                r.removeFromTop (space);
            }

            if (inputDeviceDropDown != nullptr)
            {
                Rectangle<int> row (r.removeFromTop (h));

                inputLevelMeter->setBounds (row.removeFromRight (testButton != nullptr ? testButton->getWidth() : row.getWidth() / 6));
                row.removeFromRight (space);
                inputDeviceDropDown->setBounds (row);
                r.removeFromTop (space);
            }

            if (outputChanList != nullptr)
            {
                outputChanList->setBounds (r.removeFromTop (outputChanList->getBestHeight (maxListBoxHeight)));
                outputChanLabel->setBounds (0, outputChanList->getBounds().getCentreY() - h / 2, r.getX(), h);
                r.removeFromTop (space);
            }

            if (inputChanList != nullptr)
            {
                inputChanList->setBounds (r.removeFromTop (inputChanList->getBestHeight (maxListBoxHeight)));
                inputChanLabel->setBounds (0, inputChanList->getBounds().getCentreY() - h / 2, r.getX(), h);
                r.removeFromTop (space);
            }

            r.removeFromTop (space * 2);

            if (showAdvancedSettingsButton != nullptr)
            {
                showAdvancedSettingsButton->setBounds (r.withHeight (h));
                showAdvancedSettingsButton->changeWidthToFitText();
            }

            const bool advancedSettingsVisible = showAdvancedSettingsButton == nullptr
                                                    || ! showAdvancedSettingsButton->isVisible();

            if (sampleRateDropDown != nullptr)
            {
                sampleRateDropDown->setVisible (advancedSettingsVisible);
                sampleRateDropDown->setBounds (r.removeFromTop (h));
                r.removeFromTop (space);
            }

            if (bufferSizeDropDown != nullptr)
            {
                bufferSizeDropDown->setVisible (advancedSettingsVisible);
                bufferSizeDropDown->setBounds (r.removeFromTop (h));
                r.removeFromTop (space);
            }

            r.removeFromTop (space);

            if (showUIButton != nullptr || resetDeviceButton != nullptr)
            {
                Rectangle<int> buttons (r.removeFromTop (h));

                if (showUIButton != nullptr)
                {
                    showUIButton->setVisible (advancedSettingsVisible);
                    showUIButton->changeWidthToFitText (h);
                    showUIButton->setBounds (buttons.removeFromLeft (showUIButton->getWidth()));
                    buttons.removeFromLeft (space);
                }

                if (resetDeviceButton != nullptr)
                {
                    resetDeviceButton->setVisible (advancedSettingsVisible);
                    resetDeviceButton->changeWidthToFitText (h);
                    resetDeviceButton->setBounds (buttons.removeFromLeft (resetDeviceButton->getWidth()));
                }

                r.removeFromTop (space);
            }

            setSize (getWidth(), r.getY());
        }
        else
        {
            jassertfalse;
        }
    }

    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override
    {
        if (comboBoxThatHasChanged == nullptr)
            return;

        AudioDeviceManager::AudioDeviceSetup config;
        setup.manager->getAudioDeviceSetup (config);
        String error;

        if (comboBoxThatHasChanged == outputDeviceDropDown
              || comboBoxThatHasChanged == inputDeviceDropDown)
        {
            if (outputDeviceDropDown != nullptr)
                config.outputDeviceName = outputDeviceDropDown->getSelectedId() < 0 ? String()
                                                                                    : outputDeviceDropDown->getText();

            if (inputDeviceDropDown != nullptr)
                config.inputDeviceName = inputDeviceDropDown->getSelectedId() < 0 ? String()
                                                                                  : inputDeviceDropDown->getText();

            if (! type.hasSeparateInputsAndOutputs())
                config.inputDeviceName = config.outputDeviceName;

            if (comboBoxThatHasChanged == inputDeviceDropDown)
                config.useDefaultInputChannels = true;
            else
                config.useDefaultOutputChannels = true;

            error = setup.manager->setAudioDeviceSetup (config, true);

            showCorrectDeviceName (inputDeviceDropDown, true);
            showCorrectDeviceName (outputDeviceDropDown, false);

            updateControlPanelButton();
            resized();
        }
        else if (comboBoxThatHasChanged == sampleRateDropDown)
        {
            if (sampleRateDropDown->getSelectedId() > 0)
            {
                config.sampleRate = sampleRateDropDown->getSelectedId();
                error = setup.manager->setAudioDeviceSetup (config, true);
            }
        }
        else if (comboBoxThatHasChanged == bufferSizeDropDown)
        {
            if (bufferSizeDropDown->getSelectedId() > 0)
            {
                config.bufferSize = bufferSizeDropDown->getSelectedId();
                error = setup.manager->setAudioDeviceSetup (config, true);
            }
        }

        if (error.isNotEmpty())
            AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                              TRANS("Error when trying to open audio device!"),
                                              error);
    }

    bool showDeviceControlPanel()
    {
        if (AudioIODevice* const device = setup.manager->getCurrentAudioDevice())
        {
            Component modalWindow;
            modalWindow.setOpaque (true);
            modalWindow.addToDesktop (0);
            modalWindow.enterModalState();

            return device->showControlPanel();
        }

        return false;
    }

    void buttonClicked (Button* button) override
    {
        if (button == showAdvancedSettingsButton)
        {
            showAdvancedSettingsButton->setVisible (false);
            resized();
        }
        else if (button == showUIButton)
        {
            if (showDeviceControlPanel())
            {
                setup.manager->closeAudioDevice();
                setup.manager->restartLastAudioDevice();
                getTopLevelComponent()->toFront (true);
            }
        }
        else if (button == testButton && testButton != nullptr)
        {
            setup.manager->playTestSound();
        }
        else if (button == resetDeviceButton)
        {
            resetDevice();
        }
    }

    void updateAllControls()
    {
        updateOutputsComboBox();
        updateInputsComboBox();

        updateControlPanelButton();
        updateResetButton();

        if (AudioIODevice* const currentDevice = setup.manager->getCurrentAudioDevice())
        {
            if (setup.maxNumOutputChannels > 0
                 && setup.minNumOutputChannels < setup.manager->getCurrentAudioDevice()->getOutputChannelNames().size())
            {
                if (outputChanList == nullptr)
                {
                    addAndMakeVisible (outputChanList
                        = new ChannelSelectorListBox (setup, ChannelSelectorListBox::audioOutputType,
                                                      TRANS ("(no audio output channels found)")));
                    outputChanLabel = new Label (String(), TRANS("Active output channels:"));
                    outputChanLabel->setJustificationType (Justification::centredRight);
                    outputChanLabel->attachToComponent (outputChanList, true);
                }

                outputChanList->refresh();
            }
            else
            {
                outputChanLabel = nullptr;
                outputChanList = nullptr;
            }

            if (setup.maxNumInputChannels > 0
                 && setup.minNumInputChannels < setup.manager->getCurrentAudioDevice()->getInputChannelNames().size())
            {
                if (inputChanList == nullptr)
                {
                    addAndMakeVisible (inputChanList
                        = new ChannelSelectorListBox (setup, ChannelSelectorListBox::audioInputType,
                                                      TRANS("(no audio input channels found)")));
                    inputChanLabel = new Label (String(), TRANS("Active input channels:"));
                    inputChanLabel->setJustificationType (Justification::centredRight);
                    inputChanLabel->attachToComponent (inputChanList, true);
                }

                inputChanList->refresh();
            }
            else
            {
                inputChanLabel = nullptr;
                inputChanList = nullptr;
            }

            updateSampleRateComboBox (currentDevice);
            updateBufferSizeComboBox (currentDevice);
        }
        else
        {
            jassert (setup.manager->getCurrentAudioDevice() == nullptr); // not the correct device type!

            inputChanLabel = nullptr;
            outputChanLabel = nullptr;
            sampleRateLabel = nullptr;
            bufferSizeLabel = nullptr;

            inputChanList = nullptr;
            outputChanList = nullptr;
            sampleRateDropDown = nullptr;
            bufferSizeDropDown = nullptr;

            if (outputDeviceDropDown != nullptr)
                outputDeviceDropDown->setSelectedId (-1, dontSendNotification);

            if (inputDeviceDropDown != nullptr)
                inputDeviceDropDown->setSelectedId (-1, dontSendNotification);
        }

        sendLookAndFeelChange();
        resized();
        setSize (getWidth(), getLowestY() + 4);
    }

    void changeListenerCallback (ChangeBroadcaster*) override
    {
        updateAllControls();
    }

    void resetDevice()
    {
        setup.manager->closeAudioDevice();
        setup.manager->restartLastAudioDevice();
    }

private:
    AudioIODeviceType& type;
    const AudioDeviceSetupDetails setup;

    ScopedPointer<ComboBox> outputDeviceDropDown, inputDeviceDropDown, sampleRateDropDown, bufferSizeDropDown;
    ScopedPointer<Label> outputDeviceLabel, inputDeviceLabel, sampleRateLabel, bufferSizeLabel, inputChanLabel, outputChanLabel;
    ScopedPointer<TextButton> testButton;
    ScopedPointer<Component> inputLevelMeter;
    ScopedPointer<TextButton> showUIButton, showAdvancedSettingsButton, resetDeviceButton;

    void showCorrectDeviceName (ComboBox* const box, const bool isInput)
    {
        if (box != nullptr)
        {
            AudioIODevice* const currentDevice = setup.manager->getCurrentAudioDevice();

            const int index = type.getIndexOfDevice (currentDevice, isInput);

            box->setSelectedId (index + 1, dontSendNotification);

            if (testButton != nullptr && ! isInput)
                testButton->setEnabled (index >= 0);
        }
    }

    void addNamesToDeviceBox (ComboBox& combo, bool isInputs)
    {
        const StringArray devs (type.getDeviceNames (isInputs));

        combo.clear (dontSendNotification);

        for (int i = 0; i < devs.size(); ++i)
            combo.addItem (devs[i], i + 1);

        combo.addItem (getNoDeviceString(), -1);
        combo.setSelectedId (-1, dontSendNotification);
    }

    int getLowestY() const
    {
        int y = 0;

        for (auto* c : getChildren())
            y = jmax (y, c->getBottom());

        return y;
    }

    void updateControlPanelButton()
    {
        AudioIODevice* const currentDevice = setup.manager->getCurrentAudioDevice();
        showUIButton = nullptr;

        if (currentDevice != nullptr && currentDevice->hasControlPanel())
        {
            addAndMakeVisible (showUIButton = new TextButton (TRANS ("Control Panel"),
                                                              TRANS ("Opens the device's own control panel")));
            showUIButton->addListener (this);
        }

        resized();
    }

    void updateResetButton()
    {
        if (AudioIODevice* const currentDevice = setup.manager->getCurrentAudioDevice())
        {
            if (currentDevice->hasControlPanel())
            {
                if (resetDeviceButton == nullptr)
                {
                    addAndMakeVisible (resetDeviceButton = new TextButton (TRANS ("Reset Device"),
                        TRANS ("Resets the audio interface - sometimes needed after changing a device's properties in its custom control panel")));

                    resetDeviceButton->addListener (this);
                    resized();
                }

                return;
            }
        }

        resetDeviceButton = nullptr;
    }

    void updateOutputsComboBox()
    {
        if (setup.maxNumOutputChannels > 0 || ! type.hasSeparateInputsAndOutputs())
        {
            if (outputDeviceDropDown == nullptr)
            {
                outputDeviceDropDown = new ComboBox (String());
                outputDeviceDropDown->addListener (this);
                addAndMakeVisible (outputDeviceDropDown);

                outputDeviceLabel = new Label (String(),
                                               type.hasSeparateInputsAndOutputs() ? TRANS("Output:")
                                                                                  : TRANS("Device:"));
                outputDeviceLabel->attachToComponent (outputDeviceDropDown, true);

                if (setup.maxNumOutputChannels > 0)
                {
                    addAndMakeVisible (testButton = new TextButton (TRANS("Test"),
                                                                    TRANS("Plays a test tone")));
                    testButton->addListener (this);
                }
            }

            addNamesToDeviceBox (*outputDeviceDropDown, false);
        }

        showCorrectDeviceName (outputDeviceDropDown, false);
    }

    void updateInputsComboBox()
    {
        if (setup.maxNumInputChannels > 0 && type.hasSeparateInputsAndOutputs())
        {
            if (inputDeviceDropDown == nullptr)
            {
                inputDeviceDropDown = new ComboBox (String());
                inputDeviceDropDown->addListener (this);
                addAndMakeVisible (inputDeviceDropDown);

                inputDeviceLabel = new Label (String(), TRANS("Input:"));
                inputDeviceLabel->attachToComponent (inputDeviceDropDown, true);

                addAndMakeVisible (inputLevelMeter
                    = new SimpleDeviceManagerInputLevelMeter (*setup.manager));
            }

            addNamesToDeviceBox (*inputDeviceDropDown, true);
        }

        showCorrectDeviceName (inputDeviceDropDown, true);
    }

    void updateSampleRateComboBox (AudioIODevice* currentDevice)
    {
        if (sampleRateDropDown == nullptr)
        {
            addAndMakeVisible (sampleRateDropDown = new ComboBox (String()));

            sampleRateLabel = new Label (String(), TRANS("Sample rate:"));
            sampleRateLabel->attachToComponent (sampleRateDropDown, true);
        }
        else
        {
            sampleRateDropDown->clear();
            sampleRateDropDown->removeListener (this);
        }

        const Array<double> rates (currentDevice->getAvailableSampleRates());

        for (int i = 0; i < rates.size(); ++i)
        {
            const int rate = roundToInt (rates[i]);
            sampleRateDropDown->addItem (String (rate) + " Hz", rate);
        }

        sampleRateDropDown->setSelectedId (roundToInt (currentDevice->getCurrentSampleRate()), dontSendNotification);
        sampleRateDropDown->addListener (this);
    }

    void updateBufferSizeComboBox (AudioIODevice* currentDevice)
    {
        if (bufferSizeDropDown == nullptr)
        {
            addAndMakeVisible (bufferSizeDropDown = new ComboBox (String()));

            bufferSizeLabel = new Label (String(), TRANS("Audio buffer size:"));
            bufferSizeLabel->attachToComponent (bufferSizeDropDown, true);
        }
        else
        {
            bufferSizeDropDown->clear();
            bufferSizeDropDown->removeListener (this);
        }

        const Array<int> bufferSizes (currentDevice->getAvailableBufferSizes());

        double currentRate = currentDevice->getCurrentSampleRate();
        if (currentRate == 0)
            currentRate = 48000.0;

        for (int i = 0; i < bufferSizes.size(); ++i)
        {
            const int bs = bufferSizes[i];
            bufferSizeDropDown->addItem (String (bs) + " samples (" + String (bs * 1000.0 / currentRate, 1) + " ms)", bs);
        }

        bufferSizeDropDown->setSelectedId (currentDevice->getCurrentBufferSizeSamples(), dontSendNotification);
        bufferSizeDropDown->addListener (this);
    }

public:
    //==============================================================================
    class ChannelSelectorListBox  : public ListBox,
                                    private ListBoxModel
    {
    public:
        enum BoxType
        {
            audioInputType,
            audioOutputType
        };

        //==============================================================================
        ChannelSelectorListBox (const AudioDeviceSetupDetails& setupDetails,
                                const BoxType boxType, const String& noItemsText)
            : ListBox (String(), nullptr),
              setup (setupDetails), type (boxType), noItemsMessage (noItemsText)
        {
            refresh();
            setModel (this);
            setOutlineThickness (1);
        }

        void refresh()
        {
            items.clear();

            if (AudioIODevice* const currentDevice = setup.manager->getCurrentAudioDevice())
            {
                if (type == audioInputType)
                    items = currentDevice->getInputChannelNames();
                else if (type == audioOutputType)
                    items = currentDevice->getOutputChannelNames();

                if (setup.useStereoPairs)
                {
                    StringArray pairs;

                    for (int i = 0; i < items.size(); i += 2)
                    {
                        const String& name = items[i];

                        if (i + 1 >= items.size())
                            pairs.add (name.trim());
                        else
                            pairs.add (getNameForChannelPair (name, items[i + 1]));
                    }

                    items = pairs;
                }
            }

            updateContent();
            repaint();
        }

        int getNumRows() override
        {
            return items.size();
        }

        void paintListBoxItem (int row, Graphics& g, int width, int height, bool) override
        {
            if (isPositiveAndBelow (row, items.size()))
            {
                g.fillAll (findColour (ListBox::backgroundColourId));

                const String item (items [row]);
                bool enabled = false;

                AudioDeviceManager::AudioDeviceSetup config;
                setup.manager->getAudioDeviceSetup (config);

                if (setup.useStereoPairs)
                {
                    if (type == audioInputType)
                        enabled = config.inputChannels [row * 2] || config.inputChannels [row * 2 + 1];
                    else if (type == audioOutputType)
                        enabled = config.outputChannels [row * 2] || config.outputChannels [row * 2 + 1];
                }
                else
                {
                    if (type == audioInputType)
                        enabled = config.inputChannels [row];
                    else if (type == audioOutputType)
                        enabled = config.outputChannels [row];
                }

                const int x = getTickX();
                const float tickW = height * 0.75f;

                getLookAndFeel().drawTickBox (g, *this, x - tickW, (height - tickW) / 2, tickW, tickW,
                                              enabled, true, true, false);

                g.setFont (height * 0.6f);
                g.setColour (findColour (ListBox::textColourId, true).withMultipliedAlpha (enabled ? 1.0f : 0.6f));
                g.drawText (item, x + 5, 0, width - x - 5, height, Justification::centredLeft, true);
            }
        }

        void listBoxItemClicked (int row, const MouseEvent& e) override
        {
            selectRow (row);

            if (e.x < getTickX())
                flipEnablement (row);
        }

        void listBoxItemDoubleClicked (int row, const MouseEvent&) override
        {
            flipEnablement (row);
        }

        void returnKeyPressed (int row) override
        {
            flipEnablement (row);
        }

        void paint (Graphics& g) override
        {
            ListBox::paint (g);

            if (items.size() == 0)
            {
                g.setColour (Colours::grey);
                g.setFont (13.0f);
                g.drawText (noItemsMessage,
                            0, 0, getWidth(), getHeight() / 2,
                            Justification::centred, true);
            }
        }

        int getBestHeight (int maxHeight)
        {
            return getRowHeight() * jlimit (2, jmax (2, maxHeight / getRowHeight()),
                                            getNumRows())
                       + getOutlineThickness() * 2;
        }

    private:
        //==============================================================================
        const AudioDeviceSetupDetails setup;
        const BoxType type;
        const String noItemsMessage;
        StringArray items;

        static String getNameForChannelPair (const String& name1, const String& name2)
        {
            String commonBit;

            for (int j = 0; j < name1.length(); ++j)
                if (name1.substring (0, j).equalsIgnoreCase (name2.substring (0, j)))
                    commonBit = name1.substring (0, j);

            // Make sure we only split the name at a space, because otherwise, things
            // like "input 11" + "input 12" would become "input 11 + 2"
            while (commonBit.isNotEmpty() && ! CharacterFunctions::isWhitespace (commonBit.getLastCharacter()))
                commonBit = commonBit.dropLastCharacters (1);

            return name1.trim() + " + " + name2.substring (commonBit.length()).trim();
        }

        void flipEnablement (const int row)
        {
            jassert (type == audioInputType || type == audioOutputType);

            if (isPositiveAndBelow (row, items.size()))
            {
                AudioDeviceManager::AudioDeviceSetup config;
                setup.manager->getAudioDeviceSetup (config);

                if (setup.useStereoPairs)
                {
                    BigInteger bits;
                    BigInteger& original = (type == audioInputType ? config.inputChannels
                                                                   : config.outputChannels);

                    for (int i = 0; i < 256; i += 2)
                        bits.setBit (i / 2, original [i] || original [i + 1]);

                    if (type == audioInputType)
                    {
                        config.useDefaultInputChannels = false;
                        flipBit (bits, row, setup.minNumInputChannels / 2, setup.maxNumInputChannels / 2);
                    }
                    else
                    {
                        config.useDefaultOutputChannels = false;
                        flipBit (bits, row, setup.minNumOutputChannels / 2, setup.maxNumOutputChannels / 2);
                    }

                    for (int i = 0; i < 256; ++i)
                        original.setBit (i, bits [i / 2]);
                }
                else
                {
                    if (type == audioInputType)
                    {
                        config.useDefaultInputChannels = false;
                        flipBit (config.inputChannels, row, setup.minNumInputChannels, setup.maxNumInputChannels);
                    }
                    else
                    {
                        config.useDefaultOutputChannels = false;
                        flipBit (config.outputChannels, row, setup.minNumOutputChannels, setup.maxNumOutputChannels);
                    }
                }

                String error (setup.manager->setAudioDeviceSetup (config, true));

                if (error.isNotEmpty())
                {
                    //xxx
                }
            }
        }

        static void flipBit (BigInteger& chans, int index, int minNumber, int maxNumber)
        {
            const int numActive = chans.countNumberOfSetBits();

            if (chans [index])
            {
                if (numActive > minNumber)
                    chans.setBit (index, false);
            }
            else
            {
                if (numActive >= maxNumber)
                {
                    const int firstActiveChan = chans.findNextSetBit (0);

                    chans.setBit (index > firstActiveChan
                                     ? firstActiveChan : chans.getHighestBit(),
                                  false);
                }

                chans.setBit (index, true);
            }
        }

        int getTickX() const
        {
            return getRowHeight();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChannelSelectorListBox)
    };

private:
    ScopedPointer<ChannelSelectorListBox> inputChanList, outputChanList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioDeviceSettingsPanel)
};


//==============================================================================
AudioDeviceSelectorComponent::AudioDeviceSelectorComponent (AudioDeviceManager& dm,
                                                            const int minInputChannels_,
                                                            const int maxInputChannels_,
                                                            const int minOutputChannels_,
                                                            const int maxOutputChannels_,
                                                            const bool showMidiInputOptions,
                                                            const bool showMidiOutputSelector,
                                                            const bool showChannelsAsStereoPairs_,
                                                            const bool hideAdvancedOptionsWithButton_)
    : deviceManager (dm),
      itemHeight (24),
      minOutputChannels (minOutputChannels_),
      maxOutputChannels (maxOutputChannels_),
      minInputChannels (minInputChannels_),
      maxInputChannels (maxInputChannels_),
      showChannelsAsStereoPairs (showChannelsAsStereoPairs_),
      hideAdvancedOptionsWithButton (hideAdvancedOptionsWithButton_)
{
    jassert (minOutputChannels >= 0 && minOutputChannels <= maxOutputChannels);
    jassert (minInputChannels >= 0 && minInputChannels <= maxInputChannels);

    const OwnedArray<AudioIODeviceType>& types = deviceManager.getAvailableDeviceTypes();

    if (types.size() > 1)
    {
        deviceTypeDropDown = new ComboBox (String());

        for (int i = 0; i < types.size(); ++i)
            deviceTypeDropDown->addItem (types.getUnchecked(i)->getTypeName(), i + 1);

        addAndMakeVisible (deviceTypeDropDown);
        deviceTypeDropDown->addListener (this);

        deviceTypeDropDownLabel = new Label (String(), TRANS("Audio device type:"));
        deviceTypeDropDownLabel->setJustificationType (Justification::centredRight);
        deviceTypeDropDownLabel->attachToComponent (deviceTypeDropDown, true);
    }

    if (showMidiInputOptions)
    {
        addAndMakeVisible (midiInputsList
                            = new MidiInputSelectorComponentListBox (deviceManager,
                                                                     "(" + TRANS("No MIDI inputs available") + ")"));

        midiInputsLabel = new Label (String(), TRANS ("Active MIDI inputs:"));
        midiInputsLabel->setJustificationType (Justification::topRight);
        midiInputsLabel->attachToComponent (midiInputsList, true);

        if (BluetoothMidiDevicePairingDialogue::isAvailable())
        {
            addAndMakeVisible (bluetoothButton = new TextButton (TRANS("Bluetooth MIDI"),
                                                                 TRANS("Scan for bluetooth MIDI devices")));
            bluetoothButton->addListener (this);
        }
    }
    else
    {
        midiInputsList = nullptr;
        midiInputsLabel = nullptr;
        bluetoothButton = nullptr;
    }

    if (showMidiOutputSelector)
    {
        addAndMakeVisible (midiOutputSelector = new ComboBox (String()));
        midiOutputSelector->addListener (this);

        midiOutputLabel = new Label ("lm", TRANS("MIDI Output:"));
        midiOutputLabel->attachToComponent (midiOutputSelector, true);
    }
    else
    {
        midiOutputSelector = nullptr;
        midiOutputLabel = nullptr;
    }

    deviceManager.addChangeListener (this);
    updateAllControls();
    startTimer (1000);
}

AudioDeviceSelectorComponent::~AudioDeviceSelectorComponent()
{
    deviceManager.removeChangeListener (this);
}

void AudioDeviceSelectorComponent::setItemHeight (int newItemHeight)
{
    itemHeight = newItemHeight;
    resized();
}

void AudioDeviceSelectorComponent::resized()
{
    Rectangle<int> r (proportionOfWidth (0.35f), 15, proportionOfWidth (0.6f), 3000);
    const int space = itemHeight / 4;

    if (deviceTypeDropDown != nullptr)
    {
        deviceTypeDropDown->setBounds (r.removeFromTop (itemHeight));
        r.removeFromTop (space * 3);
    }

    if (audioDeviceSettingsComp != nullptr)
    {
        audioDeviceSettingsComp->resized();
        audioDeviceSettingsComp->setBounds (r.removeFromTop (audioDeviceSettingsComp->getHeight())
                                                .withX (0).withWidth (getWidth()));
        r.removeFromTop (space);
    }

    if (midiInputsList != nullptr)
    {
        midiInputsList->setBounds (r.removeFromTop (midiInputsList->getBestHeight (jmin (itemHeight * 8,
                                                                                         getHeight() - r.getY() - space - itemHeight))));
        r.removeFromTop (space);
    }

    if (bluetoothButton != nullptr)
    {
        bluetoothButton->setBounds (r.removeFromTop (24));
        r.removeFromTop (space);
    }

    if (midiOutputSelector != nullptr)
        midiOutputSelector->setBounds (r.removeFromTop (itemHeight));

    r.removeFromTop (itemHeight);
    setSize (getWidth(), r.getY());
}

void AudioDeviceSelectorComponent::timerCallback()
{
    // TODO
    // unfortunately, the AudioDeviceManager only gives us changeListenerCallbacks
    // if an audio device has changed, but not if a MIDI device has changed.
    // This needs to be implemented properly. Until then, we use a workaround
    // where we update the whole component once per second on a timer callback.
    updateAllControls();
}

void AudioDeviceSelectorComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == deviceTypeDropDown)
    {
        if (AudioIODeviceType* const type = deviceManager.getAvailableDeviceTypes() [deviceTypeDropDown->getSelectedId() - 1])
        {
            audioDeviceSettingsComp = nullptr;
            deviceManager.setCurrentAudioDeviceType (type->getTypeName(), true);
            updateAllControls(); // needed in case the type hasn't actually changed
        }
    }
    else if (comboBoxThatHasChanged == midiOutputSelector)
    {
        String midiDeviceName (midiOutputSelector->getText());

        if (midiDeviceName == getNoDeviceString())
            midiDeviceName = String();

        deviceManager.setDefaultMidiOutput (midiDeviceName);
    }
}

void AudioDeviceSelectorComponent::changeListenerCallback (ChangeBroadcaster*)
{
    updateAllControls();
}

void AudioDeviceSelectorComponent::updateAllControls()
{
    if (deviceTypeDropDown != nullptr)
        deviceTypeDropDown->setText (deviceManager.getCurrentAudioDeviceType(), dontSendNotification);

    if (audioDeviceSettingsComp == nullptr
         || audioDeviceSettingsCompType != deviceManager.getCurrentAudioDeviceType())
    {
        audioDeviceSettingsCompType = deviceManager.getCurrentAudioDeviceType();
        audioDeviceSettingsComp = nullptr;

        if (AudioIODeviceType* const type
                = deviceManager.getAvailableDeviceTypes() [deviceTypeDropDown == nullptr
                                                            ? 0 : deviceTypeDropDown->getSelectedId() - 1])
        {
            AudioDeviceSetupDetails details;
            details.manager = &deviceManager;
            details.minNumInputChannels = minInputChannels;
            details.maxNumInputChannels = maxInputChannels;
            details.minNumOutputChannels = minOutputChannels;
            details.maxNumOutputChannels = maxOutputChannels;
            details.useStereoPairs = showChannelsAsStereoPairs;

            AudioDeviceSettingsPanel* sp = new AudioDeviceSettingsPanel (*type, details, hideAdvancedOptionsWithButton);
            audioDeviceSettingsComp = sp;
            addAndMakeVisible (sp);
            sp->updateAllControls();
        }
    }

    if (midiInputsList != nullptr)
    {
        midiInputsList->updateDevices();
        midiInputsList->updateContent();
        midiInputsList->repaint();
    }

    if (midiOutputSelector != nullptr)
    {
        midiOutputSelector->clear();

        const StringArray midiOuts (MidiOutput::getDevices());

        midiOutputSelector->addItem (getNoDeviceString(), -1);
        midiOutputSelector->addSeparator();

        for (int i = 0; i < midiOuts.size(); ++i)
            midiOutputSelector->addItem (midiOuts[i], i + 1);

        int current = -1;

        if (deviceManager.getDefaultMidiOutput() != nullptr)
            current = 1 + midiOuts.indexOf (deviceManager.getDefaultMidiOutputName());

        midiOutputSelector->setSelectedId (current, dontSendNotification);
    }

    resized();
}

void AudioDeviceSelectorComponent::buttonClicked (Button* btn)
{
    if (bluetoothButton == btn)
    {
        if (! RuntimePermissions::isGranted (RuntimePermissions::bluetoothMidi))
            RuntimePermissions::request (RuntimePermissions::bluetoothMidi, nullptr);

        if (RuntimePermissions::isGranted (RuntimePermissions::bluetoothMidi))
            BluetoothMidiDevicePairingDialogue::open();
    }
}

ListBox* AudioDeviceSelectorComponent::getMidiInputSelectorListBox() const noexcept
{
    return midiInputsList;
}

} // namespace juce
