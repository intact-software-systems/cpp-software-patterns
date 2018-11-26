#ifdef SSL_SUPPORT

#include "NetworkLib/Socket/SslSocket.h"
#include <assert.h>

#include <fcntl.h>
#include <errno.h>
#ifdef USE_GCC
#include <sys/utsname.h>
#endif

namespace NetworkLib
{

SslSocket::SslSocket()
    : AbstractSocket()
    , sslctx_(0)
    , ssl_(0)
    , serverCertificate_(0)
    , sslMethod_(0)
    , isEncrypted_(false)
{
    // TODO
}

SslSocket::SslSocket(int sock, string hostName, int port)
    : AbstractSocket(sock, hostName, port)
    , sslctx_(0)
    , ssl_(0)
    , serverCertificate_(0)
    , sslMethod_(0)
    , isEncrypted_(false)
{
    IDEBUG() << "ERROR! SslSocket is not encrypted or initialized properly using this constructor!" << endl;
}

SslSocket::~SslSocket()
{
    close();
}

void SslSocket::reset()
{
    // TODO: Make sure they are freed!
    sslctx_ 			= 0;
    ssl_ 				= 0;
    serverCertificate_  = 0;
    sslMethod_ 			= 0;
}

int SslSocket::Write(const char* bytes, const int &length)
{
    int ret = SslNetworkFunctions::mySSLwrite(ssl_, bytes, length);
    if(ret <= -1)
        IWARNING()  << "ERROR! Could not write to socket! ret: " << ret << endl;
    return ret;
}

int SslSocket::Read(char *bytes, const int &length)
{
    int ret = SslNetworkFunctions::mySSLread(ssl_, bytes, length);
    if(ret <= -1)
        IWARNING()  << "ERROR! Could not read from socket! ret: " << ret << endl;
    return ret;
}

bool SslSocket::connectToHostEncrypted(const string &hostName, const int &port)
{
    MutexLocker mutexLocker(&mutexLock_);

    if(isValid() == true)
    {
        IWARNING()  << "WARNING! Socket already opened!" << endl;
        return true;
    }

    sslctx_ = SslNetworkFunctions::CreateCTX();
    if(sslctx_ == NULL)
    {
        IWARNING()  << "ERROR! Could not create SSL_CTX!" << endl;
        return false;
    }

    socketDescriptor_ = SslNetworkFunctions::connectSSL(ssl_, sslctx_, hostName.c_str(), port);
    if(socketDescriptor_ <= -1)
    {
        IWARNING()  << "ERROR! Could not connect to " << hostName << "," << port << endl;
        return false;
    }

    // -- success --
    this->hostName_ 	= hostName;
    this->portNumber_ 	= port;
    socketMode_ 		= AbstractSocket::SslClientMode;
    isEncrypted_ 		= true;

    return isEncrypted_;
}

bool SslSocket::startServerEncryption()
{
    MutexLocker lock(&mutexLock_);

    if(isValid() == false)
    {
        IWARNING()  << "WARNING! Socket already closed!" << endl;
        return false;
    }

    ASSERT(sslctx_ == NULL);
    sslctx_ = SslNetworkFunctions::CreateCTX();

    // -- debug --
    if(sslctx_ == NULL)
    {
        IWARNING()  << "ERROR! Could not create SSL_CTX!" << endl;
        return false;
    }
    else if(certificateFile_.empty())
    {
        IWARNING()  << "ERROR! certificate file is empty!" << endl;
        return false;
    }
    else if(keyFile_.empty())
    {
        IWARNING()  << "ERROR! key file is empty!" << endl;
        return false;
    }
    // -- debug --

    SslNetworkFunctions::LoadCertificates(sslctx_, certificateFile_.c_str(), keyFile_.c_str());

    int ret = SslNetworkFunctions::acceptSSLInit(ssl_, sslctx_, socketDescriptor_);
    if(ret <= -1)
    {
        IWARNING()  << "ERROR! Could not start server encryption on socket " << socketDescriptor_ << endl;
        return false;
    }

    // -- success --
    //portNumber_ 		= serverPort;
    //hostName_ 			= hostName;
    socketMode_ 		= AbstractSocket::SslServerMode;
    isEncrypted_ 		= true;

    return true;
}

void SslSocket::close()
{
    MutexLocker lock(&mutexLock_);

    if(isValid() == false)
    {
        IWARNING()  << "WARNING! Socket already closed!" << endl;
        return;
    }
    if(ssl_ == 0) return;

    SslNetworkFunctions::closeSSL(ssl_, socketDescriptor_);

    ASSERT(sslctx_);
    // TODO: What does this do?

#ifdef MACX
    ICRITICAL() << "Double check the freeing of ssl!";
    delete sslctx_;
#else
    SSL_CTX_free(sslctx_);
#endif

    reset();
}

bool SslSocket::isEncrypted()
{
    MutexLocker lock(&mutexLock_);
    return isEncrypted_;
}


bool SslSocket::setLocalCertificate(std::string certificateFile)
{
    MutexLocker lock(&mutexLock_);

    if(isEncrypted_ == true)
    {
        ASSERT(isValid());
        IWARNING()  << "WARNING! Socket already encrypted!" << endl;
        return false;
    }
    certificateFile_ = certificateFile;

    return true;
}

bool SslSocket::setPrivateKey(std::string keyFile)
{
    MutexLocker lock(&mutexLock_);

    if(isEncrypted_ == true)
    {
        ASSERT(isValid());
        IWARNING()  << "WARNING! Socket already encrypted!" << endl;
        return false;
    }

    keyFile_ = keyFile;
    return true;
}

}

#endif
