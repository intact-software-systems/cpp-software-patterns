#include"NetworkLib/Serialize/ByteArray.h"
#include<stdlib.h>

namespace NetworkLib
{
/* -------------------------------------------------------------
    class ByteArray
--------------------------------------------------------------*/

ByteArray::ByteArray(int sz)
    : SerializeWriter()
    , byteArray_(NULL)
    , currentPosition_(0)
    , currentSize_(sz)
{
    ASSERT(sz >= 0);
    if(currentSize_ > 0)
        expandArray(currentSize_);
}

ByteArray::ByteArray(const ByteArray &other)
    : SerializeWriter()
    , byteArray_(NULL)
    , currentPosition_(0)
    , currentSize_(0)
{
    copyData(other.GetData(), other.GetTotalSize());
}

ByteArray::~ByteArray()
{
    Clear();
}

ByteArray& ByteArray::operator=(const ByteArray &rhs)
{
    if(this != &rhs)
    {
        Clear();
        copyData(rhs.GetData(), rhs.GetTotalSize());
    }
    return *this;
}

ByteArray& ByteArray::operator+=(const ByteArray &rhs)
{
    if(this != &rhs)
        copyData(rhs.GetData(), rhs.GetTotalSize());

    return *this;
}

/* -------------------------------------------------------------
    virtual functions from SerializeWriter
--------------------------------------------------------------*/

int ByteArray::Write(const char *bytes, const int &length)
{
    MutexLocker lock(&writeMutex_);
    return copyData((const char*)bytes, length);
}

int ByteArray::WriteInt8(const char &data)
{
    MutexLocker lock(&writeMutex_);
    return copyData((const char*)&data, sizeof(data));
}

int ByteArray::WriteInt16(const short &data)
{
    MutexLocker lock(&writeMutex_);
    return copyData((const char*)&data, sizeof(data));
}

int ByteArray::WriteInt32(const int32_t &data)
{
    MutexLocker lock(&writeMutex_);
    return copyData((const char*)&data, sizeof(data));
}

int ByteArray::WriteInt64(const int64_t &data)
{
    MutexLocker lock(&writeMutex_);
    return copyData((const char*)&data, sizeof(data));
}

int ByteArray::WriteDouble(const double &data)
{
    MutexLocker lock(&writeMutex_);
    return copyData((const char*)&data, sizeof(data));
}

int ByteArray::WriteString(const std::string &str)
{
    MutexLocker lock(&writeMutex_);
    int length = str.length();
    copyData((const char*)&length, sizeof(length));
    if(str.length() <= 0) return 0;

    return copyData(str.c_str(), (int)str.length());
}

bool ByteArray::Flush(IOWriter *writer)
{
    MutexLocker lock(&writeMutex_);

    if(currentPosition_ <= 0)
    {
        //IDEBUG() << "WARNING! Buffer is empty!" << endl;
        //clearArray();
        return true;
    }
    ASSERT(byteArray_ != NULL);
    //IDEBUG() << " flushing out array: " << endl;
    //printArray();

    int bytesWritten = writer->Write((const char*)byteArray_, currentPosition_);
    if(bytesWritten != currentPosition_)
    {
        clearArray();
        std::stringstream stream;
        stream << PRETTY_FUNCTION << "ERROR! Could not write data to socket! " << bytesWritten << " != " << currentPosition_ << std::endl;
        throw Exception(stream.str());
        return false;
    }

    clearArray();
    return true;
}

// TODO: I should flush out a predefined datagramSize!
bool ByteArray::Flush(IOWriter *writer, const std::string &hostAddress, const int &port)
{
    MutexLocker lock(&writeMutex_);

    if(currentPosition_ <= 0)
    {
        IWARNING() << "Buffer is empty!";
        //clearArray();
        return true;
    }
    ASSERT(byteArray_ != NULL);

    int bytesWritten = writer->WriteDatagram((const char*)byteArray_, currentPosition_, hostAddress, port);
    if(bytesWritten != currentPosition_)
    {
        //clearArray();
        std::stringstream stream;
        stream << TSPRETTY_FUNCTION << "ERROR! Could not write data to socket! " << bytesWritten << " != " << currentPosition_ << std::endl;
        throw Exception(stream.str());
        return false;
    }

    return true;
}

int ByteArray::GetLength() const
{
    MutexLocker lock(&writeMutex_);
    return currentPosition_;
}

int64_t ByteArray::GetBytesWritten() const
{
    MutexLocker lock(&writeMutex_);
    return (int64_t)currentPosition_;
}

bool ByteArray::IsBuffered() const
{
    return true;
}

const char* ByteArray::GetData() const
{
    MutexLocker lock(&writeMutex_);
    printArray();
    return (const char*)byteArray_;
}

int ByteArray::GetTotalSize() const
{
    MutexLocker lock(&writeMutex_);
    return currentSize_;
}

bool ByteArray::IsEmpty() const
{
    MutexLocker lock(&writeMutex_);
    return (currentPosition_ == 0);
}

bool ByteArray::SetPosition(int pos)
{
    MutexLocker lock(&writeMutex_);

    if(currentSize_ < pos)
    {
        IWARNING() << "Position beyond end of buffer!";
        return false;
    }

    this->currentPosition_ = pos;

    return false;
}


void ByteArray::Clear()
{
    MutexLocker lock(&writeMutex_);
    clearArray();
}

void ByteArray::printArray() const
{
    IDEBUG() << " byte array(" << currentPosition_ << "):" ;
    for(int i = 0; i < currentPosition_; i++)
        std::cerr << byteArray_[i] << ",";
    std::cerr << std::endl;
}

int ByteArray::copyData(const char *bytes, const int &length)
{
    ASSERT(bytes);
    ASSERT(length > 0);
    int currentAvailable = currentSize_ - currentPosition_;
    if(currentAvailable < length)
        expandArray(std::max(currentSize_ * 2, 512));

    for(int i = 0; i < length; i++, currentPosition_++)
        byteArray_[currentPosition_] = bytes[i];

    return length;
}

void ByteArray::expandArray(const int &sz)
{
    ASSERT(sz > 0);
    ASSERT(sz >= currentSize_);

    char *newByteArray = new char[sz];
    if(newByteArray == NULL)
    {
        throw BaseLib::FatalException("NetworkLib::ByteArray::expandArray(sz): Failed to create array! Out of memory!");
    }

    if(byteArray_ != NULL)
    {
        IDEBUG() << " Expanding array size from " << currentSize_ << " to size : " << sz << ". Buffer's current position " << currentPosition_;

        memcpy(newByteArray, byteArray_, currentSize_);
        delete [] byteArray_;
    }

    byteArray_ = newByteArray;
    currentSize_ = sz;
}

void ByteArray::clearArray()
{
    if(byteArray_ != NULL)
        delete [] byteArray_;

    byteArray_ = NULL;
    currentSize_ = 0;
    currentPosition_ = 0;
}

} // namespace NetworkLib

