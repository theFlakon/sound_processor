#include "waveform.hpp"

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
