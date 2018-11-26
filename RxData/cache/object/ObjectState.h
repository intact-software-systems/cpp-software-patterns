#ifndef RxData_Cache_ObjectState_h_IsIncluded
#define RxData_Cache_ObjectState_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/Export.h"

namespace RxData
{
/**
 * Lifecycle states of an object.
 * 
 * @author KVik 
 */
namespace ObjectState
{
    enum Type
    {
        OBJECT_VOID,
        OBJECT_NEW,
        OBJECT_NOT_MODIFIED,
        OBJECT_MODIFIED,
        OBJECT_DELETED
    };
}

}

#endif

