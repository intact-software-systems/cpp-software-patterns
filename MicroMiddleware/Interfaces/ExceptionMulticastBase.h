#ifndef MicroMiddleware_ExceptionMulticastBase_h_IS_INCLUDED
#define MicroMiddleware_ExceptionMulticastBase_h_IS_INCLUDED

#include<string>
#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/IPPublisher.h"
#include"MicroMiddleware/Net/IPSubscriberListener.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"
#include"MicroMiddleware/MiddlewareException.h"
#include"MicroMiddleware/RmiInterface.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

// -------------------------------------------------------
//				The shared data
// -------------------------------------------------------
class DLL_STATE ExceptionMessage : public RmiObjectBase
{
public:
	ExceptionMessage()
	{}
	ExceptionMessage(const HostInformation &hostInfo, const string &d, string stamp, MiddlewareException::ExceptionDescription ex = MiddlewareException::GENERAL_EXCEPTION)
		: hostInformation(hostInfo)
		, message(d)
		, timeStamp(stamp)
		, exceptionDescription(ex)
	{}
	~ExceptionMessage() 
	{}

	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		hostInformation.Write(writer);
		writer->WriteString(timeStamp);
		writer->WriteString(message);
		writer->WriteInt32((int)exceptionDescription);
	}
	
	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		hostInformation.Read(reader);
		timeStamp				= reader->ReadString();
		message					= reader->ReadString();
		exceptionDescription	= (MiddlewareException::ExceptionDescription)reader->ReadInt32();
	}

	void SetHostInformation(const HostInformation &hostInfo)					{ hostInformation = hostInfo; }
	void SetMessage(const string &d)											{ message = d; }
	void SetTimeStamp(const string stamp)										{ timeStamp = stamp; }
	
	void SetExceptionDescription(MiddlewareException::ExceptionDescription ex)	{ exceptionDescription = ex; }

	HostInformation GetHostInformation()	const { return hostInformation; }
	string			GetComponentName()		const { return hostInformation.GetComponentName(); }
	string			GetMessage()			const { return message; }
	string			GetTimeStamp()			const { return timeStamp; }
	
	MiddlewareException::ExceptionDescription GetExceptionDescription()	const { return exceptionDescription; }

	friend std::ostream& operator<<(std::ostream &ostr, const ExceptionMessage &msg)
	{
		ostr << "-- " << msg.GetExceptionDescription() << " in " << msg.GetComponentName() << " --" << endl;
		ostr << "Local timestamp:       \t" << msg.GetTimeStamp() << endl;
		ostr << "Component information: \t" << msg.GetHostInformation() << endl;
		ostr << "Message details:       \t" << msg.GetMessage() << endl;
		return ostr;
	}

private:
	HostInformation		hostInformation;
	string				message;
	string				timeStamp;

	MiddlewareException::ExceptionDescription exceptionDescription;
};


// -------------------------------------------------------
//				Publisher Interface
// -------------------------------------------------------
class DLL_STATE ExceptionMulticastInterface : public RmiInterface
{
public:
	ExceptionMulticastInterface() { }
	~ExceptionMulticastInterface() { }

	enum MethodID {
		Method_PostException = 11
	};

	enum InterfaceId
	{
		ExceptionMulticastInterfaceId = 8
	};

	virtual int GetInterfaceId() const { return ExceptionMulticastInterface::ExceptionMulticastInterfaceId; }
	
	/**
	* Posts (multicasts) the ExceptionMessage to a default multicast address and port.
	*
	* @param exceptionMessage	Contains the HostInformation of the component where the exception occured, 
	*							and a message that describes the exception.
	*/
	virtual void PostException(ExceptionMessage &exceptionMessage) = 0;
};

// -------------------------------------------------------
//			Proxy - Publisher class
// -------------------------------------------------------
class DLL_STATE ExceptionMulticastPublisher : virtual public IPPublisher, virtual public ExceptionMulticastInterface
{
public:
	IPPUBLISHER_COMMON(ExceptionMulticastPublisher);

	virtual void PostException(ExceptionMessage &exceptionMessage)
	{
		BeginMarshal(ExceptionMulticastInterface::Method_PostException);
		exceptionMessage.Write(GetSocketWriter());
		EndMarshal();
	}
};

// -------------------------------------------------------
//			Stub - Subscriber class 
// -------------------------------------------------------
class DLL_STATE ExceptionMulticastSubscriber : public IPSubscriberListener
{
public:
	ExceptionMulticastSubscriber(InterfaceHandle handle, ExceptionMulticastInterface* testInt, bool autoStart)
		: IPSubscriberListener(handle, autoStart)
		, testInterface(testInt)
	{ }
	virtual ~ExceptionMulticastSubscriber() {}

	virtual void RMInvocation(int methodId, RmiBase* stubClient)
	{
		switch(methodId)
		{
			case ExceptionMulticastInterface::Method_PostException:
			{
				ExceptionMessage exceptionMessage;
				exceptionMessage.Read(stubClient->GetSocketReader());
				stubClient->EndUnmarshal();
				
				testInterface->PostException(exceptionMessage);
				break;
			}
			default:
				IPSubscriberListener::RMInvocation(methodId, stubClient);
				break;
		}
	}
private:
	ExceptionMulticastInterface *testInterface;
};

/**
* @brief Multicast exception when a middleware exception occurs.
*/
class DLL_STATE ExceptionMulticast
{
public:
	static void PostException(const std::string &message, MiddlewareException::ExceptionDescription ex = MiddlewareException::GENERAL_EXCEPTION);
	static InterfaceHandle	GetMulticastHandle()	{ return ExceptionMulticast::multicastHandle; }

private:
	ExceptionMulticast();
	~ExceptionMulticast();

	static ExceptionMulticast* GetExceptionMulticast();

	ExceptionMulticastPublisher*	GetExceptionPublisher()	{ return exceptionPublisher; }

private:
	static ExceptionMulticast		*exceptionMulticast;
	static InterfaceHandle			multicastHandle;
	ExceptionMulticastPublisher		*exceptionPublisher;
};


} // namespace MicroMiddleware

#endif  // ExceptionMulticastBase_h_IS_INCLUDED


