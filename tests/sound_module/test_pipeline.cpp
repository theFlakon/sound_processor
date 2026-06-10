#include "filters.hpp"
#include "pipeline.hpp"
#include "waveform.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace {

class RecordingFilter: public IFilter
{
public:
    RecordingFilter(int id, std::vector<int>* log): _id(id), _log(log)
    {
    }
    void apply(Waveform&) const override
    {
        _log->push_back(_id);
    }

private:
    int _id;
    std::vector<int>* _log;
};

class AppendFilter: public IFilter
{
public:
    explicit AppendFilter(int16_t v): _v(v)
    {
    }
    void apply(Waveform& w) const override
    {
        w.getSoundSamplesBuffer().push_back(_v);
    }

private:
    int16_t _v;
};

}  // namespace

// ─────────────────────────────────────────────
// TEST CASES
// ─────────────────────────────────────────────
TEST_CASE("PipeLine: новый пайплайн пуст", "[pipeline]")  // NOLINT
{
    PipeLine pipeline;
    REQUIRE(pipeline.getFiltersCnt() == 0);
}

TEST_CASE("PipeLine: addFilter увеличивает счётчик", "[pipeline]")  // NOLINT
{
    std::vector<int> log;
    PipeLine pipeline;

    pipeline.addFilter(std::make_unique<RecordingFilter>(1, &log));
    REQUIRE(pipeline.getFiltersCnt() == 1);

    pipeline.addFilter(std::make_unique<RecordingFilter>(2, &log));
    REQUIRE(pipeline.getFiltersCnt() == 2);
}

TEST_CASE("PipeLine: operator[] возвращает добавленный фильтр",  // NOLINT
          "[pipeline]")
{
    std::vector<int> log;
    PipeLine pipeline;
    pipeline.addFilter(std::make_unique<RecordingFilter>(42, &log));

    Waveform wf;
    pipeline[0].apply(wf);
    REQUIRE(log == std::vector<int>{42});
}

TEST_CASE("PipeLine: const operator[] доступен на const-пайплайне",  // NOLINT
          "[pipeline]")
{
    std::vector<int> log;
    PipeLine pipeline;
    pipeline.addFilter(std::make_unique<RecordingFilter>(7, &log));

    const PipeLine& cref = pipeline;
    Waveform wf;
    cref[0].apply(wf);
    REQUIRE(log == std::vector<int>{7});
}

TEST_CASE("PipeLine: apply на пустом пайплайне ничего не делает",  // NOLINT
          "[pipeline][edge]")
{
    PipeLine pipeline;
    Waveform wf;
    wf.getSoundSamplesBuffer() = {1, 2, 3};
    REQUIRE_NOTHROW(pipeline.apply(wf));
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{1, 2, 3});
}

TEST_CASE("PipeLine: apply вызывает фильтры в порядке добавления",  // NOLINT
          "[pipeline]")
{
    std::vector<int> log;
    PipeLine pipeline;
    pipeline.addFilter(std::make_unique<RecordingFilter>(1, &log));
    pipeline.addFilter(std::make_unique<RecordingFilter>(2, &log));
    pipeline.addFilter(std::make_unique<RecordingFilter>(3, &log));

    Waveform wf;
    pipeline.apply(wf);
    REQUIRE(log == std::vector<int>{1, 2, 3});
}

TEST_CASE("PipeLine: apply вызывает каждый фильтр ровно один раз",  // NOLINT
          "[pipeline]")
{
    std::vector<int> log;
    PipeLine pipeline;
    pipeline.addFilter(std::make_unique<RecordingFilter>(5, &log));

    Waveform wf;
    pipeline.apply(wf);
    REQUIRE(log.size() == 1);
}

TEST_CASE("PipeLine: каждый фильтр видит результат предыдущего",  // NOLINT
          "[pipeline]")
{
    PipeLine pipeline;
    pipeline.addFilter(std::make_unique<AppendFilter>(1));
    pipeline.addFilter(std::make_unique<AppendFilter>(2));
    pipeline.addFilter(std::make_unique<AppendFilter>(3));

    Waveform wf;
    pipeline.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{1, 2, 3});
}

TEST_CASE("PipeLine: цепочка реальных AmplFilter перемножается",  // NOLINT
          "[pipeline][integration]")
{
    PipeLine pipeline;
    pipeline.addFilter(std::make_unique<AmplFilter>(2.0));
    pipeline.addFilter(std::make_unique<AmplFilter>(3.0));

    Waveform wf;
    wf.getSoundSamplesBuffer() = {100, -50};
    pipeline.apply(wf);
    REQUIRE(wf.getSoundSamplesBuffer() == std::vector<int16_t>{600, -300});
}
