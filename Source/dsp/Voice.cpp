#include <cmath>
#include "Voice.h"

double Voice::note2freq(int _note) 
{
	return 440 * pow(2.0, (_note - 69) / 12.0); 
}

// Triggers mallet and noise generator
void Voice::trigger(double srate, int _note, double _vel)
{
	resA.clear();
	resB.clear();
	note = _note;
	rel = false;
	vel = _vel;
	freq = note2freq(note);
	mallet.trigger(srate);
	noise.attack(vel);
	if (resA.on) resA.activate();
	if (resB.on) resB.activate();
	updateResonators();
}

void Voice::release()
{
	rel = true;
	noise.release();
	updateResonators();
}

void Voice::clear()
{
	mallet.clear();
	noise.clear();
	resA.clear();
	resB.clear();
}

void Voice::updateResonators()
{
	if (resA.on) resA.update(freq, vel, rel);
	if (resB.on) resB.update(freq, vel, rel);
}