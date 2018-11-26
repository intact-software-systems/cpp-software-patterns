#ifndef RxData_Cache_RelationName_h_IsIncluded
#define RxData_Cache_RelationName_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * @author KVik
 */
class DLL_STATE RelationName
{
private:
	std::string name_;
	
public:
	RelationName(std::string name)
        : name_(name)
	{ }

	std::string GetName() const
	{
		return name_;
	}
};

}

#endif

