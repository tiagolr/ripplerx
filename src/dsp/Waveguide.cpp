#include <cmath>
#include "JuceHeader.h"
#include "Waveguide.h"

void Waveguide::update(double f_0, double vel, double pitch_bend, bool isRelease)
{
	base_freq = f_0;
	auto f_k = base_freq * pitch_bend;
	auto tlen = srate / f_k;
	if (is_closed) tlen *= 0.5; // fix closed tube one octave lower
	read_ptr_frac = write_ptr - tlen;
	if (read_ptr_frac < 0) read_ptr_frac += tube_len;

	auto decay_k = fmin(100.0, exp(log(decay) + vel * vel_decay * (log(100) - log(0.01))));
	if (isRelease) decay_k *= rel;
	tube_decay = decay_k
		? exp(-juce::MathConstants<double>::pi / base_freq / (srate * decay_k / 125000)) // 125000 set by hear so that decay approximates in seconds
		: 0.0;
}

void Waveguide::applyPitchBend(double pitch_bend)
{
	auto f_k = base_freq * pitch_bend;
	auto tlen = srate / f_k;
	if (is_closed) tlen *= 0.5; // fix closed tube one octave lower

	// Compute fractional read position
	read_ptr_frac = write_ptr - tlen;
	if (read_ptr_frac < 0) read_ptr_frac += tube_len;
}

double Waveguide::process(double input)
{
	int i0 = (int)read_ptr_frac;
	int i1 = (i0 + 1) % tube_len;
	double frac = read_ptr_frac - i0;
	double sample = tube[i0] * (1.0 - frac) + tube[i1] * frac;

	// Apply lowpass filter for frequency damping (tube radius)
	y = radius * sample + (1.0 - radius) * y1;
	y1 = y;

	// Apply decay to sample
	auto dsample = y * tube_decay;
	if (is_closed) dsample *= -1.0; // closed tube: only odd harmonics
	tube[write_ptr] = input + dsample;

	// Increment pointers
	write_ptr = (write_ptr + 1) % tube_len;
	read_ptr_frac += 1.0;
	if (read_ptr_frac >= tube_len) read_ptr_frac -= tube_len;

	return dsample;
}

void Waveguide::clear()
{
	y = y1 = write_ptr = 0;
	read_ptr_frac = 0.0;
	std::fill_n(tube.get(), tube_len, 0.0);
}