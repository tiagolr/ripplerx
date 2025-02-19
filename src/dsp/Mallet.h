// Copyright 2025 tilr
// Mallet generator, a simple unit impulse ran through a bandpass filter
#pragma once
#include "Filter.h"

class Mallet
{
public:
	Mallet() {};
	~Mallet() {};

	void trigger(double srate, double freq);
	void clear();
	double process();

	double impulse = 0.0;
	int elapsed = 0;

private:
	double env = 0.0;
	Filter filter{};
};