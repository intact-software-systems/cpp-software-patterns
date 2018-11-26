#include"NetworkLib/Socket/TcpSocket.h"
#include"NetworkLib/Utility/NetworkFunctions.h"

namespace NetworkLib {

TcpSocket::TcpSocket() : AbstractSocket()
{
}

TcpSocket::TcpSocket(int clientSocket, const std::string& hostName, int port)
    : AbstractSocket(clientSocket, hostName, port)
{
}

TcpSocket::TcpSocket(int clientSocket) : AbstractSocket(clientSocket)
{

}

TcpSocket::~TcpSocket(void)
{
    close();
}

bool TcpSocket::connectToHost(const std::string& hostName, const int& port)
{
    MutexLocker locker(&mutexLock_);

    if(isValid())
    {
        IWARNING() << "Socket already opened!";
        return true;
    }

    try
    {
        socketDescriptor_ = NetworkFunctions::connect(hostName.c_str(), port);
        if(socketDescriptor_ == -1)
        {
            throw BaseLib::Exception("Failed to open socket!");
        }

        hostName_   = hostName;
        portNumber_ = port;
    }
    catch(BaseLib::Exception ex)
    {
        //IWARNING() << " Failed to open socket!" << ex.what();
        return false;
    }
    catch(...)
    {
        //IWARNING() << " Failed to open socket!";
        return false;
    }

    return true;
}

#if 0
void TcpSocket::connectWindows(const string &hostName, const int &port)
{
    try
    {
        clientSocket_= NetworkFunctions::Windows::connect(hostName.c_str(), port);
        if(clientSocket_ == -1)
        {
            throw("Failed to open socket!");
        }
        else
        {
            //networkStream_ = new NetworkStream(clientSocket_);
            isOpen_ = true;
        }
    }
    catch(...)
    {
        isOpen_ = false;
        close();
    }
}
#endif

}; // namespaces end ..


