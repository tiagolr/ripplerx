#include "Mallet.h"
#include <cmath>

void Mallet::trigger(double srate, double freq)
{
	filter.bp(srate, freq, 0.707);
	elapsed = (int)(srate/10.0); // countdown (100ms)
	impulse = 1.0;
	env = exp(-100.0/srate);
}

void Mallet::clear()
{
	elapsed = 0;
	impulse = 0.0;
}

double Mallet::process()
{
	if (elapsed == 0) return 0.0;
	auto sample = filter.df1(impulse) * 2.0;
	elapsed -= 1;
	impulse *= env;

	return sample;
}