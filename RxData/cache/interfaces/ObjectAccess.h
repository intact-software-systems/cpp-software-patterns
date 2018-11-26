#ifndef RxData_Cache_ObjectAccess_h_IsIncluded
#define RxData_Cache_ObjectAccess_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/cache/interfaces/ObjectWriter.h"
#include"RxData/cache/interfaces/ObjectReader.h"

namespace RxData
{
/**
 * Interface to be implemented by the data-dispatchers from the cache.
 *
 * Example: A wrapper around a data-base access object. A wrapper around a topic subscriber.
 *
 * @author KVik
 *
 * @param <DATA>
 */
template <typename DATA, typename KEY>
class ObjectAccess : public ObjectWriter<DATA, KEY>, public ObjectReader<DATA, KEY>
{
public:
    CLASS_TRAITS(ObjectAccess)
};

}

#endif
