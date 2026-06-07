#include "dispatcher.hpp"
#include "filter_registry.hpp"
#include "io_bin.hpp"
#include "output.hpp"

void Dispatcher::process(ArgsParser& argsParser)
{
    OutputHandler outputHandler{};
    std::unique_ptr<Waveform> sound{};
    PipeLine pipeLine{};

    if(argsParser.getHelpMsgStatus())
    {
        outputHandler.printHelpMsg();
        return;
    }

    sound = readInput(argsParser.getInputFile());
    pipeLine = constructPipeLine(argsParser.getFilterDescriptorsVec());
    pipeLine.apply(*sound);

    writeOutput(argsParser.getOutputFile(), *sound);
}

PipeLine Dispatcher::constructPipeLine(
    const std::vector<FilterDescriptor>& filterDescriptorsVec)
{
    PipeLine pipeLine{};

    for(const FilterDescriptor& filterDescriptor: filterDescriptorsVec)
    {
        std::unique_ptr<IFilter> filter = FilterRegistry::createFilter(
            filterDescriptor.filterName, filterDescriptor.options);

        pipeLine.addFilter(std::move(filter));
    }

    return pipeLine;
}

void Dispatcher::writeOutput(const std::filesystem::path& fileName,
                             const Waveform& waveform)
{
    IOBinWriter writer(fileName);

    writer.writeFile(waveform);
}

std::unique_ptr<Waveform>
Dispatcher::readInput(const std::filesystem::path& fileName)
{
    std::unique_ptr<Waveform> sound = std::make_unique<Waveform>();
    IOBinReader reader(fileName);

    reader.readFile(*sound);

    return sound;
}
