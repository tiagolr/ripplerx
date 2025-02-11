#include "Partial.h"
#include <cmath>
#include <JuceHeader.h>

// ---------------------------------------------------

void Partial::update(double f_0, double ratio, double ratio_max, double vel, bool isRelease)
{
	auto inharm_k = fmin(1.0, exp(log(inharm) + vel * vel_inharm * -log(0.0001))) - 0.0001; // normalize velocity contribution on a logarithmic scale
	inharm_k = sqrt(1 + inharm_k * (ratio - 1) * (ratio - 1));
	auto f_k = f_0 * ratio * inharm_k;

	if (f_k >= 0.48 * srate || f_k < 20) {
		b0 = b2 = a1 = a2 = 0.0;
		a0 = 1.0;
		return;
	}

	auto f_max = fmin(20000.0, f_0 * ratio_max * inharm_k);
	auto omega = juce::MathConstants<double>::twoPi * f_k / srate;
	auto alpha = juce::MathConstants<double>::twoPi / srate; // aprox 1 sec decay

	auto decay_k = fmin(100.0, exp(log(decay) + vel * vel_decay * (log(100.0) - log(0.01)))); // normalize velocity contribution on a logarithmic scale
	if (isRelease) decay_k *= rel;

	auto damp_k = damp <= 0
		? pow(f_0 / f_k, damp * 2.0)
		: pow(f_max / f_k, damp * 2.0);

	decay_k /= damp_k;

	auto tone_gain = tone <= 0
		? pow(f_k / f_0, tone * 12 / 6)
		: pow(f_k / f_max, tone * 12 / 6);

	auto amp_k = fabs(sin(juce::MathConstants<double>::pi * k * fmin(.5, hit + vel_hit * vel / 2.0)));
	amp_k *= f_k; // push higher frequencies
	amp_k *= 0.01;

	// Bandpass filter coefficients (normalized)
	b0 = alpha * tone_gain * amp_k;
	b2 = -alpha * tone_gain * amp_k;
	a0 = 1.0 + alpha / decay_k;
	a1 = -2.0 * cos(omega);
	a2 = 1.0 - alpha / decay_k;
}

double Partial::process(double input)
{
	auto output = ((b0 * input + b2 * x2) - (a1 * y1 + a2 * y2)) / a0;
	x2 = x1;
	x1 = input;
	y2 = y1;
	y1 = output;

	return output;
}

void Partial::clear()
{
	x1 = x2 = y1 = y2 = 0;
}