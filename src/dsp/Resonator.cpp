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
	double _rel, double inharm, double _cut, double _radius, double vel_decay, double vel_hit, double vel_inharm, double vel_damp)
{
	on = _on;
	nmodel = model;
	npartials = _partials;
	decay = _decay;
	radius = _radius;
	srate = _srate;
	cut = _cut;

	auto freq = 20.0 * std::pow(20000.0 / 20.0, cut < 0.0 ? 1 + cut : cut); // map 1..0 to 20..20000, with inverse scale for negative norm;
	if (_cut < 0.0) {
		filter.lp(srate, freq, 0.707);
	}
	else {
		filter.hp(srate, freq, 0.707);
	}

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
		partial.vel_damp = vel_damp;
		partial.srate = _srate;
	}

	waveguide.decay = decay;
	waveguide.radius = radius;
	waveguide.is_closed = model == ModelNames::ClosedTube;
	waveguide.srate = srate;
	waveguide.vel_decay = vel_decay;
	waveguide.rel = _rel;
}

void Resonator::update(double freq, double vel, bool isRelease, double pitch_bend, std::array<double,64> model)
{
	if (nmodel < 7) {
		for (Partial& partial : partials) {
			auto idx = partial.k - 1; // clears lnt-arithmetic-overflow warning when accessing _model[k-1] directly
			partial.update(freq, model[idx], model[model.size() - 1], vel, pitch_bend, isRelease);
		}
	}
	else
		waveguide.update(model[0] * freq, vel, pitch_bend, isRelease);
}

void Resonator::applyPitchBend(double bend)
{
	if (active) {
		if (nmodel < 7) {
			for (int p = 0; p < npartials; ++p) {
				partials[p].applyPitchBend(bend);
			}
		}
		else {
			waveguide.applyPitchBend(bend);
		}
	}
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
	filter.clear(0.0);
}
