#include "dispatcher.hpp"
#include "exceptions.hpp"
#include "io_bin.hpp"
#include "output.hpp"
#include <iostream>

void Dispatcher::process(ArgsParser& argsParser)
{
    OutputHandler outputHandler{};
    Waveform waveform{};

    if(argsParser.getHelpMsgStatus())
    {
        outputHandler.printHelpMsg();
        return;
    }

    if(argsParser.getInputFileStatus())
    {
        IOBinReader reader(argsParser.getInputFileName());

        try
        {
            reader.readFile(waveform);
        }
        catch(const IOException& error)
        {
            std::cerr << error.what() << std::endl;
            throw AppException("Something went wrong");
        }
    }
}
