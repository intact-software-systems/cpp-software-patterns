#ifndef NetworkLib_MulticastSocketStream_h_IsIncluded
#define NetworkLib_MulticastSocketStream_h_IsIncluded

#include "NetworkLib/IncludeExtLibs.h"
#include "NetworkLib/Socket/AbstractSocket.h"
#include "NetworkLib/Socket/IOBase.h"
#include "NetworkLib/Export.h"

namespace NetworkLib
{

// TODO: This fits better with multicast write, reading is more difficult?
class DLL_STATE MulticastSocketStream : public IOWriter, public IOReader
{
public:
    MulticastSocketStream();
	virtual ~MulticastSocketStream();

    CLASS_TRAITS(MulticastSocketStream)

public:
    virtual int Read(char* bytes, const int &length);
	virtual int ReadDatagram(char* bytes, const int &length, std::string &hostAddress, int &port);

	virtual int Write(const char* bytes, const int &length);
	virtual int WriteDatagram(const char* bytes, const int &length, const std::string &hostAddress, const int &port);

	virtual void Close();
	virtual bool Flush();

	int64 Available() const;
    bool IsConnected() const;
	bool IsBound() const;

	int socketDescriptor() const;
	AbstractSocket::SocketType GetSocketType() const;

private:
	int readDefault(char *bytes, const int &length);
	int writeDefault(const char* bytes, const int &length);

	int readDefault(char *bytes, const int &length, std::string &hostAddress, int &port);
	int writeDefault(const char *bytes, const int &length, const std::string &hostAddress, const int &port);

private:
	bool 					isClosed_;
	AbstractSocket          *abstractSocket_;
	mutable Mutex			mutexLock_;
	mutable Mutex			readMutex_;
	mutable Mutex			writeMutex_;
};

}

#endif
