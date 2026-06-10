#include "args_parser.hpp"
#include "filter_registry.hpp"

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
    CmdLineArgs2PipelineConverter converter{};

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
            break;

        case 'f':
        {
            --optind;

            converter.setFilterDescriptor(argc, argv, optind);
            converter.addFilter2PipeLine(_pipeline);

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

void CmdLineArgs2PipelineConverter::setFilterDescriptor(int argc, char* argv[],
                                                        int& argvIdx)
{
    std::unique_ptr<FilterDescriptor> result =
        std::make_unique<FilterDescriptor>();

    result->filterName = std::string(argv[argvIdx++]);

    while(argvIdx < argc &&
          (argv[argvIdx][0] != '-' || std::isdigit(argv[argvIdx][1])))
    {
        result->options.emplace_back(argv[argvIdx]);
        ++argvIdx;
    }

    _currDescriptor = std::move(result);
}

void CmdLineArgs2PipelineConverter::addFilter2PipeLine(PipeLine& pipeline)
{
    if(_currDescriptor == nullptr)
        return;

    FilterDescriptor filterDescriptor = *_currDescriptor;

    std::unique_ptr<IFilter> filter = FilterRegistry::createFilter(
        filterDescriptor.filterName, filterDescriptor.options);

    pipeline.addFilter(std::move(filter));

    _currDescriptor.reset();
}

const std::filesystem::path& ArgsParser::getInputFile() const
{
    return _inputFile;
}

const std::filesystem::path& ArgsParser::getOutputFile() const
{
    return _outputFile;
}

const PipeLine& ArgsParser::getPipeline() const
{
    return _pipeline;
}

bool ArgsParser::getHelpMsgStatus() const
{
    return _printHelpMsg;
}

bool ArgsParser::isOutFile() const
{
    return _isOutFile;
}

void ArgsParser::checkFileExistence(const std::filesystem::path& file)
{
    if(!std::filesystem::exists(file))
        throw FileNotFoundException(file);
}
