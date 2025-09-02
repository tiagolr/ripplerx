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

	void init(double srate, int filterMode, double freq, double q, double att, double dec, double sus, double rel, double vel_freq, double vel_q);
	double process();
	void attack(double vel);
	void initFilter();
	void release();
	void clear();

	double vel_freq = 0.0;
	double vel_q = 0.0;
	double srate = 44100.0;
	double vel = 0.0;
	double q = 0.707;
	bool filter_active = false;
private:
	Filter filter{};
	Envelope env{};
	int fmode = 0;
	double freq = 0.0;
};