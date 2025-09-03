// Copyright 2025 tilr
// Mallet generator, a simple unit impulse ran through a bandpass filter
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
};

class Sampler;

class Mallet
{
public:
	Mallet(Sampler& sampler) : sampler(sampler) {};
	~Mallet() {};

	void trigger(MalletType type, double srate, double freq);
	void clear();
	double process();

	void loadSample(juce::String filepath);
	void loadSampleFromBinary(std::unique_ptr<juce::InputStream> stream);

	double srate = 44100.0;

	// impulse mallet fields
	double impulse = 0.0;
	int countdown = 0;
	double env = 0.0;

	// sample mallet fields
	double playback = INFINITY;
	double playback_speed = 1.0;

private:
	Sampler& sampler;
	MalletType type = kImpulse;
	Filter filter{};
};