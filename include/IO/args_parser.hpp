#ifndef ARGS_PARSER
#define ARGS_PARSER

#include "pipeline.hpp"
#include <exceptions.hpp>
#include <filesystem>
#include <getopt.h>
#include <memory>
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
    // Basic Methods
public:
    void parse(int argc,
               char* argv[]);  // Main func that parses input args

    // Getters && setters
public:
    template <typename T_> void setInputFile(const T_& fileName)
    {
        _inputFile = fileName;
    }

    template <typename T_> void setOutputFile(const T_& fileName)
    {
        _outputFile = fileName;
        _isOutFile = true;
    }

    const std::filesystem::path& getInputFile() const;
    const std::filesystem::path& getOutputFile() const;
    const PipeLine& getPipeline() const;
    bool getHelpMsgStatus() const;
    bool isOutFile() const;

private:
    void addFiltArg2PipeLine(std::vector<std::string>& filterOptions);

    void checkFileExistence(const std::filesystem::path& file);

    PipeLine _pipeline{};
    std::filesystem::path _inputFile{};
    std::filesystem::path _outputFile{};
    bool _printHelpMsg{};
    bool _isOutFile{};
};

class CmdLineArgs2PipelineConverter
{
public:
    void setFilterDescriptor(int argc, char* argv[], int& argvIdx);
    void addFilter2PipeLine(PipeLine& pipeline);

private:
    std::unique_ptr<FilterDescriptor> _currDescriptor{};
};

#endif
