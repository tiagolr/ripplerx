// Copyright 2025 tilr
// Mallet generator, a simple unit impulse ran through a bandpass filter
#pragma once
#include "Filter.h"
#include "vector"
#include "JuceHeader.h"

struct InternalSample
{
	const char* name;
	const void* data;
	size_t size;
};

class Sampler
{
public:
	Sampler() {};
	~Sampler() {};

	void loadSample(juce::String filepath);
	void loadSampleFromBinary(std::unique_ptr<juce::InputStream> stream);
	double waveLerp(double pos);
	double waveCubic(double pos);

	// sample mallet fields
	std::vector<double> waveform = {};
	bool isUserFile = false;
	double wavesrate = 44100.0;
};