#include "Resonator.h"
#include <cmath>
#include <JuceHeader.h>

Resonator::Resonator()
{
	for (int i = 0; i < globals::MAX_PARTIALS; ++i) {
		partials.push_back(Partial(i + 1));
	}
}

void Resonator::setParams(double _srate, bool _on, int model, int _partials, double _decay, double damp, double tone, double hit,
	double _rel, double inharm, double _cut, double _radius, double vel_decay, double vel_hit, double vel_inharm)
{
	on = _on;
	nmodel = model;
	npartials = _partials;
	decay = _decay;
	radius = _radius;
	rel = _rel;
	srate = _srate;
	cut = _cut;

	filter.hp(srate, cut, 0.707);

	for (Partial& partial : partials) {
		partial.damp = damp;
		partial.decay = decay;
		partial.hit = hit;
		partial.inharm = inharm;
		partial.rel = _rel;
		partial.tone = tone;
		partial.vel_decay = vel_decay;
		partial.vel_hit = vel_hit;
		partial.vel_inharm = vel_inharm;
		partial.srate = _srate;
	}

	waveguide.decay = decay;
	waveguide.radius = radius;
	waveguide.is_closed = model == Models::ClosedTube;
	waveguide.srate = srate;
	waveguide.vel_decay = vel_decay;
	waveguide.rel = rel;
}

void Resonator::update(double freq, double vel, bool isRelease, std::array<double,64> model)
{
	if (nmodel < 7) {
		for (Partial& partial : partials) {
			auto idx = partial.k - 1; // clears lnt-arithmetic-overflow warning when accessing _model[k-1] directly
			partial.update(freq, model[idx], model[model.size() - 1], vel, isRelease);
		}
	}
	else
		waveguide.update(model[0] * freq, vel, isRelease);
}

void Resonator::activate()
{
	active = true;
	silence = 0;
}

double Resonator::process(double input)
{
	double out = 0.0;

	if (active) { // use active and silence to turn off strings process if not in use
		if (nmodel < 7) {
			for (int p = 0; p < npartials; ++p) {
				out += partials[p].process(input);
			}
		}
		else
			out += waveguide.process(input); // waveguide process
	}

	if (fabs(out) + fabs(input) > 0.00001)
		silence = 0;
	else
        silence += 1;

	if (silence >= srate)
		active = false;

	return out;
}

void Resonator::clear()
{
	for (Partial& partial : partials) {
		partial.clear();
	}
	waveguide.clear();
}
