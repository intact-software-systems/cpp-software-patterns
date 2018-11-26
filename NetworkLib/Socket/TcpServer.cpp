#include"NetworkLib/Socket/TcpServer.h"
#include"NetworkLib/Socket/TcpSocket.h"
#include"NetworkLib/Utility/NetworkFunctions.h"
#include"NetworkLib/Utility/UtilityFunctions.h"

using namespace std;

namespace NetworkLib {

/**
 *  TODO: The server should keep track of every client-socket it spawns!!
 */
TcpServer::TcpServer() : AbstractSocket()
{ }

TcpServer::~TcpServer()
{
    close();
}

bool TcpServer::listen(std::string hostName, int serverPort)
{
    MutexLocker lock(&mutexLock_);

    try
    {
        if(isValid())
        {
            IWARNING() << "TCP server is already running.";
            return true;
        }

        socketDescriptor_ = NetworkFunctions::bindListen(serverPort);
        if(socketDescriptor_ <= -1)
        {
            IWARNING() << "Could not bind to " << serverPort << " on " << hostName;
            return false;
        }

        // -- success --
        portNumber_ = serverPort;
        if(hostName.empty())
        {
            hostName_ = GetLocalHostName();
        }
        else
        {
            hostName_ = hostName;
        }
    }
    catch(Exception ex)
    {
        ICRITICAL() << "\n" << ex.what();
        return false;
    }
    catch(const char* msg)
    {
        ICRITICAL() << msg;
        return false;
    }
    catch(...)
    {
        ICRITICAL() << "Could not listen to " << serverPort;
        return false;
    }

    return true;
}

void TcpServer::close()
{
    MutexLocker mutexLocker(&mutexLock_);

    if(!isValid())
    {
        IWARNING() << "TCP server is already closed.";
        return;
    }


    UtilityFunctions::ShutdownSocket(socketDescriptor_);

    char buffer[1024];
    while(recv(socketDescriptor_, buffer, sizeof(buffer), 0) > 0)
    {
        IWARNING() << ".";
    }

    UtilityFunctions::CloseSocket(socketDescriptor_);
}

NetworkLib::TcpSocket* TcpServer::acceptTcpSocket()
{
    sockaddr_in addr;
    int         clientSocket = NetworkFunctions::my_accept(socketDescriptor_, addr);
    if(clientSocket <= -1)
    {
        IDEBUG() << "ERROR! Could not accept client!";
        return NULL;
    }

    UtilityFunctions::SetSocketNonBlocking(clientSocket);

    int    clientPort = UtilityFunctions::GetPortNtohs((sockaddr*) &addr);
    string clientName = UtilityFunctions::GetAddress((sockaddr*) &addr);

    // TODO: The server should keep track of every client-socket it spawns!!

    return new NetworkLib::TcpSocket(clientSocket, clientName, clientPort);
}

std::string TcpServer::GetLocalHostName()
{
#ifdef USE_GCC
    utsname u;
    uname(&u);
    return std::string(u.nodename);
#else
    char name[1000];
    int error = gethostname(name, sizeof(name));
    return error ? "" : string(name);
#endif
}

// Use select(). Are there better alternatives?
int TcpServer::waitForIncomingConnection(int64 milliseconds)
{
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(socketDescriptor_, &fdset);

    if(milliseconds == LONG_MAX)
    {
        int err = select(socketDescriptor_ + 1, &fdset, NULL, NULL, NULL);
        if(err < 0)
        {
            ICRITICAL() << strerror(errno);
            return -1;
        }
    }
    else
    {
        struct timeval tv = {0, 0};
        Utility::ClockInitWaitTimevalMsec(tv, 1000);

        int err = select(socketDescriptor_ + 1, &fdset, NULL, NULL, &tv);
        if(err < 0)
        {
            ICRITICAL() << strerror(errno);
            return -1;
        }
    }

    if(!FD_ISSET(socketDescriptor_, &fdset))
    {
        return -1;
    }

    sockaddr_in addr;
    int         clientSocket = NetworkFunctions::my_accept(socketDescriptor_, addr); //O_NONBLOCK);
    if(clientSocket <= -1)
    {
        IWARNING() << "ERROR! Could not accept client!";
        return -1;
    }

    return clientSocket;
}

bool TcpServer::isValid()
{
    return socketDescriptor_ > INVALID_SOCKET;
}

#if 0
void TcpServer::startWindows(string hostName, int serverPort)
{
    assert(isRunning_ == false);
    try
    {
        serverPort_ = serverPort;
        hostName_ = hostName;

        serverSocket_ = NetworkFunctions::Windows::bindListen(serverPort_);
        isRunning_ = true;
    }
    catch(const char *msg)
    {
        cout << "ERROR! : " << msg << endl;
        isRunning_ = false;
    }
}


TcpSocket* TcpServer::acceptTcpSocketWindows()
{
    sockaddr_in addr;
    socklen_t addr_length = sizeof(addr);

    SOCKET clientSocket = 0;
    int error = 0;

    bool wouldBlockError = true;

    int counter = 0;
    while(isRunning_ && wouldBlockError)
    {
        clientSocket = WSAAccept(serverSocket_, (sockaddr*)&addr, &addr_length, 0, 0);

        if(clientSocket == INVALID_SOCKET)
        {
            error = WSAGetLastError();
            wouldBlockError = (error == WSAEWOULDBLOCK);
            if(wouldBlockError)
            {
                if(counter < 1000)
                    Thread::Sleep(10);
                else
                    Thread::Sleep(1000);
            }
        }
        else {
            wouldBlockError = false;
            counter = 0;
        }
        counter++;
    }

    if(!isRunning_)
        throw ("TcpServer::AcceptTcpSocket interrupted by socket close");

    if(clientSocket == INVALID_SOCKET)
    throw ("Could not accept the TCP client: ", error);

    u_long arg = 0;
    if(ioctlsocket(clientSocket, FIONBIO, &arg)!=0)
        throw ("Could not set the socket to non-blocking mode");

    int clientPort = ntohs(addr.sin_port);
    string clientName(inet_ntoa(addr.sin_addr));

    return new TcpSocket(clientSocket, clientName, clientPort);
    return NULL;
}

#endif

};


