#ifndef ARGS_PARSER
#define ARGS_PARSER

#include "general.hpp"
#include <getopt.h>
#include <vector>

using FilterDescriptor = int;

enum class ParseStatusCode
{
    validArgs = 0,     // Correct arguments
    invalidArgs = -1,  // Incorrect arguments
};

class ArgsParser
{
public:
    ArgsParser() = default;  // Default constrs/destrs will suffice our goals
    ParseStatusCode
    parse(int argc, char* argv[],
          ProgramOptions& options);  // Main func that parses input args

    // Getters && setters
    const std::vector<FilterDescriptor>& getFiltersVec() const;

private:
    std::vector<FilterDescriptor> _filterDescriptors{};
};

#endif
