// Copyright 2025 tilr
// Each pressed key triggers a voice, there are max 16 polyphony voices in audioProcessor
// Voices hold resonators, a mallet and noise generator, they also calculate split frequencies for coupled resonators

#pragma once
#include "Mallet.h"
#include "Noise.h"
#include "Resonator.h"
#include "tuple"

class Voice
{
public:
	Voice() {};
	~Voice() {};

	double note2freq(int _note);
	void trigger(double srate, int _note, double vel, double malletFreq);
	void release();
	void clear();
	double inline freqShift(double fa, double fb);
	std::tuple<std::array<double, 64>, std::array<double, 64>> calcFrequencyShifts();
	void setCoupling(bool _couple, double _split);
	void updateResonators();

	int note = 0;
	double freq = 0.0;
	double vel = 0.0;
	bool rel = false; // release state
	bool couple = false;
	double split = 0.0;

	Mallet mallet{};
	Noise noise{};
	Resonator resA{};
	Resonator resB{};

private:
};