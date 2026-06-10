#include "filters.hpp"
#include "waveform.hpp"
#include <cstring>

static uint32_t placeTag(const char* str);

RiffChunk& Waveform::getRiffChunk()
{
    return _riffChunk;
}

FmtChunk& Waveform::getFmtChunk()
{
    return _fmtChunk;
}

DataChunk& Waveform::getDataChunk()
{
    return _dataChunk;
}

std::vector<int16_t>& Waveform::getSoundSamplesBuffer()
{
    return _soundSamples;
}

const RiffChunk& Waveform::getRiffChunk() const
{
    return _riffChunk;
}

const FmtChunk& Waveform::getFmtChunk() const
{
    return _fmtChunk;
}

const DataChunk& Waveform::getDataChunk() const
{
    return _dataChunk;
}

const SoundSamplesVec& Waveform::getSoundSamplesBuffer() const
{
    return _soundSamples;
}

std::unique_ptr<Waveform> makeDefaultTone()
{
    std::unique_ptr<Waveform> sound = std::make_unique<Waveform>();

    sound->getFmtChunk().sampleRate = 44100;

    SinGenerator(440.0, 1000.0).apply(*sound);

    RiffChunk& riff = sound->getRiffChunk();
    FmtChunk& fmt = sound->getFmtChunk();
    DataChunk& data = sound->getDataChunk();

    riff.sign = placeTag("RIFF");
    riff.waveid = placeTag("WAVE");
    fmt.sign = placeTag("fmt ");
    fmt.size = 16;
    data.sign = placeTag("data");

    return sound;
}

uint32_t placeTag(const char* str)
{
    uint32_t result{};
    std::memcpy(&result, str, 4);

    return result;
}
