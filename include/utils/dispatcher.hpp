#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "args_parser.hpp"

class Dispatcher
{
public:
    Dispatcher() = default;

    void process(ArgsParser& argsParser);

private:
    std::unique_ptr<Waveform> readInput(const std::filesystem::path& fileName);
    void writeOutput(const std::filesystem::path& fileName,
                     const Waveform& waveform);
};

#endif
