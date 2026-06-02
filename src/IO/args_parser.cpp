#include "args_parser.hpp"
#include "globals.hpp"

ParseStatusCode ArgsParser::parse(int argc, char* argv[])
{
    int opt = 0, optionIndex = 0;

    const struct option LONG_OPTIONS[FLAGS_CNT + 1] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {nullptr, 0, nullptr, 0}};

    const char* SHORT_OPTIONS = "hi:o:";  // NOLINT

    if(argc <= 1)
        return ParseStatusCode::helpMsg;

    while((opt = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS,
                             &optionIndex)) != -1)
    {
        switch(opt)
        {
        case 'h':
            return ParseStatusCode::helpMsg;

        case 'i':
            setInFileName(std::string(optarg));
            break;

        case 'o':
            setOutFileName(std::string(optarg));
            break;

        default:
            return ParseStatusCode::invalidArgs;
        }
    }

    return ParseStatusCode::validArgs;
}

const std::string& ArgsParser::getInFileName() const
{
    return _inFileName;
}

const std::string& ArgsParser::getOutFileName() const
{
    return _outFileName;
}

const std::vector<FilterDescriptor>& ArgsParser::getFiltersVec() const
{
    return _filterDescriptors;
}

void ArgsParser::setInFileName(std::string inFileName)
{
    _inFileName = std::move(inFileName);
}

void ArgsParser::setOutFileName(std::string outFileName)
{
    _outFileName = std::move(outFileName);
}
