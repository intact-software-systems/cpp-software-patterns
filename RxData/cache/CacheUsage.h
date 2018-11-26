#ifndef RxData_Cache_CacheUsage_h_IsIncluded
#define RxData_Cache_CacheUsage_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/Export.h"

namespace RxData
{
/**
 * The cache_usage indicates whether the cache is intended to support write operations 
 * (WRITE_ONLY or READ_WRITE) or not (READ_ONLY).
 * 
 * @author KVik
 */
namespace CacheUsage
{
    enum Type
    {
        READ_ONLY,
        WRITE_ONLY,
        READ_WRITE
    };
}

}

#endif

