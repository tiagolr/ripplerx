#pragma once

#include <JuceHeader.h>
#include <juce_gui_basics/juce_gui_basics.h>

class RipplerXAudioProcessor;

class Rotary : public juce::SettableTooltipClient, public juce::Component, private juce::AudioProcessorValueTreeState::Listener {
public:
    Rotary(RipplerXAudioProcessor& p, juce::String paramId, juce::String name);
    ~Rotary() override;
    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;

    void parameterChanged (const juce::String& parameterID, float newValue) override;

protected:
    juce::String paramId;
    juce::String name;
    RipplerXAudioProcessor& audioProcessor;

private:
    float deg130 = 130.0f * juce::MathConstants<float>::pi / 180.0f;
    void draw_rotary_slider(juce::Graphics& g, float normed_value);

    float pixels_per_percent{100.0f};
    float cur_normed_value{0.0f};
    juce::Point<int> last_mouse_position;
    juce::Point<int> start_mouse_pos;
};