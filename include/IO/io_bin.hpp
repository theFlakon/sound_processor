#ifndef IO_BIN_HPP
#define IO_BIN_HPP

#include "exceptions.hpp"
#include "waveform.hpp"
#include <filesystem>
#include <fstream>
#include <string>

/**
 * @brief Base class for binary file handlers.
 *
 * Stores a file path and provides access to it for derived reader/writer
 * classes.
 */
class IOBinHandler
{
public:
    /**
     * @brief Constructs handler with a given file name.
     * @param fileName Path to the file
     */
    explicit IOBinHandler(const std::string& fileName): _file(fileName)
    {
    }

    /**
     * @brief Returns path to the associated file.
     */
    const std::filesystem::path& getFile() const
    {
        return _file;
    }

protected:
    std::filesystem::path _file;  ///< File path
};

/**
 * @brief Reads binary waveform data from file.
 *
 * Responsible for parsing binary file format into Waveform structure.
 */
class IOBinReader: public IOBinHandler
{
public:
    /**
     * @brief Constructs binary reader.
     * @param fileName Input file path
     */
    explicit IOBinReader(const std::string& fileName): IOBinHandler(fileName)
    {
    }

    /**
     * @brief Reads full waveform from file.
     * @param waveform Output waveform
     */
    void readFile(Waveform& waveform);

private:
    /**
     * @brief Reads a binary chunk from stream.
     *
     * @tparam T_ Chunk type
     * @param stream Input stream
     * @param chunk Output chunk
     * @throws FileReadException if read fails
     */
    template <typename T_> void readChunk(std::ifstream& stream, T_& chunk)
    {
        stream.read(reinterpret_cast<char*>(&chunk), sizeof(chunk));

        if(!stream)
            throw FileReadException(_file);
    }

    /**
     * @brief Reads audio samples from stream.
     *
     * @param stream Input stream
     * @param dest Destination buffer
     * @param dataChunkSize Size of data chunk in bytes
     */
    void readSamples(std::ifstream& stream, std::vector<int16_t>& dest,
                     uint32_t dataChunkSize);
};

/**
 * @brief Writes waveform data to binary file.
 *
 * Serializes Waveform structure into binary format.
 */
class IOBinWriter: public IOBinHandler
{
public:
    /**
     * @brief Constructs binary writer.
     * @param fileName Output file path
     */
    explicit IOBinWriter(const std::string& fileName): IOBinHandler(fileName)
    {
    }

    /**
     * @brief Writes waveform to file.
     * @param waveform Input waveform
     */
    void writeFile(const Waveform& waveform);

private:
    /**
     * @brief Writes a binary chunk to stream.
     *
     * @tparam T_ Chunk type
     * @param stream Output stream
     * @param chunk Data to write
     * @throws FileWriteException if write fails
     */
    template <typename T_>
    void writeChunk(std::ofstream& stream, const T_& chunk)
    {
        stream.write(reinterpret_cast<const char*>(&chunk), sizeof(chunk));
        if(!stream)
            throw FileWriteException(_file);
    }

    /**
     * @brief Writes audio samples to stream.
     *
     * @param stream Output stream
     * @param source Sample buffer
     */
    void writeSamples(std::ofstream& stream,
                      const std::vector<int16_t>& source);
};

#endif
