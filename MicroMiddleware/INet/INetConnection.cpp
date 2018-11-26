#include "MicroMiddleware/INet/INetConnection.h"
#include "MicroMiddleware/INet/INetConnectionManager.h"
#include "MicroMiddleware/Runtime.h"
#include "MicroMiddleware/RmiInterface.h"
#include "MicroMiddleware/CommonDefines.h"

namespace MicroMiddleware
{
// -----------------------------------------------
// Initialize INetConnection
// -----------------------------------------------
INetConnection::INetConnection(SocketStream* socketStream
								, const InterfaceHandle &handle
								, ConnectionInterface::ConnectionType type
								, bool autoStart) 
	: ConnectionInterface(handle, type)
	, messageParser_(INetMessageParser::Ptr(new INetBase(socketStream, handle)))
	, runThread_(true)
	, socketDescriptor_(socketStream->socketDescriptor())
{
	init();
	
	if(autoStart) start();
}

INetConnection::INetConnection(const InterfaceHandle &handle
								, ConnectionInterface::ConnectionType type
								, bool autoStart) 
	: ConnectionInterface(handle, type)
	, messageParser_(INetMessageParser::Ptr(new INetBase(handle)))
	, runThread_(true)
	, socketDescriptor_(-1)
{
	init();

	if(autoStart) start();
}

INetConnection::~INetConnection()
{
	if(socketDescriptor_ > -1)
	{
        UtilityFunctions::CloseSocket(socketDescriptor_);
		socketDescriptor_ = -1;
	}

	mutex_.lock();
	runThread_ = false;
	mutex_.unlock();

	wait();
}

bool INetConnection::IsConnected()
{
	if(!messageParser_) return false;

	switch(GetConnectionType())
	{
		case ConnectionInterface::TcpServer:
		case ConnectionInterface::TcpClient:
			return messageParser_->IsConnected();
		case ConnectionInterface::UdpSender:
		case ConnectionInterface::UdpReceiver:
		case ConnectionInterface::MulticastSender:
		case ConnectionInterface::MulticastReceiver:
			return messageParser_->IsBound();
		default:
			IFATAL() << " Could not identify connection type!" << (int)GetConnectionType();
			break;
	}
	return messageParser_->IsConnected();
}

// -----------------------------------------------
// Initialize INetConnection
// -----------------------------------------------
void INetConnection::init()
{
	connectionInformation_ = Runtime::GetHostInformation();
	
	stringstream name;
	name << "MicroMiddleware.INetConnection." << connectionInformation_.GetComponentName() << "." << interfaceHandle_;
	connectionInformation_.SetComponentName(name.str());

	switch(GetConnectionType())
	{
		case ConnectionInterface::TcpServer:
			connectionInformation_.SetHostDescription(HostInformation::STUB);
			break;
		case ConnectionInterface::TcpClient:
			connectionInformation_.SetHostDescription(HostInformation::PROXY);
			break;
		case ConnectionInterface::UdpSender:
			connectionInformation_.SetHostDescription(HostInformation::PUBLISHER);
			break;
		case ConnectionInterface::UdpReceiver:
			connectionInformation_.SetHostDescription(HostInformation::SUBSCRIBER);
			break;
		case ConnectionInterface::MulticastSender:
			connectionInformation_.SetHostDescription(HostInformation::PUBLISHER);
			break;
		case ConnectionInterface::MulticastReceiver:
			connectionInformation_.SetHostDescription(HostInformation::SUBSCRIBER);
			break;
		default:
			IFATAL() << " Could not identify connection type!";
			break;
	}
	
	connectionInformation_.SetPortNumber(interfaceHandle_.GetPortNumber());
	if(connectionInformation_.GetHostName().empty())
		connectionInformation_.SetHostName(interfaceHandle_.GetHostName());

	if(IsConnected()) 	
		connectionInformation_.SetOnlineStatus(HostInformation::ONLINE);
	else			
		connectionInformation_.SetOnlineStatus(HostInformation::OFFLINE);
}

// -----------------------------------------------
// INetConnection should be able to reconnect if disconnected!
// -----------------------------------------------
void INetConnection::run()
{
	try
	{
		ASSERT(runThread_);
		notifyRegisteredInterfaces(IsConnected());

		while(runThread_)
		{
			if(IsConnected())
			{
				processRpc();		
				if(runThread_ == false)
					break;
			}

			ASSERT(IsConnected() == false);

			switch(GetConnectionType())
			{
				case ConnectionInterface::TcpClient:
				{
					// attempt to connect
					bool connected = connectToHost(GetInterfaceHandle());
					if(connected == false) 
					{
						// -- debug --
						IDEBUG() << "INetConnection is not connected!";
						// -- debug --

						connectionInformation_.SetOnlineStatus(HostInformation::OFFLINE);
						notifyRegisteredInterfaces(connected); 
						sleep(1);
					}
					else
					{
						// -- debug --
						ASSERT(socketDescriptor_ > -1);
						ASSERT(IsConnected() == true);
						IDEBUG() << "Reconnected adding socket " << socketDescriptor_ << " to connectionManager!";
						// -- debug --

						connectionInformation_.SetOnlineStatus(HostInformation::ONLINE);
						INetConnectionManager::GetOrCreate()->AddConnection(socketDescriptor_, this);
						notifyRegisteredInterfaces(connected);
					}
					break;
				}
				case ConnectionInterface::TcpServer:
				{
					// -- debug --
					IDEBUG() << " INetConnection is a server connection. Disconnecting!";
					//ASSERT(runThread_ == false);
					// -- debug --

					runThread_ = false;
					connectionInformation_.SetOnlineStatus(HostInformation::OFFLINE);
					notifyRegisteredInterfaces(false);
					break;
				}
				case ConnectionInterface::UdpSender:
				case ConnectionInterface::UdpReceiver:
				case ConnectionInterface::MulticastSender:
				case ConnectionInterface::MulticastReceiver:
				{
					bool connected = bindToInterfaceHandle(interfaceHandle_);
					if(connected == false) 
					{
						// -- debug --
						IDEBUG() << "INetConnection is not connected!";
						// -- debug --

						connectionInformation_.SetOnlineStatus(HostInformation::OFFLINE);
						notifyRegisteredInterfaces(connected); 
						sleep(1);
					}
					else
					{
						// -- debug --
						ASSERT(socketDescriptor_ > -1);
						ASSERT(IsConnected() == true);
						IDEBUG() << "Reconnected adding socket " << socketDescriptor_ << " to connectionManager!";
						// -- debug --

						connectionInformation_.SetOnlineStatus(HostInformation::ONLINE);
						INetConnectionManager::GetOrCreate()->AddConnection(socketDescriptor_, this);
						notifyRegisteredInterfaces(connected);
					}
					break;
				}
				default:
				    IFATAL() << "Could not identify connection type!";
					break;
			}
		}
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught " << ex.what();
	}

	INetConnectionManager::GetOrCreate()->RemoveConnection(this);
}

void INetConnection::notifyRegisteredInterfaces(bool connected)
{
	HostInformation info = connectionInformation_;

	if(connected)	info.SetOnlineStatus(HostInformation::ONLINE);
	else 			info.SetOnlineStatus(HostInformation::OFFLINE);	

	for(MapIdInterface::iterator it = mapIdInterface_.begin(), it_end = mapIdInterface_.end(); it != it_end; ++it)
	{
		INetInterface *inet = it->second;
		ASSERT(inet);

		switch(inet->GetInterfaceType())
		{
			case HostInformation::PROXY:
				{
					stringstream stream;
					stream << inet->GetServiceName() << ".proxy." << interfaceHandle_;
					cout << COMPONENT_FUNCTION << stream.str() << endl;
					
					info.SetComponentName(stream.str());
					info.SetHostDescription(HostInformation::PROXY);
					
					inet->SetConnection(info);
					break;
				}
			case HostInformation::STUB:
				{
					info.SetHostDescription(HostInformation::STUB);
					break;
				}
			case HostInformation::PUBLISHER:
				{
					stringstream stream;
					stream << inet->GetServiceName() << ".publisher." << interfaceHandle_;
					cout << COMPONENT_FUNCTION << stream.str() << endl;
					
					info.SetComponentName(stream.str());
					info.SetHostDescription(HostInformation::PUBLISHER);
					
					inet->SetConnection(info);
					break;
				}
			case HostInformation::SUBSCRIBER:
				{
					info.SetHostDescription(HostInformation::SUBSCRIBER);
					break;
				}
			default:
				IDEBUG() << "ERROR! Could not identify: " << inet->GetInterfaceType() ;
				break;
		}
	}
}

// -----------------------------------------------
// Reads remote-procedure-calls and distributes it to right
// Proxy/Stub/Publisher/Subscriber
// IDEA: Create an event loop that waits for incoming data from a socket selector
// The event loop uses call-backs to processRpc that identifies correct interface and
// further dispatches RPC to the correct interface.
// -----------------------------------------------
void INetConnection::processRpc()
{
	try
	{
		IWARNING()  << " Waiting for data on interface : " << interfaceHandle_ ;
		ASSERT(messageParser_);
		
		while(runThread_)
		{
			RpcIdentifier rpc;
			SerializeReader::Ptr reader;
			// TODO: What to do with reader
			// - use it as input argument to the functions in this class
			// - requires rewrite of INetBase, possibly proxy/stub
			bool ret = messageParser_->BeginUnmarshal(&rpc, reader);
			if(ret == false)
			{
				IDEBUG() << "WARNING! Connection lost " << interfaceHandle_ ;
				break;
			}

			// -----------------------------------------------
			// DONE: Enable keep alive messages
			// 		- NO: Create a timeout in BeginUnmarshal and send keepAlive in given intervals
			//		- Yes: Do it in a seperate thread? NO: seperate KeepAliveComponent?
			//		- DONE by INetConnectionManager!
			// DONE: How does a Proxy detect that a Stub is disconnected?
			//		- Send a reply StubDisconnected
			//		- TODO: Check if it is a TwoWay or OneWay Rpc?
			// DONE: How do I find the right position in the TCP stream?
			//		- Add RPC size in the RpcIdentifier
			//		- then, this object can fast forward over those packets
			// -----------------------------------------------
			
			ASSERT(rpc.IsValid());
			bool isProcessed = middlewareRpc(rpc, reader);
			if(isProcessed)
			{
				// Processed a Middleware RPC. Wait for next RPC.
				continue;
			}
			// else isProcessed == false

			// ------------------------------------------------------------------------
			// Read/Write Rules: 
			// - ReadLock must be obtained before reading
			// - WriteLock must be obtained before writing
			//
			// - ReadLock must never be obtained by Proxy/Stub without notification
			// - WriteLock is used by Proxy/Stub without notification
			// ------------------------------------------------------------------------
			
			// Proxy or Stub calls
			switch(rpc.GetConnectionId())
			{
				case RpcIdentifier::ProxyConnection:	// always RPC return value from remote stub
				{
					isProcessed = ProxyConnection(rpc, reader);
					break;
				}
				case RpcIdentifier::StubConnection:			// always RPC call from remote proxy
				case RpcIdentifier::SubscriberConnection:	// always RPC from remote Publisher
				{
					isProcessed = StubConnection(rpc, reader);
					break;
				}
				case RpcIdentifier::PublisherConnection:
				{
					ASSERT(rpc.GetConnectionId() != RpcIdentifier::PublisherConnection);
					break;
				}
				default:
					stringstream str;
					str << PRETTY_FUNCTION << "ERROR! Could not identify " << rpc << endl;
					iFatal(str.str().c_str());
					break;
			}
			
			if(isProcessed == false) 
			{
				// if RPC was not processed/read then attempt to read it from the socket-stream and drop it 
				isProcessed = readUnprocessedRpc(rpc, reader);
				if(isProcessed == false) 
				{
					// Couldn't recover
					break;
				}
				// else Recovered and connection is still Ok
			}
		}
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception: " << ex.msg();
	}
	catch(...)
	{
		IWARNING() << "Unknown exception!";
	}

	INetConnectionManager::GetOrCreate()->RemoveConnection(socketDescriptor_, this);
}

// -----------------------------------------------
// Process middleware-specific RPCs
// TODO: These could be moved to an interface outside this class! see issue #157
// -----------------------------------------------
bool INetConnection::middlewareRpc(const RpcIdentifier &rpc, NetworkLib::SerializeReader::Ptr &reader)
{
	bool isProcessed = true;

	switch(rpc.GetMethodId())
	{
		// Middleware methods
		case RmiInterface::KeepAlive:
			{
				//read but ignore content 
				//to get around NAT issues
				messageParser_->EndUnmarshal(reader);

				cout << COMPONENT_FUNCTION << " KeepAlive " << rpc << " on " << GetInterfaceHandle() << endl;
				break;	
			}
		case RmiInterface::ProxyDisconnected:
			{
				RpcIdentifier recvRpc;
				messageParser_->ReceiveRpcObject(&recvRpc, reader);
				messageParser_->EndUnmarshal(reader);

				//TODO: Check if it is a TwoWay or OneWay Rpc?
				cout << COMPONENT_FUNCTION<< "ProxyDisconnected received " << rpc << ". Do something about: " << recvRpc << endl;
				break;
			}
		case RmiInterface::StubDisconnected:
			{
				RpcIdentifier recvRpc;
				messageParser_->ReceiveRpcObject(&recvRpc, reader);
				messageParser_->EndUnmarshal(reader);

				//TODO: Check if it is a TwoWay or OneWay Rpc?
				cout << COMPONENT_FUNCTION<< "StubDisconnected received " << rpc << ". Do something about: " << recvRpc << endl;
				break;
			}
		case RmiInterface::SubscriberDisconnected:
			{
				RpcIdentifier recvRpc;
				messageParser_->ReceiveRpcObject(&recvRpc, reader);
				messageParser_->EndUnmarshal(reader);

				//TODO: Check if it is a TwoWay or OneWay Rpc?
				cout << COMPONENT_FUNCTION<< "SubscriberDisconnected received " << rpc << ". Do something about: " << recvRpc << endl;
				break;
			}
		case RmiInterface::PublisherDisconnected:
			{
				RpcIdentifier recvRpc;
				messageParser_->ReceiveRpcObject(&recvRpc, reader);
				messageParser_->EndUnmarshal(reader);

				//TODO: Check if it is a TwoWay or OneWay Rpc?
				cout << COMPONENT_FUNCTION<< "PublisherDisconnected received " << rpc << ". Do something about: " << recvRpc << endl;
				break;
			}
		default:
			isProcessed = false;
			break;
	}

	return isProcessed;
}

// -----------------------------------------------
// Attempt to process RPCs that the middleware is not able to handle currently
// TODO: Create Interface for Unprocessed RPCs
// -----------------------------------------------
bool INetConnection::readUnprocessedRpc(RpcIdentifier &rpc, NetworkLib::SerializeReader::Ptr &reader)
{
	// -- start debug --
	ASSERT(rpc.IsValid());
	this->print(cerr);
	// -- end debug --

	// if RPC was not processed/read then attempt to read it from the socket-stream and drop it 
	if(rpc.GetRpcSize() == 0) // -> cannot recover, drop connection
	{
		cout << COMPONENT_FUNCTION<< "WARNING! Connection " << GetInterfaceHandle() << " dropped due to " << rpc << endl ;
		messageParser_->Close();
		return false;
	}
	
	// Read entire RPC into buffer and drop it
	{
		ASSERT(rpc.GetRpcSize() > 0);
		ASSERT(reader);

		cerr << COMPONENT_FUNCTION<< "Dropping rpc " << rpc << endl;

        std::vector<char> buf(rpc.GetRpcSize(), 0x00);
        reader->Read(&buf[0], rpc.GetRpcSize());
		messageParser_->EndUnmarshal(reader);
	}

	// -- notify remote Stub/Proxy about disconnection!
	switch(rpc.GetConnectionId())
	{
		case RpcIdentifier::ProxyConnection:	
			{
				RpcIdentifier replyRpc(RmiInterface::ProxyDisconnected, RmiInterface::ConnectionStateUpdate, EndianConverter::ENDIAN_LITTLE, 0, rpc.GetConnectionId());
				messageParser_->ExecuteRpc(&replyRpc, &rpc);
				break;
			}
		case RpcIdentifier::StubConnection:		
			{
				RpcIdentifier replyRpc(RmiInterface::StubDisconnected, RmiInterface::ConnectionStateUpdate, EndianConverter::ENDIAN_LITTLE, 0, rpc.GetConnectionId());
				messageParser_->ExecuteRpc(&replyRpc, &rpc);
				break;
			}
		case RpcIdentifier::PublisherConnection:	
			{
				RpcIdentifier replyRpc(RmiInterface::PublisherDisconnected, RmiInterface::ConnectionStateUpdate, EndianConverter::ENDIAN_LITTLE, 0, rpc.GetConnectionId());
				messageParser_->ExecuteRpc(&replyRpc, &rpc);
				break;
			}
		case RpcIdentifier::SubscriberConnection:
			{
				RpcIdentifier replyRpc(RmiInterface::SubscriberDisconnected, RmiInterface::ConnectionStateUpdate, EndianConverter::ENDIAN_LITTLE, 0, rpc.GetConnectionId());
				messageParser_->ExecuteRpc(&replyRpc, &rpc);
				break;
			}
		default:
			stringstream str;
			str << COMPONENT_FUNCTION<< "ERROR! Could not identify " << rpc << endl;
			iFatal(str.str().c_str());
			return false;
	}
	return true;
}

// -----------------------------------------------
// always RPC return value from remote stub
// -----------------------------------------------
bool INetConnection::ProxyConnection(const RpcIdentifier &rpc, NetworkLib::SerializeReader::Ptr &reader)
{
	MutexLocker lock(&mutex_);
	
	// -------------------------------------------------------------------------
	// Logic:
	// find proxy that is waiting for return value
	// wake up proxy and wait until it obtains readlock
	// proxy releases readlock when finished reading
	// -------------------------------------------------------------------------

	IDEBUG() << " RpcIdentifier " << rpc;

	switch(rpc.GetMethodId())
	{
		// TODO: Anything?
		default:
		{
			INetInterfaceIdentifier interfaceIdentifier(rpc);
			ASSERT(interfaceIdentifier.IsValid());

			if(mapIdInterface_.count(interfaceIdentifier) <= 0)
			{
				ICRITICAL() << "Proxy interface is not present!";
				cout << COMPONENT_FUNCTION<< " Proxy interface " << interfaceIdentifier << " is not present!" << endl;
				//ASSERT(mapIdInterface_.count(rpc.GetInterfaceName()) > 0);
				break;
			}

			ASSERT(mapIdInterface_.count(interfaceIdentifier) > 0);
			
			INetInterface *proxy = mapIdInterface_[interfaceIdentifier];
			
			// -- debug --
			ASSERT(proxy != NULL);
			ASSERT(proxy->IsValid());
			// -- debug --

			if(messageParser_->IsBufferedRead() == false)
				messageParser_->ReadLock();

			proxy->ReadCall(rpc, messageParser_, reader);
			
			if(messageParser_->IsBufferedRead() == false)
				messageParser_->ReadUnlock();
			
			return true;
		}
	}

	return false;
}

// -----------------------------------------------
// RPC call
// find stub that call is meant for 
// -----------------------------------------------
bool INetConnection::StubConnection(const RpcIdentifier &rpc, NetworkLib::SerializeReader::Ptr &reader)
{
	MutexLocker lock(&mutex_);
	
	// -------------------------------------------------------------------------
	// Logic:
	// find stub listening for RPC calls
	// add rpc to its queue
	// wake up stub and wait until it obtains readlock
	// stub releases readlock when finished reading
	// -------------------------------------------------------------------------

	//IDEBUG() << " RpcIdentifier " << rpc ;
	switch(rpc.GetMethodId())
	{
		// TODO: Anything?
		default:
		{
			INetInterfaceIdentifier interfaceIdentifier(rpc);
			if(mapIdInterface_.count(interfaceIdentifier) <= 0)
			{
				IDEBUG()  << "ERROR! Stub interface " << interfaceIdentifier << " is not present!";
				//ASSERT(mapIdInterface_.count(rpc.GetInterfaceName()) > 0);
				break;
			}

			ASSERT(mapIdInterface_.count(interfaceIdentifier) > 0);
			INetInterface *inet = mapIdInterface_[interfaceIdentifier];
			
			// -- start debug --
			ASSERT(inet != NULL);
			ASSERT(inet->IsValid());
			ASSERT(inet->GetInterfaceType() == HostInformation::STUB || inet->GetInterfaceType() == HostInformation::SUBSCRIBER);
			// -- end debug --

			if(messageParser_->IsBufferedRead() == false)
				messageParser_->ReadLock();

			inet->ReadCall(rpc, messageParser_, reader);
			
			if(messageParser_->IsBufferedRead() == false)
				messageParser_->ReadUnlock();

			return true;
		}
	}

	return false;
}

// -----------------------------------------------
// register/remove interfaces
// -----------------------------------------------
bool INetConnection::RegisterInterface(INetInterface *inet)
{
	// -- start check --
	ASSERT(inet);
	ASSERT(inet->IsValid());
	if(mapIdInterface_.count(inet->GetInterfaceIdentifier()) > 0)
	{
		IDEBUG() << "ERROR! Interface " << inet->GetInterfaceIdentifier() << " is already present!" ;
		ASSERT(mapIdInterface_.count(inet->GetInterfaceIdentifier()) <= 0);
		return false;
	}
	ASSERT(mapIdInterface_.count(inet->GetInterfaceIdentifier()) <= 0);
	// -- end check --
	
	HostInformation info = connectionInformation_;
	if(bool connected = IsConnected())
		info.SetOnlineStatus(HostInformation::ONLINE);
	else
		info.SetOnlineStatus(HostInformation::OFFLINE);
	
	switch(inet->GetInterfaceType())
	{
		case HostInformation::PROXY:
		{
			inet->SetBase(messageParser_);
			info.SetHostDescription(HostInformation::PROXY);
			break;
		}
		case HostInformation::STUB:
		{
			info.SetHostDescription(HostInformation::STUB);
			break;
		}
		case HostInformation::PUBLISHER:
		{
			inet->SetBase(messageParser_);
			info.SetHostDescription(HostInformation::PUBLISHER);
			break;
		}
		case HostInformation::SUBSCRIBER:
		{
			info.SetHostDescription(HostInformation::SUBSCRIBER);
			break;
		}
		default:
			IDEBUG() << "ERROR! Could not identify: " << inet->GetInterfaceType() ;
			return false;
			break;
	}

	cout << COMPONENT_FUNCTION << info << endl;

	inet->SetConnection(info);
	
	mapIdInterface_.insert(pair<INetInterfaceIdentifier, INetInterface*>(inet->GetInterfaceIdentifier(), inet));
	return true;
}

bool INetConnection::RemoveInterface(INetInterface *inet)
{
	// -- start check --
	ASSERT(inet);
	ASSERT(inet->IsValid());
	if(mapIdInterface_.count(inet->GetInterfaceIdentifier()) <= 0)
	{
		IDEBUG() << "WARNING! Interface " << inet->GetInterfaceIdentifier() << " is not present!" ;
		return false;
	}
	ASSERT(mapIdInterface_.count(inet->GetInterfaceIdentifier()) > 0);
	// -- end check --
	

	HostInformation info = connectionInformation_;
	switch(inet->GetInterfaceType())
	{
		case HostInformation::PROXY:
		{
			info.SetHostDescription(HostInformation::PROXY);
			break;
		}
		case HostInformation::STUB:
		{
			info.SetHostDescription(HostInformation::STUB);
			break;
		}
		case HostInformation::PUBLISHER:
		{
			info.SetHostDescription(HostInformation::PUBLISHER);
			break;
		}
		case HostInformation::SUBSCRIBER:
		{
			info.SetHostDescription(HostInformation::SUBSCRIBER);
			break;
		}
		default:
			IDEBUG() << "ERROR! Could not identify: " << inet->GetInterfaceType() ;
			return false;
	}

	info.SetOnlineStatus(HostInformation::OFFLINE);	
	inet->SetConnection(info);
	
	mapIdInterface_.erase(inet->GetInterfaceIdentifier());
	return true;
}

// -----------------------------------------------
// stop connection
// -----------------------------------------------
void INetConnection::Stop(bool waitForTermination)
{
	try
	{
		MutexLocker lock(&mutex_);
		
		runThread_ = false;

		if(messageParser_)
			messageParser_->Close();
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught: " << ex.what();
	}	

	if(waitForTermination)
		wait();
}

// -----------------------------------------------
// Called from outside to KeepAlive connection
//  -> Called in INetConnectionManager::run()
// -----------------------------------------------
bool INetConnection::KeepAlive()
{
	try
	{
		if(IsConnected() == false) return false;

		MutexLocker lock(&mutex_);
		
		// Only send keep alive if it has been more than xx seconds since last activity
		if(messageParser_->DoKeepAlive() == true)
		{
			RpcIdentifier keepAlive(RmiInterface::KeepAlive, 
								RmiInterface::ConnectionStateUpdate, 
								EndianConverter::ENDIAN_LITTLE);


			cout << COMPONENT_FUNCTION << " Sending KeepAlive " << keepAlive << " on " << GetInterfaceHandle() << endl;
			messageParser_->ExecuteRpc(&keepAlive);
		}
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught : " << ex.what();
		return false;
	}

	return true;
}

// -----------------------------------------------
// Sets up a TcpSocket connection to handle
// TODO: Call alternative NAT punching algorithms
// using private/public addresses in InterfaceHandle
// TODO: implement NAT punching algorithms in TcpSocket and UdpSocket
// -----------------------------------------------
bool INetConnection::connectToHost(const InterfaceHandle &handle)
{
	ASSERT(IsConnected() == false);
	ASSERT(GetConnectionType() == ConnectionInterface::TcpClient);
	
	bool connected = false;
	
	try
	{
		// 1. Check if I am behind NAT -> Should probably be discovered upon start of the application
		// bool isBechindNAT = INetNATPuncher::TcpNatPunchToPeer(handle);
		// if(isBehindNAT) // then do something


		socketDescriptor_ = -1;
		
		TcpSocket::Ptr tcpSocket(new TcpSocket());
		IDEBUG() << "Connecting to " << handle ;
		connected = tcpSocket->connectToHost(handle.GetHostName(), handle.GetPortNumber());
		if(connected == false)
		{
			throw("ERROR! connecting!");
		}

		IDEBUG() << "Connected to " << handle ;
		messageParser_->InitSocketObjects(tcpSocket);
		
		socketDescriptor_ = tcpSocket->socketDescriptor();
		ASSERT(socketDescriptor_ > -1);
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! " << ex.msg() << " connecting to " << interfaceHandle_ ;
	}
	catch(...)
	{
		//IDEBUG() << "ERROR! connecting to " << interfaceHandle_ ;
		cerr << ";";
	}

	return connected;
}

// -----------------------------------------------
// Sets up a UdpSocket connection to handle
// -----------------------------------------------
bool INetConnection::bindToInterfaceHandle(InterfaceHandle &handle)
{
	ASSERT(IsConnected());
	ASSERT(GetConnectionType() == ConnectionInterface::UdpSender || GetConnectionType() == ConnectionInterface::UdpReceiver);

	bool connected = false;

	try
	{
		socketDescriptor_ = -1;
		IDEBUG() << "Binding to " << handle ;

		int portNumber = handle.GetPortNumber();
        UdpSocket::Ptr udpSocket;

		switch(GetConnectionType())
		{
			case ConnectionInterface::UdpSender:
			{
				udpSocket = UdpSocket::Ptr(new UdpSocket(UdpSocket::UnicastSender));
				connected = udpSocket->bindSender(handle.GetHostName(), portNumber);
				break;
			}
			case ConnectionInterface::UdpReceiver:
			{
				udpSocket = UdpSocket::Ptr(new UdpSocket(UdpSocket::UnicastReceiver));
				connected = udpSocket->bindReceiver(handle.GetHostName(), portNumber);
				break;
			}
			case ConnectionInterface::MulticastSender:
			{
				udpSocket = UdpSocket::Ptr(new UdpSocket(UdpSocket::MulticastSender));
				connected = udpSocket->bindSender(handle.GetHostName(), portNumber);
				break;
			}
			case ConnectionInterface::MulticastReceiver:
			{
				udpSocket = UdpSocket::Ptr(new UdpSocket(UdpSocket::MulticastReceiver));
				connected = udpSocket->bindReceiver(handle.GetHostName(), portNumber);
				break;
			}
			default:
				iFatal() << PRETTY_FUNCTION << " Could not recognize connection type!";
				break;
		}
		
		if(connected == false)
		{
			throw Exception("INetConnection::bindToInterfaceHandle(): ERROR! binding error!");
		}

		handle.SetPortNumber(portNumber);

		IDEBUG() << " bound to " << handle ;
		messageParser_->InitSocketObjects(udpSocket);

		socketDescriptor_ = udpSocket->socketDescriptor();
		ASSERT(socketDescriptor_ > -1);
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! " << ex.msg() << " connecting to " << handle ;
	}
	catch(...)
	{
		cerr << ";";
	}
	return connected;
}

void INetConnection::print(std::ostream &ostr)
{
	if(mapIdInterface_.empty() == false)
	{
		int i = 0;
		ostr << COMPONENT_FUNCTION << "On " << this->GetInterfaceHandle() << " " << mapIdInterface_.size() << " service-interfaces: " << endl;
		for(MapIdInterface::iterator it = mapIdInterface_.begin(), it_end = mapIdInterface_.end(); it != it_end; ++it)
			ostr << ++i << ": " << it->first << endl;
	}
}

}; // namespace end




