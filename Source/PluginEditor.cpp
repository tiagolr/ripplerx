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
    setSize (650, 300);
    auto bounds = getLocalBounds();
    setScaleFactor(audioProcessor.scale);

    r1 = std::make_unique<Rotary>(p, "mallet_mix", "MIX", LabelFormat::Percent);
    addAndMakeVisible(*r1);
    r1->setBounds(250,100,70,80);

    r2 = std::make_unique<Rotary>(p, "mallet_res", "RES", LabelFormat::Percent, "vel_mallet_res");
    addAndMakeVisible(*r2);
    r2->setBounds(250+70,100,70,80);

    addAndMakeVisible(sizeMenu);
    sizeMenu.addItem("100%", 1);
    sizeMenu.addItem("150%", 2);
    sizeMenu.addItem("200%", 3);
    sizeMenu.setSelectedId(audioProcessor.scale == 1.0f ? 1 : audioProcessor.scale == 1.5f ? 2 : 3);
    sizeMenu.onChange = [this]()
    {
        const int value = sizeMenu.getSelectedId();
        scale = value == 1 ? 1.0f : value == 2 ? 1.5f : 2.0f;
        audioProcessor.setScale(scale);
        setScaleFactor(audioProcessor.scale);
    };
    sizeMenu.setBounds(10,10,80,25);

    addAndMakeVisible(slider);
    slider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTooltip("This is some tooltip");
    slider.setTextValueSuffix(" dB");
    slider.setRange(0.0f, 1.0f, 0.01f);
    slider.setPopupDisplayEnabled(true, false, this);
    slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    slider.setBounds(50,150,80,80);

    addAndMakeVisible(label);
    label.setText("Slider Label", dontSendNotification);
    label.setColour(Label::ColourIds::textColourId, Colour(0xFF666666));
    label.attachToComponent(&slider, false);

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
}

//==============================================================================

void RipplerXAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colour(globals::COLOR_BACKGROUND));

    g.setColour(Colour(globals::COLOR_NEUTRAL));
    g.setFont(FontOptions (14.0f));
    //g.drawFittedText("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void RipplerXAudioProcessorEditor::resized()
{
}
