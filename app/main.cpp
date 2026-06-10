#include "args_parser.hpp"
#include "dispatcher.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    ArgsParser parser{};

    try
    {
        parser.parse(argc, argv);
    }
    catch(const AppException& error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        Dispatch::process(parser);
    }
    catch(const AppException& error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
