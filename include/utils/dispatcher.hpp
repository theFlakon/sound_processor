#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "general.hpp"

enum class ProcessStatusCode
{
    okStatus = 1,
    notOkStatus = -1
};

class Dispatcher
{
public:
    Dispatcher() = default;

    ProcessStatusCode process(ProgramOptions options);
};

#endif
