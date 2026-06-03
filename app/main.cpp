#include "args_parser.hpp"
#include "dispatcher.hpp"

int main(int argc, char* argv[])
{
    ArgsParser parser{};
    Dispatcher dispatcher{};

    ParseStatusCode parseStatusCode = parser.parse(argc, argv);

    if(parseStatusCode != ParseStatusCode::validArgs)
        return EXIT_FAILURE;

    dispatcher.process(parser);

    return EXIT_SUCCESS;
}
