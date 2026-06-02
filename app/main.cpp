#include "args_parser.hpp"
#include "dispatcher.hpp"

int main(int argc, char* argv[])
{
    ArgsParser parser{};
    Dispatcher dispatcher{};
    ProgramOptions options{};

    ParseStatusCode parseStatusCode = parser.parse(argc, argv, options);

    if(parseStatusCode == ParseStatusCode::validArgs)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
