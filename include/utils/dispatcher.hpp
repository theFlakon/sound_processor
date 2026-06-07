#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "args_parser.hpp"
#include "pipeline.hpp"
#include <vector>

class Dispatcher
{
public:
    Dispatcher() = default;

    void process(ArgsParser& argsParser);

private:
    std::unique_ptr<Waveform> readInput(const std::filesystem::path& fileName);
    void writeOutput(const std::filesystem::path& fileName,
                     const Waveform& waveform);

    PipeLine constructPipeLine(
        const std::vector<FilterDescriptor>& filterDescriptorsVec);
};

#endif
