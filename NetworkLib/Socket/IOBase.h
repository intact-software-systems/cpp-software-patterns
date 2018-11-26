#pragma once

#include "NetworkLib/IncludeExtLibs.h"
#include "NetworkLib/Export.h"

namespace NetworkLib {

/**
 * @brief The IOWriter class
 */
class DLL_STATE IOWriter
{
public:
    virtual ~IOWriter()
    {}

    CLASS_TRAITS(IOWriter)

    virtual int Write(const char* bytes, const int& length) = 0;
    virtual int WriteDatagram(const char* bytes, const int& length, const std::string& hostAddress, const int& port) = 0;
    virtual void Close() = 0;
    virtual bool Flush() = 0;
};

/**
 * @brief The IOReader class
 */
class DLL_STATE IOReader
{
public:
    virtual ~IOReader()
    {}

    CLASS_TRAITS(IOReader)

    virtual int Read(char* bytes, const int& length) = 0;
    virtual int ReadDatagram(char* bytes, const int& length, std::string& hostAddress, int& port) = 0;
    virtual void Close() = 0;
};

/**
 * @brief The IOReaderWriter class
 */
class DLL_STATE IOReaderWriter : public IOReader, public IOWriter
{
public:
    virtual ~IOReaderWriter()
    {}

    CLASS_TRAITS(IOReaderWriter)
};


/**
 * @brief The BufferWriter class
 */
template <typename T, typename Container = std::vector<T> >
class DLL_STATE BufferWriter
{
public:
    CLASS_TRAITS(BufferWriter)

    virtual bool Write(const Container&)
    { return false; }

    virtual void Clear()
    {}
};

/**
 * @brief The BufferReader class
 */
template <typename T, typename Container = std::vector<T> >
class DLL_STATE BufferReader
{
public:
    virtual ~BufferReader()
    {}

    CLASS_TRAITS(BufferReader)

    virtual Container Read()
    {
        return Container();
    }
};

}

