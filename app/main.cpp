#include "args_parser.hpp"
#include "dispatcher.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    ArgsParser parser{};
    Dispatcher dispatcher{};

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
        dispatcher.process(parser);
    }
    catch(const AppException& error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
