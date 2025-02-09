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

    setSize (650, 300);
    auto bounds = getLocalBounds();
    setScaleFactor(audioProcessor.scale);

    r1 = std::make_unique<Rotary>(p, "mallet_mix", "Mix", LabelFormat::Percent);
    addAndMakeVisible(*r1);
    r1->setBounds(250,50,70,75);

    r2 = std::make_unique<Rotary>(p, "mallet_res", "Res", LabelFormat::Percent, "vel_mallet_res");
    addAndMakeVisible(*r2);
    r2->setBounds(250,50+75,70,75);

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
    sizeMenu.setBounds(10,10,80,25);

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
