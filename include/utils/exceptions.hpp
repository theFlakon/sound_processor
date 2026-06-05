#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>

class AppException: public std::runtime_error
{
public:
    explicit AppException(const std::string& msg): std::runtime_error(msg)
    {
    }
};

class IOException: public AppException
{
public:
    explicit IOException(const std::string& path, const std::string& reason)
        : AppException("IO error [" + path + "]: " + reason), _path(path)
    {
    }

    const std::string& path() const
    {
        return _path;
    }

private:
    std::string _path;
};

class InvalidArgsException: public IOException
{
public:
    explicit InvalidArgsException(const std::string& description)
        : IOException("Invalid flag", description)
    {
    }
};

class FileNotFoundException: public IOException
{
public:
    explicit FileNotFoundException(const std::string& path)
        : IOException(path, "file not found")
    {
    }
};

class FileReadException: public IOException
{
public:
    explicit FileReadException(const std::string& path)
        : IOException(path, "read failed")
    {
    }
};

#endif
