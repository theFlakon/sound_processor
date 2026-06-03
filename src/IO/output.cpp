#include "output.hpp"
#include <iostream>
#include <string>

void OutputHandler::printHelpMsg()
{
    const std::string HELP_MSG = "Help";

    std::cout << HELP_MSG << std::endl;
}

void printErrorMsg(ErrorType errType, const char* msg)
{
    switch(errType)
    {
    case ErrorType::invalidArgsError:
        std::cerr << "An error occured!\n"
                  << msg << "\n"
                  << "Please, check your input data or type -h (--help) to "
                     "know all the possible options"
                  << std::endl;
        return;

    case ErrorType::fileError:
        std::cerr << "An error occured while working with your file!\n"
                  << msg << std::endl;

    default:
        return;
    }
}
