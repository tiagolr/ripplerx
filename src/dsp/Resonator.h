// Copyright 2025 tilr
// Resonator holds a number of Partials and a Waveguide
// depending on the selected model uses the Partials bank or Waveguide to process input
// the partials are tuned by selected model by Voice.h

#pragma once
#include <vector>
#include <array>
#include "../Globals.h"
#include "Partial.h"
#include "Waveguide.h"
#include "Filter.h"
#include "Models.h"

class Resonator
{
public:
	Resonator();
	~Resonator() {};

	void setParams(double srate, bool on, int model, int partials, double decay, double damp, double tone, double hit,
		double rel, double inharm, double cut,double radius, double vel_decay, double vel_hit, double vel_inharm, 
		double vel_damp, double vel_tone);

	void activate();
	void update(double frequency, double vel, bool isRelease, double pitch_bend, std::array<double, 64> _model, std::array<double, 64> modelGain);
	void clear();
	double process(double x);
	void applyPitchBend(double bend);

	int silence = 0; // counter of samples of silence
	bool active = false; // returns to false if samples of silence run for a bit
	double srate = 0.0;
	bool on = false;
	int nmodel = 0;
	int npartials = 0;
	double decay = 0.0;
	double radius = 0.0;
	double cut = 0.0;

	std::vector<Partial> partials;
	Waveguide waveguide{};
	Filter filter{};

private:

};

