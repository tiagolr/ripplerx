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
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize (650, 300);
    auto bounds = getLocalBounds();
    setScaleFactor(audioProcessor.scale);

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
    keyboardComponent.setBounds(bounds.withTop(bounds.getHeight() - 60));
    keyboardComponent.setLowestVisibleKey(24);
    keyboardComponent.setScrollButtonsVisible(false);
    keyboardComponent.setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, Colour(globals::COLOR_ACTIVE));
    keyboardComponent.setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, Colour(globals::COLOR_ACTIVE));
}

RipplerXAudioProcessorEditor::~RipplerXAudioProcessorEditor()
{
}

//==============================================================================
void RipplerXAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colour(globals::COLOR_BACKGROUND));

    g.setColour(Colour(globals::COLOR_NEUTRAL));
    g.setFont(FontOptions (16.0f));
    g.drawFittedText("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void RipplerXAudioProcessorEditor::resized()
{
}
