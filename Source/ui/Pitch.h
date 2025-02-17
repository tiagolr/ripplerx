#pragma once

#include <JuceHeader.h>
#include <juce_gui_basics/juce_gui_basics.h>

class RipplerXAudioProcessor;

class Pitch : public juce::Component, private juce::AudioProcessorValueTreeState::Listener {
public:
    Pitch(RipplerXAudioProcessor& p, juce::String name, juce::String coarseParamId, juce::String fineParamId);
    ~Pitch() override;
    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    void applyChange(float change);

    void parameterChanged (const juce::String& parameterID, float newValue) override;

protected:
    juce::String coarse_param_id;
    juce::String fine_param_id;
    RipplerXAudioProcessor& audioProcessor;

private:
    bool is_coarse = false;
    float cur_coarse = 0.0f;
    float cur_fine = 0.0f;
    float pitch = 0.0f;
    juce::Point<int> last_mouse_pos;
    juce::Point<int> start_mouse_pos;
};