#ifndef FILTER_REGISTRY_HPP
#define FILTER_REGISTRY_HPP

#include "filters.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

using FilterFactory =
    std::function<std::unique_ptr<IFilter>(const std::vector<std::string>&)>;

/**
 * @brief Registry for creating filters by name.
 *
 * Provides factory-based construction of filters from string identifiers
 * and parameter lists.
 */
class FilterRegistry
{
public:
    /**
     * @brief Creates filter by name.
     *
     * @param name Filter name identifier
     * @param params Filter parameters
     * @return Constructed filter instance
     * @throws std::runtime_error if filter is not found or parameters are
     * invalid
     */
    static std::unique_ptr<IFilter>
    createFilter(const std::string& name,
                 const std::vector<std::string>& params);

private:
    /**
     * @brief Returns internal filter factory table.
     *
     * @return Map of filter names to factory functions
     */
    static const std::unordered_map<std::string, FilterFactory>&
    getFiltersTable();
};

#endif
