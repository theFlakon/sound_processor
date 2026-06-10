#include "exceptions.hpp"
#include "filter_registry.hpp"
#include "filters.hpp"

static double parseDouble(const std::string& str);
static double parseInt(const std::string& str);

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
    using Params = std::vector<std::string>;
    using Ptr = std::unique_ptr<IFilter>;

    static const std::unordered_map<std::string, FilterFactory> TABLE = {

        {"ampl",
         [](const Params& params) -> Ptr
         {
             if(params.size() != 1)
                 throw AppException("ampl expects 1 parameter");

             return std::make_unique<AmplFilter>(parseDouble(params[0]));
         }},

        {"normalize",
         [](const Params& params) -> Ptr
         {
             if(params.size() > 1)
                 throw AppException("normalize expects 0 or 1 parameters");

             if(params.empty())
                 return std::make_unique<NormalizeFilter>();  // peak = 1.0

             return std::make_unique<NormalizeFilter>(parseDouble(params[0]));
         }},

        {"silence",
         [](const Params& params) -> Ptr
         {
             if(params.size() != 3)
                 throw AppException("silence expects 3 parameters");

             return std::make_unique<SilenceFilter>(
                 params[0],                // unit ("sec" / "ms")
                 parseDouble(params[1]),   // start
                 parseDouble(params[2]));  // end
         }},

        {"timestretch",
         [](const Params& params) -> Ptr
         {
             if(params.size() != 1)
                 throw AppException("timestretch expects 1 parameter");

             return std::make_unique<TimestretchFilter>(parseDouble(params[0]));
         }},

        {"lowpass",
         [](const Params& params) -> Ptr
         {
             if(params.size() != 1)
                 throw AppException("lowpass expects 1 parameter");

             return std::make_unique<LowpassFilter>(parseInt(params[0]));
         }},

        {"generator",
         [](const Params& params) -> Ptr
         {
             if(params.empty())
                 throw AppException("generator expects a type (sin / am / fm)");

             const std::string& type = params[0];

             if(type == "sin")
             {
                 if(params.size() != 3)
                     throw AppException("generator sin expects 2 parameters");
                 return std::make_unique<SinGenerator>(
                     parseDouble(params[1]),   // frequency_hz
                     parseDouble(params[2]));  // duration_ms
             }

             if(type == "am")
             {
                 if(params.size() != 6)
                     throw AppException("generator am expects 5 parameters");

                 return std::make_unique<AmGenerator>(
                     parseDouble(params[1]),   // amplitude
                     parseDouble(params[2]),   // carrier_hz
                     parseDouble(params[3]),   // modulation_hz
                     parseDouble(params[4]),   // depth
                     parseDouble(params[5]));  // duration_ms
             }

             if(type == "fm")
             {
                 if(params.size() != 6)
                     throw AppException("generator fm expects 5 parameters");

                 return std::make_unique<FmGenerator>(
                     parseDouble(params[1]),   // amplitude
                     parseDouble(params[2]),   // carrier_hz
                     parseDouble(params[3]),   // modulation_hz
                     parseDouble(params[4]),   // deviation_hz
                     parseDouble(params[5]));  // duration_ms
             }

             throw AppException("Unknown generator type: " + type);
         }},
    };
    return TABLE;
}

double parseDouble(const std::string& str)
{
    try
    {
        return std::stod(str);
    }
    catch(const std::exception&)
    {
        throw AppException("expected a number, got: " + str);
    }
}
double parseInt(const std::string& str)
{
    try
    {
        return std::stoi(str);
    }
    catch(const std::exception&)
    {
        throw AppException("expected an integer, got: " + str);
    }
}
