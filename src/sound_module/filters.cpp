#include "filters.hpp"
#include "utils_templates.hpp"
#include "waveform.hpp"
#include <cmath>
#include <cstddef>

static void setupWaveform(Waveform& sound, uint32_t numSamples);

void AmplFilter::apply(Waveform& sound) const
{
    std::vector<int16_t>& samplesVec = sound.getSoundSamplesBuffer();

    if(samplesVec.empty())
        return;

    for(auto& sample: samplesVec)
    {
        float scaled = sample * static_cast<float>(_factor);
        sample = static_cast<int16_t>(
            std::clamp(scaled, getInt16Min<float>(), getInt16Max<float>()));
    }

    return;
}

void NormalizeFilter::apply(Waveform& sound) const
{
    SoundSamplesVec& samples = sound.getSoundSamplesBuffer();

    if(samples.empty())
        return;

    int currentPeak = 0;
    for(int16_t sample: samples)
        currentPeak = std::max(currentPeak, std::abs(static_cast<int>(sample)));

    if(currentPeak == 0)
        return;

    double scale = _peak * getInt16Max<double>() / currentPeak;

    for(int16_t& sample: samples)
    {
        double newValue = std::round(sample * scale);

        sample = static_cast<int16_t>(
            std::clamp(newValue, getInt16Min<double>(), getInt16Max<double>()));
    }
}

void SilenceFilter::apply(Waveform& sound) const
{
    SoundSamplesVec& samples = sound.getSoundSamplesBuffer();
    auto rate = static_cast<double>(sound.getFmtChunk().sampleRate);

    double start = _start;
    double end = _end;

    if(_unit == "ms")
    {
        start /= 1000.0;
        end /= 1000.0;
    }

    long startIdx = std::llround(start * rate);
    long endIdx = std::llround(end * rate);

    long len = endIdx - startIdx;

    if(len <= 0)
        return;

    size_t pos = std::min(static_cast<size_t>(startIdx), samples.size());

    samples.insert(samples.begin() + static_cast<long>(pos),
                   static_cast<std::size_t>(len), int16_t{0});

    const FmtChunk& fmt = sound.getFmtChunk();

    uint32_t bytesPerSample = fmt.bitsPerSample / 8;
    uint32_t addedBytes =
        static_cast<uint32_t>(len) * bytesPerSample * fmt.channels;

    sound.getDataChunk().size += addedBytes;
    sound.getRiffChunk().size += addedBytes;
}

void TimestretchFilter::apply(Waveform& sound) const
{
    SoundSamplesVec& samples = sound.getSoundSamplesBuffer();
    size_t oldSize = samples.size();

    if(oldSize == 0)
        return;

    auto newSize = static_cast<size_t>(
        std::llround(static_cast<double>(oldSize) * _factor));

    if(newSize == 0)
        return;

    SoundSamplesVec dst(newSize);

    for(size_t idx = 0; idx < newSize; ++idx)
    {
        double pos = static_cast<double>(idx) / _factor;

        auto leftIdx = static_cast<std::size_t>(pos);

        if(leftIdx >= oldSize)
            leftIdx = oldSize - 1;

        double frac = pos - static_cast<double>(leftIdx);

        double value;
        if(leftIdx + 1 < oldSize)
            value =
                samples[leftIdx] * (1.0 - frac) + samples[leftIdx + 1] * frac;
        else
            value = samples[leftIdx];

        dst[idx] = static_cast<int16_t>(std::llround(value));
    }

    uint32_t oldDataSize = sound.getDataChunk().size;
    samples = std::move(dst);

    const FmtChunk& fmt = sound.getFmtChunk();
    uint32_t bytesPerSample = fmt.bitsPerSample / 8;
    uint32_t newDataSize =
        static_cast<uint32_t>(newSize) * bytesPerSample * fmt.channels;

    sound.getDataChunk().size = newDataSize;
    sound.getRiffChunk().size =
        sound.getRiffChunk().size - oldDataSize + newDataSize;
}

