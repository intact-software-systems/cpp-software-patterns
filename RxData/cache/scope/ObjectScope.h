#ifndef RxData_Cache_ObjectScope_h_IsIncluded
#define RxData_Cache_ObjectScope_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * The scope an object, i.e., the object itself, or the object with all its 
 * (nested) compositions, or the object with all its (nested) compositions and 
 * all the objects that are navigable from it up till the specified depth).
 * 
 * @author KVik
 */
namespace ObjectScope
{
    enum Type
    {
        SIMPLE_OBJECT_SCOPE, 		// only the object
        CONTAINED_OBJECTS_SCOPE, 	// + contained objects
        RELATED_OBJECTS_SCOPE 		// + all related objects
    };
}

}

#endif
