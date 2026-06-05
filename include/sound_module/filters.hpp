#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "waveform.hpp"

class IFilter
{
public:
    virtual ~IFilter() = default;
    virtual bool apply(Waveform* sound) = 0;
};

#endif
