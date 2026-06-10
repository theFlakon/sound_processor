#include "dispatcher.hpp"
#include "io_bin.hpp"
#include "output.hpp"

void Dispatcher::process(ArgsParser& argsParser)
{
    OutputHandler outputHandler{};

    if(argsParser.getHelpMsgStatus())
    {
        outputHandler.printHelpMsg();
        return;
    }

    if(!argsParser.isOutFile())
        return;

    auto sound = readInput(argsParser.getInputFile());
    argsParser.getPipeline().apply(*sound);

    writeOutput(argsParser.getOutputFile(), *sound);
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
