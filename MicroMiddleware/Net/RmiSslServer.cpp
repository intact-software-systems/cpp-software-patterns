#ifdef SSL_SUPPORT

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiSslServer.h"
#include"MicroMiddleware/Net/RmiServerListener.h"
#include"MicroMiddleware/RmiInterface.h"

#if 0
#include <QSslCertificate>
#include <QSslKey>
#include <QFile>
#include <QSslConfiguration>
#endif

namespace MicroMiddleware
{
RmiSslServer::RmiSslServer(RmiServerListener *rmiSL, int socketDescriptor, bool autoStart) 
	: RmiBase(RmiBase::SslServerMode),
	runThread_(autoStart),
	interfaceHandle_(),
	sslSocket_(),
	socketDescriptor_(socketDescriptor),
	rmiListener_(rmiSL)
{
	if(autoStart) start();
}

RmiSslServer::~RmiSslServer()
{
	MutexLocker locker(&serverMutex_);
	
	if(rmiListener_) rmiListener_->RemoveServer(socketDescriptor_);
	
	if(sslSocket_)
	{
		sslSocket_->close();
	}
}

#if 1
void RmiSslServer::initRmiSslServer()
{
	ASSERT(!sslSocket_);

	sslSocket_ = SslSocket::Ptr(new SslSocket());
	bool ret = sslSocket_->setSocketDescriptor(socketDescriptor_);
	if(ret == false)
	{
		throw Exception("Invalid socket given to SslServer");
	}

	interfaceHandle_ = InterfaceHandle(sslSocket_->GetHostName(), sslSocket_->GetPortNumber());
	
	string certificateFile("/home/knuthelv/ProjectLocker/MyPrograms/trunk/server.crt");
	sslSocket_->setLocalCertificate(certificateFile);	
	
	string keyFile("/home/knuthelv/ProjectLocker/MyPrograms/trunk/server.key");
	sslSocket_->setPrivateKey(keyFile);
	
	bool connected = sslSocket_->startServerEncryption();
	if(connected == false)
	{
		throw("ERROR! connecting!");
	}
	InitSocketObjects(sslSocket_);
}

#else
void RmiSslServer::initRmiSslServer()
{
	ASSERT(sslSocket_ == NULL);
	sslSocket_ = new SslSocket();
	bool ret = sslSocket_->setSocketDescriptor(socketDescriptor_);
	if(ret == false)
	{
		throw Exception("Invalid socket given to SslServer");
	}

	interfaceHandle_ = InterfaceHandle(sslSocket_->GetHostName(), sslSocket_->GetPortNumber());
	
	//sslSocket_->ignoreSslErrors();
	QFile *crtFile = new QFile("/home/knuthelv/ProjectLocker/MyPrograms/trunk/server.crt");
	crtFile->open(QIODevice::ReadOnly);
	//crtFile->open("/home/knuthelv/ProjectLocker/MyPrograms/trunk/server.crt", QIODevice::ReadOnly);
	ASSERT(crtFile->isOpen());
	QSslCertificate sslCert(crtFile);
	ASSERT(sslCert.isValid());
	ASSERT(sslCert.isNull() == false);
	sslSocket_->setLocalCertificate(sslCert);
	
	QFile *keyFile = new QFile("/home/knuthelv/ProjectLocker/MyPrograms/trunk/server.key");
	keyFile->open(QIODevice::ReadOnly);
	ASSERT(keyFile->isOpen());
	QSslKey sslKey(keyFile, QSsl::Rsa);
	ASSERT(sslKey.isNull() == false);
	sslSocket_->setPrivateKey(sslKey);
	
	// Notice the platform dependency here; the location of the CA
	// certificate bundle is specific to the OS.
	//sslSocket_->addCaCertificates("/etc/ssl/certs");	
	
	QSslConfiguration config = sslSocket_->sslConfiguration();
	ASSERT(config.isNull() == false);

	sslSocket_->startServerEncryption();
	bool connected = sslSocket_->waitForEncrypted();
	if(connected == false)
	{
		throw("ERROR! connecting!");
	}
	InitSocketObjects((AbstractSocket*)sslSocket_);
}
#endif

// client monitor wait on event -> socket closed, or runThread == false!
// wait for incoming RMI calls
// read type of RMI and 
void RmiSslServer::run()
{
	try
	{
		initRmiSslServer();

		ASSERT(sslSocket_->isEncrypted());
		
		IWARNING()  << " connected to " << interfaceHandle_ << endl;
		
		while(runThread_)
		{
			int methodId = BeginUnmarshal();
			//IDEBUG() << " Read method id : " << methodId << endl;
			
			switch(methodId)
			{
				// RmiInterface::ReleaseInterface:
				// break;
				default:
					InvokeRMI(methodId);
					break;
			}
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << " Exception : " << ex.msg() << endl;
	}
	catch(const char *str)
	{
		IDEBUG() << " Exception : " << str << endl;
	}
	catch(std::runtime_error err)
	{
		IDEBUG() << " Exception : " << err.what() << endl;
	}
	catch(...)
	{
		IDEBUG() << " Unknown exception! " << endl;
	}

	rmiListener_->RemoveServer(socketDescriptor_);

	//deleteLater();
}

void RmiSslServer::InvokeRMI(int methodId)
{
	try
	{
		ASSERT(rmiListener_);
		rmiListener_->RMInvocation(methodId, (RmiBase*)this);
	}
	catch(Exception ex)
	{
		IDEBUG() << "Exception! " << ex.msg() << endl;
		BeginMarshal(RmiInterface::ReturnException);
		GetSocketWriter()->WriteString("Exception message");
		EndMarshal();	
	}
	catch(...)
	{
		IDEBUG() << "Exception! " << endl;
		BeginMarshal(RmiInterface::ReturnException);
		GetSocketWriter()->WriteString("Exception message");
		EndMarshal();	
	}
}

void RmiSslServer::StartOrRestartServer(const InterfaceHandle &interfaceHandle)
{
	MutexLocker lock(&serverMutex_);
	
	interfaceHandle_ = interfaceHandle;
	startOrRestartServer();
}

void RmiSslServer::StopServer(bool waitForTermination)
{
	lock(); 
		try
		{
			runThread_ = false;
			if(sslSocket_) sslSocket_->close();
			rmiMonitor_.wakeAll();
		}
		catch(...)
		{
			IDEBUG() << " Exception caught!" << endl;
		}
	unlock();

	if(waitForTermination)
		wait();
}

void RmiSslServer::startOrRestartServer()
{
	if(isRunning())
	{
		runThread_ = false;
		if(sslSocket_) sslSocket_->close();
		wait();
	}

	runThread_ = true;
	start();
}

}

#endif
