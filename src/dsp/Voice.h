// Copyright 2025 tilr
// Each pressed key triggers a voice, there are max 16 polyphony voices in audioProcessor
// Voices hold A and B resonators, a mallet and noise generator
// they also calculate split frequencies for coupled resonators
// and tune the resonators modals by providing the models

#pragma once
#include <array>
#include <chrono>
#include "Mallet.h"
#include "Noise.h"
#include "Resonator.h"
#include "tuple"
#include "libMTSClient.h"

class Models;
class Sampler;

using namespace std::chrono;

class Voice
{
public:
	Voice(Models& m, Sampler& s) : models(m), mallet(s) {}
	~Voice() {}

	double note2freq(int _note, MTSClient *mts);
	void trigger(double srate, int _note, double vel, MalletType malletType, double malletFreq, double malletKTrack, MTSClient *mts);
	void triggerStart();
	double fadeOut();
	void release();
	void clear();
	void setPitch(double a_coarse, double b_coarse, double a_fine, double b_fine, double pitch_bend);
	void setRatio(double _a_ratio, double _b_ratio);
	void applyPitch(std::array<double, 64>& model, double factor);
	void applyPitchBend(double bend);
	double processOscillators(bool isA);
	double inline freqShift(double fa, double fb) const;
	std::tuple<std::array<double, 64>, std::array<double, 64>> calcFrequencyShifts(
		std::array<double, 64>& aModel,
		std::array<double, 64>& bModel
	);
	void setCoupling(bool _couple, double _split);
	void updateResonators();

	int note = 0;
	double freq = 0.0;
	double vel = 0.0;
	bool isRelease = false;
	bool isPressed = false; // used for audioIn
	bool couple = false;
	double malletKtrack = 0.0;
	double split = 0.0;
	double srate = 44100.0;
	double a_ratio = 1.0; // used to recalculate models
	double b_ratio = 1.0; // used to recalculate models
	int64_t pressed_ts = 0; // timestamp used to order notes
	int64_t release_ts = 0; // timestamp used to order notes

	// used to fade out on repeat notes
	bool isFading = false;
	int fadeTotalSamples = 0;
	int fadeSamples = 0;
	MalletType malletType = kImpulse;
	double malletFreq = 0.0;
	double newFreq = 0.0;
	double newVel = 0.0;
	int newNote = 0;

	double aPitchFactor = 1.0;
	double bPitchFactor = 1.0;
	double pitchBend = 1.0;

	Mallet mallet;
	Noise noise{};
	Resonator resA{};
	Resonator resB{};

private:
	Models& models;
	std::array<double, 64> aPhases = {};
	std::array<double, 64> bPhases = {};
};
