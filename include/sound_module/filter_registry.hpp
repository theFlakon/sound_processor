#ifndef FILTER_REGISTRY_HPP
#define FILTER_REGISTRY_HPP

#include "filters.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

using FilterFactory =
    std::function<std::unique_ptr<IFilter>(const std::vector<std::string>&)>;

class FilterRegistry
{
public:
    static std::unique_ptr<IFilter>
    createFilter(const std::string& name,
                 const std::vector<std::string>& params);

private:
    static const std::unordered_map<std::string, FilterFactory>&
    getFiltersTable();
};

#endif
