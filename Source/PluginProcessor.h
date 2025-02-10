/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

struct MIDIMsg {
    int offset;
    int isNoteon;
    int note;
    int vel;
};

struct PolyMsg {
    int note;
    int elapsed;
    int nvoice;
    bool release;
    double vel;
    double freq;
    double impulse;
};

//==============================================================================
/**
*/
class RipplerXAudioProcessor  : public juce::AudioProcessor
{
public:
    float scale = 1.0f;
    int polyphony = 8;
    bool velMap = false; // config used by UI to set velocity edit mode

    //==============================================================================
    RipplerXAudioProcessor();
    ~RipplerXAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    float normalizeVolSlider(float val);
    double note2freq(int note);
    void onNote (MIDIMsg msg);
    void offNote (MIDIMsg msg);
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void loadSettings();
    void saveSettings();
    void setPolyphony(int value);
    void setScale(float value);

    juce::MidiKeyboardState keyboardState;
    juce::AudioProcessorValueTreeState params;
    juce::UndoManager undoManager;

private:
    juce::ApplicationProperties settings;
    std::vector<MIDIMsg> midi;
    std::vector<PolyMsg> notes;
    int nvoice; // next 
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RipplerXAudioProcessor)
};
