#ifndef NetworkLib_InByteArray_h_IsIncluded
#define NetworkLib_InByteArray_h_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Serialize/SerializeBase.h"
#include"NetworkLib/Serialize/ByteArray.h"
#include"NetworkLib/Export.h"
namespace NetworkLib
{

// TODO: use std::vector<char> instead? Use the RingBuffer?
class DLL_STATE InByteArray : public SerializeReader
{
public:
    InByteArray(const char *data, const int &sz);
    InByteArray(const ByteArray &other);
    virtual ~InByteArray();

    CLASS_TRAITS(InByteArray)

public:
    InByteArray& operator=(const InByteArray &other);
    InByteArray& operator+=(const InByteArray &other);

    void CopyData(const char *data, const int &sz);
    bool SetReadPosition(int pos);

public:
    virtual void 	    Read(char *bytes, const int &length);
    virtual char 	    ReadInt8();
    virtual short 	    ReadInt16();
    virtual int32_t	    ReadInt32();
    virtual int64_t     ReadInt64();
    virtual double 	    ReadDouble();
    virtual std::string ReadString();

    virtual bool	IsBuffered() const;

public:
    virtual int 	GetLength() const;
    virtual int64_t GetBytesRead() const;
    virtual bool 	IsEmpty()	const;

    virtual bool    SetPosition(int pos);
    virtual void 	Clear();

public:
    char* 	GetData() const;

private:
    void 	printArray() const;
    int 	readData(char *bytes, const int &length);
    void 	clearArray();
    void 	expandAndCopyArray(const char *data, const int &sz);

private:
    mutable Mutex	readMutex_;
    const char 		*byteArray_;
    int	  			currentPosition_;
    int   			currentSize_;
};

} // namespace NetworkLib

#endif //




