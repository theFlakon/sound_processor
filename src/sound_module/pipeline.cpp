#include "pipeline.hpp"

void PipeLine::apply(Waveform& sound)
{
    for(size_t filterIdx = 0; filterIdx < getFiltersCnt(); ++filterIdx)
        (*this)[filterIdx].apply(sound);
}

void PipeLine::addFilter(std::unique_ptr<IFilter> filter)
{
    _filters.push_back(std::move(filter));
}

size_t PipeLine::getFiltersCnt() const
{
    return _filters.size();
}

IFilter& PipeLine::operator[](size_t idx)
{
    return *_filters[idx];
}

const IFilter& PipeLine::operator[](size_t idx) const
{
    return *_filters[idx];
}
