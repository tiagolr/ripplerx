#pragma once
#include "Mallet.h"
#include "Noise.h"

class Voice
{
public:
	Voice() {};
	~Voice() {};

	double note2freq(int note);
	void trigger(double srate, int note, double vel);
	void release();
	//double process();

	int note = 0;
	double freq = 0.0;
	double vel = 0.0;
	bool rel = false; // release state

	Mallet mallet{};
	Noise noise{};
private:
};