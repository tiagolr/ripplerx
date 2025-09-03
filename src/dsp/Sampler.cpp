#include "Sampler.h"
#include <cmath>

static const InternalSample samples[] = {
	{ "__sample1__", BinaryData::Bells2_xml, BinaryData::Bells2_xmlSize },
};

void Sampler::loadSample(String path)
{
	isUserFile = false;
	std::unique_ptr<juce::InputStream> inputStream;

	for (const auto& sample : samples) {
		if (path == sample.name) {
			inputStream = std::make_unique<juce::MemoryInputStream>(sample.data, sample.size, false);
			loadSampleFromBinary(std::move(inputStream));
			return;
		}
	}

	File audioFile(path);
	if (!audioFile.existsAsFile()) {
		loadSample("__sample1__");
		return;
	}

	inputStream = audioFile.createInputStream();
	loadSampleFromBinary(std::move(inputStream));
	isUserFile = true;
}

void Sampler::loadSampleFromBinary(std::unique_ptr<juce::InputStream> stream)
{
	AudioFormatManager manager;
	manager.registerBasicFormats();

	std::unique_ptr<juce::AudioFormatReader> reader(manager.createReaderFor(std::move(stream)));
	if (reader == nullptr) {
		return loadSample("__sample1__");
	}

	try {
		AudioBuffer<float> buf((int)(reader->numChannels), (int)(reader->lengthInSamples));
		reader->read(buf.getArrayOfWritePointers(), buf.getNumChannels(), 0, buf.getNumSamples());
		wavesrate = reader->sampleRate;
		waveform.clear();
		waveform.reserve(buf.getNumSamples());
		int numChannels = buf.getNumChannels();
		int numSamples = buf.getNumSamples();

		for (int i = 0; i < numSamples; ++i) {
			double sample = 0.0;

			// convert the waveform to mono
			for (int ch = 0; ch < numChannels; ++ch)
				sample += (double)buf.getSample(ch, i);

			sample /= numChannels;

			waveform.push_back(sample);
		}
	}
	catch (...) {
		loadSample("__sample1__");
	}
}

double Sampler::waveLerp(double pos)
{
	int i = (int)pos;
	double frac = pos - i;
	int j = i + 1;
	if (j >= waveform.size()) j = 0; // wrap around
	return (1.0 - frac) * waveform[i] + frac * waveform[j];
}

double Sampler::waveCubic(double pos)
{
	int N = (int)waveform.size();
	int i1 = (int)pos;
	double x = pos - i1;

	int i0 = (i1 - 1 + N) % N;
	int i2 = (i1 + 1) % N;
	int i3 = (i1 + 2) % N;

	double y0 = waveform[i0];
	double y1 = waveform[i1];
	double y2 = waveform[i2];
	double y3 = waveform[i3];

	// Catmull-Rom cubic interpolation
	double a = -0.5 * y0 + 1.5 * y1 - 1.5 * y2 + 0.5 * y3;
	double b = y0 - 2.5 * y1 + 2.0 * y2 - 0.5 * y3;
	double c = -0.5 * y0 + 0.5 * y2;
	double d = y1;

	return ((a * x + b) * x + c) * x + d;
}