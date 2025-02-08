/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RipplerXAudioProcessorEditor::RipplerXAudioProcessorEditor (RipplerXAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (650, 300);
    auto bounds = getLocalBounds();

    addAndMakeVisible(sizeMenu);
    sizeMenu.addItem("100%", 1);
    sizeMenu.addItem("150%", 2);
    sizeMenu.addItem("200%", 3);
    sizeMenu.setSelectedId(1);
    sizeMenu.onChange = [this]()
    {
        const int value = sizeMenu.getSelectedId();
        scale = value == 1 ? 1.0f : value == 2 ? 1.5f : 2.0f;
        setScaleFactor(scale);
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
}

RipplerXAudioProcessorEditor::~RipplerXAudioProcessorEditor()
{
}

//==============================================================================
void RipplerXAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colour(0xffeeeeee));

    g.setColour(Colour(0xff666666));
    g.setFont(FontOptions (16.0f));
    g.drawFittedText("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void RipplerXAudioProcessorEditor::resized()
{
}
