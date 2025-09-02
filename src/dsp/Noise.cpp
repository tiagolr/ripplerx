#include "Noise.h"
#include <cstdlib>

void Noise::init(double _srate, int filterMode, double _freq, double _q, double att, double dec, double sus, double rel, double _vel_freq, double _vel_q)
{
	srate = _srate;
	fmode = filterMode;
	freq = _freq;
	q = _q;
	vel_freq = _vel_freq;
	vel_q = _vel_q;
	initFilter();
	env.init(srate, att, dec, sus, rel, 0.4, 0.4, 0.4);
}

void Noise::attack(double _vel) 
{
	vel = _vel;
	initFilter();
	env.attack(1.0);
}

void Noise::initFilter()
{
	double f = fmin(20000.0, fmax(20.0, exp(log(freq) + vel * vel_freq * (log(20000.0) - log(20.0)))));
	double res = fmin(4.0, fmax(0.707, q + vel * vel_q * (4.0 - 0.707)));

	filter_active = fmode == 1 || (fmode == 0 && f < 20000.0) || (fmode == 2 && f > 20.0);

	if (fmode == 0) filter.lp(srate, f, res);
	else if (fmode == 1) filter.bp(srate, f, res);
	else if (fmode == 2) filter.hp(srate, f, res);
	else throw "Unknown filter mode";
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
	env.process();
	double sample = (std::rand() / (double)RAND_MAX) * 2.0 - 1.0;
	if (filter_active)
		sample = filter.df1(sample);

	if (!env.state) 
		filter.clear(0.0); // envelope has finished, clear filter to avoid pops

	return sample * env.env;
}

