#ifndef ERR_OUTPUT_HPP
#define ERR_OUTPUT_HPP

#include "args_parser.hpp"
#include "io_bin.hpp"
#include <iostream>

struct ErrCodeProcessBase
{
    inline static const char* UNKNOWN_ERR_TYPE_MSG =  // NOLINT
        "Unknown error type";

    static const char* getUnknownErrTypeMsg()
    {
        return UNKNOWN_ERR_TYPE_MSG;
    }
};

template <typename T_> struct ErrCodeProcess: ErrCodeProcessBase
{
    const char* selectErrMsg(T_ errCode);
};

template <> struct ErrCodeProcess<IOBinStatusCode>: ErrCodeProcessBase
{
    const char* selectErrMsg(IOBinStatusCode errCode)
    {
        static const char* INVALID_INPUT_FILE_NAME =  // NOLINT
            "Invalid input file name";
        static const char* READING_ERROR =  // NOLINT
            "Something went wrong during the file reading";

        switch(errCode)
        {
        case IOBinStatusCode::invalidInputFileName:
            return INVALID_INPUT_FILE_NAME;

        case IOBinStatusCode::readError:
            return READING_ERROR;

        default:
            return getUnknownErrTypeMsg();
        }
    };
};

template <> struct ErrCodeProcess<ParseStatusCode>: ErrCodeProcessBase
{
    const char* selectErrMsg(ParseStatusCode errCode)
    {
        switch(errCode)
        {
        case ParseStatusCode::invalidArgs:
            return "Invalid arguments! ...";
        default:
            return getUnknownErrTypeMsg();
        }
    }
};

template <typename T_> class ErrOutputHandler
{
public:
    ErrOutputHandler(T_ errCode)
    {
        setErrCode(errCode);
    }

    ErrOutputHandler(const char* errMsg): _errMsg(errMsg)
    {
    }

    void printErrMsg()
    {
        std::cerr << getErrMsg() << std::endl;
    }

    const T_ getErrCode() const
    {
        return _errCode;
    }

    void setErrCode(T_ errCode)
    {
        _errCode = errCode;

        ErrCodeProcess<T_> processor{};

        const char* msg = processor.selectErrMsg(errCode);

        setErrMsg(msg);
    }

    const char* getErrMsg() const
    {
        return _errMsg;
    }

    void setErrMsg(const char* msg)
    {
        _errMsg = msg;
    }

private:
    T_ _errCode{};
    const char* _errMsg{};
};

#endif
