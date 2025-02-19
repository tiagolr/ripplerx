#pragma once

#include <JuceHeader.h>

class RipplerXAudioProcessor;

class About : public juce::Component {
public:
    About() {}
    ~About() override {}

    void mouseDown(const juce::MouseEvent& e) override;
    void paint(Graphics& g) override;
};