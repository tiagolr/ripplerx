#include "Filter.h"
#include <cmath>
#include "JuceHeader.h"

void Filter::lp(double srate, double freq, double q)
{
	auto w0 = juce::MathConstants<double>::twoPi * fmin(freq / srate, 0.49);
	auto alpha = sin(w0) / (2.0*q);

	auto a0 = 1.0 + alpha;
	auto scale = 1.0 / a0;
	a1 = cos(w0) * -2.0 * scale;
	a2 = (1.0 - alpha) * scale;

	b2 = b0 = (1.0 + a1 + a2) * 0.25;
	b1 = b0 * 2.0;
}

void Filter::bp(double srate, double freq, double q)
{
	auto w0 = juce::MathConstants<double>::twoPi * fmin(freq / srate, 0.49);
	auto alpha = sin(w0) / (2.0*q);

	auto a0 = 1.0 + alpha;
	auto scale = 1.0/a0;
	a1 = cos(w0) * -2.0 * scale;
	a2 = (1 - alpha) * scale;

	b2 = -(b0 = (1 - a2) * 0.5 * q);
	b1 = 0.0;
}

void Filter::hp(double srate, double freq, double q)
{
	auto w0 = juce::MathConstants<double>::twoPi * fmin(freq / srate, 0.49);
	auto alpha = sin(w0) / (2.0*q);

	auto a0 = 1.0 + alpha;
	auto scale = 1.0 / a0;
	a1 = cos(w0) * -2.0 * scale;
	a2 = (1.0 - alpha) * scale;

	b2 = b0 = (1.0 - a1  + a2) * 0.25;
	b1 = b0 * -2.0;
}

void Filter::clear(double input)
{
	x0 = x1 = input;
	y0 = y1 = input / (1.0 + a1 + a2) * (b0 + b1 + b2);
}

double Filter::df1(double sample)
{
	auto x2 = x1;
	x1 = x0;
	x0 = sample;

	auto y2 = y1;
	y1 = y0;
	y0 = b0*x0 + b1*x1 + b2*x2 - a1*y1 - a2*y2;

	return y0;
}