#include "Mallet.h"
#include "Sampler.h"
#include <cmath>

void Mallet::trigger(MalletType _type, double _srate, double freq)
{
	type = _type;
	srate = _srate;

	if (type == kImpulse) {
		filter.bp(srate, freq, 0.707);
		countdown = (int)(srate / 10.0); // countdown (100ms)
		impulse = 1.0;
		env = exp(-100.0 / srate);
	}
	else {
		playback_speed = sampler.wavesrate / srate;
		playback = 0.0;
	}
}

void Mallet::clear()
{
	countdown = 0;
	impulse = 0.0;
	playback = INFINITY;
}

double Mallet::process()
{
	auto sample = 0.0;

	if (type == kImpulse && countdown > 0) {
		sample = filter.df1(impulse) * 2.0;
		countdown -= 1;
		impulse *= env;
	}
	else if (type >= kUserFile && playback < sampler.waveform.size()) {
		sample = sampler.waveCubic(playback);
		playback += playback_speed * sampler.pitchfactor;
	}

	return sample;
}