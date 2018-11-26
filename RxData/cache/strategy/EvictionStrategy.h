#ifndef RxData_Cache_EvictionStrategy_h_IsIncluded
#define RxData_Cache_EvictionStrategy_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/object/ObjectCache.h"

namespace RxData
{

template <typename K>
class EvictionStrategy : public Templates::Strategy2< ISet<K>, ObjectCache::Ptr, int64>
{ };

template <typename K>
class EvictionStrategyNoOp : public EvictionStrategy<K>
{
public:
    ISet<K> Perform(ObjectCache::Ptr , int64 )
    {
        return ISet<K>();
    }
};

}

#endif
