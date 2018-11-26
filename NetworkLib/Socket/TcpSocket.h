#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/Export.h"

namespace NetworkLib {

class DLL_STATE TcpSocket : public AbstractSocket
{
public:
    TcpSocket();
    TcpSocket(int clientSocket);
    TcpSocket(int clientSocket, const std::string& hostName, int port);

    virtual ~TcpSocket();

    CLASS_TRAITS(TcpSocket)

    bool connectToHost(const std::string& hostName, const int& port);

    friend std::ostream& operator<<(std::ostream& ostr, const TcpSocket::Ptr& socket)
    {
        if(!socket)
        {
            ostr << "NULL";
            return ostr;
        }

        ostr << "TcpSocket(fd: " << socket->socketDescriptor_ << ", port:" << socket->portNumber_ << " host:" << socket->hostName_ << ")";
        return ostr;
    }

private:
    Mutex mutexLock_;
};

}

