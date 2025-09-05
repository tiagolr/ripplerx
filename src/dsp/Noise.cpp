#include "Noise.h"
#include <cstdlib>
#include <cmath>

void Noise::init(
	double _srate, int filterMode, double _freq, double _q, double _att, double _dec, double _sus, 
	double _rel, double _vel_freq, double _vel_q, double _att_ten, double _dec_ten, double _rel_ten,
	double _vel_att, double _vel_dec, double _vel_sus, double _vel_rel
)
{
	srate = _srate;
	fmode = filterMode;
	freq = _freq;
	q = _q;
	vel_freq = _vel_freq;
	vel_q = _vel_q;
	initFilter();

	att = _att;
	dec = _dec;
	sus = _sus;
	rel = _rel;
	
	att_ten = _att_ten;
	dec_ten = _dec_ten;
	rel_ten = _rel_ten;

	vel_att = _vel_att;
	vel_dec = _vel_dec;
	vel_sus = _vel_sus;
	vel_rel = _vel_rel;
	
	initEnvelope();
}

void Noise::attack(double _vel)
{
	vel = _vel;
	initFilter();
	initEnvelope();
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

static double susToDb(double val) {
	return val * 60.0 - 60.0;
}

void Noise::initEnvelope()
{
	auto normalizeVol = [](double val) {
		return val * 60.0 / 100.0 - 60.0;
	};

	auto _att = fmax(1.0, fmin(20000.0, exp(log(att) + vel * vel_att * (log(20000.0) - log(1.0)))));
	auto _dec = fmax(1.0, fmin(20000.0, exp(log(dec) + vel * vel_dec * (log(20000.0) - log(1.0)))));
	auto _sus = fmax(0.0, fmin(1.0, sus + vel * vel_sus));
	auto _rel = fmax(1.0, fmin(20000.0, exp(log(rel) + vel * vel_rel * (log(20000.0) - log(1.0)))));

	env.init(srate, _att, _dec, susToDb(_sus), _rel, att_ten, dec_ten, rel_ten);
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