void LowpassFilter::apply(Waveform& sound) const
{
    SoundSamplesVec& samples = sound.getSoundSamplesBuffer();
    size_t samplesSize = samples.size();

    if(samplesSize == 0 || _windowSize == 1)
        return;

    int half = _windowSize / 2;
    long len = static_cast<long>(samplesSize);

    SoundSamplesVec destVec(samplesSize);

    for(long idx = 0; idx < len; ++idx)
    {
        long sum = 0;

        for(int k = -half; k <= half; ++k)
        {
            long lidx = std::clamp(idx + k, 0L, len - 1);
            sum += samples[static_cast<std::size_t>(lidx)];
        }

        double avg = static_cast<double>(sum) / _windowSize;
        destVec[static_cast<std::size_t>(idx)] =
            static_cast<int16_t>(std::llround(avg));
    }

    samples = std::move(destVec);
}

void SinGenerator::apply(Waveform& sound) const
{
    uint32_t rate = sound.getFmtChunk().sampleRate;
    auto numSamples =
        static_cast<uint32_t>(std::llround(_durationMs / 1000.0 * rate));

    SoundSamplesVec& samples = sound.getSoundSamplesBuffer();
    samples.assign(numSamples, int16_t{0});

    for(uint32_t idx = 0; idx < numSamples; ++idx)
    {
        double tmp = static_cast<double>(idx) / rate;
        double val = getInt16Max<double>() * std::sin(2.0 * M_PI * _freq * tmp);

        samples[idx] = clampInt16(val);
    }

    setupWaveform(sound, numSamples);
}

void AmGenerator::apply(Waveform& sound) const
{
    uint32_t rate = sound.getFmtChunk().sampleRate;
    auto numSamples =
        static_cast<uint32_t>(std::llround(_durationMs / 1000.0 * rate));

    SoundSamplesVec& samples = sound.getSoundSamplesBuffer();
    samples.assign(numSamples, int16_t{0});

    for(uint32_t idx = 0; idx < numSamples; ++idx)
    {
        double tmp = static_cast<double>(idx) / rate;

        double envelope =
            1.0 + _depth * std::sin(2.0 * M_PI * _modulationHz * tmp);
        double carry = std::sin(2.0 * M_PI * _carrierHz * tmp);

        double val = _amplitude * getInt16Max<double>() * envelope * carry;

        samples[idx] = clampInt16<double>(val);
    }

    setupWaveform(sound, numSamples);
}

void FmGenerator::apply(Waveform& sound) const
{
    uint32_t rate = sound.getFmtChunk().sampleRate;
    auto numSamples =
        static_cast<uint32_t>(std::llround(_durationMs / 1000.0 * rate));

    SoundSamplesVec& samples = sound.getSoundSamplesBuffer();
    samples.assign(numSamples, int16_t{0});

    double modIndex = _modulationHz > 0.0 ? _deviationHz / _modulationHz : 0.0;

    for(uint32_t idx = 0; idx < numSamples; ++idx)
    {
        double tmp = static_cast<double>(idx) / rate;

        double phase = 2.0 * M_PI * _carrierHz * tmp +
                       modIndex * std::sin(2.0 * M_PI * _modulationHz * tmp);
        double val = _amplitude * getInt16Max<int>() * std::sin(phase);
        samples[idx] = clampInt16(val);
    }

    setupWaveform(sound, numSamples);
}

void setupWaveform(Waveform& sound, uint32_t numSamples)
{
    FmtChunk& fmt = sound.getFmtChunk();

    fmt.compression = 1;
    fmt.channels = 1;
    fmt.bitsPerSample = 16;
    fmt.blockAlign = fmt.channels * fmt.bitsPerSample / 8;
    fmt.bytesPerSecond = fmt.sampleRate * fmt.blockAlign;

    uint32_t dataSize = numSamples * fmt.blockAlign;

    sound.getDataChunk().size = dataSize;
    sound.getRiffChunk().size = 36 + dataSize;
}
