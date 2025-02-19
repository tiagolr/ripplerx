#include "Envelope.h"
#include "cmath"

// Normalize tension from [-1,1] to [0.001..1, 100 (linear), 2..1]
double Envelope::normalizeTension(double t) 
{
	t += 1.0;
	return t == 1.0 ? 100.0 
		: t > 1.0 ? 3.001 - t : 0.001 + t;
}

void Envelope::init(double srate, double a, double d, double s, double r, double tensionA, double tensionD, double tensionR) {
	att = fmax(a, 1.0) * 0.001 * srate;
	dec = fmax(d, 1.0) * 0.001 * srate;
	sus = pow(10.0, fmin(s, 0.0) / 20.0);
	rel = fmax(r, 1.0) * 0.001 * srate;

	ta = normalizeTension(tensionA);
	td = normalizeTension(-1.0 * tensionD);
	tr = normalizeTension(-1.0 * tensionR);
}

std::tuple<double, double> Envelope::calcCoefs(double targetB1, double targetB2, double targetC, double rate, double tension, double mult)
{
	double c, b, t;
	if (tension > 1.0) {  // slow-start shape
		t = pow(tension - 1, 3.0);
		c = exp(log((targetC + t) / t) / rate);
		b = (targetB1 - mult * t) * (1 - c);
	} else {			  // fast-start shape (inverse exponential)
		t = pow(tension, 3);
		c = exp(-log((targetC + t) / t) / rate);
		b = (targetB2 + mult * t) * (1 - c);
	}
	return std::make_tuple(b, c);
}

void Envelope::recalcCoefs() 
{
	// calculate attack coefficients
	auto [b, c] = calcCoefs(0.0, scale, scale, att, ta, 1.0);
	ab = b;
	ac = c;
	// calculate decay coefficients
	auto [b2, c2] = calcCoefs(1.0, sus*scale, (1.0-sus)*scale, dec, td, -1.0);
	db = b2;
	dc = c2;
}

void Envelope::reset()
{
	state = 0;
	env = 0.0;
}

void Envelope::attack(double _scale)
{
	scale = _scale;
	recalcCoefs();
	state = 1;
}

void Envelope::decay()
{
	env = scale;
	state = 2;
}

void Envelope::sustain()
{
	env = scale * sus;
	state = 4;
}

void Envelope::release()
{
	auto [b, c] = calcCoefs(fmax(env, sus)*scale, 0.0, fmax(env, sus)*scale, rel, tr, -1.0);
	rb = b;
	rc = c;
	state = 8;
}

int Envelope::process()
{
	if (!state) return 0;

	if (state == 1) { // attack
		env = ab + env * ac;
		if (env >= scale) decay();
	}

	else if (state == 2) { // decay
		env = db + env * dc;
		if (env <= sus * scale) sustain();
	}

	else if (state == 8) { // release
		env = rb + env * rc;
		if (env <= 0) reset();
	}

	return state;
}

