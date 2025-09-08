#include "Mallet.h"
#include "Sampler.h"
#include <cmath>

void Mallet::trigger(MalletType _type, double _srate, double freq, int note, double _ktrack)
{
	type = _type;
	srate = _srate;
	ktrack = _ktrack;

	if (type == kImpulse) {
		impulse_filter.bp(srate, freq, 0.707);
		countdown = (int)(srate / 10.0); // countdown (100ms)
		impulse = 1.0;
		env = exp(-100.0 / srate);
	}
	else {
		keytrack_factor = std::pow(2.0, ((note - 60) / 12.0) * ktrack);
		playback_speed = sampler.wavesrate / srate;
		playback = 0.0;
	}
}

void Mallet::clear()
{
	countdown = 0;
	impulse = 0.0;
	playback = INFINITY;
	impulse_filter.clear(0.0);
	sample_filter.clear(0.0);
}

double Mallet::process()
{
	auto sample = 0.0;

	if (type == kImpulse && countdown > 0) {
		sample = impulse_filter.df1(impulse) * 2.0;
		countdown -= 1;
		impulse *= env;
	}
	else if (type >= kUserFile && playback < sampler.waveform.size()) {
		sample = sampler.waveCubic(playback);
		playback += playback_speed * sampler.pitchfactor * keytrack_factor;

		if (!disable_filter) {
			sample = sample_filter.df1(sample);
		}
	}

	return sample;
}

void Mallet::setFilter(double norm)
{
	double freq = 20.0 * std::pow(20000.0/20.0, norm < 0.0 ? 1 + norm : norm); // map 1..0 to 20..20000, with inverse scale for negative norm
	disable_filter = norm == 0.0;
	if (!disable_filter) {
		if (norm < 0.0) {
			sample_filter.lp(srate, freq, 0.707);
		}
		else {
			sample_filter.hp(srate, freq, 0.707);
		}
	}
}