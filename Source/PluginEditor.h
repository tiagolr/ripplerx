/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ui/Rotary.h"
#include "ui/CustomLookAndFeel.h"

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
    CustomLookAndFeel customLookAndFeel;

    ComboBox sizeMenu;
    ComboBox noiseFilterMenu;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> noiseFilterAttachment;
    Label noiseLabel;
    Label envelopeLabel;
    Label malletLabel;

    std::unique_ptr<Rotary> noiseMix;
    std::unique_ptr<Rotary> noiseRes;
    std::unique_ptr<Rotary> noiseFreq;
    std::unique_ptr<Rotary> noiseQ;
    std::unique_ptr<Rotary> noiseA;
    std::unique_ptr<Rotary> noiseD;
    std::unique_ptr<Rotary> noiseS;
    std::unique_ptr<Rotary> noiseR;

    std::unique_ptr<Rotary> malletMix;
    std::unique_ptr<Rotary> malletRes;
    std::unique_ptr<Rotary> malletStiff;

    TooltipWindow tooltipWindow;
    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RipplerXAudioProcessorEditor)
};
