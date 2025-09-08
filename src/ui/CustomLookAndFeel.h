#pragma once

#include <JuceHeader.h>
#include "../Globals.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel(bool isDark);
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override;
    int getPopupMenuBorderSize() override;
    void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown) override;

    void drawLinearSlider(
        juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const juce::Slider::SliderStyle style,
        juce::Slider& slider
    ) override;

    bool isDark = false;

private:
    juce::Typeface::Ptr typeface;
};