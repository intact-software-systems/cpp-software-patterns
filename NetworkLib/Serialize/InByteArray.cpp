#include"NetworkLib/Serialize/InByteArray.h"

namespace NetworkLib
{
/* -------------------------------------------------------------
    class InByteArray
 --------------------------------------------------------------*/
InByteArray::InByteArray(const char *data, const int &sz)
    : byteArray_(data)
    , currentPosition_(0)
    , currentSize_(sz)
{

}

InByteArray::InByteArray(const ByteArray &other)
    : byteArray_(NULL)
    , currentPosition_(0)
    , currentSize_(0)
{
    expandAndCopyArray(other.GetData(), other.GetLength());
}

InByteArray::~InByteArray()
{
    Clear();
}

InByteArray& InByteArray::operator=(const InByteArray &other)
{
    if(this != &other)
    {
        //clearArray();
        Clear();
        expandAndCopyArray(other.GetData(), other.GetLength());
    }
    return *this;
}

InByteArray& InByteArray::operator+=(const InByteArray &other)
{
    if(this != &other)
    {
        expandAndCopyArray(other.GetData(), other.GetLength());
    }
    return *this;
}

/* -------------------------------------------------------------
    Special functions for InByteArray
 --------------------------------------------------------------*/
void InByteArray::CopyData(const char *data, const int &sz)
{
    ASSERT(sz > 0);
    ASSERT(data != NULL);

    MutexLocker lock(&readMutex_);
    expandAndCopyArray(data, sz);
}

bool InByteArray::SetReadPosition(int pos)
{
    ASSERT(pos >= 0);

    MutexLocker lock(&readMutex_);
    if(pos > currentSize_) return false;

    currentPosition_ = pos;
    return true;
}

int64_t InByteArray::GetBytesRead() const
{
    MutexLocker lock(&readMutex_);
    return (int64_t)currentPosition_;
}

/* -------------------------------------------------------------
    virtual functions from SerializeReader
 --------------------------------------------------------------*/
void InByteArray::Read(char *bytes, const int &length)
{
    MutexLocker lock(&readMutex_);
    readData((char*)bytes, length);
}

char InByteArray::ReadInt8()
{
    char data = 0;
    Read((char*)&data, sizeof(data));
    return data;
}

short InByteArray::ReadInt16()
{
    short data = 0;
    Read((char*)&data, sizeof(data));
    return data;
}

int32_t InByteArray::ReadInt32()
{
    int32_t data = 0;
    Read((char*)&data, sizeof(data));
    return data;
}

int64_t InByteArray::ReadInt64()
{
    int64_t data = 0;
    Read((char*)&data, sizeof(data));
    return data;
}

double InByteArray::ReadDouble()
{
    double data = 0.0;
    Read((char*)&data, sizeof(data));
    return data;
}

std::string InByteArray::ReadString()
{
    int32_t length = ReadInt32();
    if(length <= 0) return std::string("");

    char *bytes = new char[length + 1];

    Read(bytes, length);

    bytes[length] = 0;
    std::string str(bytes);
    delete [] bytes;
    return str;
}

/*int InByteArray::GetLength() const
{
    MutexLocker lock(&readMutex_);
    return currentSize_;
}*/

bool InByteArray::IsBuffered() const
{
    return true;
}

char* InByteArray::GetData() const
{
    MutexLocker lock(&readMutex_);
    printArray();
    return (char*)byteArray_;
}

int InByteArray::GetLength() const
{
    MutexLocker lock(&readMutex_);
    return currentSize_;
}

bool InByteArray::IsEmpty()	const
{
    MutexLocker lock(&readMutex_);
    return (currentPosition_ == 0);
}

bool InByteArray::SetPosition(int pos)
{
    MutexLocker lock(&readMutex_);

    if(currentSize_ < pos)
    {
        IWARNING() << "Position beyond end of buffer!";
        return false;
    }

    this->currentPosition_ = pos;

    return false;
}

void InByteArray::Clear()
{
    MutexLocker lock(&readMutex_);
    clearArray();
}

// private non-thread safe functions
void InByteArray::printArray() const
{
    std::cerr << PRETTY_FUNCTION << " byte array(" << currentPosition_ << "):" ;
    for(int i = 0; i < currentPosition_; i++)
        std::cerr << byteArray_[i] << ",";
    std::cerr << std::endl;
}

int InByteArray::readData(char *bytes, const int &length)
{
    // -- start debug --
    ASSERT(bytes);
    ASSERT(length > 0);
    // -- end debug --

    int currentAvailable = currentSize_ - currentPosition_;

    // -- start debug --
    if(currentAvailable < length)
    {
        std::stringstream str;
        str << TSPRETTY_FUNCTION << "ERROR!: " << currentAvailable << " < " << length << std::endl;
        std::cerr << str.str() << std::endl;
        throw Exception(str.str());
    }
    if(byteArray_ == NULL)
    {
        std::stringstream str;
        str << TSPRETTY_FUNCTION << "WARNING! Empty array!" << std::endl;
        std::cerr << str.str() << std::endl;
        throw Exception(str.str());
    }
    // -- end debug --

    for(int i = 0; i < length; i++, currentPosition_++)
        bytes[i] = byteArray_[currentPosition_];

    return length;
}

void InByteArray::expandAndCopyArray(const char *data, const int &sz)
{
    ASSERT(sz > 0);
    ASSERT(currentSize_ >= 0);

    int newSize = currentSize_ + sz;

    char *newByteArray = new char[newSize];
    if(newByteArray == NULL)
    {
        throw BaseLib::FatalException("NetworkLib::InByteArray::expandAndCopyArray(data, sz): Failed to create array! Out of memory!");
    }

    if(byteArray_ != NULL)
    {
        // -- debug --
        IDEBUG() << "Expanding array size from " << currentSize_ << " to size : " << newSize << ". Buffer's current position " << currentPosition_;
        // -- debug --

        memcpy(newByteArray, byteArray_, currentSize_);
        delete [] byteArray_;
    }

    memcpy(newByteArray + currentSize_, data, sz);

    // -- debug --
    IDEBUG() << " Copying new array " << data << " to buffer ";
    printArray();
    // -- end debug --

    byteArray_ = newByteArray;
    currentSize_ = newSize;
}

void InByteArray::clearArray()
{
    if(byteArray_ != NULL)
        delete [] byteArray_;

    byteArray_ = NULL;
    currentSize_ = 0;
    currentPosition_ = 0;
}

} // namespace NetworkLib


