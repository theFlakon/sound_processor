#include "filters.hpp"
#include "waveform.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <cstdint>
#include <vector>

namespace {

Waveform makeWaveform(std::vector<int16_t> samples, uint32_t sampleRate = 44100,
                      uint16_t channels = 1, uint16_t bitsPerSample = 16)
{
    Waveform wf;

    FmtChunk& fmt = wf.getFmtChunk();
    fmt.compression = 1;
    fmt.channels = channels;
    fmt.sampleRate = sampleRate;
    fmt.bitsPerSample = bitsPerSample;
    fmt.blockAlign = static_cast<uint16_t>(channels * (bitsPerSample / 8));
    fmt.bytesPerSecond = sampleRate * fmt.blockAlign;

    wf.getDataChunk().size =
        static_cast<uint32_t>(samples.size() * sizeof(int16_t));

    wf.getSoundSamplesBuffer() = std::move(samples);
    return wf;
}

int maxAbs(const std::vector<int16_t>& v)
{
    int m = 0;
    for(int16_t s: v)
        m = std::max(m, std::abs(static_cast<int>(s)));
    return m;
}

}  // namespace

// ─────────────────────────────────────────────
// TEST CASES
// ─────────────────────────────────────────────

TEST_CASE("AmplFilter: умножение на коэффициент", "[ampl]")
{
    Waveform wf = makeWaveform({100, -50, 0});
    AmplFilter f{2.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{200, -100, 0});
}

TEST_CASE("AmplFilter: коэффициент 0 обнуляет сигнал", "[ampl]")
{
    Waveform wf = makeWaveform({100, -50, 12345});
    AmplFilter f{0.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{0, 0, 0});
}

TEST_CASE("AmplFilter: насыщение на верхней и нижней границах", "[ampl]")
{
    Waveform wf = makeWaveform({20000, -20000});
    AmplFilter f{2.0};
    f.apply(wf);
    // 40000 -> 32767, -40000 -> -32768
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{32767, -32768});
}

TEST_CASE("AmplFilter: пустой сигнал не меняется", "[ampl][edge]")
{
    Waveform wf = makeWaveform({});
    AmplFilter f{3.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer().empty());
}

TEST_CASE("NormalizeFilter: peak=1.0 -> максимум по модулю == 32767",
          "[normalize]")
{
    Waveform wf = makeWaveform({3000, -6000, 1500});
    NormalizeFilter f{1.0};
    f.apply(wf);
    REQUIRE(maxAbs(wf.getSoundSamplesBuffer()) == 32767);
}

TEST_CASE("NormalizeFilter: знаки сохраняются", "[normalize]")
{
    Waveform wf = makeWaveform({1000, -2000});
    NormalizeFilter f{1.0};
    f.apply(wf);
    const auto& s = wf.getSoundSamplesBuffer();
    REQUIRE(s[0] > 0);
    REQUIRE(s[1] < 0);
}

TEST_CASE("NormalizeFilter: peak=0.5 уменьшает максимум вдвое", "[normalize]")
{
    Waveform wf = makeWaveform({3000, -6000});
    NormalizeFilter f{0.5};
    f.apply(wf);
    const int peak = maxAbs(wf.getSoundSamplesBuffer());
    REQUIRE(peak >= 16383);
    REQUIRE(peak <= 16384);
}

TEST_CASE("NormalizeFilter: полностью нулевой сигнал не меняется",
          "[normalize][edge]")
{
    Waveform wf = makeWaveform({0, 0, 0});
    NormalizeFilter f{1.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{0, 0, 0});
}

TEST_CASE("NormalizeFilter: пустой сигнал не меняется", "[normalize][edge]")
{
    Waveform wf = makeWaveform({});
    NormalizeFilter f{1.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer().empty());
}

