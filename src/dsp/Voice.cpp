#include <cmath>
#include "Voice.h"
#include "Models.h"

double Voice::note2freq(int _note, MTSClient *mts)
{
    if (mts == nullptr)
    {
        return 440 * pow(2.0, (_note - 69) / 12.0);
    }
    else
    {
        return MTS_NoteToFrequency(mts, static_cast<char>(_note), -1);
    }
}

// Triggers mallet and noise generator
void Voice::trigger(double _srate, int _note, double _vel, MalletType _malletType, double _malletFreq, MTSClient* mts)
{
	srate = _srate;
	malletType = _malletType;
	malletFreq = _malletFreq;
	
	newVel = _vel;
	newNote = _note;
	newFreq = note2freq(newNote, mts);

	// fade out active voice before re-triggering
	if ((resA.on && resA.active) || (resB.on && resB.active)) {
		isFading = true;
		fadeTotalSamples = (int)(globals::REPEAT_NOTE_FADE_MS * 0.001 * srate);
		fadeSamples = fadeTotalSamples;
		updateResonators();
	}
	else {
		triggerStart();
	}
}

double Voice::fadeOut()
{
	fadeSamples--;
	if (fadeSamples <= 0) {
		isFading = false;
		triggerStart();
	}
	return isFading ? (double)fadeSamples / (double)fadeTotalSamples : 1.0;
}

void Voice::triggerStart()
{
	resA.clear();
	resB.clear();
	isRelease = false;
	isPressed = true;
	note = newNote;
	vel = newVel;
	freq = newFreq;

	mallet.trigger(malletType, srate, malletFreq);
	noise.attack(vel);
	if (resA.on) resA.activate();
	if (resB.on) resB.activate();
	updateResonators();
}

void Voice::release()
{
	isRelease = true;
	isPressed = false;
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

void Voice::setCoupling(bool _couple, double _split) {
	couple = _couple;
	split = _split;
}

void Voice::setPitch(double a_coarse, double b_coarse, double a_fine, double b_fine, double _pitch_bend)
{
	aPitchFactor = pow(2.0, (a_coarse + a_fine / 100.0) / 12.0);
	bPitchFactor = pow(2.0, (b_coarse + b_fine / 100.0) / 12.0);
	pitchBend = _pitch_bend;
}

void Voice::setRatio(double _a_ratio, double _b_ratio)
{
	a_ratio = _a_ratio;
	b_ratio = _b_ratio;
}

void Voice::applyPitch(std::array<double, 64>& model, double factor)
{
	for (double& ratio : model)
		ratio *= factor;
}

void Voice::applyPitchBend(double bend)
{
	if (bend != pitchBend) {
		pitchBend = bend;
		if (resA.on) resA.applyPitchBend(bend);
		if (resB.on) resB.applyPitchBend(bend);
	}
}

double inline Voice::freqShift(double fa, double fb) const
{
	auto avg = (fa + fb) / 2.0;
	auto k = split + cos(avg) / 5.0; // add some pseudo random offset to coupling so that frequency couples are not in perfect sync
	auto w = avg + sqrt(pow((fa - fb) / 2.0, 2.0) + pow(k / 2.5, 2.0));
	return fabs(fmax(fa, fb) - w);
}

/**
* When resonators are coupled in serial a frequency split is applied
* using the formula f +-= (fa + fb) / 2 + sqrt(((fa - fb) / 2)**2 + k**2) where k is the coupling strength
*/
std::tuple<std::array<double, 64>, std::array<double,64>> Voice::calcFrequencyShifts(
	std::array<double, 64>& aModel,
	std::array<double, 64>& bModel
) {
	std::array<double, 64> aShifts = aModel;
	std::array<double, 64> bShifts = bModel;

	double fa, fb, shift;
	for (int i = 0; i < 64; ++i) {
		fa = aModel[i];
		for (int j = 0; j < 64; ++j) {
			fb = bModel[j];
			if (fabs(fa - fb) <= 4.0) {
				shift = freqShift(fa * freq, fb * freq) / freq;
				aShifts[i] += fa > fb ? shift : -shift;
				bShifts[i] += fa > fb ? -shift : shift;
			}
		}
	}
	
	return std::tuple<std::array<double,64>, std::array<double,64>> (aShifts, bShifts);
}

void Voice::updateResonators()
{
	std::array<double,64> aModel = models.aModels[resA.nmodel];
	std::array<double,64> bModel = models.bModels[resB.nmodel];
	std::array<double, 64> aGain = models.getGains((ModalModels)resA.nmodel);
	std::array<double, 64> bGain = models.getGains((ModalModels)resB.nmodel);

	if (resA.nmodel == ModalModels::Djembe) {
		aModel = models.calcDjembe(freq, a_ratio);
	}
	if (resB.nmodel == ModalModels::Djembe) {
		bModel = models.calcDjembe(freq, b_ratio);
	}

	if (aPitchFactor != 1.0) applyPitch(aModel, aPitchFactor);
	if (bPitchFactor != 1.0) applyPitch(bModel, bPitchFactor);

	// if coupling mode is serial apply frequency splitting
	if (couple && resA.on && resB.on) {
		auto [aShifts, bShifts] = calcFrequencyShifts(aModel, bModel);
		aModel = aShifts;
		bModel = bShifts;
	}

	if (resA.on) resA.update(freq, vel, isRelease, pitchBend, aModel, aGain);
	if (resB.on) resB.update(freq, vel, isRelease, pitchBend, bModel, bGain);
}
