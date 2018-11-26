#ifndef NetworkLib_NetObjectBase_h_Included
#define NetworkLib_NetObjectBase_h_Included

#include"NetworkLib/Export.h"

namespace NetworkLib
{

class SerializeWriter;
class SerializeReader;

// TODO: forward declaration of ptr
//FORWARD_CLASS_TRAITS(SerializeWriter);
//FORWARD_CLASS_TRAITS(SerializeReader);

class DLL_STATE NetObjectBase
{
public:
	NetObjectBase() { }
	virtual ~NetObjectBase() { }

	virtual void Write(SerializeWriter *) const = 0;
	virtual void Read(SerializeReader *) = 0;
};

} // namespace NetworkLib

#endif // NetworkLib_NetObjectBase_h_Included


