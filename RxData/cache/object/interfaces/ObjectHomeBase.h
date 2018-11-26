#ifndef RxData_Cache_ObjectHomeBase_h_IsIncluded
#define RxData_Cache_ObjectHomeBase_h_IsIncluded

#include"RxData/IncludeExtLibs.h"
#include"RxData/Export.h"

#include"RxData/Cache/CacheBase.h"

namespace DataManagement { namespace Cache
{
/**
 * @brief The ObjectHomeBase class
 */
class DLL_STATE ObjectHomeBase
{
public:
    CLASS_TRAITS(ObjectHomeBase)

    virtual void refresh(CacheBase::Ptr) = 0;
};

}}

#endif // OBJECTHOMEBASE_H
