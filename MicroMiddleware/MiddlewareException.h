#ifndef MicroMiddleware_MiddlewareException_h_IS_INCLUDED
#define MicroMiddleware_MiddlewareException_h_IS_INCLUDED

#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class ExceptionMulticastPublisher;

/**
* @brief Exception thrown if a problem concerning the middleware occurs
*/
class DLL_STATE MiddlewareException : public std::runtime_error
{
public:
	enum ExceptionDescription { NO_EXCEPTION_DESCRIPTION = 0,
								GENERAL_EXCEPTION,
								GENERAL_WARNING,
								GENERAL_ERROR,
								INTERFACE_ERROR,
								CONFIGURATION_ERROR,
								SETTINGS_ERROR,
								DATA_ERROR,
								DATA_INACTIVITY_ERROR,
								IP_MULTICAST_ERROR
	};

private:
	ExceptionDescription exceptionDescription;

public:

	/**
	* Constructs a middleware exception with the given message.
	*/
	MiddlewareException(const std::string& message);

	/**
	* Constructs a middleware exception with the given message.
	*/
	MiddlewareException(const std::string& message, MiddlewareException::ExceptionDescription ex);

	/**
	* @return	ExceptionDescription, a description to categorize the MiddlewareException.
	*/
	MiddlewareException::ExceptionDescription	GetExceptionDescription()	const	{ return exceptionDescription; }
};

DLL_STATE ostream& operator<<(ostream &str, const MiddlewareException::ExceptionDescription &hd);

} // namespace MicroMiddleware

#endif // MicroMiddleware_MiddlewareException_h_IS_INCLUDED


