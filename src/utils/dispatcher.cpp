#include "dispatcher.hpp"
#include "io_bin.hpp"
#include "output.hpp"

static void writeOutput(const std::filesystem::path& fileName,
                        const Waveform& waveform);
static std::unique_ptr<Waveform>
readInput(const std::filesystem::path& fileName);

void Dispatch::process(ArgsParser& argsParser)
{
    const std::filesystem::path& outFile = argsParser.getOutputFile();

    if(argsParser.getHelpMsgStatus())
    {
        StdOut::printHelpMsg();
        return;
    }

    if(outFile.empty())
    {
        StdOut::printEmptyOutFileMsg();
        return;
    }

    auto sound = readInput(argsParser.getInputFile());
    argsParser.getPipeline().apply(*sound);

    writeOutput(argsParser.getOutputFile(), *sound);
}

void writeOutput(const std::filesystem::path& fileName,
                 const Waveform& waveform)
{
    IOBinWriter writer(fileName);

    writer.writeFile(waveform);
}

std::unique_ptr<Waveform> readInput(const std::filesystem::path& fileName)
{
    std::unique_ptr<Waveform> sound = std::make_unique<Waveform>();

    if(fileName.empty())
        return makeDefaultTone();

    IOBinReader reader(fileName);

    reader.readFile(*sound);

    return sound;
}
