// Copyright 2025 tilr
// Port of Envlib (tilr) based of zenvelib.v1 by dwelx
// provides adsr envelopes with tension/shape and scale controls
#pragma once
#include <tuple>

class Envelope
{
public:
	Envelope() {};
	~Envelope() {};

	void init(double srate, double a, double d, double s, double r, double tensionA, double tensionD, double tensionR);
	void reset();
	void attack(double scale);
	void release();
	void sustain();
	void decay();
	int process();

	double att = 0.0;
	double dec = 0.0;
	double sus = 0.0;
	double rel = 0.0;
	double scale = 0.0;
	double env = 0.0;
	int state = 0; // 0 Off, 1 Attack, 2 Decay, 4 Sustain, 8 Release

	double ab = 0.0; // attack coeff
	double ac = 0.0; // attack coeff
	double db = 0.0; // decay coeff
	double dc = 0.0; // decay coeff
	double rb = 0.0; // release coeff
	double rc = 0.0; // release coeff
	double ta = 0.0; // tension att
	double td = 0.0; // tension dec
	double tr = 0.0; // tension rel

private:
	inline double normalizeTension(double t);
	std::tuple<double, double> calcCoefs(double targetB1, double targetB2, double targetC, double rate, double tension, double mult);
	void recalcCoefs(); // calcs coefficients for attack and decay
};