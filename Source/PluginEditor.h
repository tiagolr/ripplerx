/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ui/Rotary.h"

//==============================================================================
/**
*/
class RipplerXAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    RipplerXAudioProcessorEditor (RipplerXAudioProcessor&);
    ~RipplerXAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    RipplerXAudioProcessor& audioProcessor;

    float scale = 1;
    ComboBox sizeMenu;
    Slider slider;
    Label label;
    std::unique_ptr<Rotary> r1;
    std::unique_ptr<Rotary> r2;

    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RipplerXAudioProcessorEditor)
};
