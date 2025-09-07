// Copyright (C) 2025 tilr
// Noise generator with a filter and envelope
#pragma once
#include "Filter.h"
#include "Envelope.h"

class Noise
{
public:
	Noise() {};
	~Noise() {};

	void init(double srate, int filterMode, double freq, double q, double att, double dec, 
		double sus, double rel, double vel_freq, double vel_q, double att_ten, double dec_ten, double rel_ten,
		double vel_att, double vel_dec, double vel_sus, double vel_rel
	);
	double process();
	void attack(double vel);
	void initFilter();
	void initEnvelope();
	void release();
	void clear();
	double processOSC(double input);

	double att = 0.0;
	double dec = 0.0;
	double sus = -60.0;
	double rel = 0.0;
	double att_ten = 0.0;
	double dec_ten = 0.0;
	double rel_ten = 0.0;
	double vel_att = 0.0;
	double vel_dec = 0.0;
	double vel_sus = 0.0;
	double vel_rel = 0.0;

	double vel_freq = 0.0;
	double vel_q = 0.0;
	double srate = 44100.0;
	double vel = 0.0;
	double q = 0.707;
	bool filter_active = false;

	Envelope env{};

private:
	Filter filter{};
	Filter osc_filter{}; // filter duplicate used on oscillator exciters signal
	int fmode = 0;
	double freq = 0.0;
};