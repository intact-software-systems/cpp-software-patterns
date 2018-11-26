#ifndef NetworkLib_SocketObserver_h_IsIncluded
#define NetworkLib_SocketObserver_h_IsIncluded

#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/Export.h"

namespace NetworkLib
{

class DLL_STATE SocketAcceptedObserver
{
public:
    virtual void OnSocketAccepted(AbstractSocket::Ptr) = 0;
};

class DLL_STATE SocketDisconnectedObserver
{
public:
    virtual void OnSocketDisconnected(AbstractSocket::Ptr) = 0;
};

class DLL_STATE SocketReadyToReadObserver
{
public:
    virtual void OnSocketReadyToRead(AbstractSocket::Ptr) = 0;
};

class DLL_STATE SocketObserver
{
public:
    virtual void OnSocketConnected() = 0;
    virtual void OnSocketDisconnected() = 0;
    //virtual void error(AbstractSocket::SocketError socketError) = 0;
    virtual void OnHostFound() = 0;
    //virtual void proxyAuthenticationRequired(const QNetworkProxy & proxy, QAuthenticator * authenticator)
    virtual void OnSocketStateChanged(AbstractSocket::SocketState socketState) = 0;
};

class DLL_STATE SocketMonitorObserver
{
public:
    virtual void OnBytesDataRead(AbstractSocket::Ptr, int bytesRead) = 0;
    virtual void OnBytesDataWritten(AbstractSocket::Ptr, int bytesWritten) = 0;
};

class DLL_STATE SocketReaderObserver
{
public:
    virtual void OnDataRead(AbstractSocket::Ptr) = 0;
    virtual void OnDataReadFailure(AbstractSocket::Ptr) = 0;
};

class DLL_STATE SocketWriterObserver
{
public:
    virtual void OnDataWritten(AbstractSocket::Ptr) = 0;
    virtual void OnDataWriteFailure(AbstractSocket::Ptr) = 0;
};


//class DLL_STATE SocketReaderObserver
//{
//public:
//    virtual void OnRawDataAvailable(FlowId &flowId, std::vector<char> &byteArray) = 0;
//};

}

#endif
