#include "args_parser.hpp"

ParseStatusCode ArgsParser::parse(int argc, char* argv[])
{
    const size_t FLAGS_CNT = 3;

    int opt = 0, optionIndex = 0;

    const struct option LONG_OPTIONS[FLAGS_CNT + 1] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {nullptr, 0, nullptr, 0}};

    const char* SHORT_OPTIONS = "hi:o:";  // NOLINT

    if(argc <= 1)
    {
        _printHelpMsg = true;
        return ParseStatusCode::validArgs;
    }

    while((opt = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS,
                             &optionIndex)) != -1)
    {
        switch(opt)
        {
        case 'h':
            _printHelpMsg = true;
            return ParseStatusCode::validArgs;

        case 'i':
            _isInputFile = true;
            _inputFileName = std::string(optarg);
            break;

        case 'o':
            _outputFileName = std::string(optarg);
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

const std::string& ArgsParser::getInputFileName() const
{
    return _inputFileName;
}

const std::string& ArgsParser::getOutputFileName() const
{
    return _outputFileName;
}

bool ArgsParser::getHelpMsgStatus() const
{
    return _printHelpMsg;
}

bool ArgsParser::getInputFileStatus() const
{
    return _isInputFile;
}
