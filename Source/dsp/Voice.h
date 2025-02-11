#pragma once
#include "Mallet.h"
#include "Noise.h"
#include "Modal.h"

class Voice
{
public:
	Voice() {};
	~Voice() {};

	double note2freq(int _note);
	void trigger(double srate, int _note, double vel);
	void release();
	void reset();
	void updateResonators();

	int note = 0;
	double freq = 0.0;
	double vel = 0.0;
	bool rel = false; // release state

	Mallet mallet{};
	Noise noise{};
	Resonator resA{};
	Resonator resB{};

private:
};