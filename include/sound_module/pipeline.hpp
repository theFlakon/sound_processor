#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "filters.hpp"
#include <memory>

/**
 * @brief Processing pipeline for sequential audio filters.
 *
 * Stores a list of filters and applies them in order to a waveform.
 */
class PipeLine
{
public:
    /**
     * @brief Applies all filters in sequence.
     *
     * @param sound Audio data to process
     */
    void apply(Waveform& sound) const;

public:
    /**
     * @brief Adds a filter to the pipeline.
     *
     * Ownership is transferred to the pipeline.
     *
     * @param filter Filter to add
     */
    void addFilter(std::unique_ptr<IFilter> filter);

    /**
     * @brief Returns number of filters in pipeline.
     */
    size_t getFiltersCnt() const;

    /**
     * @brief Access filter by index (mutable).
     * @param idx Index of filter
     */
    IFilter& operator[](size_t idx);

    /**
     * @brief Access filter by index (read-only).
     * @param idx Index of filter
     */
    const IFilter& operator[](size_t idx) const;

private:
    std::vector<std::unique_ptr<IFilter>> _filters;  ///< ordered filter chain
};

#endif
