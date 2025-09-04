#pragma once

#include <vector>
#include <cmath>
#include <functional>
#include <algorithm>

class Utils
{
public:
    inline static constexpr double LOG_MAX_OVER_MIN_FREQ = 6.907755278982137; // log(20000 / 20)

    inline static double normalToFreq(double norm)
    {
        return 20.0 * std::exp(norm * LOG_MAX_OVER_MIN_FREQ);
    }

    inline static double freqToNormal(double norm)
    {
        return std::log(norm / 20.0) / LOG_MAX_OVER_MIN_FREQ;
    }

    inline static double gainTodB(double gain)
    {
        return gain == 0 ? -60.0 : 20.0 * std::log10(gain);
    }

    static float normalToFreqf(float min, float max, float norm)
    {
        return min * std::exp(norm * std::log(max / min));
    }

    static float freqToNormalf(float min, float max, float norm)
    {
        return std::log(norm / min) / std::log(max / min);
    }
};


/*
* A re-implementaiton of LookupTableTransform with additional cubic interpolation
*/
class LookupTable
{
public:
    LookupTable() = default;

    template <typename Func>
    LookupTable(Func fn, double min_, double max_, size_t size_)
    {
        init(fn, min_, max_, size_);
    }

    template <typename Func>
    void init(Func fn, double min_, double max_, size_t size_)
    {
        if (max_ <= min_) throw std::invalid_argument("max must be greater than min");
        if (size_ < 2) throw std::invalid_argument("size must be at least 2");
        min = min_;
        max = max_;
        size = size_;
        values.resize(size);

        scaler = (size > 1) ? (size - 1) / (max - min) : 0.0;
        offset = -min * scaler;

        for (size_t i = 0; i < size; ++i) {
            double x = static_cast<double>(i) / (size - 1); // Normalized [0, 1]
            double mappedX = min + x * (max - min);
            mappedX = std::clamp(mappedX, min, max);
            values[i] = fn(mappedX);
        }
    }

    inline double operator()(double input) const
    {
        input = std::clamp(input, min, max);
        double normalizedIndex = input * scaler + offset;
        size_t index = static_cast<size_t>(std::floor(normalizedIndex));

        if (index >= size - 1)
            return values.back();

        double frac = normalizedIndex - index;
        return values[index] + frac * (values[index + 1] - values[index]);
    }

    inline double cubic(double input) const
    {
        input = std::clamp(input, min, max);
        double index = input * scaler + offset;
        int i = (int)index;
        double t = index - i;

        int i0 = std::max(0, i - 1);
        int i1 = i;
        int i2 = std::min((int)size - 1, i + 1);
        int i3 = std::min((int)size - 1, i + 2);

        double y0 = values[i0];
        double y1 = values[i1];
        double y2 = values[i2];
        double y3 = values[i3];

        double a0 = y3 - y2 - y0 + y1;
        double a1 = y0 - y1 - a0;
        double a2 = y2 - y0;
        double a3 = y1;

        return (a0 * t * t * t) + (a1 * t * t) + (a2 * t) + a3;
    }

    const std::vector<double>& getValues() const { return values; }
    size_t getSize() const { return size; }
    double getMin() const { return min; }
    double getMax() const { return max; }

private:
    std::vector<double> values;
    double min = 0.0;
    double max = 1.0;
    double scaler = 0.0;
    double offset = 0.0;
    size_t size = 0;
};