#ifndef FILTERS_CHECKS_HPP
#define FILTERS_CHECKS_HPP

#include "exceptions.hpp"
#include "utils_templates.hpp"
#include <string>
#include <string_view>

namespace Fcheck {

/**
 * @brief Checks that value is finite and >= 0.
 *
 * Used for validating non-negative numeric parameters.
 *
 * @tparam T_ Numeric type
 * @param value Value to validate
 * @param name Parameter name (used in exception message)
 * @throws FilterParamError if value is NaN/inf or < 0
 */
template <typename T_>
inline void requireNonNegative(T_ value, std::string_view name)
{
    if(!isFloatValid(value) || value < 0.0)
        throw FilterParamError(std::string(name) + " must be >= 0");
}

/**
 * @brief Checks that value is finite and > 0.
 *
 * @tparam T_ Numeric type
 * @param value Value to validate
 * @param name Parameter name
 * @throws FilterParamError if value is invalid or <= 0
 */
template <typename T_>
inline void requirePositive(T_ value, std::string_view name)
{
    if(!isFloatValid(value) || value <= 0.0)
        throw FilterParamError(std::string(name) + " must be > 0");
}

/**
 * @brief Checks that value is within inclusive range.
 *
 * Validates that:
 * lowerBound <= value <= upperBound
 *
 * @tparam T_ Numeric type
 * @param value Value to validate
 * @param lowerBound Lower bound (inclusive)
 * @param upperBound Upper bound (inclusive)
 * @param name Parameter name
 * @throws FilterParamError if value is out of range or invalid
 */
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
