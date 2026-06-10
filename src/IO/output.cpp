#include "output.hpp"
#include <iostream>

void StdOut::printEmptyOutFileMsg()
{
    constexpr const char* EMPTY_OUT_FILE_MESSAGE = "You haven't entered the \
name of output file. Nothing has been done.";

    std::cout << EMPTY_OUT_FILE_MESSAGE << std::endl;
}

void StdOut::printHelpMsg()
{
    constexpr const char* HELP_MESSAGE =
        R"(sound_processor - WAV audio processing

USAGE:
    sound_processor [-i <input>] [-o <output>] [-f <filter> [params...]]...

OPTIONS:
    -h, --help                  show this help and exit
    -i, --input  <file>         input WAV file
    -o, --output <file>         output WAV file
    -f, --filter <name> [...]   add a filter to the pipeline;
                                may be given multiple times; filters are
                                applied in the order listed

FILTERS:
    ampl <factor>
        multiply every sample by factor (factor >= 0); values outside the
        int16 range are clamped

    normalize [peak]
        scale so the maximum absolute sample becomes the fraction peak

    silence <unit> <start> <end>
        insert silence over the interval [start, end]
        unit       time unit: sec or ms
        start      start time (start >= 0)
        end        end time (end >= start)

    timestretch <factor>
        change duration by factor using linear interpolation
        (factor > 0; >1 stretches, <1 compresses)

    lowpass <window_size>
        smooth the signal with a moving average (window_size odd, >= 1)

GENERATORS (produce the whole signal; any input is ignored):
    generator sin <frequency_hz> <duration_ms>
        sine wave of the given frequency and duration

    generator am <amplitude> <carrier_hz> <modulation_hz> <depth> <duration_ms>
        amplitude modulation (0 <= amplitude <= 1, 0 <= depth <= 1)

    generator fm <amplitude> <carrier_hz> <modulation_hz> <deviation_hz> <duration_ms>
        frequency modulation (0 <= amplitude <= 1, modulation_hz > 0)

)";

    std::cout << HELP_MESSAGE << std::endl;
}
