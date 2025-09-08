#include "Sampler.h"
#include <cmath>

//"Click 1", "Click 2", "Click 3", "Blip", "Blop", "Metal 1", "Metal 2", "Wood", "Perc 1", "Perc 2"
static const InternalSample samples[] = {
	{ MalletType::kSample1, BinaryData::click1_wav, BinaryData::click1_wavSize },
	{ MalletType::kSample2, BinaryData::click2_wav, BinaryData::click2_wavSize },
	{ MalletType::kSample3, BinaryData::click3_wav, BinaryData::click3_wavSize },
	{ MalletType::kSample4, BinaryData::click4_wav, BinaryData::click4_wavSize },
	{ MalletType::kSample5, BinaryData::click5_wav, BinaryData::click5_wavSize },
	{ MalletType::kSample6, BinaryData::click6_wav, BinaryData::click6_wavSize },
	{ MalletType::kSample7, BinaryData::blip_wav, BinaryData::blip_wavSize },
	{ MalletType::kSample8, BinaryData::blop_wav, BinaryData::blop_wavSize },
	{ MalletType::kSample9, BinaryData::metal1_wav, BinaryData::metal1_wavSize },
	{ MalletType::kSample10, BinaryData::metal2_wav, BinaryData::metal2_wavSize },
	{ MalletType::kSample11, BinaryData::wood_wav, BinaryData::wood_wavSize },
	{ MalletType::kSample12, BinaryData::strike_wav, BinaryData::strike_wavSize },
	{ MalletType::kSample13, BinaryData::perc1_wav, BinaryData::perc1_wavSize },
	{ MalletType::kSample14, BinaryData::perc2_wav, BinaryData::perc2_wavSize}
};

void Sampler::setPitch(double semis)
{
	pitchfactor = std::pow(2.0, (semis / 12.0));
}

void Sampler::loadEncoded(String encoded)
{
	waveform.clear();
	juce::MemoryBlock block;
	block.fromBase64Encoding(encoded);

	juce::MemoryInputStream mi(block, false);
	while (mi.getNumBytesRemaining() >= sizeof(double))
		waveform.push_back(mi.readDouble());
}

void Sampler::loadSample(String path)
{
	File audioFile(path);
	if (!audioFile.existsAsFile()) {
		loadInternalSample(MalletType::kSample1); // fallback
		return;
	}

	std::unique_ptr<juce::InputStream> inputStream = audioFile.createInputStream();
	loadSampleFromBinary(std::move(inputStream));
	isUserFile = true;
}

void Sampler::loadInternalSample(MalletType type)
{
	for (const auto& sample : samples) {
		if (type == sample.type) {
			auto inputStream = std::make_unique<juce::MemoryInputStream>(sample.data, sample.size, false);
			loadSampleFromBinary(std::move(inputStream));
			isUserFile = false;
			return;
		}
	}

	loadInternalSample(MalletType::kSample1); // fallback
}

void Sampler::loadSampleFromBinary(std::unique_ptr<juce::InputStream> stream)
{
	AudioFormatManager manager;
	manager.registerBasicFormats();

	std::unique_ptr<juce::AudioFormatReader> reader(manager.createReaderFor(std::move(stream)));
	if (reader == nullptr) {
		return loadInternalSample(MalletType::kSample1);
	}

	try {
		AudioBuffer<float> buf((int)(reader->numChannels), (int)(reader->lengthInSamples));
		reader->read(buf.getArrayOfWritePointers(), buf.getNumChannels(), 0, buf.getNumSamples());
		wavesrate = reader->sampleRate;
		waveform.clear();
		waveform.reserve(buf.getNumSamples());
		int numChannels = buf.getNumChannels();
		int numSamples = std::min(3 * (int)wavesrate, buf.getNumSamples());

		for (int i = 0; i < numSamples; ++i) {
			double sample = 0.0;
			// convert the waveform to mono
			for (int ch = 0; ch < numChannels; ++ch)
				sample += (double)buf.getSample(ch, i);
			sample /= numChannels;

			waveform.push_back(sample);
		}

		// normalize waveform
		double maxVal = 0.0;
		for (double x : waveform) {
			maxVal = std::max(maxVal, std::fabs(x));
		}
		if (maxVal > 0.0) {
			double scale = 1.0 / maxVal;
			for (double& x : waveform) {
				x *= scale;
			}
		}
	}
	catch (...) {
		return loadInternalSample(MalletType::kSample1);
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