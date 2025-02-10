// Copyright (C) 2025 tilr
// Simple noise generator with a filter and envelope
#pragma once
#include "Filter.h"
#include "Envelope.h"

class Noise
{
public:
	Noise() {};
	~Noise() {};

	void init(double srate, int filterMode, double freq, double q, double att, double dec, double sus, double rel);
	double process();
	void attack(double vel);
	void release();
	void reset();

	double vel = 0.0;
private:
	Filter filter{};
	Envelope env{};
	int fmode = 0;
	double ffreq = 0.0;
};