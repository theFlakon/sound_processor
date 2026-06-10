#include "exceptions.hpp"
#include "io_bin.hpp"
#include "waveform.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <limits>
#include <vector>

namespace {

uint32_t tag(const char (&s)[5])
{
    uint32_t v{};
    std::memcpy(&v, s, 4);
    return v;
}

struct TempFile
{
    std::filesystem::path path;

    explicit TempFile(const std::string& name = "test_io_bin_tmp.wav")
        : path(std::filesystem::temp_directory_path() / name)
    {
    }

    ~TempFile()
    {
        std::error_code ec;
        std::filesystem::remove(path, ec);
    }

    std::string str() const
    {
        return path.string();
    }
};

Waveform makeAndWriteWaveform(const TempFile& tmp, uint16_t channels = 1,
                              uint32_t sampleRate = 44100,
                              uint16_t bitsPerSample = 16,
                              const std::vector<int16_t>& samples = {
                                  0x0001, 0x0002, 0x7FFF, -1})
{
    Waveform wf;

    auto dataBytes = static_cast<uint32_t>(samples.size() * sizeof(int16_t));

    // --- RiffChunk ---
    RiffChunk& riff = wf.getRiffChunk();
    riff.sign = tag("RIFF");
    riff.waveid = tag("WAVE");
    riff.size = 4 + 24 + 8 + dataBytes;

    // --- FmtChunk ---
    FmtChunk& fmt = wf.getFmtChunk();
    fmt.sign = tag("fmt ");
    fmt.size = 16;
    fmt.compression = 1;
    fmt.channels = channels;
    fmt.sampleRate = sampleRate;
    fmt.bitsPerSample = bitsPerSample;
    fmt.bytesPerSecond = sampleRate * channels * (bitsPerSample / 8);
    fmt.blockAlign = static_cast<uint16_t>(channels * (bitsPerSample / 8));

    // --- DataChunk ---
    DataChunk& data = wf.getDataChunk();
    data.sign = tag("data");
    data.size = dataBytes;

    // --- Samples ---
    wf.getSoundSamplesBuffer() = samples;

    IOBinWriter writer(tmp.str());
    writer.writeFile(wf);

    return wf;
}

// ─────────────────────────────────────────────
// TEST CASES
// ─────────────────────────────────────────────

TEST_CASE("IOBinHandler stores and returns file path", "[handler]")  // NOLINT
{
    IOBinReader reader("/some/path/file.wav");
    REQUIRE(reader.getFile() == std::filesystem::path("/some/path/file.wav"));
}

TEST_CASE("Round-trip: written file is read back correctly",
          "[roundtrip]")  // NOLINT
{
    TempFile tmp;
    const std::vector<int16_t> samples = {100, -200, 32767, -32768, 0};
    Waveform original = makeAndWriteWaveform(tmp, 1, 44100, 16, samples);

    Waveform loaded;
    IOBinReader reader(tmp.str());
    reader.readFile(loaded);

    SECTION("RIFF chunk")  // NOLINT
    {
        const RiffChunk& orig = original.getRiffChunk();
        const RiffChunk& read = loaded.getRiffChunk();
        REQUIRE(orig.sign == read.sign);
        REQUIRE(orig.waveid == read.waveid);
        REQUIRE(orig.size == read.size);
    }

    SECTION("FmtChunk fields")  // NOLINT
    {
        const FmtChunk& of = original.getFmtChunk();
        const FmtChunk& rf = loaded.getFmtChunk();
        REQUIRE(rf.compression == of.compression);
        REQUIRE(rf.channels == of.channels);
        REQUIRE(rf.sampleRate == of.sampleRate);
        REQUIRE(rf.bitsPerSample == of.bitsPerSample);
        REQUIRE(rf.bytesPerSecond == of.bytesPerSecond);
        REQUIRE(rf.blockAlign == of.blockAlign);
    }

    SECTION("DataChunk size field")  // NOLINT
    {
        REQUIRE(loaded.getDataChunk().size == original.getDataChunk().size);
    }

    SECTION("Samples are identical")  // NOLINT
    {
        REQUIRE(loaded.getSoundSamplesBuffer() == samples);
    }
}

TEST_CASE("Round-trip: empty samples buffer", "[roundtrip][edge]")  // NOLINT
{
    TempFile tmp("test_empty.wav");
    makeAndWriteWaveform(tmp, 1, 44100, 16, {});

    Waveform loaded;
    IOBinReader(tmp.str()).readFile(loaded);

    REQUIRE(loaded.getSoundSamplesBuffer().empty());
    REQUIRE(loaded.getDataChunk().size == 0);
}

TEST_CASE("Round-trip: single sample", "[roundtrip][edge]")  // NOLINT
{
    TempFile tmp("test_single.wav");
    makeAndWriteWaveform(tmp, 1, 8000, 16, {42});

    Waveform loaded;
    IOBinReader(tmp.str()).readFile(loaded);

    REQUIRE(loaded.getSoundSamplesBuffer().size() == 1);
    REQUIRE(loaded.getSoundSamplesBuffer()[0] == 42);
}

TEST_CASE("Round-trip: boundary sample values preserved",
          "[roundtrip][edge]")  // NOLINT
{
    TempFile tmp("test_bounds.wav");
    std::vector<int16_t> boundary = {std::numeric_limits<int16_t>::min(),
                                     std::numeric_limits<int16_t>::max(), 0};
    makeAndWriteWaveform(tmp, 1, 44100, 16, boundary);

    Waveform loaded;
    IOBinReader(tmp.str()).readFile(loaded);

    REQUIRE(loaded.getSoundSamplesBuffer() == boundary);
}

TEST_CASE("Writer overwrites existing file", "[writer]")  // NOLINT
{
    TempFile tmp("test_overwrite.wav");

    std::vector<int16_t> first = {1, 2, 3};
    makeAndWriteWaveform(tmp, 1, 44100, 16, first);

    std::vector<int16_t> second = {9, 8, 7, 6, 5};
    makeAndWriteWaveform(tmp, 1, 44100, 16, second);

    Waveform loaded;
    IOBinReader(tmp.str()).readFile(loaded);

    REQUIRE(loaded.getSoundSamplesBuffer() == second);
}

TEST_CASE("Reader throws on missing file", "[reader][error]")  // NOLINT
{
    Waveform wf;
    IOBinReader reader("/nonexistent/path/file.wav");
    REQUIRE_THROWS_AS(reader.readFile(wf), FileReadException);
}

TEST_CASE("Writer throws on inaccessible path", "[writer][error]")  // NOLINT
{
    Waveform wf;
    IOBinWriter writer("/nonexistent_dir/file.wav");
    REQUIRE_THROWS_AS(writer.writeFile(wf), FileWriteException);
}

TEST_CASE("IOBinWriter getFile returns correct path", "[handler]")  // NOLINT
{
    std::string path = "/tmp/some_output.wav";
    IOBinWriter writer(path);
    REQUIRE(writer.getFile() == std::filesystem::path(path));
}

TEST_CASE("Written file size matches expected", "[writer]")  // NOLINT
{
    TempFile tmp("test_size.wav");
    std::vector<int16_t> samples = {1, 2, 3, 4};
    makeAndWriteWaveform(tmp, 1, 44100, 16, samples);

    uintmax_t fileSize = std::filesystem::file_size(tmp.path);

    uintmax_t expected =
        sizeof(RiffChunk) + sizeof(FmtChunk) + sizeof(DataChunk) +
        static_cast<uintmax_t>(samples.size() * sizeof(int16_t));

    REQUIRE(fileSize == expected);
}
}  // namespace
