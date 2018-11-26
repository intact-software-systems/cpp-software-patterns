#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/Export.h"

namespace NetworkLib {

class TcpSocket;

class DLL_STATE TcpServer : public AbstractSocket
{
public:
    TcpServer();
    ~TcpServer();

    CLASS_TRAITS(TcpServer)

    bool listen(std::string hostName = std::string(""), int port = 0);
    void close();

    NetworkLib::TcpSocket* acceptTcpSocket();
    static std::string GetLocalHostName();

    int waitForIncomingConnection(int64 milliseconds = LONG_MAX);

    virtual bool isValid();

public:
    friend std::ostream& operator<<(std::ostream& ostr, const TcpServer::Ptr& socket)
    {
        if(!socket)
        {
            ostr << "NULL";
            return ostr;
        }

        ostr << "TcpServer(fd: " << socket->socketDescriptor_ << ", port:" << socket->portNumber_ << " host:" << socket->hostName_ << ")";
        return ostr;
    }

private:
    Mutex mutexLock_;
};

}