TEST_CASE("SilenceFilter: вставка тишины в середину (sec)", "[silence]")
{
    Waveform wf = makeWaveform({1, 2, 3, 4}, 1000);
    SilenceFilter f{"sec", 0.002, 0.004};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() ==
            std::vector<int16_t>{1, 2, 0, 0, 3, 4});
}

TEST_CASE("SilenceFilter: обновление размеров в заголовке", "[silence]")
{
    Waveform wf = makeWaveform({1, 2, 3, 4}, 1000);

    uint32_t oldData = wf.getDataChunk().size;
    uint32_t oldRiff = wf.getRiffChunk().size;

    SilenceFilter f{"sec", 0.002, 0.004};
    f.apply(wf);
    REQUIRE(wf.getDataChunk().size == oldData + 4);
    REQUIRE(wf.getRiffChunk().size == oldRiff + 4);
}

TEST_CASE("SilenceFilter: единицы ms", "[silence]")
{
    Waveform wf = makeWaveform({1, 2, 3, 4}, 1000);
    SilenceFilter f{"ms", 0.0, 2.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() ==
            std::vector<int16_t>{0, 0, 1, 2, 3, 4});
}

TEST_CASE("SilenceFilter: нулевая длина (start == end) -> без изменений",
          "[silence][edge]")
{
    Waveform wf = makeWaveform({1, 2, 3, 4}, 1000);
    SilenceFilter f{"sec", 0.002, 0.002};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{1, 2, 3, 4});
}

TEST_CASE("SilenceFilter: точка вставки за концом -> дописывается в конец",
          "[silence][edge]")
{
    Waveform wf = makeWaveform({1, 2, 3, 4}, 1000);
    SilenceFilter f{"sec", 0.010, 0.012};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() ==
            std::vector<int16_t>{1, 2, 3, 4, 0, 0});
}

TEST_CASE("TimestretchFilter: factor=2 удлиняет с интерполяцией",
          "[timestretch]")
{
    Waveform wf = makeWaveform({0, 100});
    TimestretchFilter f{2.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() ==
            std::vector<int16_t>{0, 50, 100, 100});
}

TEST_CASE("TimestretchFilter: factor=0.5 укорачивает", "[timestretch]")
{
    Waveform wf = makeWaveform({0, 50, 100, 150});
    TimestretchFilter f{0.5};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{0, 100});
}

TEST_CASE("TimestretchFilter: размер заголовка пересчитан", "[timestretch]")
{
    Waveform wf = makeWaveform({0, 100}, 1000);
    TimestretchFilter f{2.0};
    f.apply(wf);
    REQUIRE(wf.getDataChunk().size == 8);
}

TEST_CASE("TimestretchFilter: пустой сигнал не меняется", "[timestretch][edge]")
{
    Waveform wf = makeWaveform({});
    TimestretchFilter f{2.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer().empty());
}

TEST_CASE("LowpassFilter: окно 3 усредняет соседей", "[lowpass]")  // NOLINT
{
    Waveform wf = makeWaveform({0, 30, 0});
    LowpassFilter f{3};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{10, 10, 10});
}

TEST_CASE("LowpassFilter: постоянный сигнал не искажается",
          "[lowpass]")  // NOLINT
{
    Waveform wf = makeWaveform({100, 100, 100, 100, 100});
    LowpassFilter f{3};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() ==
            std::vector<int16_t>{100, 100, 100, 100, 100});
}

TEST_CASE("LowpassFilter: окно 1 не меняет сигнал",
          "[lowpass][edge]")  // NOLINT
{
    Waveform wf = makeWaveform({1, 2, 3});
    LowpassFilter f{1};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{1, 2, 3});
}

TEST_CASE("LowpassFilter: число отсчётов не меняется",
          "[lowpass]")  // NOLINT
{
    Waveform wf = makeWaveform({5, 10, 15, 20});
    LowpassFilter f{3};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer().size() == 4);
}

