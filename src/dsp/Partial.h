// Copyright 2025 tilr
// Partial is a second order bandpass filter with extra variables for decay, frequency and amplitude calculation

#pragma once
#include "Utils.h"
#include "../Globals.h"

class Partial
{
public:
	Partial(int n) { k = n; };
	~Partial() {};

	static LookupTable a1LUT;
	static void initA1LUT(double sampleRate);

	void update(double freq, double ratio, double ratio_max, double vel, double pitch_bend, bool isRelease);
	double process(double input);
	void clear();
	void applyGain(double gain);
	void applyPitchBend(double bend);

	double srate = 0.0;
	int k = 0; // Partial num
	double decay = 0.0;
	double damp = 0.0;
	double tone = 0.0;
	double hit = 0.0;
	double rel = 0.0;
	double inharm = 0.0;
	double radius = 0.0;
	double vel_decay = 0.0;
	double vel_hit = 0.0;
	double vel_inharm = 0.0;
	double vel_damp = 0.0;
	double vel_tone = 0.0;

private:
	double base_f_k = 1000.0;
	bool out_of_range = false;
	double b0 = 0.0;
	double b2 = 0.0;
	double a0 = 1.0;
	double a1 = 0.0;
	double a2 = 0.0;

	double x1 = 0.0;
	double x2 = 0.0;
	double y1 = 0.0;
	double y2 = 0.0;
};