#ifndef OUTPUT_HPP
#define OUTPUT_HPP

enum class ErrorType
{
    invalidArgsError = 1,
    fileError = 2,
};

void printErrorMsg(ErrorType errType, const char* msg);

class OutputHandler
{
public:
    OutputHandler() = default;

    void printHelpMsg();
};

#endif
