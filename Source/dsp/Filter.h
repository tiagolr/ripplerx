// Copyright 2025 tilr
// Incomplete port of Theo Niessink <theo@taletn.com> rbj filters
#pragma once

class Filter
{
public:
	Filter() {};
	~Filter() {};

	void lp(double srate, double freq, double q);
	void bp(double srate, double freq, double q);
	void hp(double srate, double freq, double q);
	void clear(double input);
	double df1(double sample);

private:
	double a1 = 0.0;
	double a2 = 0.0;
	double b0 = 0.0;
	double b1 = 0.0;
	double b2 = 0.0;
	double x0 = 0.0;
	double x1 = 0.0;
	double y0 = 0.0;
	double y1 = 0.0;
};