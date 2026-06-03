#include "io_bin.hpp"
#include <iostream>

static size_t calcNumSamples(uint16_t bitsPerSample, uint16_t channelsCnt,
                             uint32_t dataChunkSize);

void IOBinReader::readFile(Waveform& waveform)
{
    const std::string& fileName = getFileName();

    std::ifstream inputFileStream(fileName, std::ios::binary);

    if(!inputFileStream.is_open())
        throw FileNotFoundException(fileName);

    FmtChunk& fmtChunk = waveform.getFmtChunk();
    DataChunk& dataChunk = waveform.getDataChunk();
    std::vector<int16_t>& samplesDest = waveform.getSoundSamplesBuffer();

    readChunk(inputFileStream, waveform.getRiffChunk());
    readChunk(inputFileStream, fmtChunk);
    readChunk(inputFileStream, dataChunk);

    size_t numSamples = calcNumSamples(fmtChunk.bitsPerSample,
                                       fmtChunk.channels, dataChunk.size);
    samplesDest.resize(numSamples);
    readSamples(inputFileStream, samplesDest, dataChunk.size);

    std::cout << numSamples << std::endl;
}

void IOBinReader::readSamples(std::ifstream& inputFileStream,
                              std::vector<int16_t>& dest,
                              uint32_t dataChunkSize)
{
    inputFileStream.read(reinterpret_cast<char*>(dest.data()), dataChunkSize);

    if(!inputFileStream)
        throw FileReadException("samples");
}

size_t calcNumSamples(uint16_t bitsPerSample, uint16_t channelsCnt,
                      uint32_t dataChunkSize)
{
    size_t bytesPerSample = (bitsPerSample / 8) * channelsCnt;
    return dataChunkSize / bytesPerSample;
}
