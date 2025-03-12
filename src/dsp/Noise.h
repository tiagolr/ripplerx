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

	void init(double srate, int filterMode, double freq, double q, double att, double dec, double sus, double rel, double noise_density);
	double process();
	void attack(double vel);
	void release();
	void clear();

	double vel = 0.0;
	double noise_density = 0.0;
private:
	Filter filter{};
	Envelope env{};
	int fmode = 0;
	double ffreq = 0.0;
};