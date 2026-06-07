#include "exceptions.hpp"
#include "filter_registry.hpp"
#include "filters.hpp"

std::unique_ptr<IFilter>
FilterRegistry::createFilter(const std::string& name,
                             const std::vector<std::string>& params)
{
    const auto& filtersTable = getFiltersTable();
    auto it = filtersTable.find(name);

    if(it == filtersTable.end())
        throw AppException("Unknown filter: " + name);

    return it->second(params);
}

const std::unordered_map<std::string, FilterFactory>&
FilterRegistry::getFiltersTable()
{
    static const std::unordered_map<std::string, FilterFactory> TABLE = {
        {"ampl",
         [](const std::vector<std::string>& params) -> std::unique_ptr<IFilter>
         {
             if(params.size() != 1)
                 throw AppException("ampl expects 1 parameter");

             double coeff = std::stod(params[0]);

             if(coeff < 0.0)
                 throw AppException("ampl coeff must be non-negative");

             return std::make_unique<AmplFilter>(coeff);
         }}};

    return TABLE;
}
