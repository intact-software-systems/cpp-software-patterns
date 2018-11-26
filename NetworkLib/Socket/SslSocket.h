#ifdef SSL_SUPPORT

#pragma once

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Utility/SslNetworkFunctions.h"
#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/Export.h"

namespace NetworkLib 
{

class DLL_STATE SslSocket : public AbstractSocket
{
public:
	SslSocket();
    SslSocket(int sock, std::string hostName, int port);
	~SslSocket();
	
    CLASS_TRAITS(SslSocket)

    virtual int Write(const char* bytes, const int &length);
	virtual int Read(char *bytes, const int &length);

public:
    bool connectToHostEncrypted(const std::string &hostName, const int &port);
	virtual void close();

	bool isEncrypted();

public:
	// server functions
	bool startServerEncryption();
	bool setLocalCertificate(std::string certificateFile);
	bool setPrivateKey(std::string keyFile);

private:
	void reset();

private:
	Mutex 		mutexLock_;
  	SSL_CTX* 	sslctx_;
  	SSL*     	ssl_;
  	X509*    	serverCertificate_;
  	SSL_METHOD* sslMethod_;
	bool		isEncrypted_;

	std::string keyFile_;
	std::string certificateFile_;
};

} // namespace NetworkLib

#endif
