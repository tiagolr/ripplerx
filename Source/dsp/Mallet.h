#pragma once
#include "Filter.h"

class Mallet
{
public:
	Mallet() {};
	~Mallet() {};

	void trigger(double srate);
	void setFreq(double srate, double freq);
	void clear();
	double process();

	double impulse = 0.0;
	int elapsed = 0;

private:
	double env = 0.0;
	Filter filter{};
};