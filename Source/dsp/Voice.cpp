#include <cmath>
#include "Voice.h"

double Voice::note2freq(int _note) 
{
	return 440 * pow(2.0, (_note - 69) / 12.0); 
}

// Triggers mallet and noise generator
void Voice::trigger(double srate, int _note, double _vel, double malletFreq)
{
	resA.clear();
	resB.clear();
	note = _note;
	rel = false;
	vel = _vel;
	freq = note2freq(note);
	mallet.trigger(srate, malletFreq);
	noise.attack(1.0);
	if (resA.on) resA.activate();
	if (resB.on) resB.activate();
	updateResonators();
}

void Voice::release()
{
	rel = true;
	noise.release();
	updateResonators();
}

void Voice::clear()
{
	mallet.clear();
	noise.clear();
	resA.clear();
	resB.clear();
}

void Voice::setCoupling(bool _couple, double _split) {
	couple = _couple;
	split = _split;
}

double inline Voice::freqShift(double fa, double fb) 
{
	auto avg = (fa + fb) / 2.0;
	auto k = split + cos(avg) / 5.0; // add some pseudo random offset to coupling so that frequency couples are not in perfect sync
	auto w = avg + sqrt(pow((fa - fb) / 2.0, 2.0) + pow(k / 2.5, 2.0));
	return fabs(fmax(fa, fb) - w);
}

/**
* When resonators are coupled in serial a frequency split is applied
* using the formula f +-= (fa + fb) / 2 + sqrt(((fa - fb) / 2)**2 + k**2) where k is the coupling strength
*/
std::tuple<std::array<double, 64>, std::array<double,64>> Voice::calcFrequencyShifts()
{
	std::array<double, 64> aModel = resA.models[resA.nmodel];
	std::array<double, 64> bModel = resB.models[resB.nmodel];
	std::array<double, 64> aShifts = resA.models[resA.nmodel];
	std::array<double, 64> bShifts = resB.models[resB.nmodel];

	double fa, fb, shift;
	for (int i = 0; i < 64; ++i) {
		fa = aModel[i];
		for (int j = 0; j < 64; ++j) {
			fb = bModel[j];
			if (fabs(fa - fb) <= 4.0) {
				shift = freqShift(fa * freq, fb * freq) / freq;
				aShifts[i] += fa > fb ? shift : -shift;
				bShifts[i] += fa > fb ? -shift : shift;
			}
		}
	}
	
	return std::tuple<std::array<double,64>, std::array<double,64>> (aShifts, bShifts);
}

void Voice::updateResonators()
{
	std::array<double,64> aModel;
	std::array<double,64> bModel;

	// if coupling mode is serial apply frequency splitting
	if (couple && resA.on && resB.on) {
		auto [aShifts, bShifts] = calcFrequencyShifts();
		aModel = aShifts;
		bModel = bShifts;
	}
	else {
		aModel = resA.models[resA.nmodel];
		bModel = resB.models[resB.nmodel];
	}

	if (resA.on) resA.update(freq, vel, rel, aModel);
	if (resB.on) resB.update(freq, vel, rel, bModel);
}