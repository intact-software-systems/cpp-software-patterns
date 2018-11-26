#include "NetworkLib/Socket/MulticastSocketStream.h"

namespace NetworkLib
{

MulticastSocketStream::MulticastSocketStream()
    : isClosed_(false)
{
}

MulticastSocketStream::~MulticastSocketStream()
{

}

int MulticastSocketStream::Read(char* bytes, const int &length)
{
	MutexLocker lock(&readMutex_);

    if(isClosed_) return -1;

	return readDefault(bytes, length);
}

int MulticastSocketStream::ReadDatagram(char* bytes, const int &length, std::string &hostAddress, int &port)
{
	MutexLocker lock(&readMutex_);

    if(isClosed_) return -1;

	return readDefault(bytes, length);
}

// TODO: What if socket is non-blocking?
// then read will return immediately when there is nothing to read!
int MulticastSocketStream::readDefault(char* bytes, const int &length)
{
	//ASSERT(abstractSocket_->isValid());
	//if(abstractSocket_->bytesAvailable() < length && length >= 0)
	//	abstractSocket_->waitForReadyRead(1000);

	/*int bytesAvailable = 0;
	while(bytesAvailable < length && length > 0)
	{
		bytesAvailable = abstractSocket_->bytesAvailable();
		if(bytesAvailable <= -1) return -1;

		if(bytesAvailable < length)
		{
			if(abstractSocket_->isValid() == false) return -1;

			abstractSocket_->waitForReadyRead(1000);
		}
	}*/

	//cout << "." ;
	return abstractSocket_->Read(bytes, length);
}

int MulticastSocketStream::readDefault(char* bytes, const int &length, std::string &hostAddress, int &port)
{
	if(abstractSocket_->bytesAvailable() < length && length > 0)
		abstractSocket_->waitForReadyRead(1000);

	return abstractSocket_->ReadDatagram(bytes, length, hostAddress, port);
}

int MulticastSocketStream::Write(const char* bytes, const int &length)
{
	MutexLocker lock(&writeMutex_);

    return writeDefault(bytes, length);
}

int MulticastSocketStream::WriteDatagram(const char* bytes, const int &length, const std::string &hostAddress, const int &port)
{
	MutexLocker lock(&writeMutex_);

	return writeDefault(bytes, length);
}

int MulticastSocketStream::writeDefault(const char* bytes, const int &length)
{
	return abstractSocket_->Write(bytes, length);
}

int MulticastSocketStream::writeDefault(const char* bytes, const int &length, const std::string &hostAddress, const int &port)
{
	return abstractSocket_->WriteDatagram(bytes, length, hostAddress, port);
}

int64 MulticastSocketStream::Available() const
{
	MutexLocker lock(&mutexLock_);
    return abstractSocket_->bytesAvailable();
}

bool MulticastSocketStream::IsConnected() const
{
	MutexLocker lock(&mutexLock_);
    return abstractSocket_->isValid();
}

bool MulticastSocketStream::IsBound() const
{
	MutexLocker lock(&mutexLock_);

	int ret = (int)abstractSocket_->state();

	switch(ret)
	{
		case 0:
		case 1:
		case 2:
		//case 3:
		//case 5:
		case 6:
		{
			IWARNING() << "Socket not bound! ret = " << ret;
			return false;
		}
		case 3:
		case 4:
		case 5:
		{
			return true;
		}
		default:
		{
			ICRITICAL() << "ERROR! Could not identify socket state!";
			break;
		}
	}

	return false;
}

void MulticastSocketStream::Close()
{
	MutexLocker lock(&mutexLock_);
	abstractSocket_->close();
	isClosed_ = true;
}

bool MulticastSocketStream::Flush()
{
	MutexLocker lock(&mutexLock_);
	return abstractSocket_->flush();
}

int MulticastSocketStream::socketDescriptor() const
{
	MutexLocker lock(&mutexLock_);
	return abstractSocket_->socketDescriptor();
}

AbstractSocket::SocketType MulticastSocketStream::GetSocketType() const
{
	MutexLocker lock(&mutexLock_);
	return abstractSocket_->socketType();
}

}
