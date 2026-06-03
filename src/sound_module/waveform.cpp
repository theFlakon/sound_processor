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
