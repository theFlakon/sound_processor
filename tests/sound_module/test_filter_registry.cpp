#include "exceptions.hpp"
#include "filter_registry.hpp"
#include "filters.hpp"
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <string>
#include <vector>

namespace {

using Params = std::vector<std::string>;

template <typename T>
bool isFilterOfType(const std::string& name, const Params& params)
{
    std::unique_ptr<IFilter> f = FilterRegistry::createFilter(name, params);
    return dynamic_cast<T*>(f.get()) != nullptr;
}

}  // namespace

// ─────────────────────────────────────────────
// TEST CASES
// ─────────────────────────────────────────────
TEST_CASE("createFilter: ampl создаёт AmplFilter", "[registry]")
{
    REQUIRE(isFilterOfType<AmplFilter>("ampl", {"2.0"}));
}

TEST_CASE("createFilter: normalize с параметром", "[registry]")
{
    REQUIRE(isFilterOfType<NormalizeFilter>("normalize", {"0.8"}));
}

TEST_CASE("createFilter: normalize без параметров (peak по умолчанию)",
          "[registry]")
{
    REQUIRE(isFilterOfType<NormalizeFilter>("normalize", {}));
}

TEST_CASE("createFilter: silence создаёт SilenceFilter", "[registry]")
{
    REQUIRE(isFilterOfType<SilenceFilter>("silence", {"ms", "0", "500"}));
}

TEST_CASE("createFilter: timestretch создаёт TimestretchFilter", "[registry]")
{
    REQUIRE(isFilterOfType<TimestretchFilter>("timestretch", {"1.5"}));
}

TEST_CASE("createFilter: lowpass создаёт LowpassFilter", "[registry]")
{
    REQUIRE(isFilterOfType<LowpassFilter>("lowpass", {"3"}));
}

TEST_CASE("createFilter: generator sin", "[registry][generator]")
{
    REQUIRE(isFilterOfType<SinGenerator>("generator", {"sin", "440", "1000"}));
}

TEST_CASE("createFilter: generator am", "[registry][generator]")
{
    REQUIRE(isFilterOfType<AmGenerator>(
        "generator", {"am", "0.8", "440", "5", "0.5", "1000"}));
}

TEST_CASE("createFilter: generator fm", "[registry][generator]")
{
    REQUIRE(isFilterOfType<FmGenerator>(
        "generator", {"fm", "0.8", "440", "5", "50", "1000"}));
}

TEST_CASE("createFilter: возвращаемый указатель не пустой", "[registry]")
{
    auto f = FilterRegistry::createFilter("ampl", {"2.0"});
    REQUIRE(f != nullptr);
}

TEST_CASE("createFilter: неизвестное имя -> AppException", "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("bogus", {}), AppException);
}

TEST_CASE("createFilter: имя чувствительно к регистру", "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("Ampl", {"2.0"}),
                      AppException);
}

TEST_CASE("createFilter: ampl без параметров -> AppException",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("ampl", {}), AppException);
}

TEST_CASE("createFilter: ampl с лишним параметром -> AppException",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("ampl", {"1", "2"}),
                      AppException);
}

TEST_CASE("createFilter: normalize с двумя параметрами -> AppException",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("normalize", {"0.5", "0.6"}),
                      AppException);
}

TEST_CASE("createFilter: silence с неверным числом параметров -> AppException",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("silence", {"ms", "0"}),
                      AppException);
}

TEST_CASE("createFilter: timestretch без параметров -> AppException",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("timestretch", {}),
                      AppException);
}

TEST_CASE("createFilter: lowpass без параметров -> AppException",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("lowpass", {}),
                      AppException);
}

TEST_CASE("createFilter: generator без типа -> AppException",
          "[registry][generator][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("generator", {}),
                      AppException);
}

TEST_CASE("createFilter: generator с неизвестным типом -> AppException",
          "[registry][generator][error]")
{
    REQUIRE_THROWS_AS(
        FilterRegistry::createFilter("generator", {"square", "1", "2"}),
        AppException);
}

TEST_CASE(
    "createFilter: generator sin с неверным числом параметров -> AppException",
    "[registry][generator][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("generator", {"sin", "440"}),
                      AppException);
}

TEST_CASE(
    "createFilter: generator am с неверным числом параметров -> AppException",
    "[registry][generator][error]")
{
    REQUIRE_THROWS_AS(
        FilterRegistry::createFilter("generator", {"am", "0.8", "440"}),
        AppException);
}

TEST_CASE(
    "createFilter: generator fm с неверным числом параметров -> AppException",
    "[registry][generator][error]")
{
    REQUIRE_THROWS_AS(
        FilterRegistry::createFilter("generator", {"fm", "0.8", "440"}),
        AppException);
}

TEST_CASE("createFilter: normalize peak вне [0,1] -> FilterParamError",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("normalize", {"5"}),
                      FilterParamError);
}

TEST_CASE("createFilter: ampl отрицательный factor -> FilterParamError",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("ampl", {"-1"}),
                      FilterParamError);
}

TEST_CASE("createFilter: timestretch factor <= 0 -> FilterParamError",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("timestretch", {"0"}),
                      FilterParamError);
}

TEST_CASE("createFilter: lowpass чётное окно -> FilterParamError",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("lowpass", {"4"}),
                      FilterParamError);
}

TEST_CASE("createFilter: silence неверный unit -> FilterParamError",
          "[registry][error]")
{
    REQUIRE_THROWS_AS(
        FilterRegistry::createFilter("silence", {"hours", "0", "1"}),
        FilterParamError);
}

TEST_CASE(
    "createFilter: нечисловой параметр пробрасывает std::invalid_argument",
    "[registry][error]")
{
    REQUIRE_THROWS_AS(FilterRegistry::createFilter("ampl", {"abc"}),
                      AppException);
}
