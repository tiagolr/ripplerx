

#include "Partial.h"
#include <cmath>
#include <JuceHeader.h>

/**
 * a1 coefficient lookup table
 * used for re-tuning of the partial during pitch bends
 */
LookupTable Partial::a1LUT;
void Partial::initA1LUT(double sampleRate)
{
    static double a1LUTSrate = 0.0;
    if (a1LUTSrate != sampleRate) {
        constexpr double fMin = 20.0;
        constexpr double fMax = 20000.0;
        constexpr size_t LUT_SIZE = 4096;

        a1LUT.init(
            [sampleRate](double f) {
                double omega = 2.0 * juce::MathConstants<double>::pi * f / sampleRate;
                return -2.0 * std::cos(omega);
            },
            fMin, fMax, LUT_SIZE
        );

        a1LUTSrate = sampleRate;
    }
}

void Partial::update(double f_0, double ratio, double ratio_max, double vel, double pitch_bend, bool isRelease)
{
	out_of_range = false;
	auto inharm_k = fmax(0.0, fmin(1.0, exp(log(inharm) + vel * vel_inharm * -log(0.0001)) - 0.0001)); // normalize velocity contribution on a logarithmic scale
	inharm_k = sqrt(1 + inharm_k * (ratio - 1) * (ratio - 1));
	auto f_k = f_0 * ratio * inharm_k;
	base_f_k = f_k;
	f_k *= pitch_bend;

	auto decay_k = fmax(0.01, fmin(100.0, exp(log(decay) + vel * vel_decay * (log(100.0) - log(0.01))))); // normalize velocity contribution on a logarithmic scale
	if (isRelease)
		decay_k *= rel;

	if (f_k >= 20000.0 || f_k < 20.0 || decay_k == 0.0) {
		//b0 = b2 = a1 = a2 = 0.0;
		//a0 = 1.0;
		//return;
		out_of_range = true;
	}

	auto f_max = fmin(20000.0, f_0 * ratio_max * inharm_k);
	auto omega = juce::MathConstants<double>::twoPi * f_k / srate;
	auto alpha = juce::MathConstants<double>::twoPi / srate; // aprox 1 sec decay

	auto damp_base = std::fmin(1.0, std::fmax(-1.0, damp + vel_damp * 2.0 * vel));
	auto damp_k = damp_base <= 0
		? pow(f_0 / f_k, damp_base * 2.0)
		: pow(f_max / f_k, damp_base * 2.0);

	decay_k /= damp_k;

	auto tone_base = std::fmin(1.0, std::fmax(-1.0, tone + vel_tone * 2.0 * vel));
	auto tone_gain = tone_base <= 0
		? pow(f_k / f_0, tone_base * 12 / 6)
		: pow(f_k / f_max, tone_base * 12 / 6);

	auto amp_k = fabs(sin(juce::MathConstants<double>::pi * k * fmax(0.02, fmin(.5, hit + vel_hit * vel / 2.0))));
	amp_k *= 35.0;

	// Bandpass filter coefficients (normalized)
	b0 = alpha * tone_gain * amp_k;
	b2 = -alpha * tone_gain * amp_k;
	a0 = decay_k ? 1.0 + alpha / decay_k : 0.0;
	a1 = -2.0 * cos(omega);
	a2 = decay_k ? 1.0 - alpha / decay_k : 0.0;
}

double Partial::process(double input)
{
	if (out_of_range) return 0.0;
	auto output = ((b0 * input + b2 * x2) - (a1 * y1 + a2 * y2)) / a0;
	x2 = x1;
	x1 = input;
	y2 = y1;
	y1 = output;

	return output;
}

void Partial::applyGain(double gain)
{
	b0 *= gain;
	b2 *= gain;
}

void Partial::applyPitchBend(double pitch_bend)
{
	out_of_range = false;
	auto f_k = base_f_k * pitch_bend;
	if (f_k < 20.0 || f_k > 20000.0) {
		out_of_range = true;
		return;
	}
	a1 = a1LUT(f_k);
}

void Partial::clear()
{
	x1 = x2 = y1 = y2 = 0;
}