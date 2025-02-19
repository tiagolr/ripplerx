#include "About.h"

void About::mouseDown(const juce::MouseEvent& e) 
{
	(void)e;
	setVisible(false);
};

void About::paint(Graphics& g) 
{
	auto bounds = getBounds().reduced(50);
	g.setColour(Colour(0xdd000000));
	g.fillRect(bounds);

	bounds.reduce(20,20);
	g.setColour(Colours::white);
	g.setFont(FontOptions(30.f));
	g.drawText("RipplerX", bounds.removeFromTop(35), Justification::centred);
	g.setFont(FontOptions(20.f));
	g.drawText(std::string("v") + PROJECT_VERSION, bounds.removeFromTop(25), Justification::centred);
	g.setFont(FontOptions(16.0f));
	g.drawText("Copyright (C) Tilr 2025", bounds.removeFromTop(22), Justification::centred);
	g.drawText("github.com/tiagolr/ripplerx", bounds.removeFromTop(22), Justification::centred);
	bounds.removeFromTop(40);
	g.drawText("- Ctrl for fine slider adjustments.", bounds.removeFromTop(22), Justification::centredLeft);
	g.drawText("- Shift to adjust velocity mapping.", bounds.removeFromTop(22), Justification::centredLeft);
	g.drawText("- Double click to reset sliders.", bounds.removeFromTop(22), Justification::centredLeft);
	g.drawText("- Rippler supports Audio In:", bounds.removeFromTop(22), Justification::centredLeft);
	g.drawText("  Depending on the DAW create an audio routing into the synth.", bounds.removeFromTop(22), Justification::centredLeft);
	g.drawText("  Play the audio and play notes, the sound should excite the resonators.", bounds.removeFromTop(22), Justification::centredLeft);
	g.drawText("  Check the github link for more details.", bounds.removeFromTop(22), Justification::centredLeft);

};

