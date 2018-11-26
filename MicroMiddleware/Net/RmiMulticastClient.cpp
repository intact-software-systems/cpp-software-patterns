#include "MicroMiddleware/Net/RmiMulticastClient.h"
#include "MicroMiddleware/RmiInterface.h"
#include "MicroMiddleware/NetObjects/HostInformation.h"

namespace MicroMiddleware
{
// Join group?

void RmiMulticastClient::run()
{
	bool ret = connectToServer();
	if(ret == false)
		IDEBUG() << " Error connecting to " << interfaceHandle_ ;

	try
	{
		while(tcpSocket_->isValid())
		{
			bool readyRead = tcpSocket_->waitForReadyRead(5000);
			if(readyRead == false) continue;

			short methodId = BeginUnmarshal();
			switch(methodId)
			{
				case RmiInterface::AddTarget:
				{
					InterfaceHandle handle;
					ReceiveRMI(&handle);
					StartAndAddClient(handle);
					break;
				}
				case RmiInterface::RemoveTarget:
				{
					InterfaceHandle handle;
					ReceiveRMI(&handle);
					StopAndRemoveClient(handle);
					break;
				}
				default:
					RMInvocation(methodId);
					break;
			}
		}
	}
	catch(Exception ex)
	{
		IWARNING()  << " Exception : " << ex.msg() ;
	}

	IDEBUG() << " Stopping " << interfaceHandle_ ;
}

bool RmiMulticastClient::connectToServer()
{
	bool connected = IsConnected();
	if(connected == false)
	{
		try
		{
            tcpSocket_ = TcpSocket::Ptr( new TcpSocket());

			IDEBUG() << "Connecting to " << interfaceHandle_;
			connected = tcpSocket_->connectToHost(interfaceHandle_.GetHostName(),interfaceHandle_.GetPortNumber());
			//if((connected = tcpSocket_.waitForConnected(50000)) == false)
			if(connected == false)
			{
				throw Exception("Timeout! Could not connect.");
			}

			InitSocketObjects(tcpSocket_);
		}
		catch(Exception ex)
		{
			IDEBUG() << "ERROR! " << ex.msg() << " connecting to " << interfaceHandle_ ;
		}
		catch(...)
		{
			IDEBUG() << "ERROR! connecting to " << interfaceHandle_ ;
		}
	}
	return connected;
}

void RmiMulticastClient::StopMulticastClient(bool waitForTermination)
{
	MutexLocker lock(&rmiMutex_);
	runThread_ = false;

    if(tcpSocket_) tcpSocket_->close();

    rmiMonitor_.wakeAll();
	if(waitForTermination)
		wait();
}

void RmiMulticastClient::StartOrRestartMulticastClient(const InterfaceHandle &handle)
{
	MutexLocker lock(&rmiMutex_);
	if(tcpSocket_) tcpSocket_->close();
	interfaceHandle_ = handle;
	if(isRunning() == false)
	{
		runThread_ = true;
		start();
	}
	else
	{
		rmiMonitor_.wakeAll();
	}
}

void RmiMulticastClient::StartOrRestartMulticastClient()
{
	MutexLocker lock(&rmiMutex_);
	if(tcpSocket_) tcpSocket_->close();
	if(isRunning() == false)
	{
		runThread_ = true;
		start();
	}
	else
	{
		rmiMonitor_.wakeAll();
	}
}

void RmiMulticastClient::AddMulticastClients(GroupInformation &groupInfo)
{
	MutexLocker lock(&rmiMutex_);
	IWARNING()  << " Adding clients " ;
	
	for(GroupInformation::MapHostInformation::iterator it = groupInfo.GetGroupMembers().begin(), it_end = groupInfo.GetGroupMembers().end(); it != it_end; ++it)
	{
		HostInformation &hostInfo = it->second;
		IWARNING()  << hostInfo ;

		InterfaceHandle handle(hostInfo.GetHostName(), hostInfo.GetPortNumber());

		RmiClient *rmiClient = StartAndAddClient(handle);
		rmiClient->WaitConnected();
		
		IWARNING()  << " Adding as target " << interfaceHandle_ ;
		ASSERT(rmiClient->IsConnected());

		rmiClient->AddTarget(interfaceHandle_);
	}
}

}; // namespace end

