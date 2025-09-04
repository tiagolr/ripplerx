#pragma once

#include <JuceHeader.h>

class RipplerXAudioProcessor;

class TensionCtrl : public juce::Component, private juce::AudioProcessorValueTreeState::Listener {
public:
    TensionCtrl(RipplerXAudioProcessor& p, juce::String paramId, bool invert);
    ~TensionCtrl() override;
    void paint(juce::Graphics& g) override;

    void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;

protected:
    juce::String paramId;
    RipplerXAudioProcessor& audioProcessor;

private:
    bool mouseOver = false;
    bool invert = false;

    float pixels_per_percent{ 100.0f };
    float cur_normed_value{ 0.0f };
    juce::Point<int> last_mouse_position;
    juce::Point<int> start_mouse_pos;
    bool mouse_down = false;
    bool mouse_down_shift = false;
};