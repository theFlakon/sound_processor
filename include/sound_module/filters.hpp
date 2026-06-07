#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "waveform.hpp"

class IFilter
{
public:
    virtual ~IFilter() = default;
    virtual void apply(Waveform& sound) = 0;
};

class AmplFilter: public IFilter
{
public:
    AmplFilter(double factor): _factor(factor)
    {
    }

    void apply(Waveform& sound) override;

private:
    double _factor{};
};

/*
class TransformingFilter: public IFilter
{
public:
    virtual ~TransformingFilter() = default;
    TransformingFilter(SoundSamplesVec externalSamples)
        : _samples(externalSamples) {};

private:
    SoundSamplesVec _samples;
};

class GeneratorFilter: public IFilter
{
public:
    virtual ~GeneratorFilter() = default;

private:

};
*/

#endif
