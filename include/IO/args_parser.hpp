#ifndef ARGS_PARSER
#define ARGS_PARSER

#include <exceptions.hpp>
#include <getopt.h>
#include <string>
#include <vector>

using FilterOptions = std::vector<std::string>;

struct FilterDescriptor
{
    std::string filterName{};
    FilterOptions options{};
};

class ArgsParser
{
    // Constructs && Destructors
public:
    ArgsParser() = default;

    // Basic Methods
public:
    void parse(int argc,
               char* argv[]);  // Main func that parses input args

    // Getters && setters
public:
    const std::string& getInputFileName() const;
    const std::string& getOutputFileName() const;
    bool getHelpMsgStatus() const;
    bool getInputFileStatus() const;

private:
    void addFiltArg2PipeLine(std::vector<std::string>& filterOptions);

    std::vector<FilterDescriptor> _filterDescriptorsVec{};
    std::string _inputFileName{};
    std::string _outputFileName{};
    bool _printHelpMsg{};
    bool _isInputFile{};
};

#endif
