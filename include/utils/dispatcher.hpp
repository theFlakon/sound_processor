#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "args_parser.hpp"

enum class ProcessStatusCode
{
    okStatus = 1,
    notOkStatus = -1
};

class Dispatcher
{
public:
    Dispatcher() = default;

    ProcessStatusCode process(ArgsParser& argsParser);
};

#endif
