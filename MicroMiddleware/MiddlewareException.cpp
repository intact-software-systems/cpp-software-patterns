#include "MicroMiddleware/MiddlewareException.h"
#include "MicroMiddleware/Interfaces/ExceptionMulticastBase.h"
#include "MicroMiddleware/IncludeExtLibs.h"

namespace MicroMiddleware
{

ostream& operator<<(ostream &str, const MiddlewareException::ExceptionDescription &hd)
{
#define PRINTOUT( name ) case name: str << #name; break;
	switch(hd)
	{
		PRINTOUT(MiddlewareException::NO_EXCEPTION_DESCRIPTION)
		PRINTOUT(MiddlewareException::GENERAL_EXCEPTION)
		PRINTOUT(MiddlewareException::GENERAL_WARNING)
		PRINTOUT(MiddlewareException::GENERAL_ERROR)
		PRINTOUT(MiddlewareException::INTERFACE_ERROR)
		PRINTOUT(MiddlewareException::CONFIGURATION_ERROR)
		PRINTOUT(MiddlewareException::SETTINGS_ERROR)
		PRINTOUT(MiddlewareException::DATA_ERROR)
		PRINTOUT(MiddlewareException::DATA_INACTIVITY_ERROR)
		PRINTOUT(MiddlewareException::IP_MULTICAST_ERROR)
		default:
			cerr << "operator<<(ostream, MiddlewareException::ExceptionDescription): ERROR! MiddlewareException::ExceptionDescription is undefined " << endl;
			break;
	}
#undef PRINTOUT
	return str;
}


MiddlewareException::MiddlewareException(const std::string& message) 
		: std::runtime_error(message) 
		, exceptionDescription(MiddlewareException::GENERAL_EXCEPTION)
{
	ExceptionMulticast::PostException(message, exceptionDescription);
}

MiddlewareException::MiddlewareException(const std::string& message, MiddlewareException::ExceptionDescription ex) 
		: std::runtime_error(message) 
		, exceptionDescription(ex)
{
	// IP_MULTICAST_ERROR is thrown in PublisherBase if IP Multicast is not properly setup on the computer it is running on
	if(exceptionDescription != MiddlewareException::IP_MULTICAST_ERROR) 
		ExceptionMulticast::PostException(message, exceptionDescription);
}

} // namespace MicroMiddleware

