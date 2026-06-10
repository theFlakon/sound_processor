#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "filters.hpp"
#include <memory>

class PipeLine
{
public:
    void apply(Waveform& sound) const;

public:
    void addFilter(std::unique_ptr<IFilter> filter);
    size_t getFiltersCnt() const;
    IFilter& operator[](size_t idx);
    const IFilter& operator[](size_t idx) const;

private:
    std::vector<std::unique_ptr<IFilter>> _filters;
};

#endif
