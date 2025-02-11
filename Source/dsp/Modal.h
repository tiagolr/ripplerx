#pragma once
#include <vector>
#include <array>
#include "../Globals.h"

class Partial
{
public:
	Partial(int n) { k = n; };
	~Partial() {};

	void update(double freq, double ratio, double ratio_max, double vel, bool isRelease);
	double process(double input);
	void clear();

	double srate = 0.0;
	int k = 0; // partial num, used for hit calcs
	double decay = 0.0;
	double damp = 0.0;
	double tone = 0.0;
	double hit = 0.0;
	double rel = 0.0;
	double inharm = 0.0;
	double ratio = 0.0;
	double cut = 0.0;
	double radius = 0.0;
	double vel_decay = 0.0;
	double vel_hit = 0.0;
	double vel_inharm = 0.0;

private:
	double b0 = 0.0; 
	double b2 = 0.0;
	double a0 = 0.0; 
	double a1 = 0.0;
	double a2 = 0.0;

	double x1 = 0.0;
	double x2 = 0.0;
	double y1 = 0.0;
	double y2 = 0.0;
};

class Resonator
{
public:
	Resonator();
	~Resonator() {};

	static double bfree[64];
	static std::array<std::array<double, 64>, 9> models;

	void setParams(double srate, bool on, int model, int partials, double decay, double damp, double tone, double hit, 
		double rel, double inharm, double ratio, double cut,double radius, double vel_decay, double vel_hit, double vel_inharm);

	void update(double frequency, double vel, bool isRelease);

	double srate = 0.0;
	bool on = false;
	int nmodel = 0;
	int npartials = 0;
	double decay = 0.0;
	double radius = 0.0;
	double rel = 0.0;

	std::vector<Partial> partials;

private:

};

