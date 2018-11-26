#ifndef NetworkLib_ByteArray_h_IsIncluded
#define NetworkLib_ByteArray_h_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Serialize/SerializeBase.h"

#include"NetworkLib/Export.h"
namespace NetworkLib
{
/*
vector of char is nice because the memory is contiguious. Therefore you can use it with a lot of C API's such as berkley sockets or file APIs. You can do the following, for example:
  std::vector<char> vect;
  ...
  send(sock, &vect[0], vect.size());

and it will work fine.

You can essentially treat it just like any other dynamically allocated char buffer. You can scan up and down looking for magic numbers or patters. You can parse it partially in place. For receiving from a socket you can very easily resize it to append more data.

The downside is resizing is not terribly efficient (resize or preallocate prudently) and deletion from the front of the array will also be very ineficient. If you need to, say, pop just one or two chars at a time off the front of the data structure very frequently, copying to a deque before this processing may be an option. This costs you a copy and deque memory isn't contiguous, so you can't just pass a pointer to a C API.

Bottom line, learn about the data structures and their tradeoffs before diving in, however vector of char is typically what I see used in general practice.
*/

// TODO: Rename to OutByteArray
class DLL_STATE ByteArray : public SerializeWriter
{
public:
    ByteArray(int sz = 512);
    ByteArray(const ByteArray &other);
    virtual ~ByteArray();

    ByteArray& operator=(const ByteArray &other);
    //ByteArray operator+(const ByteArray &other) const;
    ByteArray& operator+=(const ByteArray &other);

    CLASS_TRAITS(ByteArray)

public:
    virtual int Write(const char *bytes, const int &length);
    virtual int WriteInt8(const char &data);
    virtual int WriteInt16(const short &data);
    virtual int WriteInt32(const int32_t &data);
    virtual int WriteInt64(const int64_t &data);
    virtual int WriteDouble(const double &data);
    virtual int WriteString(const std::string &str);
    virtual bool Flush(IOWriter *writer);
    virtual bool Flush(IOWriter *writer, const std::string &hostAddress, const int &port);
    virtual int GetLength()	const;
    virtual bool IsBuffered() const;

    virtual const char* GetData() const;

public:
    int GetTotalSize()		const;

public:
    virtual bool 	IsEmpty()	const;
    virtual bool    SetPosition(int pos);

    virtual void 	Clear();
    virtual int64_t GetBytesWritten() const;

private:
    void printArray() const;
    int copyData(const char *bytes, const int &length);
    void expandArray(const int &sz);
    void clearArray();

private:
    mutable Mutex	writeMutex_;
    char 			*byteArray_;
    int	  			currentPosition_;
    int   			currentSize_;
};

} // namespace NetworkLib

#endif // NetworkLib_ByteArray_h_IsIncluded

