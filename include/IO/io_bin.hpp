#ifndef IO_BIN_HPP
#define IO_BIN_HPP

#include "exceptions.hpp"
#include "waveform.hpp"
#include <filesystem>
#include <fstream>
#include <string>

class IOBinHandler
{
public:
    explicit IOBinHandler(const std::string& fileName): _file(fileName)
    {
    }

    const std::filesystem::path& getFile() const
    {
        return _file;
    }

protected:
    std::filesystem::path _file;
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
            throw FileReadException(_file);
    }

    void readSamples(std::ifstream& stream, std::vector<int16_t>& dest,
                     uint32_t dataChunkSize);
};

class IOBinWriter: public IOBinHandler
{
public:
    explicit IOBinWriter(const std::string& fileName): IOBinHandler(fileName)
    {
    }

    void writeFile(const Waveform& waveform);

private:
    template <typename T_>
    void writeChunk(std::ofstream& stream, const T_& chunk)
    {
        stream.write(reinterpret_cast<const char*>(&chunk), sizeof(chunk));
        if(!stream)
            throw FileWriteException(_file);
    }

    void writeSamples(std::ofstream& stream,
                      const std::vector<int16_t>& source);
};

#endif
