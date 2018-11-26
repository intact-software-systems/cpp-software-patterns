#ifndef RxData_Cache_CacheKind_h_IsIncluded
#define RxData_Cache_CacheKind_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/Export.h"

namespace RxData
{
/**
 * The kind describes whether a CacheBase instance represents a Cache or a CacheAccess.
 * 
 * @author KVik
 */
namespace CacheKind
{
    enum Type
    {
        CACHE_KIND,
        CACHEACCESS_KIND
    };
}

}

#endif
