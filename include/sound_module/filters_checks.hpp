#ifndef FILTERS_CHECKS_HPP
#define FILTERS_CHECKS_HPP

#include "exceptions.hpp"
#include "utils_templates.hpp"
#include <string>
#include <string_view>

namespace Fcheck {

// value >= 0
template <typename T_>
inline void requireNonNegative(T_ value, std::string_view name)
{
    if(!isFloatValid(value) || value < 0.0)
        throw FilterParamError(std::string(name) + " must be >= 0");
}

// value > 0
template <typename T_>
inline void requirePositive(T_ value, std::string_view name)
{
    if(!isFloatValid(value) || value <= 0.0)
        throw FilterParamError(std::string(name) + " must be > 0");
}

// lowerBound <= value <= upperBound
template <typename T_>
inline void requireInRange(T_ value, double lowerBound, double upperBound,
                           std::string_view name)
{
    if(!isFloatValid(value) || value < lowerBound || value > upperBound)
        throw FilterParamError(std::string(name) + " must be in [" +
                               std::to_string(lowerBound) + ", " +
                               std::to_string(upperBound) + "]");
}

}  // namespace Fcheck

#endif
