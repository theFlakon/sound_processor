#include "args_parser.hpp"
#include "filter_registry.hpp"

static FilterDescriptor getFilterDescriptor(int argc, char* argv[],
                                            int& argvIdx);

void ArgsParser::parse(int argc, char* argv[])
{
    const size_t FLAGS_CNT = 4;

    int opt = 0, optionIndex = 0;
    opterr = 0;

    const struct option LONG_OPTIONS[FLAGS_CNT + 1] = {
        {"help", no_argument, nullptr, 'h'},
        {"input", required_argument, nullptr, 'i'},
        {"output", required_argument, nullptr, 'o'},
        {"filter", required_argument, nullptr, 'f'},
        {nullptr, 0, nullptr, 0}};

    const char* SHORT_OPTIONS = "hi:o:f:";  // NOLINT

    if(argc <= 1)
    {
        _printHelpMsg = true;
        return;
    }

    while((opt = getopt_long(argc, argv, SHORT_OPTIONS, LONG_OPTIONS,
                             &optionIndex)) != -1)
    {
        switch(opt)
        {
        case 'h':
            _printHelpMsg = true;
            return;

        case 'i':
            setInputFile(optarg);
            checkFileExistence(_inputFile);
            break;

        case 'o':
            setOutputFile(optarg);
            checkFileExistence(_outputFile);
            break;

        case 'f':
        {
            --optind;
            FilterDescriptor currFilterDescr =
                getFilterDescriptor(argc, argv, optind);
            _filterDescriptorsVec.push_back(currFilterDescr);

            break;
        }

        default:
        {
            std::string invalidFlagDescr = std::string("unrecognized option ") +
                                           "'" + std::string(argv[optind - 1]) +
                                           "'" + "\n" +
                                           "Please, type -h (or --help) to "
                                           "know all the possible options list";

            throw InvalidArgsException(invalidFlagDescr);
        }
        }
    }
}

FilterDescriptor getFilterDescriptor(int argc, char* argv[], int& argvIdx)
{
    FilterDescriptor result{};

    result.filterName = std::string(argv[argvIdx++]);

    while(argvIdx < argc &&
          (argv[argvIdx][0] != '-' || std::isdigit(argv[argvIdx][1])))
    {
        result.options.emplace_back(argv[argvIdx]);
        ++argvIdx;
    }

    return result;
}

const std::filesystem::path& ArgsParser::getInputFile() const
{
    return _inputFile;
}

const std::filesystem::path& ArgsParser::getOutputFile() const
{
    return _outputFile;
}

const std::vector<FilterDescriptor>& ArgsParser::getFilterDescriptorsVec() const
{
    return _filterDescriptorsVec;
}

bool ArgsParser::getHelpMsgStatus() const
{
    return _printHelpMsg;
}

void ArgsParser::checkFileExistence(const std::filesystem::path& file)
{
    if(!std::filesystem::exists(file))
        throw FileNotFoundException(_inputFile);
}
