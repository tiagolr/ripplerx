#include "Noise.h"
#include <cstdlib>

void Noise::init(double srate, int filterMode, double freq, double q, double att, double dec, double sus, double rel)
{
	if (filterMode == 0) filter.lp(srate, freq, q);
	else if (filterMode == 1) filter.bp(srate, freq, q);
	else if (filterMode == 2) filter.hp(srate, freq, q);
	else throw "Unknown filter mode";

	fmode = filterMode;
	ffreq = freq;
	env.init(srate, att, dec, sus, rel, 0.4, 0.4, 0.4);
}

void Noise::attack(double _vel) 
{
	vel = _vel;
	env.attack(1.0);
}

void Noise::release()
{
	env.release();
}

void Noise::clear()
{
	env.reset();
	filter.clear(0.0);
}

double Noise::process()
{
	if (!env.state) return 0.0;
	auto state = env.process();
	double sample = (std::rand() / (double)RAND_MAX) * 2.0 - 1.0;
	if (fmode == 1 || (fmode == 0 && ffreq < 20000.0) || (fmode == 2 && ffreq > 20.0)) {
		sample = filter.df1(sample);
	}
	if (!env.state) {
		filter.clear(0.0); // envelope has finished, clear filter to avoid pops
	}
	return sample * env.env;
}

