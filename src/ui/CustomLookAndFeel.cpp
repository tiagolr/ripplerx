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
  setColour(TextButton::buttonColourId, Colour(globals::COLOR_ACTIVE));
  setColour(TextButton::buttonOnColourId, Colour(globals::COLOR_ACTIVE));
  setColour(TextButton::textColourOnId, Colour(globals::COLOR_BACKGROUND));
  setColour(TextButton::textColourOffId, Colour(globals::COLOR_ACTIVE));

  typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::UbuntuMedium_ttf, BinaryData::UbuntuMedium_ttfSize);
  setDefaultSansSerifTypeface(typeface);
  this->setDefaultLookAndFeel(this);
}

void CustomLookAndFeel::drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown)
{
    auto tag = button.getComponentID();
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerSize = 3.0f;

    if (tag == "button") {
        g.setColour(backgroundColour);
        if (button.getToggleState())
            g.fillRoundedRectangle(bounds, cornerSize);
        else
            g.drawRoundedRectangle(bounds.reduced(0.5f, 0.5f), cornerSize, 1.0f);
        return;
    }

    LookAndFeel_V4::drawButtonBackground(g, button, backgroundColour, isMouseOverButton, isButtonDown);
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
        auto bounds = Rectangle<int>(x, y, width, height).toFloat();
        // Draw a filled bar
        g.setColour(Colour(globals::COLOR_ACTIVE).withAlpha(0.5f));
        g.fillRoundedRectangle(bounds.reduced(0.5f), 3.f); // background

        g.setColour(Colour(globals::COLOR_ACTIVE));
        g.fillRoundedRectangle(bounds.reduced(0.5).withWidth(static_cast<float>(sliderPos - x) - 0.5f), 3.f); // filled portion

        if (slider.getComponentID() == "noise_osc") {
            String text;
            if (slider.isMouseButtonDown())       // mouse is pressed
                text = String(std::round(slider.getValue() * 1000) / 10.f) + " %";
            else
                text = "OSC";

            g.setColour(isDark ? Colour(globals::COLOR_BACKGROUND).darker(0.7f) : Colours::white);
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