#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "filters_checks.hpp"
#include "waveform.hpp"
#include <string>

/**
 * @brief Base interface for all audio filters and generators.
 */
class IFilter
{
public:
    virtual ~IFilter() = default;

    /**
     * @brief Applies filter to waveform.
     * @param sound Audio data to modify
     */
    virtual void apply(Waveform& sound) const = 0;
};

/**
 * @brief Amplifies audio signal by a constant factor.
 */
class AmplFilter: public IFilter
{
public:
    /**
     * @brief Constructs amplifier filter.
     * @param factor Amplification factor (must be >= 0)
     */
    explicit AmplFilter(double factor): _factor(factor)
    {
        Fcheck::requireNonNegative(factor, "ampl: factor");
    }

    void apply(Waveform& sound) const override;

private:
    double _factor{};  ///< amplification factor (>= 0)
};

/**
 * @brief Normalizes audio signal to a peak value.
 */
class NormalizeFilter: public IFilter
{
public:
    /**
     * @brief Constructs normalization filter.
     * @param peak Maximum allowed amplitude (0..1)
     */
    explicit NormalizeFilter(double peak = 1.0): _peak(peak)
    {
        Fcheck::requireInRange(peak, 0.0, 1.0, "normalize: peak");
    }

    void apply(Waveform& sound) const override;

private:
    double _peak{1.0};  ///< target peak amplitude (0..1)
};

/**
 * @brief Mutes a segment of audio signal.
 */
class SilenceFilter: public IFilter
{
public:
    /**
     * @brief Constructs silence filter.
     *
     * @param unit Time unit ("sec" or "ms")
     * @param start Start time
     * @param end End time
     */
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
    std::string _unit;  ///< time unit ("sec" or "ms")
    double _start{};    ///< start time
    double _end{};      ///< end time
};

/**
 * @brief Changes playback speed (time stretching).
 */
class TimestretchFilter: public IFilter
{
public:
    /**
     * @brief Constructs timestretch filter.
     * @param factor Stretch factor (> 0)
     */
    explicit TimestretchFilter(double factor): _factor(factor)
    {
        Fcheck::requirePositive(factor, "timestretch: factor");
    }

    void apply(Waveform& sound) const override;

private:
    double _factor{1.0};  ///< time stretch factor (> 0)
};

/**
 * @brief Simple moving average low-pass filter.
 */
class LowpassFilter: public IFilter
{
public:
    /**
     * @brief Constructs low-pass filter.
     * @param windowSize Size of smoothing window (odd, >= 1)
     */
    explicit LowpassFilter(int windowSize): _windowSize(windowSize)
    {
        if(windowSize < 1)
            throw FilterParamError("lowpass: window_size must be >= 1");

        if(windowSize % 2 == 0)
            throw FilterParamError("lowpass: window_size must be odd");
    }

    void apply(Waveform& sound) const override;

private:
    int _windowSize{1};  ///< smoothing window size
};

/**
 * @brief Generates sine wave signal.
 */
class SinGenerator: public IFilter
{
public:
    /**
     * @brief Constructs sine generator.
     * @param frequencyHz Frequency of sine wave
     * @param durationMs Duration in milliseconds
     */
    SinGenerator(double frequencyHz, double durationMs)
        : _freq(frequencyHz), _durationMs(durationMs)
    {
        Fcheck::requireNonNegative(_freq, "sin: frequency_hz");
        Fcheck::requireNonNegative(_durationMs, "sin: duration_ms");
    }

    void apply(Waveform& sound) const override;

private:
    double _freq{};        ///< frequency (Hz)
    double _durationMs{};  ///< duration (ms)
};

/**
 * @brief Amplitude modulation signal generator.
 */
class AmGenerator: public IFilter
{
public:
    /**
     * @brief Constructs AM generator.
     *
     * @param amplitude Base amplitude (0..1)
     * @param carrierHz Carrier frequency
     * @param modulationHz Modulation frequency
     * @param depth Modulation depth (0..1)
     * @param durationMs Duration in ms
     */
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
    double _amplitude{};     ///< base amplitude
    double _carrierHz{};     ///< carrier frequency
    double _modulationHz{};  ///< modulation frequency
    double _depth{};         ///< modulation depth
    double _durationMs{};    ///< duration
};

/**
 * @brief Frequency modulation signal generator.
 */
class FmGenerator: public IFilter
{
public:
    /**
     * @brief Constructs FM generator.
     *
     * @param amplitude Base amplitude (0..1)
     * @param carrierHz Carrier frequency
     * @param modulationHz Modulation frequency (> 0)
     * @param deviationHz Frequency deviation
     * @param durationMs Duration in ms
     */
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
    double _amplitude{};     ///< amplitude
    double _carrierHz{};     ///< carrier frequency
    double _modulationHz{};  ///< modulation frequency
    double _deviationHz{};   ///< frequency deviation
    double _durationMs{};    ///< duration
};

#endif
