#include "filters.hpp"
#include "waveform.hpp"
#include <algorithm>
#include <limits>

void AmplFilter::apply(Waveform& sound)
{
    std::vector<int16_t>& samplesVec = sound.getSoundSamplesBuffer();

    for(auto& sample: samplesVec)
    {
        float scaled = sample * static_cast<float>(_factor);
        sample = static_cast<int16_t>(std::clamp(
            scaled, static_cast<float>(std::numeric_limits<int16_t>::min()),
            static_cast<float>(std::numeric_limits<int16_t>::max())));
    }

    return;
}
