// Copyright (C) 2025 tilr
// Comb stereoizer
#pragma once
#include <tuple>
#include <vector>

class Comb
{
public:
	Comb() {};
	~Comb() {};

  void init(double srate)
  {
    pos = 0;
    buf.resize((int)(20 * srate / 1000));
    std::fill(buf.begin(), buf.end(), 0.0);
  }

  std::tuple<double, double> process(double input) {
    buf[pos] = input;
    pos = (pos + 1) % buf.size();

    return std::tuple<double, double>(
      input + buf[pos] * 0.33,
      input - buf[pos] * 0.33
    );
  }

private:
  int pos = 0;
  std::vector<double> buf;
};