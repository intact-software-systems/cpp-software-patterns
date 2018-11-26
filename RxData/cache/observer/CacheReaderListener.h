#ifndef RxData_Cache_CacheReaderListener_h_IsIncluded
#define RxData_Cache_CacheReaderListener_h_IsIncluded

#include"RxData/CommonDefines.h"
#include"RxData/Export.h"

namespace RxData
{

/**
 * Implemented by the Cache and CacheAccess and allows it to be attached to a data-producer (example: subscriber)
 * 
 * @author KVik
 */
template <typename DATA, typename KEY>
class DLL_STATE CacheReaderListener
{
public:
    virtual bool OnDataAvailable(DATA data, KEY key) = 0;
};

}

#endif
