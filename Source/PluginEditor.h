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
    void repaintVelSliders ();

private:
    RipplerXAudioProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;

    Label noiseLabel;
    Label envelopeLabel;
    Label malletLabel;

    // Top bar
    ImageButton logo;
    Label sizeLabel;
    ComboBox sizeMenu;
    Label polyLabel;
    ComboBox polyMenu;
    TextButton velButton;

    // Noise
    ComboBox noiseFilterMenu;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> noiseFilterAttachment;
    std::unique_ptr<Rotary> noiseMix;
    std::unique_ptr<Rotary> noiseRes;
    std::unique_ptr<Rotary> noiseFreq;
    std::unique_ptr<Rotary> noiseQ;
    std::unique_ptr<Rotary> noiseA;
    std::unique_ptr<Rotary> noiseD;
    std::unique_ptr<Rotary> noiseS;
    std::unique_ptr<Rotary> noiseR;

    // Mallet
    std::unique_ptr<Rotary> malletMix;
    std::unique_ptr<Rotary> malletRes;
    std::unique_ptr<Rotary> malletStiff;

    // Res A
    Label aLabel;
    ComboBox aModel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> aModelAttachment;
    ComboBox aPartials;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> aPartialsAttachment;
    std::unique_ptr<Rotary> aDecay;
    std::unique_ptr<Rotary> aDamp;
    std::unique_ptr<Rotary> aTone;
    std::unique_ptr<Rotary> aHit;
    std::unique_ptr<Rotary> aRel;
    std::unique_ptr<Rotary> aInharm;
    std::unique_ptr<Rotary> aRatio;
    std::unique_ptr<Rotary> aCut;
    std::unique_ptr<Rotary> aRadius;

    // Res B
    Label bLabel;
    ComboBox bModel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> bModelAttachment;
    ComboBox bPartials;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> bPartialsAttachment;
    std::unique_ptr<Rotary> bDecay;
    std::unique_ptr<Rotary> bDamp;
    std::unique_ptr<Rotary> bTone;
    std::unique_ptr<Rotary> bHit;
    std::unique_ptr<Rotary> bRel;
    std::unique_ptr<Rotary> bInharm;
    std::unique_ptr<Rotary> bRatio;
    std::unique_ptr<Rotary> bCut;
    std::unique_ptr<Rotary> bRadius;

    // Coupling
    ComboBox couple;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> coupleAttachment;
    std::unique_ptr<Rotary> abMix;
    std::unique_ptr<Rotary> abSplit;
    std::unique_ptr<Rotary> gain;

    TooltipWindow tooltipWindow;
    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RipplerXAudioProcessorEditor)
};
