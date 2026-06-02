#ifndef ARGS_PARSER
#define ARGS_PARSER

#include <getopt.h>
#include <string>
#include <vector>

using FilterDescriptor = int;

enum class ParseStatusCode
{
    validArgs = 0,     // Correct arguments
    helpMsg = 1,       // Show help msg
    invalidArgs = -1,  // Incorrect arguments
};

class ArgsParser
{
public:
    ArgsParser() = default;  // Default constrs/destrs will suffice our goals
    ParseStatusCode parse(int argc,
                          char* argv[]);  // Main func that parses input args

    // Getters && setters
    const std::string& getInFileName() const;
    const std::string& getOutFileName() const;
    const std::vector<FilterDescriptor>& getFiltersVec() const;

    void setInFileName(std::string inFileName);
    void setOutFileName(std::string outFileName);

private:
    std::string _inFileName;
    std::string _outFileName;
    std::vector<FilterDescriptor> _filterDescriptors;
};

#endif
