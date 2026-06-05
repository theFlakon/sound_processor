#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include "args_parser.hpp"

class Dispatcher
{
public:
    Dispatcher() = default;

    void process(ArgsParser& argsParser);
};

#endif
