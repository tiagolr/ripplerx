// Copyright 2025 tilr
// Each pressed key triggers a voice, there are max 16 polyphony voices in audioProcessor
// Voices hold A and B resonators, a mallet and noise generator
// they also calculate split frequencies for coupled resonators
// and tune the resonators modals by providing the models

#pragma once
#include <array>
#include "Mallet.h"
#include "Noise.h"
#include "Resonator.h"
#include "tuple"

class Voice
{
public:
	Voice() {};
	~Voice() {};

	// static ratio models shared by voices, definition in .cpp
	static std::array<double, 64> bFree;
	static std::array<std::array<double, 64>, 9> aModels;
	static std::array<std::array<double, 64>, 9> bModels;

	// triggered on model ratio param changes
	void static recalcBeam(bool resA, double ratio);
	void static recalcMembrane(bool resA, double ratio);
	void static recalcPlate(bool resA, double ratio);

	double note2freq(int _note);
	void trigger(double srate, int _note, double vel, double malletFreq);
	void release();
	void clear();
	void setPitch(double a_coarse, double b_coarse, double a_fine, double b_fine);
	void applyPitch(std::array<double, 64>& model, double factor);
	double inline freqShift(double fa, double fb) const;
	std::tuple<std::array<double, 64>, std::array<double, 64>> calcFrequencyShifts();
	void setCoupling(bool _couple, double _split);
	void updateResonators();

	int note = 0;
	double freq = 0.0;
	double vel = 0.0;
	bool isRelease = false;
	bool isPressed = false; // used for audioIn
	bool couple = false;
	double split = 0.0;

	double aPitchFactor = 1.0;
	double bPitchFactor = 1.0;

	Mallet mallet{};
	Noise noise{};
	Resonator resA{};
	Resonator resB{};

private:
};