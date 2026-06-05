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
    catch(const InvalidArgsException& error)
    {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    dispatcher.process(parser);

    return EXIT_SUCCESS;
}
