#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "filters.hpp"
#include <memory>

class PipeLine
{
public:
    ~PipeLine() = default;

    bool apply(Waveform* sound);

public:
    IFilter* addFilter(IFilter* filter);
    size_t getFilterNumbers() const;
    IFilter* operator[](size_t idx) const;

protected:
    std::vector<std::unique_ptr<IFilter>> _filters;
};

#endif
