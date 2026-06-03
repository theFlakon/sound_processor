#ifndef IO_BIN_HPP
#define IO_BIN_HPP

#include "exceptions.hpp"
#include "waveform.hpp"
#include <fstream>
#include <string>

class IOBinHandler
{
public:
    explicit IOBinHandler(const std::string& fileName): _fileName(fileName)
    {
    }

    const std::string& getFileName() const
    {
        return _fileName;
    }

protected:
    std::string _fileName;
};

class IOBinReader: public IOBinHandler
{
public:
    explicit IOBinReader(const std::string& fileName): IOBinHandler(fileName)
    {
    }

    void readFile(Waveform& waveform);

private:
    template <typename T_> void readChunk(std::ifstream& stream, T_& chunk)
    {
        stream.read(reinterpret_cast<char*>(&chunk), sizeof(chunk));

        if(!stream)
            throw FileReadException(_fileName);
    }

    void readSamples(std::ifstream& stream, std::vector<int16_t>& dest,
                     uint32_t dataChunkSize);
};

/*
class IOBinWriter: public IOBinHandler
{
public:
    explicit IOBinWriter(const std::string& fileName): IOBinHandler(fileName)
    {
    }

    void writeFile(const Waveform& waveform);

private:
    template <typename T> void writeChunk(std::ofstream& stream, const T& chunk)
    {
        stream.write(reinterpret_cast<const char*>(&chunk), sizeof(chunk));
        if(!stream)
            throw FileWriteException(_fileName);
    }
};
*/

#endif
