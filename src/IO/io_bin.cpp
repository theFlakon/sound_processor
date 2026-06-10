#include "io_bin.hpp"
#include <iostream>

static size_t calcNumSamples(uint16_t bitsPerSample, uint16_t channelsCnt,
                             uint32_t dataChunkSize);

void IOBinReader::readFile(Waveform& waveform)
{
    const std::filesystem::path& file = getFile();

    std::ifstream inputFileStream(file, std::ios::binary);

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
}

void IOBinWriter::writeFile(const Waveform& waveform)
{
    const std::filesystem::path& file = getFile();

    std::ofstream outputFileStream(file, std::ios::binary);

    writeChunk(outputFileStream, waveform.getRiffChunk());
    writeChunk(outputFileStream, waveform.getFmtChunk());
    writeChunk(outputFileStream, waveform.getDataChunk());

    writeSamples(outputFileStream, waveform.getSoundSamplesBuffer());
}

void IOBinReader::readSamples(std::ifstream& inputFileStream,
                              std::vector<int16_t>& dest,
                              uint32_t dataChunkSize)
{
    inputFileStream.read(reinterpret_cast<char*>(dest.data()), dataChunkSize);

    if(!inputFileStream)
        throw FileReadException("samples");
}

void IOBinWriter::writeSamples(std::ofstream& outputFileStream,
                               const std::vector<int16_t>& source)
{
    const auto BYTES =
        static_cast<std::streamsize>(source.size() * sizeof(int16_t));

    outputFileStream.write(reinterpret_cast<const char*>(source.data()), BYTES);

    if(!outputFileStream)
        throw FileWriteException("samples");
}

size_t calcNumSamples(uint16_t bitsPerSample, uint16_t channelsCnt,
                      uint32_t dataChunkSize)
{
    size_t bytesPerSample = (bitsPerSample / 8) * channelsCnt;
    return dataChunkSize / bytesPerSample;
}
