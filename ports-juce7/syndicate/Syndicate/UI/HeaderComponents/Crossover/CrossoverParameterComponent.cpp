#include "CrossoverParameterComponent.h"

#include <algorithm>
#include "WEFilters/StereoWidthProcessorParameters.h"
#include "UIUtilsCrossover.h"
#include "General/CoreMath.h"

namespace {
    constexpr int FREQUENCY_TEXT_HEIGHT {16};
    constexpr int FREQUENCY_TEXT_MARGIN {2};
}

CrossoverParameterComponent::CrossoverParameterComponent(SyndicateAudioProcessor& processor)
        : _processor(processor) {
    _rebuildChainLabels();
    _setLabelsText();
}

void CrossoverParameterComponent::paint(juce::Graphics &g) {
    _drawSliderThumbs(g);
    _drawFrequencyText(g);
}

void CrossoverParameterComponent::resized() {
    _resizeChainLabels();
}

void CrossoverParameterComponent::onNumChainsChanged() {
    _rebuildChainLabels();
    _setLabelsText();
    resized();
}

void CrossoverParameterComponent::_drawSliderThumbs(juce::Graphics& g) {
    ModelInterface::forEachCrossover(_processor.manager, [&](float crossoverFrequency) {
        const double crossoverXPos {
            UIUtils::Crossover::sliderValueToXPos(crossoverFrequency, getWidth())
        };

        const int lineLength {getHeight() / 2 - FREQUENCY_TEXT_HEIGHT / 2 - FREQUENCY_TEXT_MARGIN};

        juce::Path p;
        p.startNewSubPath(crossoverXPos, 0);
        p.lineTo(crossoverXPos, lineLength);

        p.startNewSubPath(crossoverXPos, getHeight() - lineLength);
        p.lineTo(crossoverXPos, getHeight());

        g.setColour(UIUtils::highlightColour);
        g.strokePath(p, juce::PathStrokeType(0.5f));
    });
}

void CrossoverParameterComponent::_drawFrequencyText(juce::Graphics &g) {
    constexpr int WIDTH {100};
    const int yPos {getHeight() / 2 - FREQUENCY_TEXT_HEIGHT / 2};

    g.setColour(UIUtils::highlightColour);

    ModelInterface::forEachCrossover(_processor.manager, [&](float crossoverFrequency) {

        const double crossoverXPos {
            UIUtils::Crossover::sliderValueToXPos(crossoverFrequency, getWidth())
        };

        g.drawText(
            juce::String(static_cast<int>(crossoverFrequency)) + " Hz",
            crossoverXPos - WIDTH / 2,
            yPos,
            WIDTH,
            FREQUENCY_TEXT_HEIGHT,
            juce::Justification::centred,
            false);
    });
}

void CrossoverParameterComponent::_rebuildChainLabels() {
    _chainLabels.clear();
    _secondaryLabels.clear();

    ModelInterface::forEachChain(_processor.manager, [&](int bandNumber, std::shared_ptr<PluginChain>) {
        _chainLabels.emplace_back(std::make_unique<juce::Label>("Chain Label", TRANS("")));
        addAndMakeVisible(_chainLabels.back().get());
        _chainLabels.back()->setText(juce::String(bandNumber + 1), juce::dontSendNotification);
        _chainLabels.back()->setJustificationType(juce::Justification::centred);
        _chainLabels.back()->setEditable(true, true, false);
        _chainLabels.back()->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        _chainLabels.back()->setColour(juce::Label::textColourId, UIUtils::highlightColour);
        _chainLabels.back()->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        _chainLabels.back()->setColour(juce::Label::backgroundWhenEditingColourId, UIUtils::backgroundColour);
        _chainLabels.back()->setColour(juce::Label::textWhenEditingColourId, UIUtils::highlightColour);
        _chainLabels.back()->setColour(juce::Label::outlineWhenEditingColourId, juce::Colours::transparentBlack);
        _chainLabels.back()->setColour(juce::TextEditor::highlightColourId, UIUtils::highlightColour);
        _chainLabels.back()->setColour(juce::TextEditor::highlightedTextColourId, UIUtils::neutralColour);
        _chainLabels.back()->setColour(juce::CaretComponent::caretColourId, UIUtils::highlightColour);

        auto chainLabel = _chainLabels.back().get();
        _chainLabels.back()->onTextChange = [this, bandNumber, chainLabel] () {
            _processor.setChainCustomName(bandNumber, chainLabel->getText());
        };

        _secondaryLabels.emplace_back(std::make_unique<juce::Label>("Secondary Label", TRANS("")));
        addAndMakeVisible(_secondaryLabels.back().get());
        _secondaryLabels.back()->setFont(juce::Font(12.00f, juce::Font::plain).withTypefaceStyle("Regular"));
        _secondaryLabels.back()->setJustificationType(juce::Justification::centred);
        _secondaryLabels.back()->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        _secondaryLabels.back()->setColour(juce::Label::textColourId, UIUtils::highlightColour.withBrightness(0.7));
        _secondaryLabels.back()->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        _secondaryLabels.back()->toBehind(_chainLabels.back().get());
    });
}

void CrossoverParameterComponent::_resizeChainLabels() {
   double xPosLeft {0};

    for (size_t index {0}; index < _chainLabels.size(); index++) {
        const double xPosRight {
            index < _chainLabels.size() - 1 ?
                UIUtils::Crossover::sliderValueToXPos(ModelInterface::getCrossoverFrequency(_processor.manager, index), getWidth()) :
                getWidth()
        };

        _chainLabels[index]->setBounds(
            xPosLeft + UIUtils::Crossover::SLIDER_THUMB_TARGET_WIDTH,
            0,
            xPosRight - xPosLeft - UIUtils::Crossover::SLIDER_THUMB_TARGET_WIDTH * 2, // * 2 to subtract the space added on the left and then make space on the right
            getHeight());

        _secondaryLabels[index]->setBounds(
            xPosLeft + UIUtils::Crossover::SLIDER_THUMB_TARGET_WIDTH,
            0,
            xPosRight - xPosLeft - UIUtils::Crossover::SLIDER_THUMB_TARGET_WIDTH * 2, // * 2 to subtract the space added on the left and then make space on the right
            12);

        xPosLeft = xPosRight;
    }
}

void CrossoverParameterComponent::_setLabelsText() {
    for (size_t index {0}; index < _chainLabels.size(); index++) {
        juce::String customName = ModelInterface::getChainCustomName(_processor.manager, index);

        if (customName.isEmpty()) {
            _chainLabels[index]->setText(juce::String(index + 1), juce::dontSendNotification);
            _secondaryLabels[index]->setVisible(false);
        } else {
            _chainLabels[index]->setText(customName, juce::dontSendNotification);
            _secondaryLabels[index]->setText(juce::String(index + 1), juce::dontSendNotification);
            _secondaryLabels[index]->setVisible(true);
        }
    }
}
