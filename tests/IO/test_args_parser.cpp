#include "args_parser.hpp"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <string>
#include <vector>

namespace {

class Argv
{
public:
    explicit Argv(std::vector<std::string> args): _storage(std::move(args))
    {
        _ptrs.reserve(_storage.size() + 1);

        for(auto& str: _storage)
            _ptrs.push_back(str.data());

        _ptrs.push_back(nullptr);
    }
    int argc()
    {
        return static_cast<int>(_storage.size());
    }
    char** argv()
    {
        return _ptrs.data();
    }

private:
    std::vector<std::string> _storage;
    std::vector<char*> _ptrs;
};

void resetGetopt()
{
    optind = 0;
}

void runParse(ArgsParser& parser, std::vector<std::string> args)
{
    Argv arguments(std::move(args));

    resetGetopt();

    parser.parse(arguments.argc(), arguments.argv());
}

class TempFile
{
public:
    explicit TempFile(const std::string& name)
        : _path(std::filesystem::temp_directory_path() / name)
    {
        std::ofstream(_path) << "x";
    }
    ~TempFile()
    {
        std::error_code ec;
        std::filesystem::remove(_path, ec);
    }
    const std::filesystem::path& path() const
    {
        return _path;
    }

private:
    std::filesystem::path _path;
};

// ─────────────────────────────────────────────
// TEST CASES
// ─────────────────────────────────────────────

TEST_CASE("parse: только имя программы -> запрос помощи",
          "[args][help]")  // NOLINT
{
    ArgsParser parser;
    runParse(parser, {"program"});
    REQUIRE(parser.getHelpMsgStatus());
}

TEST_CASE("parse: --help выставляет статус помощи", "[args][help]")  // NOLINT
{
    ArgsParser parser;
    runParse(parser, {"program", "--help"});
    REQUIRE(parser.getHelpMsgStatus());
}

TEST_CASE("parse: -h выставляет статус помощи", "[args][help]")  // NOLINT
{
    ArgsParser parser;
    runParse(parser, {"program", "-h"});
    REQUIRE(parser.getHelpMsgStatus());
}

TEST_CASE("parse: -h прерывает разбор раньше проверки файла",
          "[args][help]")  // NOLINT
{
    ArgsParser parser;
    REQUIRE_NOTHROW(runParse(parser, {"program", "-h", "-i", "/no/such/file"}));
    REQUIRE(parser.getHelpMsgStatus());
}

TEST_CASE("parse: -o задаёт выходной файл (без обращения к ФС)",
          "[args][io]")  // NOLINT
{
    ArgsParser parser;
    runParse(parser, {"program", "-o", "result.wav"});
    REQUIRE(parser.getOutputFile() == std::filesystem::path("result.wav"));
    REQUIRE_FALSE(parser.getHelpMsgStatus());
}

TEST_CASE("parse: --output длинная форма", "[args][io]")  // NOLINT
{
    ArgsParser parser;
    runParse(parser, {"program", "--output", "out.wav"});
    REQUIRE(parser.getOutputFile() == std::filesystem::path("out.wav"));
}

TEST_CASE("parse: -i существующий файл принимается",
          "[args][io][fs]")  // NOLINT
{
    TempFile in("sp_args_input.wav");
    ArgsParser parser;
    REQUIRE_NOTHROW(runParse(
        parser, {"program", "-i", in.path().string(), "-o", "out.wav"}));
    REQUIRE(parser.getInputFile() == in.path());
    REQUIRE(parser.getOutputFile() == std::filesystem::path("out.wav"));
}

TEST_CASE("parse: -i несуществующий файл -> FileNotFoundException",  // NOLINT
          "[args][io][fs]")
{
    ArgsParser parser;
    REQUIRE_THROWS_AS(
        runParse(parser, {"program", "-i", "/definitely/missing/xyz.wav"}),
        FileNotFoundException);
}

TEST_CASE("parse: неизвестный флаг -> InvalidArgsException",
          "[args][error]")  // NOLINT
{
    ArgsParser parser;
    REQUIRE_THROWS_AS(runParse(parser, {"program", "-x"}),
                      InvalidArgsException);
}

TEST_CASE("parse: -i без аргумента -> InvalidArgsException",
          "[args][error]")  // NOLINT
{
    ArgsParser parser;
    REQUIRE_THROWS_AS(runParse(parser, {"program", "-i"}),
                      InvalidArgsException);
}

TEST_CASE("parse: корректный фильтр не бросает", "[args][filter]")  // NOLINT
{
    ArgsParser parser;
    REQUIRE_NOTHROW(runParse(parser, {"program", "-f", "normalize", "0.8"}));
}

TEST_CASE(
    "parse: normalize без параметров допустим (peak по умолчанию)",  // NOLINT
    "[args][filter]")
{
    ArgsParser parser;
    REQUIRE_NOTHROW(runParse(parser, {"program", "-f", "normalize"}));
}

TEST_CASE("parse: несколько фильтров подряд", "[args][filter]")  // NOLINT
{
    ArgsParser parser;
    REQUIRE_NOTHROW(runParse(
        parser, {"program", "-f", "normalize", "0.8", "-f", "ampl", "2"}));
}

TEST_CASE("parse: неизвестный фильтр -> AppException",
          "[args][filter][error]")  // NOLINT
{
    ArgsParser parser;
    REQUIRE_THROWS_AS(runParse(parser, {"program", "-f", "bogus"}),
                      AppException);
}

TEST_CASE("parse: неверное число параметров фильтра -> AppException",  // NOLINT
          "[args][filter][error]")
{
    ArgsParser parser;
    REQUIRE_THROWS_AS(runParse(parser, {"program", "-f", "ampl"}),
                      AppException);
}

TEST_CASE(
    "parse: недопустимое значение параметра -> FilterParamError",  // NOLINT
    "[args][filter][error]")
{
    ArgsParser parser;
    REQUIRE_THROWS_AS(runParse(parser, {"program", "-f", "normalize", "5"}),
                      FilterParamError);
}

TEST_CASE("converter: имя + один параметр, проверка сдвига индекса",  // NOLINT
          "[converter]")
{
    Argv a({"normalize", "0.8", "-o", "out"});
    int idx = 0;
    CmdLineArgs2PipelineConverter conv;
    conv.setFilterDescriptor(a.argc(), a.argv(), idx);
    REQUIRE(idx == 2);
}

TEST_CASE(
    "converter: отрицательное число считается параметром, не флагом",  // NOLINT
    "[converter]")
{
    Argv a({"ampl", "-0.5", "-o", "out"});
    int idx = 0;
    CmdLineArgs2PipelineConverter conv;
    conv.setFilterDescriptor(a.argc(), a.argv(), idx);
    REQUIRE(idx == 2);
}

TEST_CASE(
    "converter: фильтр без параметров останавливается на флаге",  // NOLINT
    "[converter]")
{
    Argv a({"normalize", "-i", "in.wav"});
    int idx = 0;
    CmdLineArgs2PipelineConverter conv;
    conv.setFilterDescriptor(a.argc(), a.argv(), idx);
    REQUIRE(idx == 1);
}

TEST_CASE("converter: несколько параметров до следующего -f",
          "[converter]")  // NOLINT
{
    Argv a({"silence", "ms", "0", "500", "-f", "lowpass"});
    int idx = 0;
    CmdLineArgs2PipelineConverter conv;
    conv.setFilterDescriptor(a.argc(), a.argv(), idx);
    REQUIRE(idx == 4);
}

TEST_CASE("converter: addFilter2PipeLine без дескриптора — no-op",  // NOLINT
          "[converter]")
{
    PipeLine pipeline;
    CmdLineArgs2PipelineConverter conv;
    REQUIRE_NOTHROW(conv.addFilter2PipeLine(pipeline));
}
}  // namespace
