#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel(bool _isDark)
{
  isDark = _isDark;
  setColour(ComboBox::backgroundColourId, Colour(globals::COLOR_BACKGROUND));
  setColour(ComboBox::textColourId, Colour(globals::COLOR_ACTIVE));
  setColour(ComboBox::arrowColourId, Colour(globals::COLOR_ACTIVE));
  setColour(ComboBox::outlineColourId, Colour(globals::COLOR_ACTIVE));
  setColour(TooltipWindow::backgroundColourId, Colour(globals::COLOR_ACTIVE_L).darker(0.4f));
  setColour(PopupMenu::backgroundColourId, Colour(globals::COLOR_ACTIVE_L).darker(0.4f).withAlpha(0.99f));
  setColour(PopupMenu::highlightedBackgroundColourId, Colour(globals::COLOR_ACTIVE_L).darker(0.8f));

  typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::UbuntuMedium_ttf, BinaryData::UbuntuMedium_ttfSize);
  setDefaultSansSerifTypeface(typeface);
  this->setDefaultLookAndFeel(this);
}

void CustomLookAndFeel::drawLinearSlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style,
    juce::Slider& slider)
{
    if (style == juce::Slider::LinearBar)
    {
        // Draw a filled bar
        g.setColour(Colours::black.withAlpha(0.5f));
        g.fillRoundedRectangle(x - 0.5f, y-0.5f, (float)width, (float)height, 3.f); // background

        g.setColour(Colour(globals::COLOR_ACTIVE));
        g.fillRoundedRectangle(x-0.5f, y-0.5f, static_cast<float>(sliderPos - x), (float)height, 3.f); // filled portion

        //g.drawRoundedRectangle(x - 0.5f, y - 0.5f, (float)width, (float)height, 3.f, 1.f);

        if (slider.getComponentID() == "noise_dc") {
            String text;
            if (slider.isMouseButtonDown())       // mouse is pressed
                text = String(std::round(slider.getValue() * 1000) / 10.f) + " %";
            else
                text = "DC";

            g.setColour(Colours::white);
            g.setFont(15.0f);
            g.drawFittedText(text, x, y, width, height , juce::Justification::centred, 1);
        }
    }
    else
    {
        // default behavior for other linear sliders
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
    }
}

// Override the getTypefaceForFont function
juce::Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const juce::Font& /*font*/)
{
    return typeface;
}

int CustomLookAndFeel::getPopupMenuBorderSize()
{
    return 5;
}