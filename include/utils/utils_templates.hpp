#ifndef UTILS_TEMPLATES_HPP
#define UTILS_TEMPLATES_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

template <typename T_> inline T_ getInt16Max()
{
    return static_cast<T_>(std::numeric_limits<int16_t>::max());
}

template <typename T_> inline T_ getInt16Min()
{
    return static_cast<T_>(std::numeric_limits<int16_t>::min());
}

template <typename T_> inline int16_t clampInt16(T_ num)
{
    return static_cast<int16_t>(
        std::clamp(num, getInt16Min<T_>(), getInt16Max<T_>()));
}

template <typename T_> inline bool isFloatValid(T_ floatNum)
{
    return !(std::isnan(floatNum) || std::isinf(floatNum));
}

#endif
