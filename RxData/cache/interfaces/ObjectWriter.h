#ifndef RxData_Cache_ObjectWriter_h_IsIncluded
#define RxData_Cache_ObjectWriter_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

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
class ObjectWriter
{
public:
    CLASS_TRAITS(ObjectWriter)

    virtual bool Write(DATA data, KEY key) = 0;

    virtual bool WriteAll(const std::map<KEY, DATA> &values) = 0;

    virtual bool Dispose(KEY key) = 0;

    virtual void DisposeAll() = 0;

    virtual bool ContainsKey(KEY key) = 0;

    //public <KEY> bool Unregister(KEY key);
};

}

#endif
