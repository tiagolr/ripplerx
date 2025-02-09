/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Globals.h"

//==============================================================================
RipplerXAudioProcessorEditor::RipplerXAudioProcessorEditor (RipplerXAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel(&customLookAndFeel);

    setSize (650, 450);
    auto bounds = getLocalBounds();
    setScaleFactor(audioProcessor.scale);
    auto col = 10;
    auto row = 10; 

    addAndMakeVisible(sizeMenu);
    sizeMenu.addItem("100%", 1);
    sizeMenu.addItem("150%", 2);
    sizeMenu.addItem("200%", 3);
    sizeMenu.setSelectedId(audioProcessor.scale == 1.0f ? 1 : audioProcessor.scale == 1.5f ? 2 : 3);
    sizeMenu.onChange = [this]()
        {
            const int value = sizeMenu.getSelectedId();
            auto scale = value == 1 ? 1.0f : value == 2 ? 1.5f : 2.0f;
            audioProcessor.setScale(scale);
            setScaleFactor(audioProcessor.scale);
        };
    sizeMenu.setBounds(row,col,80,25);

    // NOISE SLIDERS
    col = 10; row += 30;
    noiseLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    noiseLabel.setFont(FontOptions(16.0f));
    addAndMakeVisible(noiseLabel);
    noiseLabel.setText("NOISE", NotificationType::dontSendNotification);
    noiseLabel.setBounds(col, row, 70, 20);
    noiseLabel.setJustificationType(Justification::centred);

    addAndMakeVisible(noiseFilterMenu);
    noiseFilterMenu.addItem("LP", 1);
    noiseFilterMenu.addItem("BP", 2);
    noiseFilterMenu.addItem("HP", 3);
    noiseFilterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.params, "noise_filter_mode", noiseFilterMenu);
    noiseFilterMenu.setBounds(col+70, row, 70, 20);
    row += 20;

    noiseMix = std::make_unique<Rotary>(p, "noise_mix", "Mix", LabelFormat::Percent);
    addAndMakeVisible(*noiseMix);
    noiseMix->setBounds(col,row,70,75);

    noiseRes = std::make_unique<Rotary>(p, "noise_res", "Res", LabelFormat::Percent, "vel_noise_res");
    addAndMakeVisible(*noiseRes);
    noiseRes->setBounds(col,row+75,70,75);

    noiseFreq = std::make_unique<Rotary>(p, "noise_filter_freq", "Freq", LabelFormat::Hz);
    addAndMakeVisible(*noiseFreq);
    noiseFreq->setBounds(col+70,row,70,75);

    noiseQ = std::make_unique<Rotary>(p, "noise_filter_q", "Q", LabelFormat::float1);
    addAndMakeVisible(*noiseQ);
    noiseQ->setBounds(col+70,row+75,70,75);

    row += 160;
    envelopeLabel.setColour(juce::Label::ColourIds::textColourId, Colour(globals::COLOR_NEUTRAL_LIGHT));
    envelopeLabel.setFont(FontOptions(16.0f));
    addAndMakeVisible(envelopeLabel);
    envelopeLabel.setText("ENVELOPE", NotificationType::dontSendNotification);
    envelopeLabel.setBounds(col, row, 100, 20);
    envelopeLabel.setJustificationType(Justification::centred);
    row += 20;

    noiseA = std::make_unique<Rotary>(p, "noise_att", "Attack", LabelFormat::millis);
    addAndMakeVisible(*noiseA);
    noiseA->setBounds(col,row,70,75);

    noiseD = std::make_unique<Rotary>(p, "noise_dec", "Decay", LabelFormat::millis);
    addAndMakeVisible(*noiseD);
    noiseD->setBounds(col,row+75,70,75);

    noiseS = std::make_unique<Rotary>(p, "noise_sus", "Sus", LabelFormat::Hz);
    addAndMakeVisible(*noiseS);
    noiseS->setBounds(col+70,row,70,75);

    noiseR = std::make_unique<Rotary>(p, "noise_rel", "Release", LabelFormat::millis);
    addAndMakeVisible(*noiseR);
    noiseR->setBounds(col+70,row+75,70,75);

    //

    addAndMakeVisible(keyboardComponent);
    keyboardComponent.setMidiChannel(1);
    keyboardComponent.setBounds(bounds.withTop(bounds.getHeight() - 60).withLeft(10).withWidth(bounds.getWidth()-20));
    keyboardComponent.setAvailableRange(24, 100);
    keyboardComponent.setScrollButtonsVisible(false);
    keyboardComponent.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, Colour(globals::COLOR_ACTIVE));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colour(globals::COLOR_ACTIVE).withAlpha(0.5f));
}

RipplerXAudioProcessorEditor::~RipplerXAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================

void RipplerXAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colour(globals::COLOR_BACKGROUND));

    g.setColour(Colour(globals::COLOR_NEUTRAL));
    //g.setFont(FontOptions (14.0f));
    //g.drawFittedText("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void RipplerXAudioProcessorEditor::resized()
{
}
