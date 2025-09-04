// Copyright 2025 tilr
// Waveguide for OpenTube and ClosedTube models
#pragma once
#include <memory>

class Waveguide
{
public:
	Waveguide() {
		tube = std::make_unique<double[]>(20000); // Heap allocation
	};
	~Waveguide() {};

	void update(double f_0, double vel, double pitch_bend, bool isRelease);
	double process(double input);
	void clear();

	void applyPitchBend(double bend);

	double base_freq = 1000.0;
	bool is_closed = false;
	double srate = 0.0;
	double decay = 0.0;
	double radius = 0.0;
	double rel = 0.0;
	double vel_decay = 0.0;

private:
	double read_ptr_frac = 0.0;
	int write_ptr = 0;
	double tube_decay = 0.0;
	std::unique_ptr<double[]> tube;
	int tube_len = 20000; // buffer size, 20000 allows for 10Hz at 200k srate (max_size = srate / freq_min)
	double y = 0.0;
	double y1 = 0.0;
};