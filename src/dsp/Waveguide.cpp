#include <cmath>
#include "JuceHeader.h"
#include "Waveguide.h"

void Waveguide::update(double f_0, double vel, bool isRelease)
{
	auto tlen = srate / f_0;
	if (is_closed) tlen *= 0.5; // fix closed tube one octave lower
	read_ptr = (int)(write_ptr - tlen + tube_len) % tube_len;

	auto decay_k = fmin(100.0, exp(log(decay) + vel * vel_decay * (log(100) - log(0.01))));
	if (isRelease) decay_k *= rel;
	tube_decay = decay_k
		? exp(-juce::MathConstants<double>::pi / f_0 / (srate * decay_k / 125000)) // 125000 set by hear so that decay approximates in seconds
		: 0.0;
}

double Waveguide::process(double input)
{
	auto sample = tube[read_ptr];
	// Apply lowpass filter for frequency damping (tube radius)
	y = radius * sample + (1.0 - radius) * y1;
	y1 = y;

	// Apply decay to sample
	auto dsample = y * tube_decay;
	if (is_closed) dsample *= -1.0; // closed tube, inverting dsample causes only odd harmonics
	tube[write_ptr] = input + dsample;

	write_ptr = (write_ptr + 1) % tube_len;
	read_ptr = (read_ptr + 1) % tube_len;

	return dsample;
}

void Waveguide::clear()
{
	y = y1 = read_ptr = write_ptr = 0;
	std::fill_n(tube.get(), tube_len, 0.0);
}