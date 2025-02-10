#include <cmath>
#include "Voice.h"

double Voice::note2freq(int note) 
{
	return 440 * pow(2.0, (note - 69) / 12.0); 
}

void Voice::trigger(double srate, int _note, double _vel)
{
	note = _note;
	rel = false;
	vel = _vel;
	freq = note2freq(note);
	mallet.trigger(srate);
	noise.attack(vel);
}

void Voice::release()
{
	rel = true;
	noise.release();
}