#pragma once

#include <JuceHeader.h>
#include "../Globals.h"

class RipplerXAudioProcessor;

class Bulb : public juce::Component
{
public:
	Bulb(){};
	void paint(Graphics& g) override;
    void setOn(bool isOn);
	bool isOn = false;
    int elapsed = 0;
    bool isDark = false;
	juce::Colour colour{};
};

class Meter : public juce::Component, Timer {
public:
    Meter(RipplerXAudioProcessor& p);
    ~Meter() override;

    void timerCallback() override;
    void paint(Graphics& g) override;
    void resized() override;

private:
    std::vector<std::unique_ptr<Bulb>> bulbs;
    const int totalBulbs = 4;
    RipplerXAudioProcessor& audioProcessor;
};