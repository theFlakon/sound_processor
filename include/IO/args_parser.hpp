#ifndef ARGS_PARSER
#define ARGS_PARSER

#include <getopt.h>
#include <string>
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
    ParseStatusCode parse(int argc,
                          char* argv[]);  // Main func that parses input args

    // Getters && setters
    const std::string& getInputFileName() const;
    const std::string& getOutputFileName() const;
    bool getHelpMsgStatus() const;
    bool getInputFileStatus() const;

    const std::vector<FilterDescriptor>& getFiltersVec() const;

private:
    std::string _inputFileName{};
    std::string _outputFileName{};
    bool _printHelpMsg{};
    bool _isInputFile{};
    std::vector<FilterDescriptor> _filterDescriptors{};
};

#endif
