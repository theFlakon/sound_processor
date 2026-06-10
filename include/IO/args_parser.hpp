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

/**
 * @brief Describes a single filter and its associated options.
 */
struct FilterDescriptor
{
    std::string filterName{};  ///< Name of the filter
    FilterOptions options{};   ///< List of filter options/arguments
};

/**
 * @brief Parses command line arguments and builds a processing pipeline.
 *
 * This class is responsible for:
 * - extracting input/output file paths
 * - parsing filter arguments
 * - constructing the internal pipeline representation
 * - handling help flag logic
 */
class ArgsParser
{
    // Basic Methods
public:
    /**
     * @brief Parses command line arguments.
     *
     * Main entry point for processing CLI input.
     *
     * @param argc Argument count
     * @param argv Argument vector
     */
    void parse(int argc, char* argv[]);

    // Getters && setters
public:
    /**
     * @brief Sets input file path.
     *
     * @tparam T_ Any type convertible to std::filesystem::path
     * @param fileName Input file name/path
     */
    template <typename T_> void setInputFile(const T_& fileName)
    {
        _inputFile = fileName;
    }

    /**
     * @brief Sets output file path.
     *
     * @tparam T_ Any type convertible to std::filesystem::path
     * @param fileName Output file name/path
     */
    template <typename T_> void setOutputFile(const T_& fileName)
    {
        _outputFile = fileName;
    }

    /**
     * @brief Returns input file path.
     */
    const std::filesystem::path& getInputFile() const;

    /**
     * @brief Returns output file path.
     */
    const std::filesystem::path& getOutputFile() const;

    /**
     * @brief Returns constructed processing pipeline.
     */
    const PipeLine& getPipeline() const;

    /**
     * @brief Indicates whether help message should be printed.
     */
    bool getHelpMsgStatus() const;

private:
    /**
     * @brief Adds parsed filter arguments into pipeline.
     *
     * @param filterOptions Vector of filter arguments
     */
    void addFiltArg2PipeLine(std::vector<std::string>& filterOptions);

    /**
     * @brief Checks whether a file exists.
     *
     * @param file Path to file
     * @throws std::runtime_error if file does not exist
     */
    void checkFileExistence(const std::filesystem::path& file);

private:
    PipeLine _pipeline{};                 ///< Internal processing pipeline
    std::filesystem::path _inputFile{};   ///< Input file path
    std::filesystem::path _outputFile{};  ///< Output file path
    bool _printHelpMsg{};                 ///< Flag for help message
};

/**
 * @brief Converts raw command line arguments into pipeline filters.
 *
 * This class is responsible for:
 * - reading CLI filter descriptors
 * - constructing FilterDescriptor objects
 * - appending them to a processing pipeline
 */
class CmdLineArgs2PipelineConverter
{
public:
    /**
     * @brief Parses a filter descriptor from argv.
     *
     * @param argc Argument count
     * @param argv Argument vector
     * @param argvIdx Current index in argv (will be advanced)
     */
    void setFilterDescriptor(int argc, char* argv[], int& argvIdx);

    /**
     * @brief Adds currently parsed filter to pipeline.
     *
     * @param pipeline Target pipeline
     */
    void addFilter2PipeLine(PipeLine& pipeline);

private:
    std::unique_ptr<FilterDescriptor>
        _currDescriptor{};  ///< Currently parsed filter
};

#endif
