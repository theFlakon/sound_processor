#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP

#include <cstdint>
#include <vector>

struct __attribute__((packed)) RiffChunk
{
    uint32_t sign{};
    uint32_t size{};
    uint32_t waveid{};
};

struct __attribute__((packed)) FmtChunk
{
    uint32_t sign{};            // Chunk ID: "fmt " (0x666D7420)
    uint32_t size{};            // Chunk Data Size: 16
    uint16_t compression{};     // Compression Code
    uint16_t channels{};        // Number of channels (= 1)
    uint32_t sampleRate{};      // Sample rate
    uint32_t bytesPerSecond{};  // Average bytes per second
    uint16_t blockAlign{};      // Block align
    uint16_t bitsPerSample{};   // Significant bits per sample
};

struct __attribute__((packed)) DataChunk
{
    uint32_t sign{};
    uint32_t size{};
};

class Waveform
{
public:
    Waveform() = default;

    RiffChunk& getRiffChunk();
    FmtChunk& getFmtChunk();
    DataChunk& getDataChunk();
    std::vector<int16_t>& getSoundSamplesBuffer();

private:
    RiffChunk _riffChunk{};
    FmtChunk _fmtChunk{};
    DataChunk _dataChunk{};
    std::vector<int16_t> _soundSamples{};
};

#endif
