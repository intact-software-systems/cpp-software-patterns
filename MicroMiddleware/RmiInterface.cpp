#include "MicroMiddleware/RmiInterface.h"
#include "MicroMiddleware/CommonDefines.h"
#include "MicroMiddleware/IncludeExtLibs.h"

namespace MicroMiddleware
{

std::string RmiInterface::GetInterfaceName() const
{
	return RPC_FUNCTION_STRING;
}

} // namespace MicroMiddleware

