#include "BaseLib/Debug.h"

namespace BaseLib
{

#ifndef INTACT_NO_DEBUG_OUTPUT 

//#define APPENDDEBUG TSPRETTY_FUNCTION

IDebug iInfo(std::string append)
{
	return IDebug(LogDebugBase::INFO, append);
}

IDebug iDebug(std::string append)
{ 
	return IDebug(LogDebugBase::DEBUG, append);
}

IDebug iWarning(std::string append)
{ 
	return IDebug(LogDebugBase::WARNING, append);
}

IDebug iCritical(std::string append)
{ 
	return IDebug(LogDebugBase::CRITICAL, append);
}

IDebug iFatal(std::string append)
{
	return IDebug(LogDebugBase::FATAL, append);
}

#else
INoDebug iInfo(std::string append)		{ return INoDebug(); }
INoDebug iDebug(std::string append) 	{ return INoDebug(); }
INoDebug iWarning(std::string append) 	{ return INoDebug(); }
INoDebug iCritical(std::string append) 	{ return INoDebug(); }
INoDebug iFatal(std::string append)		{ return INoDebug(); }
#endif

} // namespace BaseLib

