#pragma once

class Waveguide
{
public:
	Waveguide() {};
	~Waveguide() {};

	void update(double freq, double vel, bool isRelease);
	double process(double input);
	void clear();

	bool is_closed = false;
	double srate = 0.0;
	double decay = 0.0;
	double radius = 0.0;
	double rel = 0.0;
	double vel_decay = 0.0;

private:
	int read_ptr = 0;
	int write_ptr = 0;
	double tube_decay = 0.0;
	double tube[20000] = {};
	int tube_len = 20000; // buffer size, 20000 allows for 10Hz at 200k srate (max_size = srate / freq_min)
	double y = 0.0;
	double y1 = 0.0;
};