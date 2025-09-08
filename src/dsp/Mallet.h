// Copyright 2025 tilr
// Mallet generator using different modes
// impulse - a simple unit impulse ran through a bandpass filter
// sample - a sample based mallet
#pragma once
#include "Filter.h"
#include "vector"
#include "JuceHeader.h"

enum MalletType
{
	kImpulse,
	kReserved1,
	kReserved2,
	kReserved3,
	kReserved4,
	kReserved5,
	kReserved6,
	kReserved7,
	kReserved8,
	kReserved9,
	kReserved10,
	kUserFile,
	kSample1,
	kSample2,
	kSample3,
	kSample4,
	kSample5,
	kSample6,
	kSample7,
	kSample8,
	kSample9,
	kSample10,
	kSample11,
	kSample12,
	kSample13,
	kSample14,
};

class Sampler;

class Mallet
{
public:
	Mallet(Sampler& sampler) : sampler(sampler) {};
	~Mallet() {};

	void trigger(MalletType type, double srate, double freq, int note, bool ktrack);
	void clear();
	double process();

	void setFilter(double norm);

	double srate = 44100.0;

	// impulse mallet fields
	double impulse = 0.0;
	int countdown = 0;
	double env = 0.0;
	Filter impulse_filter{};

	// sample mallet fields
	bool ktrack = false; // key tracking
	double keytrack_factor = 1.0;
	double playback = INFINITY;
	double playback_speed = 1.0;
	bool disable_filter = false;
	Filter sample_filter{};

private:
	Sampler& sampler;
	MalletType type = kImpulse;
};