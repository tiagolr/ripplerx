// Copyright 2025 tilr
// Port of FairlyChildish limiter for Reaper

// Copyright 2006, Thomas Scott Stillwell
// All rights reserved.
//
//Redistribution and use in source and binary forms, with or without modification, are permitted
//provided that the following conditions are met:
//
//Redistributions of source code must retain the above copyright notice, this list of conditions
//and the following disclaimer.
//
//Redistributions in binary form must reproduce the above copyright notice, this list of conditions
//and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
//The name of Thomas Scott Stillwell may not be used to endorse or
//promote products derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
//FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
//BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
//STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
//THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once
#include <cmath>
#include <tuple>

class Limiter
{
public:
	Limiter() {};
	~Limiter() {};

	void init(double srate, double _thresh = 0.0, double _bias = 70.0, double rms_win = 100.0, double makeup = 0.0)
	{
		threshv = exp(_thresh * db2log);
		ratio = 20.0;
		bias = 80.0 * _bias / 100.0;
		cthresh = _thresh - bias;
		cthreshv = exp(cthresh * db2log);
		makeupv = exp(makeup * db2log);
		capsc = log2db;
		attime = 0.0002;
		reltime = 0.3;
		atcoef = exp(-1.0 / (attime * srate));
		relcoef = exp(-1.0 / (reltime * srate));
		rmscoef = exp(-1.0 / (rmstime * srate));
		rmstime = rms_win / 1000000.0;
		runave = 0.0;
	}

	std::tuple<double, double> process(double spl0, double spl1)
	{
		auto maxspl = fmax(fabs(spl0), fabs(spl1));
		maxspl = maxspl * maxspl;

		runave = maxspl + rmscoef * (runave - maxspl);
		auto det = sqrt(fmax(0.0, runave));
		auto overdb = fmax(0.0, capsc * log(det/threshv));

		if (overdb > rundb)
			rundb = overdb + atcoef * (rundb - overdb);
		else
			rundb = overdb + relcoef * (rundb - overdb);
		overdb = fmax(0.0, rundb);

		auto cratio = bias == 0.0
			? ratio
			: 1.0 + (ratio -1.0) * sqrt(overdb / bias);

		auto gr = -overdb * (cratio - 1.0) / cratio;
		auto grv = exp(gr * db2log);

		return std::tuple<double, double> (
			spl0 * grv * makeupv,
			spl1 * grv * makeupv
		);
	}

private:
  double log2db = 8.6858896380650365530225783783321; // 20 / ln(10)
  double db2log = 0.11512925464970228420089957273422;

  double rundb = 0.0;
  double runave = 0.0;
  double threshv = 0.0;
  double cthresh = 0.0;
  double cthreshv = 0.0;
  double ratio = 0;
  double bias = 0.0;
  double makeupv = 0.0;
  double capsc = 0.0;
  double timeconstant = 1.0;
  double attime = 0.0002;
  double reltime = 0.3;
  double atcoef = 0.0;
  double relcoef = 0.0;
  double rmstime = 0.0;
  double rmscoef = 0.0;
};