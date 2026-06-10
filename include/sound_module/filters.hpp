#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "filters_checks.hpp"
#include "waveform.hpp"
#include <string>

class IFilter
{
public:
    virtual ~IFilter() = default;
    virtual void apply(Waveform& sound) const = 0;
};

class AmplFilter: public IFilter
{
public:
    AmplFilter(double factor): _factor(factor)
    {
        Fcheck::requireNonNegative(factor, "ampl: factor");
    }

    void apply(Waveform& sound) const override;

private:
    double _factor{};  // factor >= 0
};

class NormalizeFilter: public IFilter
{
public:
    explicit NormalizeFilter(double peak = 1.0): _peak(peak)
    {
        Fcheck::requireInRange(peak, 0.0, 1.0, "normalize: peak");
    }
    void apply(Waveform& sound) const override;

private:
    double _peak{1.0};  // 0 <= peak <= 1
};

class SilenceFilter: public IFilter
{
public:
    SilenceFilter(std::string unit, double start, double end)
        : _unit(std::move(unit)), _start(start), _end(end)
    {
        if(_unit != "sec" && _unit != "ms")
            throw FilterParamError("silence: unit must be \"sec\" or \"ms\"");
        Fcheck::requireNonNegative(_start, "silence: start");
        if(!(_end >= _start))
            throw FilterParamError("silence: end must be >= start");
    }
    void apply(Waveform& sound) const override;

private:
    std::string _unit;  // "sec" or "ms"
    double _start{};    // start >= 0
    double _end{};      // end >= start
};

class TimestretchFilter: public IFilter
{
public:
    explicit TimestretchFilter(double factor): _factor(factor)
    {
        Fcheck::requirePositive(factor, "timestretch: factor");
    }
    void apply(Waveform& sound) const override;

private:
    double _factor{1.0};  // factor > 0
};

class LowpassFilter: public IFilter
{
public:
    explicit LowpassFilter(int windowSize): _windowSize(windowSize)
    {
        if(windowSize < 1)
            throw FilterParamError("lowpass: window_size must be >= 1");

        if(windowSize % 2 == 0)
            throw FilterParamError("lowpass: window_size must be odd");
    }
    void apply(Waveform& sound) const override;

private:
    int _windowSize{1};  // window_size >= 1 && window_size % 2 == 1
};

class SinGenerator: public IFilter
{
public:
    SinGenerator(double frequencyHz, double durationMs)
        : _freq(frequencyHz), _durationMs(durationMs)
    {
        Fcheck::requireNonNegative(_freq, "sin: frequency_hz");
        Fcheck::requireNonNegative(_durationMs, "sin: duration_ms");
    }
    void apply(Waveform& sound) const override;

private:
    double _freq{};        // frequency_hz >= 0
    double _durationMs{};  // duration_ms  >= 0
};

class AmGenerator: public IFilter
{
public:
    AmGenerator(double amplitude, double carrierHz, double modulationHz,
                double depth, double durationMs)
        : _amplitude(amplitude), _carrierHz(carrierHz),
          _modulationHz(modulationHz), _depth(depth), _durationMs(durationMs)
    {
        Fcheck::requireInRange(_amplitude, 0.0, 1.0, "am: amplitude");
        Fcheck::requireNonNegative(_carrierHz, "am: carrier_hz");
        Fcheck::requireNonNegative(_modulationHz, "am: modulation_hz");
        Fcheck::requireInRange(_depth, 0.0, 1.0, "am: depth");
        Fcheck::requireNonNegative(_durationMs, "am: duration_ms");
    }
    void apply(Waveform& sound) const override;

private:
    double _amplitude{};     // 0 <= amplitude <= 1
    double _carrierHz{};     // carrier_hz   >= 0
    double _modulationHz{};  // modulation_hz >= 0
    double _depth{};         // 0 <= depth <= 1
    double _durationMs{};    // duration_ms  >= 0
};

class FmGenerator: public IFilter
{
public:
    FmGenerator(double amplitude, double carrierHz, double modulationHz,
                double deviationHz, double durationMs)
        : _amplitude(amplitude), _carrierHz(carrierHz),
          _modulationHz(modulationHz), _deviationHz(deviationHz),
          _durationMs(durationMs)
    {
        Fcheck::requireInRange(_amplitude, 0.0, 1.0, "fm: amplitude");
        Fcheck::requireNonNegative(_carrierHz, "fm: carrier_hz");
        Fcheck::requirePositive(_modulationHz, "fm: modulation_hz");
        Fcheck::requireNonNegative(_deviationHz, "fm: deviation_hz");
        Fcheck::requireNonNegative(_durationMs, "fm: duration_ms");
    }
    void apply(Waveform& sound) const override;

private:
    double _amplitude{};     // 0 <= amplitude <= 1
    double _carrierHz{};     // carrier_hz    >= 0
    double _modulationHz{};  // modulation_hz >  0
    double _deviationHz{};   // deviation_hz  >= 0
    double _durationMs{};    // duration_ms   >= 0
};

#endif
