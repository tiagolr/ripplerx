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
#include "libMTSClient.h"

class Models;

class Voice
{
public:
	Voice(Models& m) : models(m) {}
	~Voice() {}

	double note2freq(int _note, MTSClient *mts);
	void trigger(double srate, int _note, double vel, double malletFreq, MTSClient *mts);
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
	Models& models;
};
