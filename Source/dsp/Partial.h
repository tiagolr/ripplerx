#pragma once

class Partial
{
public:
	Partial(int n) { k = n; };
	~Partial() {};

	void update(double freq, double ratio, double ratio_max, double vel, bool isRelease);
	double process(double input);
	void clear();

	double srate = 0.0;
	int k = 0; // Partial num
	double decay = 0.0;
	double damp = 0.0;
	double tone = 0.0;
	double hit = 0.0;
	double rel = 0.0;
	double inharm = 0.0;
	double radius = 0.0;
	double vel_decay = 0.0;
	double vel_hit = 0.0;
	double vel_inharm = 0.0;

private:
	double b0 = 0.0;
	double b2 = 0.0;
	double a0 = 1.0;
	double a1 = 0.0;
	double a2 = 0.0;

	double x1 = 0.0;
	double x2 = 0.0;
	double y1 = 0.0;
	double y2 = 0.0;
};