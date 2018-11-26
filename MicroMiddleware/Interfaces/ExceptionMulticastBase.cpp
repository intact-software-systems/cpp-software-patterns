#include "MicroMiddleware/Interfaces/ExceptionMulticastBase.h"
#include "MicroMiddleware/CommonDefines.h"
#include "MicroMiddleware/MiddlewareSettings.h"

namespace MicroMiddleware
{

ExceptionMulticast* ExceptionMulticast::exceptionMulticast = NULL;
InterfaceHandle ExceptionMulticast::multicastHandle = InterfaceHandle(MiddlewareSettings::GetExceptionMulticastPort(), "237.3.4.101");

ExceptionMulticast::ExceptionMulticast()
	: exceptionPublisher( new ExceptionMulticastPublisher(ExceptionMulticast::multicastHandle, true) )
	
{
		
}

ExceptionMulticast::~ExceptionMulticast()
{
	if(exceptionPublisher) 
	{
		delete exceptionPublisher;
		exceptionPublisher = NULL;
	}
}

ExceptionMulticast* ExceptionMulticast::GetExceptionMulticast()
{
	if(ExceptionMulticast::exceptionMulticast == NULL) 
		ExceptionMulticast::exceptionMulticast = new ExceptionMulticast();
	
	return ExceptionMulticast::exceptionMulticast;
}

void ExceptionMulticast::PostException(const std::string &message, MiddlewareException::ExceptionDescription exceptionDescription)
{
	ExceptionMulticast *ex = ExceptionMulticast::GetExceptionMulticast();
	if(ex == NULL) return;
	
	HostInformation hostInfo = Runtime::GetHostInformation();
	if(!hostInfo.GetComponentName().empty())
	{
		ExceptionMessage exMsg(hostInfo, message, GetTimeStamp(), exceptionDescription);
		ex->GetExceptionPublisher()->PostException(exMsg);
	}
}


} // namespace MicroMiddleware 

