#ifdef SSL_SUPPORT

#pragma once

#include"NetworkLib/Utility/NetworkFunctions.h"
#include"NetworkLib/Utility/SslNetworkFunctions.h"
#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Export.h"

namespace NetworkLib 
{

class SslSocket;

class DLL_STATE SslServer : public AbstractSocket
{
public:
	SslServer();
	~SslServer();

    virtual int Write(const char* bytes, const int &length);
	virtual int Read(char *bytes, const int &length);

    bool listen(std::string hostName = std::string(""), int port = 0);
	virtual void close();

	SslSocket* 	acceptSslSocket();

private:
	void reset();

private:
    BaseLib::Mutex 	mutexLock_;
    SSL_CTX* 		sslctx_;
    SSL*     		ssl_;
    X509*    		serverCertificate_;
    SSL_METHOD* 	sslMethod_;
};

}

#endif

