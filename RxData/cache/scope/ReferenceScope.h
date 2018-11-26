#ifndef RxData_Cache_ReferenceScope_h_IsIncluded
#define RxData_Cache_ReferenceScope_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

namespace ReferenceScope
{
    enum Type
    {
        SIMPLE_CONTENT_SCOPE, 		// only the reference content
        REFERENCED_CONTENTS_SCOPE 	// + referenced contents
    };
}

}

#endif
