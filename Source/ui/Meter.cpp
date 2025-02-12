#include "Meter.h"
#include "../PluginProcessor.h"

Meter::Meter(RipplerXAudioProcessor& p)
	: audioProcessor(p)
{
	startTimerHz(24);
}

Meter::~Meter()
{
}

void Meter::timerCallback()
{
	repaint();
}

void Meter::paint(Graphics& g)
{
	// there is a limiter that prevents high volumes so rms is compared with 0.8 to light all bulbs
	// other gimmicks gimmicks like pow(0.25) and (totalBulbs + 1) kinda makes the meter decent
	// but still totally inaccurate
	auto const rms = pow(fmin(1.0f, audioProcessor.rmsValue.load(std::memory_order_acquire)), 0.25f);
	for (auto i = 0; i < totalBulbs; i++)
		bulbs[i]->setOn(rms >= static_cast<float>(i + 1) / (totalBulbs + 1) || rms >= 0.8f);
}

void Meter::resized()
{
	const auto bounds = getLocalBounds().toFloat();
	ColourGradient gradient{ Colours::green, bounds.getBottomLeft(), Colours::red, bounds.getTopLeft(), false };
	gradient.addColour(0.5, Colours::yellow);

	const auto bulbHeight = getLocalBounds().getHeight() / totalBulbs;
	auto bulbBounds = getLocalBounds();
	bulbs.clear();
	for (auto i = 0; i < totalBulbs; i++) {
		auto bulb = std::make_unique<Bulb>();
		bulb->colour = gradient.getColourAtPosition(static_cast<double>(i) / totalBulbs);
		addAndMakeVisible(bulb.get());
		bulb->setBounds(bulbBounds.removeFromBottom(bulbHeight));
		bulbs.push_back(std::move(bulb));
	}			
}

// =============================================================

// use elapsed to smoothly turn off leds, prevents flickering
void Bulb::setOn(bool _isOn) 
{
	if (_isOn) {
		isOn = _isOn;
		elapsed = 6; // 1/4 second
	}
	else if (elapsed <= 0)
		isOn = false;

	elapsed -= 1;
}

void Bulb::paint(Graphics& g) {
	g.setColour(isOn ? colour : Colours::darkgrey);
	const auto delta = 3.f;
	const auto bounds = getLocalBounds().toFloat().reduced(delta);
	const auto side = jmin(bounds.getWidth(), bounds.getHeight());
	const auto bulbFillBounds = Rectangle<float>{ bounds.getX() + bounds.getWidth() / 2.0f - side / 2.0f, bounds.getY(), side, side };
	g.fillEllipse(bulbFillBounds);
	g.setColour(Colours::lightgrey);
	g.drawEllipse(bulbFillBounds, 1.f);
	if (isOn)
	{
		g.setGradientFill(ColourGradient{ colour.withAlpha(0.3f), bulbFillBounds.getCentre(), colour.withLightness(1.5f).withAlpha(0.f), {}, true });
		g.fillEllipse(bulbFillBounds.expanded(2.f));
	}

}