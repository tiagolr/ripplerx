#pragma once

#include <JuceHeader.h>
#include "../Globals.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel(bool isDark);
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font&) override;
    int getPopupMenuBorderSize() override;

private:
    juce::Typeface::Ptr typeface;
};