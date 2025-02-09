#include "CustomLookAndFeel.h"

CustomLookAndFeel::CustomLookAndFeel()
{
  setColour(ComboBox::backgroundColourId, Colour(globals::COLOR_BACKGROUND));
  setColour(ComboBox::textColourId, Colour(globals::COLOR_ACTIVE));
  setColour(ComboBox::arrowColourId, Colour(globals::COLOR_ACTIVE));
  setColour(ComboBox::outlineColourId, Colour(globals::COLOR_ACTIVE));
  setColour(TooltipWindow::backgroundColourId, Colour(globals::COLOR_ACTIVE).darker(0.4));
  setColour(PopupMenu::backgroundColourId, Colour(globals::COLOR_ACTIVE).darker(0.4));
  setColour(PopupMenu::highlightedBackgroundColourId, Colour(globals::COLOR_ACTIVE).darker(0.8));

  typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::UbuntuMedium_ttf, BinaryData::UbuntuMedium_ttfSize);
  setDefaultSansSerifTypeface(typeface);
  this->setDefaultLookAndFeel(this);
}

// Override the getTypefaceForFont function
juce::Typeface::Ptr CustomLookAndFeel::getTypefaceForFont(const juce::Font& /*font*/)
{
    return typeface;
}