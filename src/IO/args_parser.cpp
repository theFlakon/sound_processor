#include "args_parser.hpp"
#include "globals.hpp"

ParseStatusCode ArgsParser::parse(int argc, char* argv[],
                                  ProgramOptions& options)
{
    int opt = 0, optionIndex = 0;

    const struct option LONG_OPTIONS[FLAGS_CNT + 1] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {nullptr, 0, nullptr, 0}};

    const char* SHORT_OPTIONS = "hi:o:";  // NOLINT

    if(argc <= 1)
    {
        options.printHelpMsg = true;
        return ParseStatusCode::validArgs;
    }

    while((opt = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS,
                             &optionIndex)) != -1)
    {
        switch(opt)
        {
        case 'h':
            options.printHelpMsg = true;
            return ParseStatusCode::validArgs;

        case 'i':
            options.inputFileName = std::string(optarg);
            break;

        case 'o':
            options.outputFileName = std::string(optarg);
            break;

        default:
            return ParseStatusCode::invalidArgs;
        }
    }

    return ParseStatusCode::validArgs;
}

const std::vector<FilterDescriptor>& ArgsParser::getFiltersVec() const
{
    return _filterDescriptors;
}
