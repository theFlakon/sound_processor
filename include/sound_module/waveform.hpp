#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP

#include <cstdint>
#include <vector>

using SoundSamplesVec = std::vector<int16_t>;

/**
 * @brief RIFF header chunk of WAV file.
 */
struct __attribute__((packed)) RiffChunk
{
    uint32_t sign{};    ///< "RIFF"
    uint32_t size{};    ///< file size minus 8 bytes
    uint32_t waveid{};  ///< "WAVE"
};

/**
 * @brief WAV format chunk (fmt subchunk).
 */
struct __attribute__((packed)) FmtChunk
{
    uint32_t sign{};            ///< "fmt "
    uint32_t size{};            ///< size of format section (usually 16)
    uint16_t compression{};     ///< audio format (PCM = 1)
    uint16_t channels{};        ///< number of channels
    uint32_t sampleRate{};      ///< samples per second
    uint32_t bytesPerSecond{};  ///< byte rate
    uint16_t blockAlign{};      ///< block alignment
    uint16_t bitsPerSample{};   ///< bits per sample
};

/**
 * @brief WAV data chunk header.
 */
struct __attribute__((packed)) DataChunk
{
    uint32_t sign{};  ///< "data"
    uint32_t size{};  ///< size of audio data
};

/**
 * @brief In-memory representation of an audio waveform (WAV-like format).
 *
 * Contains WAV header chunks and raw PCM samples.
 */
class Waveform
{
public:
    Waveform() = default;

    /**
     * @brief Returns RIFF chunk (modifiable).
     */
    RiffChunk& getRiffChunk();

    /**
     * @brief Returns format chunk (modifiable).
     */
    FmtChunk& getFmtChunk();

    /**
     * @brief Returns data chunk (modifiable).
     */
    DataChunk& getDataChunk();

    /**
     * @brief Returns audio samples buffer (modifiable).
     */
    SoundSamplesVec& getSoundSamplesBuffer();

    /**
     * @brief Returns RIFF chunk (const).
     */
    const RiffChunk& getRiffChunk() const;

    /**
     * @brief Returns format chunk (const).
     */
    const FmtChunk& getFmtChunk() const;

    /**
     * @brief Returns data chunk (const).
     */
    const DataChunk& getDataChunk() const;

    /**
     * @brief Returns audio samples buffer (const).
     */
    const SoundSamplesVec& getSoundSamplesBuffer() const;

private:
    RiffChunk _riffChunk{};           ///< RIFF header
    FmtChunk _fmtChunk{};             ///< format header
    DataChunk _dataChunk{};           ///< data header
    SoundSamplesVec _soundSamples{};  ///< PCM samples
};

#endif
