#pragma once

#include <JuceHeader.h>
#include "../Globals.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override;

private:
    juce::Typeface::Ptr typeface;
};