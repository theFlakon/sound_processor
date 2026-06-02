#ifndef GENERAL_HPP
#define GENERAL_HPP

#include <string>

struct ProgramOptions
{
    std::string inputFileName{};
    std::string outputFileName{};
    bool printHelpMsg{};
};

#endif