TEST_CASE("LowpassFilter: пустой сигнал не меняется",
          "[lowpass][edge]")  // NOLINT
{
    Waveform wf = makeWaveform({});
    LowpassFilter f{3};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer().empty());
}

TEST_CASE(
    "SinGenerator: число отсчётов = duration_ms/1000 * sampleRate",  // NOLINT
    "[sin][generator]")
{
    Waveform wf = makeWaveform({}, 1000);
    SinGenerator f{440.0, 10.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer().size() == 10);
}

TEST_CASE(
    "SinGenerator: заголовок настроен (моно, 16 бит, размер data)",  // NOLINT
    "[sin][generator]")
{
    Waveform wf = makeWaveform({}, 1000);
    SinGenerator f{440.0, 10.0};
    f.apply(wf);
    REQUIRE(wf.getFmtChunk().channels == 1);
    REQUIRE(wf.getFmtChunk().bitsPerSample == 16);
    REQUIRE(wf.getDataChunk().size == 10 * sizeof(int16_t));
}

TEST_CASE("SinGenerator: частота 0 -> нулевой сигнал",
          "[sin][generator]")  // NOLINT
{
    Waveform wf = makeWaveform({}, 1000);
    SinGenerator f{0.0, 5.0};
    f.apply(wf);
    const auto& s = wf.getSoundSamplesBuffer();
    REQUIRE(s.size() == 5);
    REQUIRE(std::all_of(s.begin(), s.end(), [](int16_t v) { return v == 0; }));
}

TEST_CASE("SinGenerator: первый отсчёт нулевой (sin(0)=0)",
          "[sin][generator]")  // NOLINT
{
    Waveform wf = makeWaveform({}, 8000);
    SinGenerator f{440.0, 100.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer().front() == 0);
}

TEST_CASE("SinGenerator: игнорирует входной сигнал",
          "[sin][generator]")  // NOLINT
{
    Waveform wf = makeWaveform({111, 222, 333, 444, 555}, 1000);
    SinGenerator f{0.0, 3.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{0, 0, 0});
}

TEST_CASE("AmGenerator: amplitude 0 -> нулевой сигнал нужной длины",  // NOLINT
          "[am][generator]")
{
    Waveform wf = makeWaveform({}, 1000);
    AmGenerator f{0.0, 440.0, 5.0, 0.5, 10.0};
    f.apply(wf);
    const auto& s = wf.getSoundSamplesBuffer();
    REQUIRE(s.size() == 10);
    REQUIRE(std::all_of(s.begin(), s.end(), [](int16_t v) { return v == 0; }));
}

TEST_CASE("AmGenerator: заголовок и размер data",
          "[am][generator]")  // NOLINT
{
    Waveform wf = makeWaveform({}, 2000);
    AmGenerator f{0.5, 440.0, 5.0, 0.5, 10.0};
    f.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer().size() == 20);
    REQUIRE(wf.getDataChunk().size == 20 * sizeof(int16_t));
}

TEST_CASE("FmGenerator: amplitude 0 -> нулевой сигнал нужной длины",  // NOLINT
          "[fm][generator]")
{
    Waveform wf = makeWaveform({}, 1000);
    FmGenerator f{0.0, 440.0, 5.0, 50.0, 10.0};
    f.apply(wf);
    const auto& s = wf.getSoundSamplesBuffer();
    REQUIRE(s.size() == 10);
    REQUIRE(std::all_of(s.begin(), s.end(), [](int16_t v) { return v == 0; }));
}

TEST_CASE("FmGenerator: значения остаются в диапазоне int16",
          "[fm][generator]")  // NOLINT
{
    Waveform wf = makeWaveform({}, 4000);
    FmGenerator f{1.0, 440.0, 5.0, 200.0, 20.0};
    f.apply(wf);
    const auto& s = wf.getSoundSamplesBuffer();
    REQUIRE_FALSE(s.empty());
    REQUIRE(maxAbs(s) <= 32767);
}
