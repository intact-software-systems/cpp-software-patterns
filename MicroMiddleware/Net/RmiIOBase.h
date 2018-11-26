#ifndef MicroMiddleware_RmiIOBase_h_IsIncluded
#define MicroMiddleware_RmiIOBase_h_IsIncluded

#include<stdlib.h>

#include "MicroMiddleware/Export.h"
namespace MicroMiddleware 
{
class DLL_STATE RmiSenderBase
{
public:
	virtual void ExecuteOneWayRMI(short methodId, RmiObjectBase *obj) = 0;
	virtual void ExecuteTwoWayRMI(short methodId, RmiObjectBase *obj) = 0;
	
};

class DLL_STATE RmiReceiverBase
{
public:
	virtual void ReceiveRMI(RmiObjectBase *obj) = 0;
	virtual void ReplyRMI(RmiObjectBase *obj) = 0;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RmiIOBase_h_IsIncluded



