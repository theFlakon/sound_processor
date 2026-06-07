#ifndef ARGS_PARSER
#define ARGS_PARSER

#include <exceptions.hpp>
#include <filesystem>
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
    void parse(int argc, char* argv[]);  // Main func that parses input args

    // Getters && setters
public:
    template <typename T_> void setInputFile(const T_& fileName)
    {
        _inputFile = fileName;
    }

    template <typename T_> void setOutputFile(const T_& fileName)
    {
        _outputFile = fileName;
    }

    const std::filesystem::path& getInputFile() const;
    const std::filesystem::path& getOutputFile() const;
    const std::vector<FilterDescriptor>& getFilterDescriptorsVec() const;
    bool getHelpMsgStatus() const;

private:
    void addFiltArg2PipeLine(std::vector<std::string>& filterOptions);

    void checkFileExistence(const std::filesystem::path& file);

    std::vector<FilterDescriptor> _filterDescriptorsVec{};
    std::filesystem::path _inputFile{};
    std::filesystem::path _outputFile{};
    bool _printHelpMsg{};
};

#endif
