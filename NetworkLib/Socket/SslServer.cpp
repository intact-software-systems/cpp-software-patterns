#ifdef SSL_SUPPORT

#include"NetworkLib/Socket/SslServer.h"
#include"NetworkLib/Socket/SslSocket.h"
#include"NetworkLib/Utility/UtilityFunctions.h"

#include<fcntl.h>
#include<errno.h>
#include<assert.h>

#ifdef USE_GCC
#include<sys/utsname.h>
#endif

using namespace std;

namespace NetworkLib
{

SslServer::SslServer()
    : AbstractSocket(AbstractSocket::SslServerMode)
    , sslctx_(0)
    , ssl_(0)
    , serverCertificate_(0)
    , sslMethod_(0)
{
    // TODO
}

SslServer::~SslServer()
{
    close();
}

void SslServer::reset()
{
    // TODO: Make sure they are freed!
    sslctx_ 			= 0;
    ssl_ 				= 0;
    serverCertificate_  = 0;
    sslMethod_ 			= 0;
}


int SslServer::Write(const char* bytes, const int &length)
{
    int ret = SslNetworkFunctions::mySSLwrite(ssl_, bytes, length);
    if(ret <= -1)
        IWARNING()  << "ERROR! Could not write to socket! ret: " << ret << endl;
    return ret;
}

int SslServer::Read(char *bytes, const int &length)
{
    int ret = SslNetworkFunctions::mySSLread(ssl_, bytes, length);
    if(ret <= -1)
        IWARNING()  << "ERROR! Could not read from socket! ret: " << ret << endl;
    return ret;
}

bool SslServer::listen(string hostName, int serverPort)
{
    MutexLocker mutexLocker(&mutexLock_);

    if(isValid() == true)
    {
        IWARNING()  << "TCP server is already running." << endl;
        return true;
    }

    if(sslctx_ == NULL)
        sslctx_ = SslNetworkFunctions::CreateCTX();

    if(sslctx_ == NULL)
    {
        IWARNING()  << "ERROR! Could not create SSL_CTX!" << endl;
        return false;
    }

    socketDescriptor_ = NetworkFunctions::bindListen(serverPort);
    if(socketDescriptor_ <= -1)
    {
        IWARNING()  << "ERROR! Could not bind to " << serverPort << " on " << hostName << endl;
        return false;
    }

    // -- success --
    portNumber_ = serverPort;
    hostName_ = hostName;

    return true;
}


void SslServer::close()
{
    MutexLocker mutexLocker(&mutexLock_);

    if(isValid() == false)
    {
        IWARNING()  << "TCP server is already closed." << endl;
        return;
    }

    UtilityFunctions::ShutdownSocket(socketDescriptor_);

    char buffer[1024];
    while(recv(socketDescriptor_, buffer, sizeof(buffer), 0) > 0)
    {
        IWARNING() << ".";
    }

    UtilityFunctions::CloseSocket(socketDescriptor_);

    if(ssl_ == 0) return;

    SslNetworkFunctions::closeSSL(ssl_, socketDescriptor_);

#ifdef MACX
    ICRITICAL() << "Double check the freeing of ssl!";
    delete sslctx_;
#else
    SSL_CTX_free(sslctx_);
#endif

    reset();
}

SslSocket* SslServer::acceptSslSocket()
{
    MutexLocker mutexLocker(&mutexLock_);

    //if(isValid() == true)
    //{
    //	IWARNING()  << "WARNING! Socket already opened!" << endl;
    //	return NULL;
    //}
    //sslctx_ = SslNetworkFunctions::CreateCTX();

    ASSERT(sslctx_);
    if(sslctx_ == NULL)
    {
        IWARNING()  << "ERROR! Could not create SSL_CTX!" << endl;
        return NULL;
    }

    sockaddr_in addr;
    int clientSocket= SslNetworkFunctions::acceptSSLFull(ssl_, sslctx_, socketDescriptor_, addr); //O_BLOCKING);
    if(clientSocket <= -1)
    {
        IWARNING()  << "ERROR! Could not accept client! " << endl;
        return NULL;
    }

    int clientPort = UtilityFunctions::GetPortNtohs((sockaddr*)&addr);
    string clientName = UtilityFunctions::GetAddress((sockaddr*)&addr);

    return new SslSocket(clientSocket, clientName, clientPort);
}

}

#endif
